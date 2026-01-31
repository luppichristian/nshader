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

#include <nshader/nshader_reader.h>
#include "nshader_type_internal.h"
#include <nshader/nshader_base.h>
#include <string.h>

// Endianness conversion helpers - always read as little-endian
static uint32_t from_le32(uint32_t val) {
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  return ((val & 0xFF000000) >> 24) |
         ((val & 0x00FF0000) >> 8)  |
         ((val & 0x0000FF00) << 8)  |
         ((val & 0x000000FF) << 24);
#else
  return val;  // Already little-endian
#endif
}

// Helper to read data from buffer with bounds checking
static bool read_data(void* data, size_t size, const void** buffer, size_t* remaining) {
  if (*remaining < size) {
    return false;
  }
  memcpy(data, *buffer, size);
  *buffer = (const uint8_t*)*buffer + size;
  *remaining -= size;
  return true;
}

// Helper macros for reading primitives (with endianness conversion)
#define READ_U8(val) do { if (!read_data(&(val), sizeof(val), &buffer, &remaining)) goto error; } while(0)
#define READ_U32(val) do { uint32_t tmp; if (!read_data(&tmp, sizeof(tmp), &buffer, &remaining)) goto error; (val) = from_le32(tmp); } while(0)
#define READ_BYTES(ptr, len) do { if (!read_data((ptr), (len), &buffer, &remaining)) goto error; } while(0)

static char* read_string(const void** buffer, size_t* remaining) {
  uint32_t len_le;
  if (!read_data(&len_le, sizeof(len_le), buffer, remaining)) return NULL;
  uint32_t len = from_le32(len_le);

  if (len == 0) {
    return NULL;
  }

  char* str = (char*)nshader_malloc(len + 1);
  if (!str) {
    return NULL;
  }

  if (!read_data(str, len, buffer, remaining)) {
    nshader_free(str);
    return NULL;
  }
  str[len] = '\0';
  return str;
}

static bool read_binding(nshader_stage_binding_t* binding, const void** buffer, size_t* remaining) {
  uint32_t tmp;
  binding->name = read_string(buffer, remaining);
  if (!read_data(&tmp, sizeof(tmp), buffer, remaining)) goto error;
  binding->location = from_le32(tmp);
  if (!read_data(&tmp, sizeof(tmp), buffer, remaining)) goto error;
  binding->vector_size = from_le32(tmp);
  if (!read_data(&binding->type, sizeof(binding->type), buffer, remaining)) goto error;
  return true;

error:
  if (binding->name) {
    nshader_free(binding->name);
    binding->name = NULL;
  }
  return false;
}

