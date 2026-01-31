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

#pragma once

#include <nshader/nshader_type.h>
#include <SDL3/SDL_gpu.h>

// #############################################################################
NSHADER_HEADER_BEGIN;
// #############################################################################

// Creates an SDL_GPUShader from an nshader for a specific stage.
// Automatically selects the appropriate shader backend (DXIL, DXBC, MSL, SPV)
// based on the device's supported shader formats.
// Returns SDL_GPUShader* on success, NULL on failure.
// Caller must release the shader using nshader_sdl3_gpu_release_shader().
// The nshader must contain a blob for the requested stage and a compatible backend.
NSHADER_API SDL_GPUShader* nshader_sdl3_gpu_create_shader(
  SDL_GPUDevice* device,
  const nshader_t* shader,
  nshader_stage_type_t stage_type);

// Creates an SDL_GPUComputePipeline from an nshader.
// Automatically selects the appropriate shader backend (DXIL, DXBC, MSL, SPV)
// based on the device's supported shader formats.
// Returns SDL_GPUComputePipeline* on success, NULL on failure.
// Caller must release the pipeline using nshader_sdl3_gpu_release_compute_pipeline().
// The nshader must be a compute shader and contain a blob for a compatible backend.
NSHADER_API SDL_GPUComputePipeline* nshader_sdl3_gpu_create_compute_pipeline(
  SDL_GPUDevice* device,
  const nshader_t* shader);

// #############################################################################
NSHADER_HEADER_END;
// #############################################################################
