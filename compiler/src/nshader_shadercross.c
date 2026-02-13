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

#include "nshader_shadercross.h"
#include <nshader/nshader_base.h>
#include <string.h>

// Type mapping: SDL_shadercross -> nshader
NSHADER_API nshader_binding_type_t nshader_from_sdl_iovar_type(SDL_ShaderCross_IOVarType sdl_type) {
  switch (sdl_type) {
    case SDL_SHADERCROSS_IOVAR_TYPE_INT8:
      return NSHADER_BINDING_TYPE_INT8;
    case SDL_SHADERCROSS_IOVAR_TYPE_UINT8:
      return NSHADER_BINDING_TYPE_UINT8;
    case SDL_SHADERCROSS_IOVAR_TYPE_INT16:
      return NSHADER_BINDING_TYPE_INT16;
    case SDL_SHADERCROSS_IOVAR_TYPE_UINT16:
      return NSHADER_BINDING_TYPE_UINT16;
    case SDL_SHADERCROSS_IOVAR_TYPE_INT32:
      return NSHADER_BINDING_TYPE_INT32;
    case SDL_SHADERCROSS_IOVAR_TYPE_UINT32:
      return NSHADER_BINDING_TYPE_UINT32;
    case SDL_SHADERCROSS_IOVAR_TYPE_INT64:
      return NSHADER_BINDING_TYPE_INT64;
    case SDL_SHADERCROSS_IOVAR_TYPE_UINT64:
      return NSHADER_BINDING_TYPE_UINT64;
    case SDL_SHADERCROSS_IOVAR_TYPE_FLOAT16:
      return NSHADER_BINDING_TYPE_FLOAT16;
    case SDL_SHADERCROSS_IOVAR_TYPE_FLOAT32:
      return NSHADER_BINDING_TYPE_FLOAT32;
    case SDL_SHADERCROSS_IOVAR_TYPE_FLOAT64:
      return NSHADER_BINDING_TYPE_FLOAT64;
    default:
      return NSHADER_BINDING_TYPE_FLOAT32;  // Default fallback
  }
}

NSHADER_API nshader_stage_type_t nshader_from_sdl_shader_stage(SDL_ShaderCross_ShaderStage sdl_stage) {
  switch (sdl_stage) {
    case SDL_SHADERCROSS_SHADERSTAGE_VERTEX:
      return NSHADER_STAGE_TYPE_VERTEX;
    case SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT:
      return NSHADER_STAGE_TYPE_FRAGMENT;
    case SDL_SHADERCROSS_SHADERSTAGE_COMPUTE:
      return NSHADER_STAGE_TYPE_COMPUTE;
    default:
      return NSHADER_STAGE_TYPE_VERTEX;  // Default fallback
  }
}

// Type mapping: nshader -> SDL_shadercross
NSHADER_API SDL_ShaderCross_IOVarType nshader_to_sdl_iovar_type(nshader_binding_type_t nshader_type) {
  switch (nshader_type) {
    case NSHADER_BINDING_TYPE_INT8:
      return SDL_SHADERCROSS_IOVAR_TYPE_INT8;
    case NSHADER_BINDING_TYPE_UINT8:
      return SDL_SHADERCROSS_IOVAR_TYPE_UINT8;
    case NSHADER_BINDING_TYPE_INT16:
      return SDL_SHADERCROSS_IOVAR_TYPE_INT16;
    case NSHADER_BINDING_TYPE_UINT16:
      return SDL_SHADERCROSS_IOVAR_TYPE_UINT16;
    case NSHADER_BINDING_TYPE_INT32:
      return SDL_SHADERCROSS_IOVAR_TYPE_INT32;
    case NSHADER_BINDING_TYPE_UINT32:
      return SDL_SHADERCROSS_IOVAR_TYPE_UINT32;
    case NSHADER_BINDING_TYPE_INT64:
      return SDL_SHADERCROSS_IOVAR_TYPE_INT64;
    case NSHADER_BINDING_TYPE_UINT64:
      return SDL_SHADERCROSS_IOVAR_TYPE_UINT64;
    case NSHADER_BINDING_TYPE_FLOAT16:
      return SDL_SHADERCROSS_IOVAR_TYPE_FLOAT16;
    case NSHADER_BINDING_TYPE_FLOAT32:
      return SDL_SHADERCROSS_IOVAR_TYPE_FLOAT32;
    case NSHADER_BINDING_TYPE_FLOAT64:
      return SDL_SHADERCROSS_IOVAR_TYPE_FLOAT64;
    default:
      return SDL_SHADERCROSS_IOVAR_TYPE_FLOAT32;  // Default fallback
  }
}

NSHADER_API SDL_ShaderCross_ShaderStage nshader_to_sdl_shader_stage(nshader_stage_type_t nshader_stage) {
  switch (nshader_stage) {
    case NSHADER_STAGE_TYPE_VERTEX:
      return SDL_SHADERCROSS_SHADERSTAGE_VERTEX;
    case NSHADER_STAGE_TYPE_FRAGMENT:
      return SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT;
    case NSHADER_STAGE_TYPE_COMPUTE:
      return SDL_SHADERCROSS_SHADERSTAGE_COMPUTE;
    default:
      return SDL_SHADERCROSS_SHADERSTAGE_VERTEX;  // Default fallback
  }
}

