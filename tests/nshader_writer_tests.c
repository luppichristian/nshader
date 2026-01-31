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
#include <nshader/nshader_writer.h>
#include <stdlib.h>
#include <stdio.h>

void test_write_to_memory(void) {
    TEST_ASSERT_NOT_NULL(g_graphics_shader);

    // First, get the size needed
    size_t size_needed = nshader_write_to_memory(g_graphics_shader, NULL, 0);
    TEST_ASSERT_GREATER_THAN(0, size_needed);

    // Allocate buffer and write
    void* buffer = malloc(size_needed);
    TEST_ASSERT_NOT_NULL(buffer);

    size_t written = nshader_write_to_memory(g_graphics_shader, buffer, size_needed);
    TEST_ASSERT_EQUAL(size_needed, written);

    free(buffer);
}

void test_write_to_file(void) {
    TEST_ASSERT_NOT_NULL(g_graphics_shader);

    FILE* file = fopen("test_shader.nshader", "wb");
    TEST_ASSERT_NOT_NULL(file);

    bool success = nshader_write_to_file(g_graphics_shader, file);
    fclose(file);

    TEST_ASSERT_TRUE(success);

    // Clean up
    remove("test_shader.nshader");
}

void test_write_to_path(void) {
    TEST_ASSERT_NOT_NULL(g_graphics_shader);

    bool success = nshader_write_to_path(g_graphics_shader, "test_shader_path.nshader");
    TEST_ASSERT_TRUE(success);

    // Clean up
    remove("test_shader_path.nshader");
}
