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
}

TEST(NShaderTypeTests, BackendToString) {
    EXPECT_STREQ("DXIL", nshader_backend_to_string(NSHADER_BACKEND_DXIL));
    EXPECT_STREQ("DXBC", nshader_backend_to_string(NSHADER_BACKEND_DXBC));
    EXPECT_STREQ("MSL", nshader_backend_to_string(NSHADER_BACKEND_MSL));
    EXPECT_STREQ("SPIR-V", nshader_backend_to_string(NSHADER_BACKEND_SPV));
}

TEST(NShaderTypeTests, BackendToExtension) {
    EXPECT_STREQ(".dxil", nshader_backend_to_extension(NSHADER_BACKEND_DXIL));
    EXPECT_STREQ(".dxbc", nshader_backend_to_extension(NSHADER_BACKEND_DXBC));
    EXPECT_STREQ(".msl", nshader_backend_to_extension(NSHADER_BACKEND_MSL));
    EXPECT_STREQ(".spv", nshader_backend_to_extension(NSHADER_BACKEND_SPV));
}

TEST(NShaderTypeTests, StageTypeToString) {
    EXPECT_STREQ("Vertex", nshader_stage_type_to_string(NSHADER_STAGE_TYPE_VERTEX));
    EXPECT_STREQ("Fragment", nshader_stage_type_to_string(NSHADER_STAGE_TYPE_FRAGMENT));
    EXPECT_STREQ("Compute", nshader_stage_type_to_string(NSHADER_STAGE_TYPE_COMPUTE));
}

TEST(NShaderTypeTests, StageTypeToShaderType) {
    EXPECT_EQ(NSHADER_SHADER_TYPE_GRAPHICS, nshader_stage_type_to_shader_type(NSHADER_STAGE_TYPE_VERTEX));
    EXPECT_EQ(NSHADER_SHADER_TYPE_GRAPHICS, nshader_stage_type_to_shader_type(NSHADER_STAGE_TYPE_FRAGMENT));
    EXPECT_EQ(NSHADER_SHADER_TYPE_COMPUTE, nshader_stage_type_to_shader_type(NSHADER_STAGE_TYPE_COMPUTE));
}

TEST(NShaderTypeTests, BindingTypeToString) {
    EXPECT_STREQ("int8", nshader_binding_type_to_string(NSHADER_BINDING_TYPE_INT8));
    EXPECT_STREQ("uint8", nshader_binding_type_to_string(NSHADER_BINDING_TYPE_UINT8));
    EXPECT_STREQ("float32", nshader_binding_type_to_string(NSHADER_BINDING_TYPE_FLOAT32));
}
