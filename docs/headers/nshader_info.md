# nshader_info.h

Type definitions and metadata structures for shader reflection.

## Purpose

Defines the complete type system: backends, stages, binding types, and metadata structures. All reflection data is exposed through these types.

## Enumerations

### nshader_backend_t
Target compilation backends:
- `NSHADER_BACKEND_DXIL` - DirectX Intermediate Language (DX12)
- `NSHADER_BACKEND_DXBC` - DirectX Bytecode (DX11)
- `NSHADER_BACKEND_MSL` - Metal Shading Language
- `NSHADER_BACKEND_SPV` - SPIR-V (Vulkan/OpenGL)

### nshader_type_t
Shader program type:
- `NSHADER_SHADER_TYPE_GRAPHICS` - vertex + fragment stages
- `NSHADER_SHADER_TYPE_COMPUTE` - compute stage only

### nshader_stage_type_t
Individual shader stages:
- `NSHADER_STAGE_TYPE_VERTEX`
- `NSHADER_STAGE_TYPE_FRAGMENT`
- `NSHADER_STAGE_TYPE_COMPUTE`

### nshader_binding_type_t
Variable data types for input/output bindings: `INT8`, `UINT8`, `INT16`, `UINT16`, `INT32`, `UINT32`, `INT64`, `UINT64`, `FLOAT16`, `FLOAT32`, `FLOAT64`

## Metadata Structures

### nshader_stage_binding_t
Single input/output variable:
- `name` - UTF-8 variable name
- `location` - binding location
- `vector_size` - component count (1-4)
- `type` - data type enum

### nshader_stage_metadata_vertex_t / fragment_t
Graphics stage metadata:
- `num_samplers`, `num_storage_textures`, `num_storage_buffers`, `num_uniform_buffers`
- `inputs`, `input_count` - input bindings
- `outputs`, `output_count` - output bindings

### nshader_stage_metadata_compute_t
Compute stage metadata:
- Read-only/read-write counts for storage textures and buffers
- `num_samplers`, `num_uniform_buffers`
- `threadcount_x/y/z` - dispatch dimensions

### nshader_info_t
Top-level shader info:
- `type` - graphics or compute
- `stages`, `num_stages` - array of stage descriptors
- `backends`, `num_backends` - available compiled backends

## Utility Functions

```c
const char* nshader_backend_to_string(nshader_backend_t);      // "DXIL", "SPIR-V", etc.
const char* nshader_backend_to_extension(nshader_backend_t);   // ".dxil", ".spv", etc.
const char* nshader_stage_type_to_string(nshader_stage_type_t);
const char* nshader_binding_type_to_string(nshader_binding_type_t);
nshader_type_t nshader_stage_type_to_shader_type(nshader_stage_type_t);
```

## When to Use

- Inspecting compiled shader capabilities
- Building pipeline state objects with correct resource counts
- Validating shader compatibility before loading
- Debug/diagnostic tooling
