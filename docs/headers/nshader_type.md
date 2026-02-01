---
layout: default
title: nshader_type.h
---

# nshader_type.h

Core opaque type and accessor functions for shader objects.

## Purpose

Defines `nshader_t` as an opaque handle and provides read-only access to compiled data. Separates the internal representation from the public API.

## Types

### nshader_t
Opaque shader object. Created by compiler or reader, destroyed with `nshader_destroy()`.

### nshader_blob_t
Compiled shader bytecode:
- `size` - byte count
- `data` - pointer to bytecode (owned by `nshader_t`)

## API

```c
// Get shader metadata (type, stages, backends)
const nshader_info_t* nshader_get_info(const nshader_t* shader);

// Get compiled bytecode for specific stage/backend combination
const nshader_blob_t* nshader_get_blob(
    const nshader_t* shader,
    nshader_stage_type_t stage,
    nshader_backend_t backend);

// Check if backend is available in this shader
bool nshader_has_backend(const nshader_t* shader, nshader_backend_t backend);

// Check if stage is present in this shader
bool nshader_has_stage(const nshader_t* shader, nshader_stage_type_t stage);
```

## Example

```c
nshader_t* shader = nshader_read_from_path("shader.nshader");
const nshader_info_t* info = nshader_get_info(shader);

// Check capabilities
if (nshader_has_backend(shader, NSHADER_BACKEND_SPV)) {
    const nshader_blob_t* blob = nshader_get_blob(shader,
        NSHADER_STAGE_TYPE_VERTEX, NSHADER_BACKEND_SPV);

    // blob->data contains SPIR-V bytecode
    // blob->size is byte count
}
```

## Design Notes

- All returned pointers are owned by the `nshader_t`; valid until `nshader_destroy()`
- `nshader_get_blob()` returns NULL if stage/backend combination doesn't exist
- Info and blob queries are O(1) lookups
