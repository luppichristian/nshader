# Getting Started

## Overview

nshader solves the shader distribution problem: write HLSL once, compile to all GPU backends, package with metadata, load at runtime. No runtime shader compilation required.

**Workflow:**
1. Write HLSL shaders following SDL3 GPU resource binding conventions
2. Compile to `.nshader` binary (offline or build-time)
3. Load `.nshader` at runtime, query metadata, get backend-specific blobs

## Build & Install

```bash
git clone <repository>
cd nshader
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
cmake --install build --prefix /usr/local
```

Dependencies (SDL3, SDL_shadercross) are fetched automatically via FetchContent.

## Resource Binding Conventions

Shaders must follow SDL3 GPU binding order. See [nshader_compiler.h](headers/nshader_compiler.md) for full specification.

**Compute shaders:**
- Set 0: Samplers, read-only storage textures, read-only storage buffers
- Set 1: Read-write storage textures, read-write storage buffers
- Set 2: Uniform buffers

**Graphics shaders:**
- Vertex: Set 0 (samplers/textures/buffers), Set 1 (uniforms)
- Fragment: Set 2 (samplers/textures/buffers), Set 3 (uniforms)

## Minimal Example

```c
#include <nshader.h>

int main(void) {
    const char* hlsl =
        "float4 main() : SV_Target { return float4(1,0,0,1); }";

    nshader_compiler_stage_setup_t stage = {
        .stage_type = NSHADER_STAGE_TYPE_FRAGMENT,
        .entry_point = "main",
        .source_code = hlsl
    };

    nshader_compiler_config_t config = {
        .stages = &stage,
        .num_stages = 1
    };

    nshader_error_list_t errors = {0};
    nshader_t* shader = nshader_compiler_compile_hlsl(&config, &errors);

    if (!shader) {
        for (size_t i = 0; i < errors.num_errors; i++)
            fprintf(stderr, "%s\n", errors.errors[i]);
        nshader_error_list_free(&errors);
        return 1;
    }

    nshader_write_to_path(shader, "red.nshader");
    nshader_destroy(shader);
    return 0;
}
```

## CLI Usage

```bash
# Graphics shader with vertex + fragment
nshader compile sprite.hlsl -o sprite.nshader \
    --vertex vs_main --fragment fs_main

# Compute shader with defines
nshader compile blur.hlsl -o blur.nshader \
    --compute main -D KERNEL_SIZE=5

# Disable specific backends
nshader compile shader.hlsl -o shader.nshader \
    --vertex main --disable-dxbc --disable-msl

# Inspect compiled shader
nshader info shader.nshader --verbose

# Extract SPIR-V for debugging
nshader extract shader.nshader spv vertex -o debug.spv
```

## Pitfalls

- **Entry points must match**: The entry point name in config must match the function name in HLSL
- **One include directory**: SDL_shadercross limitation - only one `-I` path supported
- **Backend availability**: Not all backends compile on all platforms; check `nshader_has_backend()` at runtime
- **Memory ownership**: `nshader_destroy()` frees all associated memory including blobs; don't hold blob pointers after destroy
