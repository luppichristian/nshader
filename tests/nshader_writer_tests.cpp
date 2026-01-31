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

extern "C" {
#include <nshader/nshader_writer.h>
#include "nshader_compiler_tests.h"
}

TEST(NShaderWriterTests, WriteToMemory) {
  ASSERT_NE(g_graphics_shader, nullptr);

  // First, get the size needed
  size_t size_needed = nshader_write_to_memory(g_graphics_shader, nullptr, 0);
  EXPECT_GT(size_needed, 0u);

  // Allocate buffer and write
  void* buffer = malloc(size_needed);
  ASSERT_NE(buffer, nullptr);

  size_t written = nshader_write_to_memory(g_graphics_shader, buffer, size_needed);
  EXPECT_EQ(size_needed, written);

  free(buffer);
}

TEST(NShaderWriterTests, WriteToFile) {
  ASSERT_NE(g_graphics_shader, nullptr);

  FILE* file = fopen("test_shader.nsdr", "wb");
  ASSERT_NE(file, nullptr);

  bool success = nshader_write_to_file(g_graphics_shader, file);
  fclose(file);

  EXPECT_TRUE(success);

  // Clean up
  remove("test_shader.nsdr");
}

TEST(NShaderWriterTests, WriteToPath) {
  ASSERT_NE(g_graphics_shader, nullptr);

  bool success = nshader_write_to_path(g_graphics_shader, "test_shader_path.nsdr");
  EXPECT_TRUE(success);

  // Clean up
  remove("test_shader_path.nsdr");
}
