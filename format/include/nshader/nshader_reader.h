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

// Read nshader from memory buffer
// Returns nshader_t* on success, NULL on failure
// Caller must free returned shader with nshader_destroy()
NSHADER_API nshader_t* nshader_read_from_memory(const void* buffer, size_t buffer_size);

// Read nshader from FILE*
// Returns nshader_t* on success, NULL on failure
// Caller must free returned shader with nshader_destroy()
NSHADER_API nshader_t* nshader_read_from_file(FILE* file);

// Read nshader from filepath
// Returns nshader_t* on success, NULL on failure
// Caller must free returned shader with nshader_destroy()
NSHADER_API nshader_t* nshader_read_from_path(const char* filepath);

// Destroy nshader and free all associated memory
NSHADER_API void nshader_destroy(nshader_t* shader);

// #############################################################################
NSHADER_HEADER_END;
// #############################################################################