static bool read_stage_metadata(nshader_stage_type_t stage_type, nshader_stage_metadata_t* metadata, const void** buffer, size_t* remaining) {
  uint32_t tmp;
  switch (stage_type) {
    case NSHADER_STAGE_TYPE_VERTEX: {
      nshader_stage_metadata_vertex_t* vert = &metadata->vertex;
      uint32_t input_count, output_count;
      if (!read_data(&tmp, sizeof(tmp), buffer, remaining)) goto error; vert->num_samplers = from_le32(tmp);
      if (!read_data(&tmp, sizeof(tmp), buffer, remaining)) goto error; vert->num_storage_textures = from_le32(tmp);
      if (!read_data(&tmp, sizeof(tmp), buffer, remaining)) goto error; vert->num_storage_buffers = from_le32(tmp);
      if (!read_data(&tmp, sizeof(tmp), buffer, remaining)) goto error; vert->num_uniform_buffers = from_le32(tmp);
      if (!read_data(&tmp, sizeof(tmp), buffer, remaining)) goto error; input_count = from_le32(tmp);
      vert->input_count = input_count;

      if (vert->input_count > 0) {
        vert->inputs = (nshader_stage_binding_t*)nshader_calloc(vert->input_count, sizeof(nshader_stage_binding_t));
        if (!vert->inputs) goto error;
        for (size_t i = 0; i < vert->input_count; i++) {
          if (!read_binding(&vert->inputs[i], buffer, remaining)) goto error;
        }
      }

      if (!read_data(&tmp, sizeof(tmp), buffer, remaining)) goto error; output_count = from_le32(tmp);
      vert->output_count = output_count;
      if (vert->output_count > 0) {
        vert->outputs = (nshader_stage_binding_t*)nshader_calloc(vert->output_count, sizeof(nshader_stage_binding_t));
        if (!vert->outputs) goto error;
        for (size_t i = 0; i < vert->output_count; i++) {
          if (!read_binding(&vert->outputs[i], buffer, remaining)) goto error;
        }
      }
      break;
    }
    case NSHADER_STAGE_TYPE_FRAGMENT: {
      nshader_stage_metadata_fragment_t* frag = &metadata->fragment;
      uint32_t input_count, output_count;
      if (!read_data(&tmp, sizeof(tmp), buffer, remaining)) goto error; frag->num_samplers = from_le32(tmp);
      if (!read_data(&tmp, sizeof(tmp), buffer, remaining)) goto error; frag->num_storage_textures = from_le32(tmp);
      if (!read_data(&tmp, sizeof(tmp), buffer, remaining)) goto error; frag->num_storage_buffers = from_le32(tmp);
      if (!read_data(&tmp, sizeof(tmp), buffer, remaining)) goto error; frag->num_uniform_buffers = from_le32(tmp);
      if (!read_data(&tmp, sizeof(tmp), buffer, remaining)) goto error; input_count = from_le32(tmp);
      frag->input_count = input_count;

      if (frag->input_count > 0) {
        frag->inputs = (nshader_stage_binding_t*)nshader_calloc(frag->input_count, sizeof(nshader_stage_binding_t));
        if (!frag->inputs) goto error;
        for (size_t i = 0; i < frag->input_count; i++) {
          if (!read_binding(&frag->inputs[i], buffer, remaining)) goto error;
        }
      }

      if (!read_data(&tmp, sizeof(tmp), buffer, remaining)) goto error; output_count = from_le32(tmp);
      frag->output_count = output_count;
      if (frag->output_count > 0) {
        frag->outputs = (nshader_stage_binding_t*)nshader_calloc(frag->output_count, sizeof(nshader_stage_binding_t));
        if (!frag->outputs) goto error;
        for (size_t i = 0; i < frag->output_count; i++) {
          if (!read_binding(&frag->outputs[i], buffer, remaining)) goto error;
        }
      }
      break;
    }
    case NSHADER_STAGE_TYPE_COMPUTE: {
      nshader_stage_metadata_compute_t* comp = &metadata->compute;
      if (!read_data(&tmp, sizeof(tmp), buffer, remaining)) goto error; comp->num_samplers = from_le32(tmp);
      if (!read_data(&tmp, sizeof(tmp), buffer, remaining)) goto error; comp->num_readonly_storage_textures = from_le32(tmp);
      if (!read_data(&tmp, sizeof(tmp), buffer, remaining)) goto error; comp->num_readonly_storage_buffers = from_le32(tmp);
      if (!read_data(&tmp, sizeof(tmp), buffer, remaining)) goto error; comp->num_readwrite_storage_textures = from_le32(tmp);
      if (!read_data(&tmp, sizeof(tmp), buffer, remaining)) goto error; comp->num_readwrite_storage_buffers = from_le32(tmp);
      if (!read_data(&tmp, sizeof(tmp), buffer, remaining)) goto error; comp->num_uniform_buffers = from_le32(tmp);
      if (!read_data(&tmp, sizeof(tmp), buffer, remaining)) goto error; comp->threadcount_x = from_le32(tmp);
      if (!read_data(&tmp, sizeof(tmp), buffer, remaining)) goto error; comp->threadcount_y = from_le32(tmp);
      if (!read_data(&tmp, sizeof(tmp), buffer, remaining)) goto error; comp->threadcount_z = from_le32(tmp);
      break;
    }
    default:
      goto error;
  }
  return true;

error:
  return false;
}

NSHADER_API nshader_t* nshader_read_from_memory(const void* buffer, size_t buffer_size) {
  if (!buffer || buffer_size == 0) {
    return NULL;
  }

  size_t remaining = buffer_size;
  nshader_t* shader = NULL;

  // Read and validate header
  uint32_t magic, version;
  READ_U32(magic);
  if (magic != NSHADER_MAGIC) {
    goto error;
  }
  READ_U32(version);
  if (version != NSHADER_VERSION) {
    goto error;
  }

  // Allocate shader
  shader = (nshader_t*)nshader_calloc(1, sizeof(nshader_t));
  if (!shader) {
    goto error;
  }

  // Read shader info
  nshader_info_t* info = &shader->info;
  uint32_t num_stages, num_backends;
  READ_U8(info->type);
  READ_U32(num_stages);
  info->num_stages = num_stages;

  // Read stages
  if (info->num_stages > 0) {
    info->stages = (nshader_stage_t*)nshader_calloc(info->num_stages, sizeof(nshader_stage_t));
    if (!info->stages) goto error;

    for (size_t i = 0; i < info->num_stages; i++) {
      nshader_stage_t* stage = &info->stages[i];
      READ_U8(stage->type);

      char* entry_point = read_string(&buffer, &remaining);
      if (!entry_point) goto error;
      stage->entry_point = entry_point;

      if (!read_stage_metadata(stage->type, &stage->metadata, &buffer, &remaining)) {
        goto error;
      }
    }
  }

  // Read backends
  READ_U32(num_backends);
  info->num_backends = num_backends;
  if (info->num_backends > 0) {
    info->backends = (nshader_backend_t*)nshader_calloc(info->num_backends, sizeof(nshader_backend_t));
    if (!info->backends) goto error;

    for (size_t i = 0; i < info->num_backends; i++) {
      READ_U8(info->backends[i]);
    }
  }

  // Read blobs
  for (size_t stage_idx = 0; stage_idx < NSHADER_STAGE_TYPE_COUNT; stage_idx++) {
    for (size_t backend_idx = 0; backend_idx < NSHADER_BACKEND_COUNT; backend_idx++) {
      uint8_t has_blob;
      READ_U8(has_blob);

      if (has_blob) {
        nshader_blob_t* blob = (nshader_blob_t*)nshader_malloc(sizeof(nshader_blob_t));
        if (!blob) goto error;

        uint32_t blob_size;
        READ_U32(blob_size);
        blob->size = blob_size;

        uint8_t* data = (uint8_t*)nshader_malloc(blob->size);
        if (!data) {
          nshader_free(blob);
          goto error;
        }

        READ_BYTES(data, blob->size);
        blob->data = data;
        shader->blobs[stage_idx][backend_idx] = blob;
      }
    }
  }

  return shader;

error:
  if (shader) {
    nshader_destroy(shader);
  }
  return NULL;
}

