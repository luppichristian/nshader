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
#include <nshader/nshader_reader.h>
#include <nshader/nshader_writer.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void test_read_from_memory(void) {
    TEST_ASSERT_NOT_NULL(g_graphics_shader);

    // Write to memory first
    size_t size_needed = nshader_write_to_memory(g_graphics_shader, NULL, 0);
    void* buffer = malloc(size_needed);
    nshader_write_to_memory(g_graphics_shader, buffer, size_needed);

    // Read back from memory
    nshader_t* read_shader = nshader_read_from_memory(buffer, size_needed);
    TEST_ASSERT_NOT_NULL(read_shader);

    // Verify it matches
    const nshader_info_t* original_info = nshader_get_info(g_graphics_shader);
    const nshader_info_t* read_info = nshader_get_info(read_shader);

    TEST_ASSERT_EQUAL(original_info->type, read_info->type);
    TEST_ASSERT_EQUAL(original_info->num_stages, read_info->num_stages);
    TEST_ASSERT_EQUAL(original_info->num_backends, read_info->num_backends);

    nshader_destroy(read_shader);
    free(buffer);
}

void test_read_from_file(void) {
    TEST_ASSERT_NOT_NULL(g_graphics_shader);

    // Write to file first
    nshader_write_to_path(g_graphics_shader, "test_read.nshader");

    // Read back from file
    FILE* file = fopen("test_read.nshader", "rb");
    TEST_ASSERT_NOT_NULL(file);

    nshader_t* read_shader = nshader_read_from_file(file);
    fclose(file);

    TEST_ASSERT_NOT_NULL(read_shader);

    nshader_destroy(read_shader);
    remove("test_read.nshader");
}

void test_read_from_path(void) {
    TEST_ASSERT_NOT_NULL(g_graphics_shader);

    // Write to file first
    nshader_write_to_path(g_graphics_shader, "test_read_path.nshader");

    // Read back from path
    nshader_t* read_shader = nshader_read_from_path("test_read_path.nshader");
    TEST_ASSERT_NOT_NULL(read_shader);

    nshader_destroy(read_shader);
    remove("test_read_path.nshader");
}

void test_read_invalid_data(void) {
    const char* invalid_data = "This is not a valid nshader file!";
    nshader_t* shader = nshader_read_from_memory(invalid_data, strlen(invalid_data));
    TEST_ASSERT_NULL(shader);
}

void test_roundtrip_memory(void) {
    TEST_ASSERT_NOT_NULL(g_compute_shader);

    // Write to memory
    size_t size1 = nshader_write_to_memory(g_compute_shader, NULL, 0);
    void* buffer1 = malloc(size1);
    nshader_write_to_memory(g_compute_shader, buffer1, size1);

    // Read back
    nshader_t* shader1 = nshader_read_from_memory(buffer1, size1);
    TEST_ASSERT_NOT_NULL(shader1);

    // Write again
    size_t size2 = nshader_write_to_memory(shader1, NULL, 0);
    void* buffer2 = malloc(size2);
    nshader_write_to_memory(shader1, buffer2, size2);

    // Buffers should be identical
    TEST_ASSERT_EQUAL(size1, size2);
    TEST_ASSERT_EQUAL_MEMORY(buffer1, buffer2, size1);

    nshader_destroy(shader1);
    free(buffer1);
    free(buffer2);
}

void test_roundtrip_file(void) {
    TEST_ASSERT_NOT_NULL(g_compute_shader);

    const char* filename = "test_roundtrip.nshader";

    // Write
    nshader_write_to_path(g_compute_shader, filename);

    // Read
    nshader_t* shader = nshader_read_from_path(filename);
    TEST_ASSERT_NOT_NULL(shader);

    // Verify metadata matches
    const nshader_info_t* original_info = nshader_get_info(g_compute_shader);
    const nshader_info_t* read_info = nshader_get_info(shader);

    TEST_ASSERT_EQUAL(original_info->type, read_info->type);
    TEST_ASSERT_EQUAL(original_info->num_stages, read_info->num_stages);

    nshader_destroy(shader);
    remove(filename);
}
