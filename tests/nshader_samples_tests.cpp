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
#include <nshader/nshader_compiler.h>
#include <nshader/nshader_reader.h>
#include <nshader/nshader_type.h>
#include <nshader/nshader_writer.h>
}

// Helper to read entire file into string
static char* read_file(const char* filepath) {
  FILE* file = fopen(filepath, "rb");
  if (!file) {
    return nullptr;
  }

  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char* buffer = (char*)malloc(size + 1);
  if (!buffer) {
    fclose(file);
    return nullptr;
  }

  fread(buffer, 1, size, file);
  buffer[size] = '\0';
  fclose(file);

  return buffer;
}

// Helper to compile a single shader file
static nshader_t* compile_shader_file(const char* filepath, nshader_stage_type_t stage_type, const char* entry_point) {
  // Construct full path relative to project root (tests run from build/)
  char full_path[512];
  snprintf(full_path, sizeof(full_path), "../%s", filepath);

  char* source = read_file(full_path);
  if (!source) {
    printf("Failed to read file: %s (tried %s)\n", filepath, full_path);
    return nullptr;
  }

  nshader_compiler_stage_setup_t stage = {
      .stage_type = stage_type,
      .entry_point = entry_point,
      .source_code = source,
      .defines = nullptr,
      .num_defines = 0};

  nshader_compiler_config_t config = {
      .stages = &stage,
      .num_stages = 1,
      .include_dir = nullptr,
      .disable_dxil = false,
      .disable_dxbc = false,
      .disable_msl = false,
      .disable_spv = false,
      .enable_debug = false,
      .debug_name = filepath,
      .preserve_unused_bindings = false,
      .defines = nullptr,
      .num_defines = 0};

  nshader_error_list_t errors = {0};
  nshader_t* shader = nshader_compiler_compile_hlsl(&config, &errors);

  if (!shader && errors.num_errors > 0) {
    printf("Compilation errors for %s:\n", filepath);
    for (size_t i = 0; i < errors.num_errors; i++) {
      printf("  %s\n", errors.errors[i]);
    }
  }

  nshader_error_list_free(&errors);
  free(source);

  return shader;
}

// =============================================================================
// Compute Shader Sample Tests
// =============================================================================

TEST(NShaderSampleTests, CompileFillTextureComp) {
  nshader_t* shader = compile_shader_file("samples/FillTexture.comp.hlsl", NSHADER_STAGE_TYPE_COMPUTE, "main");
  ASSERT_NE(shader, nullptr);

  const nshader_info_t* info = nshader_get_info(shader);
  EXPECT_EQ(NSHADER_SHADER_TYPE_COMPUTE, info->type);
  EXPECT_TRUE(nshader_has_stage(shader, NSHADER_STAGE_TYPE_COMPUTE));

  nshader_destroy(shader);
}

TEST(NShaderSampleTests, CompileGradientTextureComp) {
  nshader_t* shader = compile_shader_file("samples/GradientTexture.comp.hlsl", NSHADER_STAGE_TYPE_COMPUTE, "main");
  ASSERT_NE(shader, nullptr);

  const nshader_info_t* info = nshader_get_info(shader);
  EXPECT_EQ(8u, info->stages[0].metadata.compute.threadcount_x);
  EXPECT_EQ(8u, info->stages[0].metadata.compute.threadcount_y);
  EXPECT_EQ(1u, info->stages[0].metadata.compute.threadcount_z);

  nshader_destroy(shader);
}

TEST(NShaderSampleTests, CompileLinearToSRGBComp) {
  nshader_t* shader = compile_shader_file("samples/LinearToSRGB.comp.hlsl", NSHADER_STAGE_TYPE_COMPUTE, "main");
  ASSERT_NE(shader, nullptr);
  nshader_destroy(shader);
}

TEST(NShaderSampleTests, CompileLinearToST2084Comp) {
  nshader_t* shader = compile_shader_file("samples/LinearToST2084.comp.hlsl", NSHADER_STAGE_TYPE_COMPUTE, "main");
  ASSERT_NE(shader, nullptr);
  nshader_destroy(shader);
}

TEST(NShaderSampleTests, CompileSpriteBatchComp) {
  nshader_t* shader = compile_shader_file("samples/SpriteBatch.comp.hlsl", NSHADER_STAGE_TYPE_COMPUTE, "main");
  ASSERT_NE(shader, nullptr);
  nshader_destroy(shader);
}

