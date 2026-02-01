# nshader_reader.h

Deserialization of `.nshader` binary files to runtime objects.

## Purpose

Loads pre-compiled shader packages at runtime. Parses the binary format, validates magic/version, and reconstructs `nshader_t` with all backends and metadata.

## API

```c
// Load from memory buffer
nshader_t* nshader_read_from_memory(const void* buffer, size_t size);

// Load from open file handle
nshader_t* nshader_read_from_file(FILE* file);

// Load from filesystem path
nshader_t* nshader_read_from_path(const char* filepath);

// Free shader and all associated memory
void nshader_destroy(nshader_t* shader);
```

All functions return NULL on failure (invalid format, missing file, allocation failure).

## Memory Ownership

`nshader_destroy()` frees:
- The `nshader_t` structure
- All stage metadata (bindings, entry point strings)
- All compiled blobs for all backends

**Do not hold pointers** to `nshader_info_t`, `nshader_blob_t`, or binding arrays after calling `nshader_destroy()`.

## Example

```c
// Runtime loading
nshader_t* shader = nshader_read_from_path("assets/shaders/sprite.nshader");
if (!shader) {
    // Handle error - file not found or corrupt
    return false;
}

const nshader_info_t* info = nshader_get_info(shader);
// Use shader...

nshader_destroy(shader);
```

## Design Notes

- File format is little-endian, platform-independent
- Validates `NSHADER_MAGIC` and `NSHADER_VERSION` on load
- Memory is allocated via `nshader_malloc`; integrates with custom allocators
