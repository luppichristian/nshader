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

#include <gtest/gtest.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
#include <nshader/nshader_reader.h>
#include <nshader/nshader_writer.h>
#include "nshader_compiler_tests.h"
}

TEST(NShaderReaderTests, ReadFromMemory) {
  ASSERT_NE(g_graphics_shader, nullptr);

  // Write to memory first
  size_t size_needed = nshader_write_to_memory(g_graphics_shader, nullptr, 0);
  void* buffer = malloc(size_needed);
  nshader_write_to_memory(g_graphics_shader, buffer, size_needed);

  // Read back from memory
  nshader_t* read_shader = nshader_read_from_memory(buffer, size_needed);
  ASSERT_NE(read_shader, nullptr);

  // Verify it matches
  const nshader_info_t* original_info = nshader_get_info(g_graphics_shader);
  const nshader_info_t* read_info = nshader_get_info(read_shader);

  EXPECT_EQ(original_info->type, read_info->type);
  EXPECT_EQ(original_info->num_stages, read_info->num_stages);
  EXPECT_EQ(original_info->num_backends, read_info->num_backends);

  nshader_destroy(read_shader);
  free(buffer);
}

TEST(NShaderReaderTests, ReadFromFile) {
  ASSERT_NE(g_graphics_shader, nullptr);

  // Write to file first
  nshader_write_to_path(g_graphics_shader, "test_read.nsdr");

  // Read back from file
  FILE* file = fopen("test_read.nsdr", "rb");
  ASSERT_NE(file, nullptr);

  nshader_t* read_shader = nshader_read_from_file(file);
  fclose(file);

  ASSERT_NE(read_shader, nullptr);

  nshader_destroy(read_shader);
  remove("test_read.nsdr");
}

TEST(NShaderReaderTests, ReadFromPath) {
  ASSERT_NE(g_graphics_shader, nullptr);

  // Write to file first
  nshader_write_to_path(g_graphics_shader, "test_read_path.nsdr");

  // Read back from path
  nshader_t* read_shader = nshader_read_from_path("test_read_path.nsdr");
  ASSERT_NE(read_shader, nullptr);

  nshader_destroy(read_shader);
  remove("test_read_path.nsdr");
}

TEST(NShaderReaderTests, ReadInvalidData) {
  const char* invalid_data = "This is not a valid nshader file!";
  nshader_t* shader = nshader_read_from_memory(invalid_data, strlen(invalid_data));
  EXPECT_EQ(shader, nullptr);
}

TEST(NShaderReaderTests, RoundtripMemory) {
  ASSERT_NE(g_compute_shader, nullptr);

  // Write to memory
  size_t size1 = nshader_write_to_memory(g_compute_shader, nullptr, 0);
  void* buffer1 = malloc(size1);
  nshader_write_to_memory(g_compute_shader, buffer1, size1);

  // Read back
  nshader_t* shader1 = nshader_read_from_memory(buffer1, size1);
  ASSERT_NE(shader1, nullptr);

  // Write again
  size_t size2 = nshader_write_to_memory(shader1, nullptr, 0);
  void* buffer2 = malloc(size2);
  nshader_write_to_memory(shader1, buffer2, size2);

  // Buffers should be identical
  EXPECT_EQ(size1, size2);
  EXPECT_EQ(0, memcmp(buffer1, buffer2, size1));

  nshader_destroy(shader1);
  free(buffer1);
  free(buffer2);
}

TEST(NShaderReaderTests, RoundtripFile) {
  ASSERT_NE(g_compute_shader, nullptr);

  const char* filename = "test_roundtrip.nsdr";

  // Write
  nshader_write_to_path(g_compute_shader, filename);

  // Read
  nshader_t* shader = nshader_read_from_path(filename);
  ASSERT_NE(shader, nullptr);

  // Verify metadata matches
  const nshader_info_t* original_info = nshader_get_info(g_compute_shader);
  const nshader_info_t* read_info = nshader_get_info(shader);

  EXPECT_EQ(original_info->type, read_info->type);
  EXPECT_EQ(original_info->num_stages, read_info->num_stages);

  nshader_destroy(shader);
  remove(filename);
}
