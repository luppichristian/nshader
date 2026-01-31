#pragma once

#include "nshader_base.h"

// #############################################################################
NSHADER_HEADER_BEGIN;
// #############################################################################

typedef enum nshader_backend_t {
  NSHADER_BACKEND_DXIL,  // DirectX Intermediate Language
  NSHADER_BACKEND_DXBC,  // DirectX Bytecode
  NSHADER_BACKEND_MSL,   // Metal Shading Language
  NSHADER_BACKEND_SPV,   // SPIR-V
  NSHADER_BACKEND_COUNT
} nshader_backend_t;

// Utility to convert backend enum to file extension
NSHADER_API const char* nshader_backend_to_extension(nshader_backend_t backend);

// #############################################################################

typedef enum nshader_type_t {
  NSHADER_SHADER_TYPE_GRAPHICS,
  NSHADER_SHADER_TYPE_COMPUTE,
  NSHADER_SHADER_TYPE_COUNT
} nshader_type_t;

// Utility to convert backend enum to string
NSHADER_API const char* nshader_backend_to_string(nshader_backend_t backend);

// #############################################################################

typedef enum nshader_stage_type_t {
  NSHADER_STAGE_TYPE_VERTEX,
  NSHADER_STAGE_TYPE_FRAGMENT,
  NSHADER_STAGE_TYPE_COMPUTE,
  NSHADER_STAGE_TYPE_COUNT
} nshader_stage_type_t;

// Utility to convert stage type enum to string
NSHADER_API const char* nshader_stage_type_to_string(nshader_stage_type_t stage_type);

// Utility to convert stage type to shader type
NSHADER_API nshader_type_t nshader_stage_type_to_shader_type(nshader_stage_type_t stage_type);

// #############################################################################

typedef enum nshader_binding_type_t {
  NSHADER_BINDING_TYPE_INT8,
  NSHADER_BINDING_TYPE_UINT8,
  NSHADER_BINDING_TYPE_INT16,
  NSHADER_BINDING_TYPE_UINT16,
  NSHADER_BINDING_TYPE_INT32,
  NSHADER_BINDING_TYPE_UINT32,
  NSHADER_BINDING_TYPE_INT64,
  NSHADER_BINDING_TYPE_UINT64,
  NSHADER_BINDING_TYPE_FLOAT16,
  NSHADER_BINDING_TYPE_FLOAT32,
  NSHADER_BINDING_TYPE_FLOAT64
} nshader_binding_type_t;

// Utility to convert binding type enum to string
NSHADER_API const char* nshader_binding_type_to_string(nshader_binding_type_t binding_type);

// #############################################################################

typedef struct nshader_stage_binding_t {
  char* name;                   // The UTF-8 name of the variable.
  uint32_t location;            // The location of the variable.
  uint32_t vector_size;         // The number of components in the vector type of the variable.
  nshader_binding_type_t type;  // The vector type of the variable.
} nshader_stage_binding_t;

typedef struct nshader_stage_metadata_vertex_t {
  // Stage related info
  uint32_t num_samplers;          // The number of samplers defined in the shader.
  uint32_t num_storage_textures;  // The number of storage textures defined in the shader.
  uint32_t num_storage_buffers;   // The number of storage buffers defined in the shader.
  uint32_t num_uniform_buffers;   // The number of uniform buffers defined in the shader.

  // Input bindings
  nshader_stage_binding_t* inputs;
  size_t input_count;

  // Output bindings
  nshader_stage_binding_t* outputs;
  size_t output_count;
} nshader_stage_metadata_vertex_t;

typedef struct nshader_stage_metadata_fragment_t {
  // Stage related info
  uint32_t num_samplers;          // The number of samplers defined in the shader.
  uint32_t num_storage_textures;  // The number of storage textures defined in the shader.
  uint32_t num_storage_buffers;   // The number of storage buffers defined in the shader.
  uint32_t num_uniform_buffers;   // The number of uniform buffers defined in the shader.

  // Input bindings
  nshader_stage_binding_t* inputs;
  size_t input_count;

  // Output bindings
  nshader_stage_binding_t* outputs;
  size_t output_count;
} nshader_stage_metadata_fragment_t;

typedef struct nshader_stage_metadata_compute_t {
  uint32_t num_samplers;                    // The number of samplers defined in the shader.
  uint32_t num_readonly_storage_textures;   // The number of readonly storage textures defined in the shader.
  uint32_t num_readonly_storage_buffers;    // The number of readonly storage buffers defined in the shader.
  uint32_t num_readwrite_storage_textures;  // The number of read-write storage textures defined in the shader.
  uint32_t num_readwrite_storage_buffers;   // The number of read-write storage buffers defined in the shader.
  uint32_t num_uniform_buffers;             // The number of uniform buffers defined in the shader.
  uint32_t threadcount_x;                   // The number of threads in the X dimension.
  uint32_t threadcount_y;                   // The number of threads in the Y dimension.
  uint32_t threadcount_z;                   // The number of threads in the Z dimension.
} nshader_stage_metadata_compute_t;

typedef union nshader_stage_metadata_t {
  nshader_stage_metadata_vertex_t vertex;      // NSHADER_STAGE_TYPE_VERTEX
  nshader_stage_metadata_fragment_t fragment;  // NSHADER_STAGE_TYPE_FRAGMENT
  nshader_stage_metadata_compute_t compute;    // NSHADER_STAGE_TYPE_COMPUTE
} nshader_stage_metadata_t;

// #############################################################################

typedef struct nshader_stage_t {
  // Stage type
  nshader_stage_type_t type;

  // Entry point function name
  const char* entry_point;

  // Accessible metadata depending on stage type
  nshader_stage_metadata_t metadata;
} nshader_stage_t;

// #############################################################################

typedef struct nshader_info_t {
  // Shader type
  nshader_type_t type;

  // Included stages
  nshader_stage_t* stages;
  size_t num_stages;

  // Supported backends for this shader
  // These are basically the available compiled outputs from SDL_shadercross
  nshader_backend_t* backends;
  size_t num_backends;
} nshader_info_t;

// #############################################################################
NSHADER_HEADER_END;
// #############################################################################