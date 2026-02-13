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

#include <nshader/nshader_info.h>
#include <SDL3_shadercross/SDL_shadercross.h>

// #############################################################################
NSHADER_HEADER_BEGIN;
// #############################################################################

// Type mapping: SDL_shadercross -> nshader
NSHADER_API nshader_binding_type_t nshader_from_sdl_iovar_type(SDL_ShaderCross_IOVarType sdl_type);
NSHADER_API nshader_stage_type_t nshader_from_sdl_shader_stage(SDL_ShaderCross_ShaderStage sdl_stage);

// Type mapping: nshader -> SDL_shadercross
NSHADER_API SDL_ShaderCross_IOVarType nshader_to_sdl_iovar_type(nshader_binding_type_t nshader_type);
NSHADER_API SDL_ShaderCross_ShaderStage nshader_to_sdl_shader_stage(nshader_stage_type_t nshader_stage);

// Convert SDL_shadercross graphics shader metadata to nshader stage metadata
NSHADER_API bool nshader_from_sdl_graphics_metadata(
  const SDL_ShaderCross_GraphicsShaderMetadata* sdl_metadata,
  nshader_stage_type_t stage_type,
  nshader_stage_metadata_t* nshader_metadata);

// Convert SDL_shadercross compute metadata to nshader stage metadata
NSHADER_API bool nshader_from_sdl_compute_metadata(
  const SDL_ShaderCross_ComputePipelineMetadata* sdl_metadata,
  nshader_stage_metadata_t* nshader_metadata);

// #############################################################################
NSHADER_HEADER_END;
// #############################################################################
