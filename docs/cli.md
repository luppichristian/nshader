---
layout: default
title: CLI Reference
---

# CLI Reference

The `nshader` command-line tool provides shader compilation, inspection, and extraction capabilities.

## Synopsis

```
nshader <command> [options]
```

## Commands

| Command | Description |
|---------|-------------|
| [`compile`](#compile) | Compile HLSL shader to nshader format |
| [`info`](#info) | Display shader information |
| [`extract`](#extract) | Extract a specific backend and stage to a file |
| `help` | Display help message |
| `version` | Display version information |

---

## compile

Compile HLSL shader source files to the nshader binary format.

### Usage

```
nshader compile [<input.hlsl>] -o <output.nshader> [options]
```

### Source Files

| Option | Description |
|--------|-------------|
| `<input.hlsl>` | Default input file for all stages |
| `--vertex-source <file>` | Source file for vertex shader (overrides default) |
| `--fragment-source <file>` | Source file for fragment shader (overrides default) |
| `--compute-source <file>` | Source file for compute shader (overrides default) |

Each active stage requires a source from either the default input file or its stage-specific `--*-source` flag. This allows you to:
- Use a single source file for all stages (traditional approach)
- Use separate source files for each stage
- Mix both approaches (default source with overrides)

### Shader Stages

| Option | Description |
|--------|-------------|
| `--vertex <entry>` | Vertex shader entry point function name |
| `--fragment <entry>` | Fragment shader entry point function name |
| `--compute <entry>` | Compute shader entry point function name |

At least one shader stage must be specified.

### Preprocessor Defines

| Option | Description |
|--------|-------------|
| `-D <NAME[=VALUE]>` | Add preprocessor define (applies to all stages) |
| `--D-vertex <NAME[=VALUE]>` | Add define for vertex stage only |
| `--D-fragment <NAME[=VALUE]>` | Add define for fragment stage only |
| `--D-compute <NAME[=VALUE]>` | Add define for compute stage only |

Global defines (`-D`) are applied to all stages. Stage-specific defines (`--D-*`) are applied only to that particular stage. Both can be used together and combined.

### Options

| Option | Description |
|--------|-------------|
| `-o <file>` | Output nshader file (required) |
| `-I <directory>` | Include directory for shader code |
| `--debug` | Enable debug information in compiled shaders |
| `--debug-name <name>` | Set debug name for the shader |
| `--preserve-bindings` | Don't cull unused resource bindings |

### Backend Control

| Option | Description |
|--------|-------------|
| `--disable-dxil` | Disable DirectX Intermediate Language backend |
| `--disable-dxbc` | Disable DirectX Bytecode backend |
| `--disable-msl` | Disable Metal Shading Language backend |
| `--disable-spv` | Disable SPIR-V backend |

By default, all available backends are enabled. Use these flags to exclude specific backends from compilation.

### Examples

**Single source file for all stages:**
```bash
nshader compile shader.hlsl -o shader.nshader \
    --vertex VSMain --fragment PSMain
```

**Separate source files for each stage:**
```bash
nshader compile \
    --vertex VSMain --vertex-source vertex.hlsl \
    --fragment PSMain --fragment-source fragment.hlsl \
    -o shader.nshader
```

**Default source with override for one stage:**
```bash
nshader compile common.hlsl -o shader.nshader \
    --vertex VSMain \
    --fragment PSMain --fragment-source custom_pixel.hlsl
```

**Compute shader with defines:**
```bash
nshader compile blur.hlsl -o blur.nshader \
    --compute main -D KERNEL_SIZE=5 -D USE_SHARED_MEMORY
```

**Stage-specific defines:**
```bash
nshader compile shader.hlsl -o shader.nshader \
    --vertex VSMain --D-vertex INSTANCED=1 \
    --fragment PSMain --D-fragment USE_TEXTURES
```

**Combining global and stage-specific defines:**
```bash
nshader compile shader.hlsl -o shader.nshader \
    -D DEBUG=1 \
    --vertex VSMain --D-vertex MAX_BONES=64 \
    --fragment PSMain --D-fragment ENABLE_SHADOWS
```

**Selective backend compilation:**
```bash
nshader compile shader.hlsl -o shader.nshader \
    --vertex main --disable-dxbc --disable-msl
```

**With include directory and debug info:**
```bash
nshader compile shader.hlsl -o shader.nshader \
    --vertex VSMain --fragment PSMain \
    -I ./shaders/include \
    --debug --debug-name "MainShader"
```

---

## info

Display information about a compiled nshader file.

### Usage

```
nshader info <shader.nshader> [options]
```

### Options

| Option | Description |
|--------|-------------|
| `-v`, `--verbose` | Show detailed information including bindings and metadata |

### Examples

**Basic info:**
```bash
nshader info shader.nshader
```

Output:
```
Shader: shader.nshader
Type: Graphics
Backends (3):
  - DXIL
  - MSL
  - SPIR-V
Stages (2):
  Vertex:
    Entry Point: VSMain
  Fragment:
    Entry Point: PSMain
```

**Verbose info:**
```bash
nshader info shader.nshader --verbose
```

Output includes additional details like sampler counts, storage buffers, uniform buffers, and input/output bindings for each stage.

---

## extract

Extract a specific backend and stage from a compiled nshader file to a separate file.

### Usage

```
nshader extract <shader.nshader> <backend> <stage> -o <output>
```

### Backends

| Value | Description |
|-------|-------------|
| `dxil` | DirectX Intermediate Language |
| `dxbc` | DirectX Bytecode |
| `msl` | Metal Shading Language |
| `spv` | SPIR-V |

### Stages

| Value | Description |
|-------|-------------|
| `vertex` | Vertex shader stage |
| `fragment` | Fragment shader stage |
| `compute` | Compute shader stage |

### Examples

**Extract SPIR-V vertex shader:**
```bash
nshader extract shader.nshader spv vertex -o vertex.spv
```

**Extract Metal fragment shader:**
```bash
nshader extract shader.nshader msl fragment -o fragment.metal
```

**Extract DXIL compute shader:**
```bash
nshader extract shader.nshader dxil compute -o compute.dxil
```

---

## Exit Codes

| Code | Description |
|------|-------------|
| 0 | Success |
| 1 | Error (invalid arguments, compilation failure, file not found, etc.) |

---

## Environment

The CLI tool uses SDL_shadercross for compilation, which may have platform-specific backend availability:

| Backend | Windows | macOS | Linux |
|---------|---------|-------|-------|
| DXIL | Yes | No | No |
| DXBC | Yes | No | No |
| MSL | Yes | Yes | Yes |
| SPIR-V | Yes | Yes | Yes |

Use `nshader info` to check which backends are present in a compiled shader.
