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
#include "test_shaders.h"
#include <nshader/nshader_compiler.h>
#include <nshader/nshader_reader.h>
#include <stdio.h>

// Global test state
nshader_t* g_graphics_shader = NULL;
nshader_t* g_compute_shader = NULL;

void test_compile_graphics_shader(void) {
    nshader_compiler_stage_setup_t stages[2] = {
        {
            .stage_type = NSHADER_STAGE_TYPE_VERTEX,
            .entry_point = "main",
            .source_code = VERTEX_SHADER_SOURCE,
            .defines = NULL,
            .num_defines = 0
        },
        {
            .stage_type = NSHADER_STAGE_TYPE_FRAGMENT,
            .entry_point = "main",
            .source_code = FRAGMENT_SHADER_SOURCE,
            .defines = NULL,
            .num_defines = 0
        }
    };

    nshader_compiler_config_t config = {
        .stages = stages,
        .num_stages = 2,
        .include_dir = NULL,
        .disable_dxil = false,
        .disable_dxbc = false,
        .disable_msl = false,
        .disable_spv = false,
        .enable_debug = false,
        .debug_name = "TestGraphicsShader",
        .preserve_unused_bindings = false,
        .defines = NULL,
        .num_defines = 0
    };

    nshader_error_list_t errors = {0};
    g_graphics_shader = nshader_compiler_compile_hlsl(&config, &errors);

    if (g_graphics_shader == NULL && errors.num_errors > 0) {
        for (size_t i = 0; i < errors.num_errors; i++) {
            printf("Compiler error: %s\n", errors.errors[i]);
        }
        nshader_error_list_free(&errors);
    }

    TEST_ASSERT_NOT_NULL(g_graphics_shader);
    nshader_error_list_free(&errors);
}

void test_compile_compute_shader(void) {
    nshader_compiler_stage_setup_t stage = {
        .stage_type = NSHADER_STAGE_TYPE_COMPUTE,
        .entry_point = "main",
        .source_code = COMPUTE_SHADER_SOURCE,
        .defines = NULL,
        .num_defines = 0
    };

    nshader_compiler_config_t config = {
        .stages = &stage,
        .num_stages = 1,
        .include_dir = NULL,
        .disable_dxil = false,
        .disable_dxbc = false,
        .disable_msl = false,
        .disable_spv = false,
        .enable_debug = false,
        .debug_name = "TestComputeShader",
        .preserve_unused_bindings = false,
        .defines = NULL,
        .num_defines = 0
    };

    nshader_error_list_t errors = {0};
    g_compute_shader = nshader_compiler_compile_hlsl(&config, &errors);

    if (g_compute_shader == NULL && errors.num_errors > 0) {
        for (size_t i = 0; i < errors.num_errors; i++) {
            printf("Compiler error: %s\n", errors.errors[i]);
        }
        nshader_error_list_free(&errors);
    }

    TEST_ASSERT_NOT_NULL(g_compute_shader);
    nshader_error_list_free(&errors);
}

void test_compile_with_defines(void) {
    nshader_compiler_define_t defines[] = {
        { .name = "TEST_DEFINE", .value = "1" },
        { .name = "COLOR_RED", .value = "float4(1,0,0,1)" }
    };

    nshader_compiler_stage_setup_t stage = {
        .stage_type = NSHADER_STAGE_TYPE_FRAGMENT,
        .entry_point = "main",
        .source_code = FRAGMENT_SHADER_SOURCE,
        .defines = NULL,
        .num_defines = 0
    };

    nshader_compiler_config_t config = {
        .stages = &stage,
        .num_stages = 1,
        .include_dir = NULL,
        .disable_dxil = false,
        .disable_dxbc = false,
        .disable_msl = false,
        .disable_spv = false,
        .enable_debug = false,
        .debug_name = NULL,
        .preserve_unused_bindings = false,
        .defines = defines,
        .num_defines = 2
    };

    nshader_error_list_t errors = {0};
    nshader_t* shader = nshader_compiler_compile_hlsl(&config, &errors);

    if (shader == NULL && errors.num_errors > 0) {
        for (size_t i = 0; i < errors.num_errors; i++) {
            printf("Compiler error: %s\n", errors.errors[i]);
        }
    }

    TEST_ASSERT_NOT_NULL(shader);
    nshader_destroy(shader);
    nshader_error_list_free(&errors);
}

void test_compile_invalid_shader(void) {
    const char* invalid_source = "this is not valid HLSL code!!!";

    nshader_compiler_stage_setup_t stage = {
        .stage_type = NSHADER_STAGE_TYPE_VERTEX,
        .entry_point = "main",
        .source_code = invalid_source,
        .defines = NULL,
        .num_defines = 0
    };

    nshader_compiler_config_t config = {
        .stages = &stage,
        .num_stages = 1,
        .include_dir = NULL,
        .disable_dxil = false,
        .disable_dxbc = false,
        .disable_msl = false,
        .disable_spv = false,
        .enable_debug = false,
        .debug_name = NULL,
        .preserve_unused_bindings = false,
        .defines = NULL,
        .num_defines = 0
    };

    nshader_error_list_t errors = {0};
    nshader_t* shader = nshader_compiler_compile_hlsl(&config, &errors);

    TEST_ASSERT_NULL(shader);
    TEST_ASSERT_GREATER_THAN(0, errors.num_errors);

    nshader_error_list_free(&errors);
}

void nshader_compiler_tests_cleanup(void) {
    if (g_graphics_shader) {
        nshader_destroy(g_graphics_shader);
        g_graphics_shader = NULL;
    }
    if (g_compute_shader) {
        nshader_destroy(g_compute_shader);
        g_compute_shader = NULL;
    }
}
