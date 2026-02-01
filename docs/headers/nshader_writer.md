---
layout: default
title: nshader_writer.h
---

# nshader_writer.h

Serialization of `nshader_t` objects to binary format.

## Purpose

Saves compiled shaders to the `.nshader` binary format for distribution and runtime loading. Used after compilation or for shader packaging tools.

## API

```c
// Write to memory buffer; returns bytes written, 0 on failure
// Pass NULL buffer to query required size
size_t nshader_write_to_memory(const nshader_t* shader, void* buffer, size_t buffer_size);

// Write to open file handle
bool nshader_write_to_file(const nshader_t* shader, FILE* file);

// Write to filesystem path
bool nshader_write_to_path(const nshader_t* shader, const char* filepath);
```

## Size Query Pattern

```c
// Query size first
size_t size = nshader_write_to_memory(shader, NULL, 0);
void* buffer = malloc(size);

// Then write
nshader_write_to_memory(shader, buffer, size);
```

## Example

```c
// After compilation
nshader_t* shader = nshader_compiler_compile_hlsl(&config, NULL);

// Save to file
if (!nshader_write_to_path(shader, "output/shader.nshader")) {
    fprintf(stderr, "Failed to write shader\n");
}

nshader_destroy(shader);
```

## Design Notes

- Output is deterministic; same input produces identical binary
- Format includes magic number and version for validation on load
- All backends and stages are included in single file