TEST(NShaderSampleTests, CompileTexturedQuadComp) {
  nshader_t* shader = compile_shader_file("samples/TexturedQuad.comp.hlsl", NSHADER_STAGE_TYPE_COMPUTE, "main");
  ASSERT_NE(shader, nullptr);
  nshader_destroy(shader);
}

TEST(NShaderSampleTests, CompileToneMapACESComp) {
  nshader_t* shader = compile_shader_file("samples/ToneMapACES.comp.hlsl", NSHADER_STAGE_TYPE_COMPUTE, "main");
  ASSERT_NE(shader, nullptr);
  nshader_destroy(shader);
}

TEST(NShaderSampleTests, CompileToneMapExtendedReinhardLuminanceComp) {
  nshader_t* shader = compile_shader_file("samples/ToneMapExtendedReinhardLuminance.comp.hlsl", NSHADER_STAGE_TYPE_COMPUTE, "main");
  ASSERT_NE(shader, nullptr);
  nshader_destroy(shader);
}

TEST(NShaderSampleTests, CompileToneMapHableComp) {
  nshader_t* shader = compile_shader_file("samples/ToneMapHable.comp.hlsl", NSHADER_STAGE_TYPE_COMPUTE, "main");
  ASSERT_NE(shader, nullptr);
  nshader_destroy(shader);
}

TEST(NShaderSampleTests, CompileToneMapReinhardComp) {
  nshader_t* shader = compile_shader_file("samples/ToneMapReinhard.comp.hlsl", NSHADER_STAGE_TYPE_COMPUTE, "main");
  ASSERT_NE(shader, nullptr);
  nshader_destroy(shader);
}

// =============================================================================
// Vertex Shader Sample Tests
// =============================================================================

TEST(NShaderSampleTests, CompileFullscreenVert) {
  nshader_t* shader = compile_shader_file("samples/Fullscreen.vert.hlsl", NSHADER_STAGE_TYPE_VERTEX, "main");
  ASSERT_NE(shader, nullptr);

  const nshader_info_t* info = nshader_get_info(shader);
  EXPECT_EQ(NSHADER_SHADER_TYPE_GRAPHICS, info->type);

  nshader_destroy(shader);
}

TEST(NShaderSampleTests, CompilePositionColorVert) {
  nshader_t* shader = compile_shader_file("samples/PositionColor.vert.hlsl", NSHADER_STAGE_TYPE_VERTEX, "main");
  ASSERT_NE(shader, nullptr);
  nshader_destroy(shader);
}

TEST(NShaderSampleTests, CompilePositionColorInstancedVert) {
  nshader_t* shader = compile_shader_file("samples/PositionColorInstanced.vert.hlsl", NSHADER_STAGE_TYPE_VERTEX, "main");
  ASSERT_NE(shader, nullptr);
  nshader_destroy(shader);
}

TEST(NShaderSampleTests, CompilePositionColorTransformVert) {
  nshader_t* shader = compile_shader_file("samples/PositionColorTransform.vert.hlsl", NSHADER_STAGE_TYPE_VERTEX, "main");
  ASSERT_NE(shader, nullptr);
  nshader_destroy(shader);
}

TEST(NShaderSampleTests, CompilePullSpriteBatchVert) {
  nshader_t* shader = compile_shader_file("samples/PullSpriteBatch.vert.hlsl", NSHADER_STAGE_TYPE_VERTEX, "main");
  ASSERT_NE(shader, nullptr);
  nshader_destroy(shader);
}

TEST(NShaderSampleTests, CompileRawTriangleVert) {
  nshader_t* shader = compile_shader_file("samples/RawTriangle.vert.hlsl", NSHADER_STAGE_TYPE_VERTEX, "main");
  ASSERT_NE(shader, nullptr);
  nshader_destroy(shader);
}

TEST(NShaderSampleTests, CompileSkyboxVert) {
  nshader_t* shader = compile_shader_file("samples/Skybox.vert.hlsl", NSHADER_STAGE_TYPE_VERTEX, "main");
  ASSERT_NE(shader, nullptr);
  nshader_destroy(shader);
}

TEST(NShaderSampleTests, CompileTexturedQuadVert) {
  nshader_t* shader = compile_shader_file("samples/TexturedQuad.vert.hlsl", NSHADER_STAGE_TYPE_VERTEX, "main");
  ASSERT_NE(shader, nullptr);
  nshader_destroy(shader);
}

