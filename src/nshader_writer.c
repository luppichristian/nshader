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


#include <nshader/nshader_writer.h>
#include "nshader_type_internal.h"
#include <nshader/nshader_base.h>
#include <string.h>

// Binary format constants
#define NSHADER_MAGIC 0x5244534E  // "NSDR" in little-endian
#define NSHADER_VERSION 1

// Endianness conversion helpers - always write as little-endian
static uint32_t to_le32(uint32_t val) {
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  return ((val & 0xFF000000) >> 24) |
         ((val & 0x00FF0000) >> 8)  |
         ((val & 0x0000FF00) << 8)  |
         ((val & 0x000000FF) << 24);
#else
  return val;  // Already little-endian
#endif
}

// Helper to write data to buffer with bounds checking
static bool write_data(const void* data, size_t size, void** buffer, size_t* remaining, size_t* written) {
  if (*buffer) {
    if (*remaining < size) {
      return false;
    }
    memcpy(*buffer, data, size);
    *buffer = (uint8_t*)*buffer + size;
    *remaining -= size;
  }
  *written += size;
  return true;
}

// Helper macros for writing primitives (with endianness conversion)
#define WRITE_U8(val) do { uint8_t v = (val); if (!write_data(&v, sizeof(v), &buffer, &remaining, &written)) return 0; } while(0)
#define WRITE_U32(val) do { uint32_t v = to_le32(val); if (!write_data(&v, sizeof(v), &buffer, &remaining, &written)) return 0; } while(0)
#define WRITE_BYTES(ptr, len) do { if (!write_data((ptr), (len), &buffer, &remaining, &written)) return 0; } while(0)

static bool write_string(const char* str, void** buffer, size_t* remaining, size_t* written) {
  uint32_t len = str ? (uint32_t)strlen(str) : 0;
  uint32_t len_le = to_le32(len);
  if (!write_data(&len_le, sizeof(len_le), buffer, remaining, written)) return false;
  if (len > 0) {
    if (!write_data(str, len, buffer, remaining, written)) return false;
  }
  return true;
}

static bool write_binding(const nshader_stage_binding_t* binding, void** buffer, size_t* remaining, size_t* written) {
  if (!write_string(binding->name, buffer, remaining, written)) return false;
  uint32_t loc = to_le32(binding->location);
  uint32_t vec = to_le32(binding->vector_size);
  if (!write_data(&loc, sizeof(loc), buffer, remaining, written)) return false;
  if (!write_data(&vec, sizeof(vec), buffer, remaining, written)) return false;
  if (!write_data(&binding->type, sizeof(binding->type), buffer, remaining, written)) return false;
  return true;
}

static bool write_stage_metadata(nshader_stage_type_t stage_type, const nshader_stage_metadata_t* metadata, void** buffer, size_t* remaining, size_t* written) {
  switch (stage_type) {
    case NSHADER_STAGE_TYPE_VERTEX: {
      const nshader_stage_metadata_vertex_t* vert = &metadata->vertex;
      uint32_t tmp;
      tmp = to_le32(vert->num_samplers); if (!write_data(&tmp, sizeof(tmp), buffer, remaining, written)) return false;
      tmp = to_le32(vert->num_storage_textures); if (!write_data(&tmp, sizeof(tmp), buffer, remaining, written)) return false;
      tmp = to_le32(vert->num_storage_buffers); if (!write_data(&tmp, sizeof(tmp), buffer, remaining, written)) return false;
      tmp = to_le32(vert->num_uniform_buffers); if (!write_data(&tmp, sizeof(tmp), buffer, remaining, written)) return false;
      tmp = to_le32((uint32_t)vert->input_count); if (!write_data(&tmp, sizeof(tmp), buffer, remaining, written)) return false;
      for (size_t i = 0; i < vert->input_count; i++) {
        if (!write_binding(&vert->inputs[i], buffer, remaining, written)) return false;
      }
      tmp = to_le32((uint32_t)vert->output_count); if (!write_data(&tmp, sizeof(tmp), buffer, remaining, written)) return false;
      for (size_t i = 0; i < vert->output_count; i++) {
        if (!write_binding(&vert->outputs[i], buffer, remaining, written)) return false;
      }
      break;
    }
    case NSHADER_STAGE_TYPE_FRAGMENT: {
      const nshader_stage_metadata_fragment_t* frag = &metadata->fragment;
      uint32_t tmp;
      tmp = to_le32(frag->num_samplers); if (!write_data(&tmp, sizeof(tmp), buffer, remaining, written)) return false;
      tmp = to_le32(frag->num_storage_textures); if (!write_data(&tmp, sizeof(tmp), buffer, remaining, written)) return false;
      tmp = to_le32(frag->num_storage_buffers); if (!write_data(&tmp, sizeof(tmp), buffer, remaining, written)) return false;
      tmp = to_le32(frag->num_uniform_buffers); if (!write_data(&tmp, sizeof(tmp), buffer, remaining, written)) return false;
      tmp = to_le32((uint32_t)frag->input_count); if (!write_data(&tmp, sizeof(tmp), buffer, remaining, written)) return false;
      for (size_t i = 0; i < frag->input_count; i++) {
        if (!write_binding(&frag->inputs[i], buffer, remaining, written)) return false;
      }
      tmp = to_le32((uint32_t)frag->output_count); if (!write_data(&tmp, sizeof(tmp), buffer, remaining, written)) return false;
      for (size_t i = 0; i < frag->output_count; i++) {
        if (!write_binding(&frag->outputs[i], buffer, remaining, written)) return false;
      }
      break;
    }
    case NSHADER_STAGE_TYPE_COMPUTE: {
      const nshader_stage_metadata_compute_t* comp = &metadata->compute;
      uint32_t tmp;
      tmp = to_le32(comp->num_samplers); if (!write_data(&tmp, sizeof(tmp), buffer, remaining, written)) return false;
      tmp = to_le32(comp->num_readonly_storage_textures); if (!write_data(&tmp, sizeof(tmp), buffer, remaining, written)) return false;
      tmp = to_le32(comp->num_readonly_storage_buffers); if (!write_data(&tmp, sizeof(tmp), buffer, remaining, written)) return false;
      tmp = to_le32(comp->num_readwrite_storage_textures); if (!write_data(&tmp, sizeof(tmp), buffer, remaining, written)) return false;
      tmp = to_le32(comp->num_readwrite_storage_buffers); if (!write_data(&tmp, sizeof(tmp), buffer, remaining, written)) return false;
      tmp = to_le32(comp->num_uniform_buffers); if (!write_data(&tmp, sizeof(tmp), buffer, remaining, written)) return false;
      tmp = to_le32(comp->threadcount_x); if (!write_data(&tmp, sizeof(tmp), buffer, remaining, written)) return false;
      tmp = to_le32(comp->threadcount_y); if (!write_data(&tmp, sizeof(tmp), buffer, remaining, written)) return false;
      tmp = to_le32(comp->threadcount_z); if (!write_data(&tmp, sizeof(tmp), buffer, remaining, written)) return false;
      break;
    }
    default:
      return false;
  }
  return true;
}

