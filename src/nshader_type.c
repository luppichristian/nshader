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

#include "nshader_type_internal.h"

NSHADER_API const nshader_info_t* nshader_get_info(const nshader_t* shader) {
  return shader ? &shader->info : NULL;
}

NSHADER_API const nshader_blob_t* nshader_get_blob(const nshader_t* shader, nshader_stage_t stage, nshader_backend_t backend) {
  if(!shader) {
    return NULL;
  }

  if(stage.type >= NSHADER_STAGE_TYPE_COUNT || backend >= NSHADER_BACKEND_COUNT) {
    return NULL;
  }

  return shader->blobs[stage.type][backend];
}

NSHADER_API bool nshader_has_backend(const nshader_t* shader, nshader_backend_t backend) {
  if(!shader) {
    return false;
  }

  const nshader_info_t* info = &shader->info;
  for (size_t i = 0; i < info->num_backends; ++i) {
    if (info->backends[i] == backend) {
      return true;
    }
  }

  return false;
}

NSHADER_API bool nshader_has_stage(const nshader_t* shader, nshader_stage_type_t stage_type) {
  if(!shader) {
    return false;
  }
  
  const nshader_info_t* info = &shader->info;
  for (size_t i = 0; i < info->num_stages; ++i) {
    if (info->stages[i].type == stage_type) {
      return true;
    }
  }
  
  return false;
}