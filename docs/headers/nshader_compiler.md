# nshader_compiler.h

HLSL shader compilation to multiple backends with reflection extraction.

## Purpose

Offline compilation of HLSL source to all supported GPU backends. Produces `nshader_t` objects containing compiled blobs and reflection metadata. Uses SDL_shadercross internally.

## Resource Binding Order

Shaders must follow SDL3 GPU binding conventions for cross-platform compatibility.

### Compute Shaders

| Backend | Binding Order |
|---------|--------------|
| SPIR-V | Set 0: samplers, RO textures, RO buffers; Set 1: RW textures, RW buffers; Set 2: uniforms |
| DXBC/DXIL | t[n] space0: samplers+RO; u[n] space1: RW; b[n] space2: uniforms |
| MSL | [[buffer]]: uniforms, RO buffers, RW buffers; [[texture]]: samplers, RO textures, RW textures |

### Graphics Shaders

| Stage | SPIR-V Sets | DXBC/DXIL Spaces |
|-------|------------|-----------------|
| Vertex | 0: resources, 1: uniforms | t/s space0, b space1 |
| Fragment | 2: resources, 3: uniforms | t/s space2, b space3 |

## Configuration

### nshader_compiler_define_t
Preprocessor define:
- `name` - define name
- `value` - optional value (NULL for simple defines)

### nshader_compiler_stage_setup_t
Per-stage compilation input:
- `stage_type` - vertex/fragment/compute
- `entry_point` - function name in HLSL
- `source_code` - HLSL source string
- `defines`, `num_defines` - stage-specific defines

### nshader_compiler_config_t
Full compilation configuration:
- `stages`, `num_stages` - stages to compile
- `include_dir` - single include path (SDL_shadercross limitation)
- `disable_dxil/dxbc/msl/spv` - skip specific backends
- `enable_debug` - include debug info
- `debug_name` - identifier for debugging
- `preserve_unused_bindings` - keep unreferenced resources
- `defines`, `num_defines` - global defines (all stages)

## API

```c
nshader_t* nshader_compiler_compile_hlsl(
    const nshader_compiler_config_t* config,
    nshader_error_list_t* out_errors);  // optional

void nshader_error_list_free(nshader_error_list_t* list);
```

Returns NULL on failure; check `out_errors` for messages.

## Example

```c
nshader_compiler_stage_setup_t stages[] = {
    { NSHADER_STAGE_TYPE_VERTEX, "vs_main", vs_hlsl, NULL, 0 },
    { NSHADER_STAGE_TYPE_FRAGMENT, "fs_main", fs_hlsl, NULL, 0 }
};

nshader_compiler_config_t config = {
    .stages = stages,
    .num_stages = 2,
    .disable_dxbc = true,  // skip DX11 backend
    .enable_debug = true
};

nshader_error_list_t errors = {0};
nshader_t* shader = nshader_compiler_compile_hlsl(&config, &errors);

if (!shader) {
    for (size_t i = 0; i < errors.num_errors; i++)
        fprintf(stderr, "Error: %s\n", errors.errors[i]);
}
nshader_error_list_free(&errors);
```

## Design Notes

- Compilation is synchronous and CPU-intensive; suitable for offline/build-time use
- Backend availability depends on platform and SDL_shadercross configuration
- Entry point names must exactly match HLSL function names
