---
layout: default
title: nshader_base.h
---

# nshader_base.h

Foundation header providing memory allocation hooks and binary format constants.

## Purpose

Enables custom memory allocators for embedded/game engines with strict memory requirements. Defines the binary format version for forward compatibility.

## Memory Allocation

```c
// Override before any nshader calls
nshader_set_memory_fns(my_malloc, my_free, my_calloc, my_realloc);
```

Default implementations use standard library functions. Override is **not thread-safe**; call once at startup before any nshader operations.

All internal allocations route through these functions:
- `nshader_malloc(size)` - allocate
- `nshader_free(ptr)` - deallocate
- `nshader_calloc(num, size)` - allocate zeroed
- `nshader_realloc(ptr, new_size)` - resize

## Format Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `NSHADER_MAGIC` | `0x5244534E` | "NSDR" in little-endian; file identifier |
| `NSHADER_VERSION` | `1` | Binary format version |

## API Visibility

`NSHADER_API` macro handles symbol visibility:
- Shared builds: dllexport/dllimport on Windows, visibility("default") on GCC
- Static builds: no decoration

## When to Use

- Integrating with custom allocator (game engines, embedded)
- Building as shared library (define `NSHADER_SHARED`, `NSHADER_EXPORTS`)
- Checking binary format compatibility