NSHADER_API nshader_t* nshader_read_from_file(FILE* file) {
  if (!file) {
    return NULL;
  }

  // Get file size
  long start_pos = ftell(file);
  if (fseek(file, 0, SEEK_END) != 0) {
    return NULL;
  }
  long end_pos = ftell(file);
  if (end_pos < 0 || fseek(file, start_pos, SEEK_SET) != 0) {
    return NULL;
  }
  size_t size = (size_t)(end_pos - start_pos);

  // Read file into buffer
  void* buffer = nshader_malloc(size);
  if (!buffer) {
    return NULL;
  }

  size_t read_size = fread(buffer, 1, size, file);
  if (read_size != size) {
    nshader_free(buffer);
    return NULL;
  }

  // Parse from memory
  nshader_t* shader = nshader_read_from_memory(buffer, size);
  nshader_free(buffer);

  return shader;
}

NSHADER_API nshader_t* nshader_read_from_path(const char* filepath) {
  if (!filepath) {
    return NULL;
  }

  FILE* file = fopen(filepath, "rb");
  if (!file) {
    return NULL;
  }

  nshader_t* shader = nshader_read_from_file(file);
  fclose(file);

  return shader;
}

static void free_stage_metadata(nshader_stage_type_t stage_type, nshader_stage_metadata_t* metadata) {
  switch (stage_type) {
    case NSHADER_STAGE_TYPE_VERTEX: {
      nshader_stage_metadata_vertex_t* vert = &metadata->vertex;
      if (vert->inputs) {
        for (size_t i = 0; i < vert->input_count; i++) {
          nshader_free(vert->inputs[i].name);
        }
        nshader_free(vert->inputs);
      }
      if (vert->outputs) {
        for (size_t i = 0; i < vert->output_count; i++) {
          nshader_free(vert->outputs[i].name);
        }
        nshader_free(vert->outputs);
      }
      break;
    }
    case NSHADER_STAGE_TYPE_FRAGMENT: {
      nshader_stage_metadata_fragment_t* frag = &metadata->fragment;
      if (frag->inputs) {
        for (size_t i = 0; i < frag->input_count; i++) {
          nshader_free(frag->inputs[i].name);
        }
        nshader_free(frag->inputs);
      }
      if (frag->outputs) {
        for (size_t i = 0; i < frag->output_count; i++) {
          nshader_free(frag->outputs[i].name);
        }
        nshader_free(frag->outputs);
      }
      break;
    }
    case NSHADER_STAGE_TYPE_COMPUTE:
      // No dynamic allocations for compute metadata
      break;
    default:
      break;
  }
}

NSHADER_API void nshader_destroy(nshader_t* shader) {
  if (!shader) {
    return;
  }

  nshader_info_t* info = &shader->info;

  // Free stages
  if (info->stages) {
    for (size_t i = 0; i < info->num_stages; i++) {
      nshader_stage_t* stage = &info->stages[i];
      nshader_free((void*)stage->entry_point);
      free_stage_metadata(stage->type, &stage->metadata);
    }
    nshader_free(info->stages);
  }

  // Free backends
  if (info->backends) {
    nshader_free(info->backends);
  }

  // Free blobs
  for (size_t stage_idx = 0; stage_idx < NSHADER_STAGE_TYPE_COUNT; stage_idx++) {
    for (size_t backend_idx = 0; backend_idx < NSHADER_BACKEND_COUNT; backend_idx++) {
      nshader_blob_t* blob = shader->blobs[stage_idx][backend_idx];
      if (blob) {
        nshader_free((void*)blob->data);
        nshader_free(blob);
      }
    }
  }

  nshader_free(shader);
}