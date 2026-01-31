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

#include "nshader_type.h"

// #############################################################################
NSHADER_HEADER_BEGIN;
// #############################################################################

typedef struct nshader_compiler_define_t {
  const char* name;   // Define name
  const char* value;  // Define value (can be NULL for simple defines)
} nshader_compiler_define_t;

typedef struct nshader_compiler_stage_setup_t {
  // Stage type
  nshader_stage_type_t stage_type;

  // Entry point function name and source code
  const char* entry_point;
  const char* source_code;

  // Defines specific to this stage
  const nshader_compiler_define_t* defines;
  size_t num_defines;
} nshader_compiler_stage_setup_t;

typedef struct nshader_compiler_config_t {
  // Configure source code for shader stages
  const nshader_compiler_stage_setup_t* stages;
  size_t num_stages;

  // Optional include directory for shader code (can be NULL)
  // Unfortunately, SDL_shadercross only supports a single include directory
  // This only works for HLSL at the moment, ...
  const char* include_dir;

  // Disable different backends (or all enabled by default)
  bool disable_dxil : 1;  // Disable DirectX Intermediate Language backend
  bool disable_dxbc : 1;  // Disable DirectX Bytecode backend
  bool disable_msl : 1;   // Disable Metal Shading Language backend
  bool disable_spv : 1;   // Disable SPIR-V backend

  // Other compilation options
  bool enable_debug;              // Enable debug info
  const char* debug_name;         // Debug name (can be NULL)
  bool preserve_unused_bindings;  // Don't cull unused resources

  // Array of preprocessor defines (applied on all stages)
  const nshader_compiler_define_t* defines;
  size_t num_defines;
} nshader_compiler_config_t;

// #############################################################################

typedef struct nshader_error_list_t {
  const char** errors;  // Array of error message strings
  size_t num_errors;    // Number of errors in the array
} nshader_error_list_t;

NSHADER_API void nshader_error_list_push(nshader_error_list_t* error_list, const char* msg);
NSHADER_API void nshader_error_list_free(nshader_error_list_t* error_list);

// #############################################################################

// Compile HLSL source to an nshader_t object
// Compiles to all requested backends and extracts reflection metadata
// Returns nshader_t* on success, NULL on failure
NSHADER_API nshader_t* nshader_compiler_compile_hlsl(
    const nshader_compiler_config_t* config,
    nshader_error_list_t* out_errors);  // Optional

// #############################################################################
NSHADER_HEADER_END;
// #############################################################################