static char* duplicate_string(const char* str) {
  if (!str) {
    return NULL;
  }
  size_t len = strlen(str);
  char* copy = (char*)nshader_malloc(len + 1);
  if (copy) {
    memcpy(copy, str, len + 1);
  }
  return copy;
}

static bool convert_bindings(
  const SDL_ShaderCross_IOVarMetadata* sdl_bindings,
  uint32_t count,
  nshader_stage_binding_t** out_bindings,
  size_t* out_count) {

  if (count == 0) {
    *out_bindings = NULL;
    *out_count = 0;
    return true;
  }

  nshader_stage_binding_t* bindings = (nshader_stage_binding_t*)nshader_calloc(count, sizeof(nshader_stage_binding_t));
  if (!bindings) {
    return false;
  }

  for (uint32_t i = 0; i < count; i++) {
    bindings[i].name = duplicate_string(sdl_bindings[i].name);
    if (!bindings[i].name && sdl_bindings[i].name) {
      // Cleanup on error
      for (uint32_t j = 0; j < i; j++) {
        nshader_free(bindings[j].name);
      }
      nshader_free(bindings);
      return false;
    }
    bindings[i].location = sdl_bindings[i].location;
    bindings[i].vector_size = sdl_bindings[i].vector_size;
    bindings[i].type = nshader_from_sdl_iovar_type(sdl_bindings[i].vector_type);
  }

  *out_bindings = bindings;
  *out_count = count;
  return true;
}

// Convert SDL_shadercross graphics shader metadata to nshader stage metadata
NSHADER_API bool nshader_from_sdl_graphics_metadata(
  const SDL_ShaderCross_GraphicsShaderMetadata* sdl_metadata,
  nshader_stage_type_t stage_type,
  nshader_stage_metadata_t* nshader_metadata) {

  if (!sdl_metadata || !nshader_metadata) {
    return false;
  }

  memset(nshader_metadata, 0, sizeof(nshader_stage_metadata_t));

  switch (stage_type) {
    case NSHADER_STAGE_TYPE_VERTEX: {
      nshader_stage_metadata_vertex_t* vert = &nshader_metadata->vertex;
      vert->num_samplers = sdl_metadata->resource_info.num_samplers;
      vert->num_storage_textures = sdl_metadata->resource_info.num_storage_textures;
      vert->num_storage_buffers = sdl_metadata->resource_info.num_storage_buffers;
      vert->num_uniform_buffers = sdl_metadata->resource_info.num_uniform_buffers;

      if (!convert_bindings(sdl_metadata->inputs, sdl_metadata->num_inputs,
                           &vert->inputs, &vert->input_count)) {
        return false;
      }

      if (!convert_bindings(sdl_metadata->outputs, sdl_metadata->num_outputs,
                           &vert->outputs, &vert->output_count)) {
        // Cleanup inputs on error
        for (size_t i = 0; i < vert->input_count; i++) {
          nshader_free(vert->inputs[i].name);
        }
        nshader_free(vert->inputs);
        return false;
      }
      break;
    }
    case NSHADER_STAGE_TYPE_FRAGMENT: {
      nshader_stage_metadata_fragment_t* frag = &nshader_metadata->fragment;
      frag->num_samplers = sdl_metadata->resource_info.num_samplers;
      frag->num_storage_textures = sdl_metadata->resource_info.num_storage_textures;
      frag->num_storage_buffers = sdl_metadata->resource_info.num_storage_buffers;
      frag->num_uniform_buffers = sdl_metadata->resource_info.num_uniform_buffers;

      if (!convert_bindings(sdl_metadata->inputs, sdl_metadata->num_inputs,
                           &frag->inputs, &frag->input_count)) {
        return false;
      }

      if (!convert_bindings(sdl_metadata->outputs, sdl_metadata->num_outputs,
                           &frag->outputs, &frag->output_count)) {
        // Cleanup inputs on error
        for (size_t i = 0; i < frag->input_count; i++) {
          nshader_free(frag->inputs[i].name);
        }
        nshader_free(frag->inputs);
        return false;
      }
      break;
    }
    default:
      return false;
  }

  return true;
}

// Convert SDL_shadercross compute metadata to nshader stage metadata
NSHADER_API bool nshader_from_sdl_compute_metadata(
  const SDL_ShaderCross_ComputePipelineMetadata* sdl_metadata,
  nshader_stage_metadata_t* nshader_metadata) {

  if (!sdl_metadata || !nshader_metadata) {
    return false;
  }

  memset(nshader_metadata, 0, sizeof(nshader_stage_metadata_t));

  nshader_stage_metadata_compute_t* comp = &nshader_metadata->compute;
  comp->num_samplers = sdl_metadata->num_samplers;
  comp->num_readonly_storage_textures = sdl_metadata->num_readonly_storage_textures;
  comp->num_readonly_storage_buffers = sdl_metadata->num_readonly_storage_buffers;
  comp->num_readwrite_storage_textures = sdl_metadata->num_readwrite_storage_textures;
  comp->num_readwrite_storage_buffers = sdl_metadata->num_readwrite_storage_buffers;
  comp->num_uniform_buffers = sdl_metadata->num_uniform_buffers;
  comp->threadcount_x = sdl_metadata->threadcount_x;
  comp->threadcount_y = sdl_metadata->threadcount_y;
  comp->threadcount_z = sdl_metadata->threadcount_z;

  return true;
}