TEST(NShaderSampleTests, CompileTexturedQuadColorWithMatrixVert) {
  nshader_t* shader = compile_shader_file("samples/TexturedQuadColorWithMatrix.vert.hlsl", NSHADER_STAGE_TYPE_VERTEX, "main");
  ASSERT_NE(shader, nullptr);
  nshader_destroy(shader);
}

TEST(NShaderSampleTests, CompileTexturedQuadWithMatrixVert) {
  nshader_t* shader = compile_shader_file("samples/TexturedQuadWithMatrix.vert.hlsl", NSHADER_STAGE_TYPE_VERTEX, "main");
  ASSERT_NE(shader, nullptr);
  nshader_destroy(shader);
}

// =============================================================================
// Fragment Shader Sample Tests
// =============================================================================

TEST(NShaderSampleTests, CompileCustomSamplingFrag) {
  nshader_t* shader = compile_shader_file("samples/CustomSampling.frag.hlsl", NSHADER_STAGE_TYPE_FRAGMENT, "main");
  ASSERT_NE(shader, nullptr);
  nshader_destroy(shader);
}

TEST(NShaderSampleTests, CompileDepthOutlineFrag) {
  nshader_t* shader = compile_shader_file("samples/DepthOutline.frag.hlsl", NSHADER_STAGE_TYPE_FRAGMENT, "main");
  ASSERT_NE(shader, nullptr);
  nshader_destroy(shader);
}

TEST(NShaderSampleTests, CompileSkyboxFrag) {
  nshader_t* shader = compile_shader_file("samples/Skybox.frag.hlsl", NSHADER_STAGE_TYPE_FRAGMENT, "main");
  ASSERT_NE(shader, nullptr);
  nshader_destroy(shader);
}

TEST(NShaderSampleTests, CompileSolidColorFrag) {
  nshader_t* shader = compile_shader_file("samples/SolidColor.frag.hlsl", NSHADER_STAGE_TYPE_FRAGMENT, "main");
  ASSERT_NE(shader, nullptr);
  nshader_destroy(shader);
}

TEST(NShaderSampleTests, CompileSolidColorDepthFrag) {
  nshader_t* shader = compile_shader_file("samples/SolidColorDepth.frag.hlsl", NSHADER_STAGE_TYPE_FRAGMENT, "main");
  ASSERT_NE(shader, nullptr);
  nshader_destroy(shader);
}

TEST(NShaderSampleTests, CompileTexturedQuadFrag) {
  nshader_t* shader = compile_shader_file("samples/TexturedQuad.frag.hlsl", NSHADER_STAGE_TYPE_FRAGMENT, "main");
  ASSERT_NE(shader, nullptr);
  nshader_destroy(shader);
}

TEST(NShaderSampleTests, CompileTexturedQuadArrayFrag) {
  nshader_t* shader = compile_shader_file("samples/TexturedQuadArray.frag.hlsl", NSHADER_STAGE_TYPE_FRAGMENT, "main");
  ASSERT_NE(shader, nullptr);
  nshader_destroy(shader);
}

TEST(NShaderSampleTests, CompileTexturedQuadColorFrag) {
  nshader_t* shader = compile_shader_file("samples/TexturedQuadColor.frag.hlsl", NSHADER_STAGE_TYPE_FRAGMENT, "main");
  ASSERT_NE(shader, nullptr);
  nshader_destroy(shader);
}

TEST(NShaderSampleTests, CompileTexturedQuadWithMultiplyColorFrag) {
  nshader_t* shader = compile_shader_file("samples/TexturedQuadWithMultiplyColor.frag.hlsl", NSHADER_STAGE_TYPE_FRAGMENT, "main");
  ASSERT_NE(shader, nullptr);
  nshader_destroy(shader);
}

// =============================================================================
// Multi-Stage Shader Tests
// =============================================================================

