---
layout: default
title: nshader_sdl3_gpu.h
---

# nshader_sdl3_gpu.h

SDL3 GPU integration for creating shader and pipeline objects.

## Purpose

Convenience layer for SDL3 GPU API users. Automatically selects the correct backend based on device capabilities and creates ready-to-use GPU objects.

## Availability

Requires `#define NSHADER_SDL3_GPU` before including `<nshader.h>`, or the header can be included directly. Links against SDL3.

## API

```c
// Create GPU shader for specific stage
// Automatically selects backend (DXIL/DXBC/MSL/SPV) based on device
SDL_GPUShader* nshader_sdl3_gpu_create_shader(
    SDL_GPUDevice* device,
    const nshader_t* shader,
    nshader_stage_type_t stage_type);

// Create compute pipeline from compute shader
SDL_GPUComputePipeline* nshader_sdl3_gpu_create_compute_pipeline(
    SDL_GPUDevice* device,
    const nshader_t* shader);
```

Returns NULL if:
- No compatible backend available for the device
- Requested stage doesn't exist in shader
- Shader type mismatch (graphics shader for compute pipeline)

## Backend Selection

The functions query `SDL_GetGPUShaderFormats()` and select the first available backend in this priority order:
1. DXIL (DirectX 12)
2. DXBC (DirectX 11)
3. MSL (Metal)
4. SPV (Vulkan/OpenGL)

## Example

```c
nshader_t* shader = nshader_read_from_path("sprite.nshader");

SDL_GPUShader* vs = nshader_sdl3_gpu_create_shader(device, shader,
    NSHADER_STAGE_TYPE_VERTEX);
SDL_GPUShader* fs = nshader_sdl3_gpu_create_shader(device, shader,
    NSHADER_STAGE_TYPE_FRAGMENT);

// Use in pipeline creation
SDL_GPUGraphicsPipelineCreateInfo pipeline_info = {
    .vertex_shader = vs,
    .fragment_shader = fs,
    // ...
};

SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(device, &pipeline_info);

// Release shaders after pipeline creation
SDL_ReleaseGPUShader(device, vs);
SDL_ReleaseGPUShader(device, fs);
nshader_destroy(shader);
```

## Design Notes

- Metadata from `nshader_info_t` is used to populate `SDL_GPUShaderCreateInfo` (sampler counts, uniform buffer counts, etc.)
- The returned `SDL_GPUShader*` must be released with `SDL_ReleaseGPUShader()`
- Thread safety follows SDL3 GPU conventions
