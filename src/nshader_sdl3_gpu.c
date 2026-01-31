/*
MIT License

Copyright (c) 2026 Christian Luppi

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <nshader/nshader_sdl3_gpu.h>
#include <nshader/nshader_type.h>
#include <nshader/nshader_info.h>
#include "nshader_type_internal.h"

// Map nshader backend to SDL GPU shader format
static SDL_GPUShaderFormat nshader_backend_to_sdl_format(nshader_backend_t backend) {
  switch (backend) {
    case NSHADER_BACKEND_DXIL:
      return SDL_GPU_SHADERFORMAT_DXIL;
    case NSHADER_BACKEND_DXBC:
      return SDL_GPU_SHADERFORMAT_DXBC;
    case NSHADER_BACKEND_MSL:
      return SDL_GPU_SHADERFORMAT_MSL;
    case NSHADER_BACKEND_SPV:
      return SDL_GPU_SHADERFORMAT_SPIRV;
    default:
      return SDL_GPU_SHADERFORMAT_INVALID;
  }
}

// Select the best backend for the device
static nshader_backend_t select_backend_for_device(SDL_GPUDevice* device, const nshader_t* shader) {
  if (!device || !shader) {
    return NSHADER_BACKEND_COUNT;
  }

  // Get supported shader formats from the device
  SDL_GPUShaderFormat supported_formats = SDL_GetGPUShaderFormats(device);

  // Priority order: DXIL > SPV > MSL > DXBC
  const nshader_backend_t backend_priority[] = {
    NSHADER_BACKEND_DXIL,
    NSHADER_BACKEND_SPV,
    NSHADER_BACKEND_MSL,
    NSHADER_BACKEND_DXBC
  };

  // Find the first backend that is both supported by the device and available in the shader
  for (size_t i = 0; i < sizeof(backend_priority) / sizeof(backend_priority[0]); i++) {
    nshader_backend_t backend = backend_priority[i];
    SDL_GPUShaderFormat format = nshader_backend_to_sdl_format(backend);

    if ((supported_formats & format) && nshader_has_backend(shader, backend)) {
      return backend;
    }
  }

  return NSHADER_BACKEND_COUNT; // No compatible backend found
}

// Get the stage metadata for a specific stage type
static const nshader_stage_t* get_stage(const nshader_t* shader, nshader_stage_type_t stage_type) {
  if (!shader) {
    return NULL;
  }

  const nshader_info_t* info = nshader_get_info(shader);
  if (!info) {
    return NULL;
  }

  for (size_t i = 0; i < info->num_stages; i++) {
    if (info->stages[i].type == stage_type) {
      return &info->stages[i];
    }
  }

  return NULL;
}

NSHADER_API SDL_GPUShader* nshader_sdl3_gpu_create_shader(
  SDL_GPUDevice* device,
  const nshader_t* shader,
  nshader_stage_type_t stage_type)
{
  if (!device || !shader) {
    return NULL;
  }

  // Validate stage type for graphics shaders
  if (stage_type != NSHADER_STAGE_TYPE_VERTEX && stage_type != NSHADER_STAGE_TYPE_FRAGMENT) {
    return NULL;
  }

  // Check if the shader has the requested stage
  if (!nshader_has_stage(shader, stage_type)) {
    return NULL;
  }

  // Select the appropriate backend
  nshader_backend_t backend = select_backend_for_device(device, shader);
  if (backend == NSHADER_BACKEND_COUNT) {
    return NULL; // No compatible backend found
  }

  // Get the shader blob for the selected backend and stage
  const nshader_blob_t* blob = nshader_get_blob(shader, stage_type, backend);
  if (!blob || !blob->data || blob->size == 0) {
    return NULL;
  }

  // Get the stage metadata
  const nshader_stage_t* stage = get_stage(shader, stage_type);
  if (!stage) {
    return NULL;
  }

  // Create SDL_GPUShaderCreateInfo
  SDL_GPUShaderCreateInfo create_info = {0};
  create_info.code_size = blob->size;
  create_info.code = blob->data;
  create_info.entrypoint = stage->entry_point ? stage->entry_point : "main";
  create_info.format = nshader_backend_to_sdl_format(backend);
  create_info.stage = (stage_type == NSHADER_STAGE_TYPE_VERTEX)
                      ? SDL_GPU_SHADERSTAGE_VERTEX
                      : SDL_GPU_SHADERSTAGE_FRAGMENT;

  // Set resource counts based on stage type
  if (stage_type == NSHADER_STAGE_TYPE_VERTEX) {
    const nshader_stage_metadata_vertex_t* vert = &stage->metadata.vertex;
    create_info.num_samplers = vert->num_samplers;
    create_info.num_storage_textures = vert->num_storage_textures;
    create_info.num_storage_buffers = vert->num_storage_buffers;
    create_info.num_uniform_buffers = vert->num_uniform_buffers;
  } else if (stage_type == NSHADER_STAGE_TYPE_FRAGMENT) {
    const nshader_stage_metadata_fragment_t* frag = &stage->metadata.fragment;
    create_info.num_samplers = frag->num_samplers;
    create_info.num_storage_textures = frag->num_storage_textures;
    create_info.num_storage_buffers = frag->num_storage_buffers;
    create_info.num_uniform_buffers = frag->num_uniform_buffers;
  }

  create_info.props = 0;

  // Create the SDL GPU shader
  return SDL_CreateGPUShader(device, &create_info);
}

NSHADER_API void nshader_sdl3_gpu_release_shader(
  SDL_GPUDevice* device,
  SDL_GPUShader* gpu_shader)
{
  if (device && gpu_shader) {
    SDL_ReleaseGPUShader(device, gpu_shader);
  }
}

NSHADER_API SDL_GPUComputePipeline* nshader_sdl3_gpu_create_compute_pipeline(
  SDL_GPUDevice* device,
  const nshader_t* shader)
{
  if (!device || !shader) {
    return NULL;
  }

  // Verify this is a compute shader
  const nshader_info_t* info = nshader_get_info(shader);
  if (!info || info->type != NSHADER_SHADER_TYPE_COMPUTE) {
    return NULL;
  }

  // Check if the shader has a compute stage
  if (!nshader_has_stage(shader, NSHADER_STAGE_TYPE_COMPUTE)) {
    return NULL;
  }

  // Select the appropriate backend
  nshader_backend_t backend = select_backend_for_device(device, shader);
  if (backend == NSHADER_BACKEND_COUNT) {
    return NULL; // No compatible backend found
  }

  // Get the compute shader blob
  const nshader_blob_t* blob = nshader_get_blob(shader, NSHADER_STAGE_TYPE_COMPUTE, backend);
  if (!blob || !blob->data || blob->size == 0) {
    return NULL;
  }

  // Get the compute stage metadata
  const nshader_stage_t* stage = get_stage(shader, NSHADER_STAGE_TYPE_COMPUTE);
  if (!stage) {
    return NULL;
  }

  const nshader_stage_metadata_compute_t* compute = &stage->metadata.compute;

  // Create SDL_GPUComputePipelineCreateInfo
  SDL_GPUComputePipelineCreateInfo create_info = {0};
  create_info.code_size = blob->size;
  create_info.code = blob->data;
  create_info.entrypoint = stage->entry_point ? stage->entry_point : "main";
  create_info.format = nshader_backend_to_sdl_format(backend);
  create_info.num_samplers = compute->num_samplers;
  create_info.num_readonly_storage_textures = compute->num_readonly_storage_textures;
  create_info.num_readonly_storage_buffers = compute->num_readonly_storage_buffers;
  create_info.num_readwrite_storage_textures = compute->num_readwrite_storage_textures;
  create_info.num_readwrite_storage_buffers = compute->num_readwrite_storage_buffers;
  create_info.num_uniform_buffers = compute->num_uniform_buffers;
  create_info.threadcount_x = compute->threadcount_x;
  create_info.threadcount_y = compute->threadcount_y;
  create_info.threadcount_z = compute->threadcount_z;
  create_info.props = 0;

  // Create the SDL GPU compute pipeline
  return SDL_CreateGPUComputePipeline(device, &create_info);
}

NSHADER_API void nshader_sdl3_gpu_release_compute_pipeline(
  SDL_GPUDevice* device,
  SDL_GPUComputePipeline* compute_pipeline)
{
  if (device && compute_pipeline) {
    SDL_ReleaseGPUComputePipeline(device, compute_pipeline);
  }
}
