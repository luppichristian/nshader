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

#include "unity.h"
#include "nshader_compiler_tests.h"
#include <nshader/nshader_type.h>

void test_get_shader_info(void) {
    TEST_ASSERT_NOT_NULL(g_graphics_shader);

    const nshader_info_t* info = nshader_get_info(g_graphics_shader);
    TEST_ASSERT_NOT_NULL(info);
    TEST_ASSERT_EQUAL(NSHADER_SHADER_TYPE_GRAPHICS, info->type);
    TEST_ASSERT_EQUAL(2, info->num_stages);
    TEST_ASSERT_GREATER_THAN(0, info->num_backends);
}

void test_has_backend(void) {
    TEST_ASSERT_NOT_NULL(g_graphics_shader);

    // At least one backend should be available
    bool has_any_backend = false;
    for (int i = 0; i < NSHADER_BACKEND_COUNT; i++) {
        if (nshader_has_backend(g_graphics_shader, i)) {
            has_any_backend = true;
            break;
        }
    }
    TEST_ASSERT_TRUE(has_any_backend);
}

void test_has_stage(void) {
    TEST_ASSERT_NOT_NULL(g_graphics_shader);

    TEST_ASSERT_TRUE(nshader_has_stage(g_graphics_shader, NSHADER_STAGE_TYPE_VERTEX));
    TEST_ASSERT_TRUE(nshader_has_stage(g_graphics_shader, NSHADER_STAGE_TYPE_FRAGMENT));
    TEST_ASSERT_FALSE(nshader_has_stage(g_graphics_shader, NSHADER_STAGE_TYPE_COMPUTE));
}

void test_get_blob(void) {
    TEST_ASSERT_NOT_NULL(g_graphics_shader);

    // Find an available backend
    nshader_backend_t available_backend = NSHADER_BACKEND_COUNT;
    for (int i = 0; i < NSHADER_BACKEND_COUNT; i++) {
        if (nshader_has_backend(g_graphics_shader, i)) {
            available_backend = i;
            break;
        }
    }

    if (available_backend != NSHADER_BACKEND_COUNT) {
        const nshader_blob_t* blob = nshader_get_blob(g_graphics_shader, NSHADER_STAGE_TYPE_VERTEX, available_backend);
        TEST_ASSERT_NOT_NULL(blob);
        TEST_ASSERT_NOT_NULL(blob->data);
        TEST_ASSERT_GREATER_THAN(0, blob->size);
    }
}

void test_compute_shader_metadata(void) {
    TEST_ASSERT_NOT_NULL(g_compute_shader);

    const nshader_info_t* info = nshader_get_info(g_compute_shader);
    TEST_ASSERT_NOT_NULL(info);
    TEST_ASSERT_EQUAL(NSHADER_SHADER_TYPE_COMPUTE, info->type);
    TEST_ASSERT_EQUAL(1, info->num_stages);

    // Check compute shader specific metadata
    TEST_ASSERT_EQUAL(NSHADER_STAGE_TYPE_COMPUTE, info->stages[0].type);
    TEST_ASSERT_EQUAL(8, info->stages[0].metadata.compute.threadcount_x);
    TEST_ASSERT_EQUAL(8, info->stages[0].metadata.compute.threadcount_y);
    TEST_ASSERT_EQUAL(1, info->stages[0].metadata.compute.threadcount_z);
}
