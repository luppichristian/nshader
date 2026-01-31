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

#pragma once

// Compute shader tests
void test_compile_FillTexture_comp(void);
void test_compile_GradientTexture_comp(void);
void test_compile_LinearToSRGB_comp(void);
void test_compile_LinearToST2084_comp(void);
void test_compile_SpriteBatch_comp(void);
void test_compile_TexturedQuad_comp(void);
void test_compile_ToneMapACES_comp(void);
void test_compile_ToneMapExtendedReinhardLuminance_comp(void);
void test_compile_ToneMapHable_comp(void);
void test_compile_ToneMapReinhard_comp(void);

// Vertex shader tests
void test_compile_Fullscreen_vert(void);
void test_compile_PositionColor_vert(void);
void test_compile_PositionColorInstanced_vert(void);
void test_compile_PositionColorTransform_vert(void);
void test_compile_PullSpriteBatch_vert(void);
void test_compile_RawTriangle_vert(void);
void test_compile_Skybox_vert(void);
void test_compile_TexturedQuad_vert(void);
void test_compile_TexturedQuadColorWithMatrix_vert(void);
void test_compile_TexturedQuadWithMatrix_vert(void);

// Fragment shader tests
void test_compile_CustomSampling_frag(void);
void test_compile_DepthOutline_frag(void);
void test_compile_Skybox_frag(void);
void test_compile_SolidColor_frag(void);
void test_compile_SolidColorDepth_frag(void);
void test_compile_TexturedQuad_frag(void);
void test_compile_TexturedQuadArray_frag(void);
void test_compile_TexturedQuadColor_frag(void);
void test_compile_TexturedQuadWithMultiplyColor_frag(void);

// Multi-stage tests
void test_compile_Skybox_graphics(void);
void test_compile_TexturedQuad_graphics(void);

// Persistence tests
void test_save_and_load_compute_shader(void);