TEST(NShaderSampleTests, CompileSkyboxGraphics) {
  char* vert_source = read_file("../samples/Skybox.vert.hlsl");
  char* frag_source = read_file("../samples/Skybox.frag.hlsl");

  ASSERT_NE(vert_source, nullptr);
  ASSERT_NE(frag_source, nullptr);

  nshader_compiler_stage_setup_t stages[2] = {
      {  .stage_type = NSHADER_STAGE_TYPE_VERTEX,
       .entry_point = "main",
       .source_code = vert_source,
       .defines = nullptr,
       .num_defines = 0},
      {.stage_type = NSHADER_STAGE_TYPE_FRAGMENT,
       .entry_point = "main",
       .source_code = frag_source,
       .defines = nullptr,
       .num_defines = 0}
  };

  nshader_compiler_config_t config = {
      .stages = stages,
      .num_stages = 2,
      .include_dir = nullptr,
      .disable_dxil = false,
      .disable_dxbc = false,
      .disable_msl = false,
      .disable_spv = false,
      .enable_debug = false,
      .debug_name = "Skybox",
      .preserve_unused_bindings = false,
      .defines = nullptr,
      .num_defines = 0};

  nshader_error_list_t errors = {0};
  nshader_t* shader = nshader_compiler_compile_hlsl(&config, &errors);

  if (!shader && errors.num_errors > 0) {
    for (size_t i = 0; i < errors.num_errors; i++) {
      printf("Error: %s\n", errors.errors[i]);
    }
  }

  ASSERT_NE(shader, nullptr);

  const nshader_info_t* info = nshader_get_info(shader);
  EXPECT_EQ(NSHADER_SHADER_TYPE_GRAPHICS, info->type);
  EXPECT_EQ(2u, info->num_stages);
  EXPECT_TRUE(nshader_has_stage(shader, NSHADER_STAGE_TYPE_VERTEX));
  EXPECT_TRUE(nshader_has_stage(shader, NSHADER_STAGE_TYPE_FRAGMENT));

  nshader_destroy(shader);
  nshader_error_list_free(&errors);
  free(vert_source);
  free(frag_source);
}

TEST(NShaderSampleTests, CompileTexturedQuadGraphics) {
  char* vert_source = read_file("../samples/TexturedQuad.vert.hlsl");
  char* frag_source = read_file("../samples/TexturedQuad.frag.hlsl");

  ASSERT_NE(vert_source, nullptr);
  ASSERT_NE(frag_source, nullptr);

  nshader_compiler_stage_setup_t stages[2] = {
      {  .stage_type = NSHADER_STAGE_TYPE_VERTEX,
       .entry_point = "main",
       .source_code = vert_source,
       .defines = nullptr,
       .num_defines = 0},
      {.stage_type = NSHADER_STAGE_TYPE_FRAGMENT,
       .entry_point = "main",
       .source_code = frag_source,
       .defines = nullptr,
       .num_defines = 0}
  };

  nshader_compiler_config_t config = {
      .stages = stages,
      .num_stages = 2,
      .include_dir = nullptr,
      .disable_dxil = false,
      .disable_dxbc = false,
      .disable_msl = false,
      .disable_spv = false,
      .enable_debug = false,
      .debug_name = "TexturedQuad",
      .preserve_unused_bindings = false,
      .defines = nullptr,
      .num_defines = 0};

  nshader_error_list_t errors = {0};
  nshader_t* shader = nshader_compiler_compile_hlsl(&config, &errors);

  ASSERT_NE(shader, nullptr);

  nshader_destroy(shader);
  nshader_error_list_free(&errors);
  free(vert_source);
  free(frag_source);
}

// =============================================================================
// Shader Persistence Tests (with real shaders)
// =============================================================================

TEST(NShaderSampleTests, SaveAndLoadComputeShader) {
  // Compile a compute shader
  nshader_t* shader = compile_shader_file("samples/GradientTexture.comp.hlsl", NSHADER_STAGE_TYPE_COMPUTE, "main");
  ASSERT_NE(shader, nullptr);

  // Save to file
  const char* filename = "test_gradient.nsdr";
  bool saved = nshader_write_to_path(shader, filename);
  EXPECT_TRUE(saved);

  // Load back
  nshader_t* loaded_shader = nshader_read_from_path(filename);
  ASSERT_NE(loaded_shader, nullptr);

  // Verify metadata matches
  const nshader_info_t* original_info = nshader_get_info(shader);
  const nshader_info_t* loaded_info = nshader_get_info(loaded_shader);

  EXPECT_EQ(original_info->type, loaded_info->type);
  EXPECT_EQ(original_info->num_stages, loaded_info->num_stages);
  EXPECT_EQ(original_info->num_backends, loaded_info->num_backends);

  // Cleanup
  nshader_destroy(shader);
  nshader_destroy(loaded_shader);
  remove(filename);
}