NSHADER_API size_t nshader_write_to_memory(const nshader_t* shader, void* buffer, size_t buffer_size) {
  if (!shader) {
    return 0;
  }

  size_t written = 0;
  size_t remaining = buffer_size;
  void* orig_buffer = buffer;

  // Write header
  uint32_t magic = NSHADER_MAGIC;
  WRITE_U32(magic);
  WRITE_U32(NSHADER_VERSION);

  // Write shader info
  const nshader_info_t* info = &shader->info;
  WRITE_U8(info->type);
  WRITE_U32((uint32_t)info->num_stages);

  // Write stages
  for (size_t i = 0; i < info->num_stages; i++) {
    const nshader_stage_t* stage = &info->stages[i];
    WRITE_U8(stage->type);
    if (!write_string(stage->entry_point, &buffer, &remaining, &written)) return 0;
    if (!write_stage_metadata(stage->type, &stage->metadata, &buffer, &remaining, &written)) return 0;
  }

  // Write backends
  WRITE_U32((uint32_t)info->num_backends);
  for (size_t i = 0; i < info->num_backends; i++) {
    WRITE_U8(info->backends[i]);
  }

  // Write blobs
  for (size_t stage_idx = 0; stage_idx < NSHADER_STAGE_TYPE_COUNT; stage_idx++) {
    for (size_t backend_idx = 0; backend_idx < NSHADER_BACKEND_COUNT; backend_idx++) {
      const nshader_blob_t* blob = shader->blobs[stage_idx][backend_idx];
      if (blob && blob->data && blob->size > 0) {
        WRITE_U8(1);  // Has blob
        WRITE_U32((uint32_t)blob->size);
        WRITE_BYTES(blob->data, blob->size);
      } else {
        WRITE_U8(0);  // No blob
      }
    }
  }

  return written;
}

NSHADER_API bool nshader_write_to_file(const nshader_t* shader, FILE* file) {
  if (!shader || !file) {
    return false;
  }

  // Calculate size needed
  size_t size = nshader_write_to_memory(shader, NULL, 0);
  if (size == 0) {
    return false;
  }

  // Allocate buffer
  void* buffer = nshader_malloc(size);
  if (!buffer) {
    return false;
  }

  // Write to buffer
  size_t written = nshader_write_to_memory(shader, buffer, size);
  if (written != size) {
    nshader_free(buffer);
    return false;
  }

  // Write to file
  size_t result = fwrite(buffer, 1, size, file);
  nshader_free(buffer);

  return result == size;
}

NSHADER_API bool nshader_write_to_path(const nshader_t* shader, const char* filepath) {
  if (!shader || !filepath) {
    return false;
  }

  FILE* file = fopen(filepath, "wb");
  if (!file) {
    return false;
  }

  bool result = nshader_write_to_file(shader, file);
  fclose(file);

  return result;
}