# nshader

Cross-platform shader compiler and runtime library. Compiles HLSL shaders to multiple GPU backends (DXIL, DXBC, MSL, SPIR-V) and packages them with reflection metadata into a single binary format for runtime loading.

Built on [SDL_shadercross](https://github.com/libsdl-org/SDL_shadercross) for shader compilation and cross-compilation.

## Features

- **Multi-backend compilation**: Single HLSL source compiles to DXIL, DXBC, Metal, and SPIR-V
- **Reflection metadata**: Extracts input/output bindings, resource counts, thread dimensions
- **Binary format**: Compact `.nshader` files contain all backends + metadata
- **SDL3 GPU integration**: Optional helpers for creating `SDL_GPUShader` and `SDL_GPUComputePipeline`
- **CLI tool**: Compile, inspect, and extract shaders from command line

## Requirements

- CMake 3.19+
- C11 compiler
- C++20 compiler (tests only)
- SDL3 (fetched automatically if not found)
- SDL_shadercross (fetched automatically if not found)

### Options

| Option | Default | Description |
|--------|---------|-------------|
| `NSHADER_BUILD_SHARED` | OFF | Build as shared library |
| `NSHADER_BUILD_TESTS` | ON | Build test suite |
| `NSHADER_BUILD_CLI` | ON | Build CLI tool |

## Documentation

See [docs](https://luppichristian.github.io/nshader/) for comprehensive API documentation and usage patterns.

## License

MIT License - see [LICENSE](LICENSE)
