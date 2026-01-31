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

#include "nshader_info.h"

// #############################################################################
NSHADER_HEADER_BEGIN;
// #############################################################################

// Opaque type defined internally
typedef struct nshader_t nshader_t;

typedef struct nshader_blob_t {
  size_t size;                
  const uint8_t* data;        
} nshader_blob_t;

// Info getters & utility functions
NSHADER_API const nshader_info_t* nshader_get_info(const nshader_t* shader);
NSHADER_API const nshader_blob_t* nshader_get_blob(const nshader_t* shader, nshader_stage_t stage, nshader_backend_t backend);
NSHADER_API bool nshader_has_backend(const nshader_t* shader, nshader_backend_t backend);
NSHADER_API bool nshader_has_stage(const nshader_t* shader, nshader_stage_type_t stage_type);

// #############################################################################
NSHADER_HEADER_END;
// #############################################################################