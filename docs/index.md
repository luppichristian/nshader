---
layout: default
title: Home
---

# nshader

Cross-platform shader compiler and runtime library. Compiles HLSL shaders to multiple GPU backends (DXIL, DXBC, MSL, SPIR-V) and packages them with reflection metadata into a single binary format.

Built on [SDL_shadercross](https://github.com/libsdl-org/SDL_shadercross).

## Features

- **Multi-backend compilation**: Single HLSL source compiles to DXIL, DXBC, Metal, and SPIR-V
- **Reflection metadata**: Extracts input/output bindings, resource counts, thread dimensions
- **Binary format**: Compact `.nshader` files contain all backends + metadata
- **SDL3 GPU integration**: Optional helpers for creating `SDL_GPUShader` and `SDL_GPUComputePipeline`
- **CLI tool**: Compile, inspect, and extract shaders from command line

## Quick Start

```bash
# Compile graphics shader
nshader compile shader.hlsl -o shader.nshader \
  --vertex main_vs --fragment main_fs

# Inspect shader
nshader info shader.nshader -v
```

```c
#include <nshader.h>

// Load at runtime
nshader_t* shader = nshader_read_from_path("shader.nshader");
const nshader_info_t* info = nshader_get_info(shader);

// Get compiled blob for specific backend/stage
const nshader_blob_t* blob = nshader_get_blob(shader,
    NSHADER_STAGE_TYPE_VERTEX, NSHADER_BACKEND_SPV);

nshader_destroy(shader);
```

## Documentation

- [Getting Started](getting-started.md) - Build, usage, pitfalls
- [CLI Reference](cli.md) - Command-line tool documentation

### Header Reference

| Header | Description |
|--------|-------------|
| [nshader_base.h](headers/nshader_base.md) | Memory allocation, format constants |
| [nshader_info.h](headers/nshader_info.md) | Types and metadata structures |
| [nshader_compiler.h](headers/nshader_compiler.md) | HLSL compilation |
| [nshader_reader.h](headers/nshader_reader.md) | Loading shaders |
| [nshader_writer.h](headers/nshader_writer.md) | Saving shaders |
| [nshader_type.h](headers/nshader_type.md) | Core opaque type and accessors |
| [nshader_sdl3_gpu.h](headers/nshader_sdl3_gpu.md) | SDL3 GPU integration |

## Source

[GitHub Repository](https://github.com/luppichristian/nshader)
