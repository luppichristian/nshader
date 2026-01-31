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
#include "nshader_type_tests.h"
#include "nshader_base_tests.h"
#include "nshader_compiler_tests.h"
#include "nshader_info_tests.h"
#include "nshader_writer_tests.h"
#include "nshader_reader_tests.h"
#include "nshader_samples_tests.h"

void setUp(void) {
    // Called before each test
}

void tearDown(void) {
    // Called after each test
}

int main(void) {
    UNITY_BEGIN();

    // =============================================================================
    // Type Module Tests
    // =============================================================================
    RUN_TEST(test_backend_to_string);
    RUN_TEST(test_backend_to_extension);
    RUN_TEST(test_stage_type_to_string);
    RUN_TEST(test_stage_type_to_shader_type);
    RUN_TEST(test_binding_type_to_string);

    // =============================================================================
    // Base Module Tests
    // =============================================================================
    RUN_TEST(test_custom_memory_allocator);

    // =============================================================================
    // Compiler Module Tests
    // =============================================================================
    RUN_TEST(test_compile_graphics_shader);
    RUN_TEST(test_compile_compute_shader);
    RUN_TEST(test_compile_with_defines);
    RUN_TEST(test_compile_invalid_shader);

    // =============================================================================
    // Info Module Tests
    // =============================================================================
    RUN_TEST(test_get_shader_info);
    RUN_TEST(test_has_backend);
    RUN_TEST(test_has_stage);
    RUN_TEST(test_get_blob);
    RUN_TEST(test_compute_shader_metadata);

    // =============================================================================
    // Writer Module Tests
    // =============================================================================
    RUN_TEST(test_write_to_memory);
    RUN_TEST(test_write_to_file);
    RUN_TEST(test_write_to_path);

    // =============================================================================
    // Reader Module Tests
    // =============================================================================
    RUN_TEST(test_read_from_memory);
    RUN_TEST(test_read_from_file);
    RUN_TEST(test_read_from_path);
    RUN_TEST(test_read_invalid_data);
    RUN_TEST(test_roundtrip_memory);
    RUN_TEST(test_roundtrip_file);

    // =============================================================================
    // Sample Shader Compilation Tests
    // =============================================================================

    // Compute shaders
    RUN_TEST(test_compile_FillTexture_comp);
    RUN_TEST(test_compile_GradientTexture_comp);
    RUN_TEST(test_compile_LinearToSRGB_comp);
    RUN_TEST(test_compile_LinearToST2084_comp);
    RUN_TEST(test_compile_SpriteBatch_comp);
    RUN_TEST(test_compile_TexturedQuad_comp);
    RUN_TEST(test_compile_ToneMapACES_comp);
    RUN_TEST(test_compile_ToneMapExtendedReinhardLuminance_comp);
    RUN_TEST(test_compile_ToneMapHable_comp);
    RUN_TEST(test_compile_ToneMapReinhard_comp);

    // Vertex shaders
    RUN_TEST(test_compile_Fullscreen_vert);
    RUN_TEST(test_compile_PositionColor_vert);
    RUN_TEST(test_compile_PositionColorInstanced_vert);
    RUN_TEST(test_compile_PositionColorTransform_vert);
    RUN_TEST(test_compile_PullSpriteBatch_vert);
    RUN_TEST(test_compile_RawTriangle_vert);
    RUN_TEST(test_compile_Skybox_vert);
    RUN_TEST(test_compile_TexturedQuad_vert);
    RUN_TEST(test_compile_TexturedQuadColorWithMatrix_vert);
    RUN_TEST(test_compile_TexturedQuadWithMatrix_vert);

    // Fragment shaders
    RUN_TEST(test_compile_CustomSampling_frag);
    RUN_TEST(test_compile_DepthOutline_frag);
    RUN_TEST(test_compile_Skybox_frag);
    RUN_TEST(test_compile_SolidColor_frag);
    RUN_TEST(test_compile_SolidColorDepth_frag);
    RUN_TEST(test_compile_TexturedQuad_frag);
    RUN_TEST(test_compile_TexturedQuadArray_frag);
    RUN_TEST(test_compile_TexturedQuadColor_frag);
    RUN_TEST(test_compile_TexturedQuadWithMultiplyColor_frag);

    // Multi-stage tests
    RUN_TEST(test_compile_Skybox_graphics);
    RUN_TEST(test_compile_TexturedQuad_graphics);

    // Persistence tests
    RUN_TEST(test_save_and_load_compute_shader);

    // =============================================================================
    // Cleanup
    // =============================================================================
    nshader_compiler_tests_cleanup();

    return UNITY_END();
}
