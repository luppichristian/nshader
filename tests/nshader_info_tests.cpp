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

extern "C" {
#include <nshader/nshader_type.h>
#include "nshader_compiler_tests.h"
}

TEST(NShaderInfoTests, GetShaderInfo) {
  ASSERT_NE(g_graphics_shader, nullptr);

  const nshader_info_t* info = nshader_get_info(g_graphics_shader);
  ASSERT_NE(info, nullptr);
  EXPECT_EQ(NSHADER_SHADER_TYPE_GRAPHICS, info->type);
  EXPECT_EQ(2u, info->num_stages);
  EXPECT_GT(info->num_backends, 0u);
}

TEST(NShaderInfoTests, HasBackend) {
  ASSERT_NE(g_graphics_shader, nullptr);

  // At least one backend should be available
  bool has_any_backend = false;
  for (int i = 0; i < NSHADER_BACKEND_COUNT; i++) {
    if (nshader_has_backend(g_graphics_shader, (nshader_backend_t)i)) {
      has_any_backend = true;
      break;
    }
  }
  EXPECT_TRUE(has_any_backend);
}

TEST(NShaderInfoTests, HasStage) {
  ASSERT_NE(g_graphics_shader, nullptr);

  EXPECT_TRUE(nshader_has_stage(g_graphics_shader, NSHADER_STAGE_TYPE_VERTEX));
  EXPECT_TRUE(nshader_has_stage(g_graphics_shader, NSHADER_STAGE_TYPE_FRAGMENT));
  EXPECT_FALSE(nshader_has_stage(g_graphics_shader, NSHADER_STAGE_TYPE_COMPUTE));
}

TEST(NShaderInfoTests, GetBlob) {
  ASSERT_NE(g_graphics_shader, nullptr);

  // Find an available backend
  nshader_backend_t available_backend = NSHADER_BACKEND_COUNT;
  for (int i = 0; i < NSHADER_BACKEND_COUNT; i++) {
    if (nshader_has_backend(g_graphics_shader, (nshader_backend_t)i)) {
      available_backend = (nshader_backend_t)(i);
      break;
    }
  }

  if (available_backend != NSHADER_BACKEND_COUNT) {
    const nshader_blob_t* blob = nshader_get_blob(g_graphics_shader, NSHADER_STAGE_TYPE_VERTEX, available_backend);
    ASSERT_NE(blob, nullptr);
    EXPECT_NE(blob->data, nullptr);
    EXPECT_GT(blob->size, 0u);
  }
}

TEST(NShaderInfoTests, ComputeShaderMetadata) {
  ASSERT_NE(g_compute_shader, nullptr);

  const nshader_info_t* info = nshader_get_info(g_compute_shader);
  ASSERT_NE(info, nullptr);
  EXPECT_EQ(NSHADER_SHADER_TYPE_COMPUTE, info->type);
  EXPECT_EQ(1u, info->num_stages);

  // Check compute shader specific metadata
  EXPECT_EQ(NSHADER_STAGE_TYPE_COMPUTE, info->stages[0].type);
  EXPECT_EQ(8u, info->stages[0].metadata.compute.threadcount_x);
  EXPECT_EQ(8u, info->stages[0].metadata.compute.threadcount_y);
  EXPECT_EQ(1u, info->stages[0].metadata.compute.threadcount_z);
}
