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

// Write nshader to memory buffer
// Returns size written on success, 0 on failure
// If buffer is NULL, only calculates size needed
NSHADER_API size_t nshader_write_to_memory(const nshader_t* shader, void* buffer, size_t buffer_size);

// Write nshader to FILE*
// Returns true on success, false on failure
NSHADER_API bool nshader_write_to_file(const nshader_t* shader, FILE* file);

// Write nshader to filepath
// Returns true on success, false on failure
NSHADER_API bool nshader_write_to_path(const nshader_t* shader, const char* filepath);

// #############################################################################
NSHADER_HEADER_END;
// #############################################################################