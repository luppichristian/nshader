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
#include <nshader/nshader_base.h>
#include <stdlib.h>

static size_t g_malloc_count = 0;
static size_t g_free_count = 0;

static void* test_malloc(size_t size) {
    g_malloc_count++;
    return malloc(size);
}

static void test_free(void* ptr) {
    if (ptr) {
        g_free_count++;
        free(ptr);
    }
}

static void* test_calloc(size_t num, size_t size) {
    g_malloc_count++;
    return calloc(num, size);
}

static void* test_realloc(void* ptr, size_t new_size) {
    return realloc(ptr, new_size);
}

void test_custom_memory_allocator(void) {
    // Note: This test should be run in isolation as it changes global state
    // Set custom allocators
    nshader_set_memory_fns(test_malloc, test_free, test_calloc, test_realloc);

    g_malloc_count = 0;
    g_free_count = 0;

    // Allocate and free some memory
    void* ptr = nshader_malloc(100);
    TEST_ASSERT_NOT_NULL(ptr);
    TEST_ASSERT_EQUAL(1, g_malloc_count);

    nshader_free(ptr);
    TEST_ASSERT_EQUAL(1, g_free_count);

    // Reset to default allocators
    nshader_set_memory_fns(malloc, free, calloc, realloc);
}
