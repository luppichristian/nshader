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

// Test shader sources
static const char* VERTEX_SHADER_SOURCE =
    "struct Input\n"
    "{\n"
    "    uint VertexIndex : SV_VertexID;\n"
    "};\n"
    "\n"
    "struct Output\n"
    "{\n"
    "    float4 Color : TEXCOORD0;\n"
    "    float4 Position : SV_Position;\n"
    "};\n"
    "\n"
    "Output main(Input input)\n"
    "{\n"
    "    Output output;\n"
    "    float2 pos;\n"
    "    if (input.VertexIndex == 0)\n"
    "    {\n"
    "        pos = (-1.0f).xx;\n"
    "        output.Color = float4(1.0f, 0.0f, 0.0f, 1.0f);\n"
    "    }\n"
    "    else if (input.VertexIndex == 1)\n"
    "    {\n"
    "        pos = float2(1.0f, -1.0f);\n"
    "        output.Color = float4(0.0f, 1.0f, 0.0f, 1.0f);\n"
    "    }\n"
    "    else\n"
    "    {\n"
    "        pos = float2(0.0f, 1.0f);\n"
    "        output.Color = float4(0.0f, 0.0f, 1.0f, 1.0f);\n"
    "    }\n"
    "    output.Position = float4(pos, 0.0f, 1.0f);\n"
    "    return output;\n"
    "}\n";

static const char* FRAGMENT_SHADER_SOURCE =
    "float4 main(float4 Color : TEXCOORD0) : SV_Target0\n"
    "{\n"
    "    return Color;\n"
    "}\n";

static const char* COMPUTE_SHADER_SOURCE =
    "[[vk::image_format(\"rgba8\")]]\n"
    "RWTexture2D<float4> OutImage : register(u0, space1);\n"
    "\n"
    "cbuffer UBO : register(b0, space2)\n"
    "{\n"
    "    float ubo_time : packoffset(c0);\n"
    "};\n"
    "\n"
    "[numthreads(8, 8, 1)]\n"
    "void main(uint3 GlobalInvocationID : SV_DispatchThreadID)\n"
    "{\n"
    "    float w, h;\n"
    "    OutImage.GetDimensions(w, h);\n"
    "    float2 size = float2(w, h);\n"
    "    float2 coord = GlobalInvocationID.xy;\n"
    "    float2 uv = coord / size;\n"
    "\n"
    "    float3 col = 0.5f.xxx + (cos((ubo_time.xxx + uv.xyx) + float3(0.0f, 2.0f, 4.0f)) * 0.5f);\n"
    "    OutImage[int2(coord)] = float4(col, 1.0f);\n"
    "}\n";
