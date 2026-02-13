/*
MIT License

Copyright (c) 2026 Christian Luppi

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <nshader/nshader_compiler.h>
#include "nshader_type_internal.h"
#include <SDL3_shadercross/SDL_shadercross.h>
#include <SDL3/SDL.h>
#include <string.h>

// #############################################################################
// Error List Management
// #############################################################################

NSHADER_API void nshader_error_list_push(nshader_error_list_t* error_list, const char* msg) {
  if (!error_list) {
    return;
  }

  size_t new_count = error_list->num_errors + 1;
  const char** new_errors = (const char**)nshader_realloc(
    (void*)error_list->errors,
    new_count * sizeof(char*)
  );

  if (!new_errors) {
    return;
  }

  // Duplicate the error message
  size_t len = strlen(msg);
  char* error_copy = (char*)nshader_malloc(len + 1);
  if (!error_copy) {
    return;
  }
  memcpy(error_copy, msg, len + 1);

  new_errors[error_list->num_errors] = error_copy;
  error_list->errors = new_errors;
  error_list->num_errors = new_count;
}

NSHADER_API void nshader_error_list_free(nshader_error_list_t* error_list) {
  if (!error_list) {
    return;
  }

  for (size_t i = 0; i < error_list->num_errors; ++i) {
    nshader_free((void*)error_list->errors[i]);
  }

  nshader_free((void*)error_list->errors);
  error_list->errors = NULL;
  error_list->num_errors = 0;
}

// #############################################################################
// Helper Functions
// #############################################################################

static SDL_ShaderCross_ShaderStage nshader_stage_to_sdl(nshader_stage_type_t stage) {
  switch (stage) {
    case NSHADER_STAGE_TYPE_VERTEX:
      return SDL_SHADERCROSS_SHADERSTAGE_VERTEX;
    case NSHADER_STAGE_TYPE_FRAGMENT:
      return SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT;
    case NSHADER_STAGE_TYPE_COMPUTE:
      return SDL_SHADERCROSS_SHADERSTAGE_COMPUTE;
    default:
      return SDL_SHADERCROSS_SHADERSTAGE_VERTEX;
  }
}

static nshader_binding_type_t sdl_iovar_type_to_nshader(SDL_ShaderCross_IOVarType type) {
  switch (type) {
    case SDL_SHADERCROSS_IOVAR_TYPE_INT8:
      return NSHADER_BINDING_TYPE_INT8;
    case SDL_SHADERCROSS_IOVAR_TYPE_UINT8:
      return NSHADER_BINDING_TYPE_UINT8;
    case SDL_SHADERCROSS_IOVAR_TYPE_INT16:
      return NSHADER_BINDING_TYPE_INT16;
    case SDL_SHADERCROSS_IOVAR_TYPE_UINT16:
      return NSHADER_BINDING_TYPE_UINT16;
    case SDL_SHADERCROSS_IOVAR_TYPE_INT32:
      return NSHADER_BINDING_TYPE_INT32;
    case SDL_SHADERCROSS_IOVAR_TYPE_UINT32:
      return NSHADER_BINDING_TYPE_UINT32;
    case SDL_SHADERCROSS_IOVAR_TYPE_INT64:
      return NSHADER_BINDING_TYPE_INT64;
    case SDL_SHADERCROSS_IOVAR_TYPE_UINT64:
      return NSHADER_BINDING_TYPE_UINT64;
    case SDL_SHADERCROSS_IOVAR_TYPE_FLOAT16:
      return NSHADER_BINDING_TYPE_FLOAT16;
    case SDL_SHADERCROSS_IOVAR_TYPE_FLOAT32:
      return NSHADER_BINDING_TYPE_FLOAT32;
    case SDL_SHADERCROSS_IOVAR_TYPE_FLOAT64:
      return NSHADER_BINDING_TYPE_FLOAT64;
    default:
      return NSHADER_BINDING_TYPE_FLOAT32;
  }
}

// #############################################################################
// Stage Compilation
// #############################################################################

typedef struct compiled_stage_t {
  nshader_stage_type_t stage_type;
  char* entry_point;

  // SPIRV bytecode
  uint8_t* spirv_data;
  size_t spirv_size;

  // Backend blobs
  uint8_t* dxil_data;
  size_t dxil_size;

  uint8_t* dxbc_data;
  size_t dxbc_size;

  uint8_t* msl_data;
  size_t msl_size;

  uint8_t* spv_data;
  size_t spv_size;

  // Metadata for graphics stages
  SDL_ShaderCross_GraphicsShaderMetadata* graphics_metadata;

  // Metadata for compute stage
  SDL_ShaderCross_ComputePipelineMetadata* compute_metadata;
} compiled_stage_t;

static void free_compiled_stage(compiled_stage_t* stage) {
  if (!stage) {
    return;
  }

  if (stage->entry_point) {
    nshader_free(stage->entry_point);
  }

  if (stage->spirv_data) {
    SDL_free(stage->spirv_data);
  }

  if (stage->dxil_data) {
    SDL_free(stage->dxil_data);
  }

  if (stage->dxbc_data) {
    SDL_free(stage->dxbc_data);
  }

  if (stage->msl_data) {
    SDL_free(stage->msl_data);
  }

  if (stage->spv_data) {
    SDL_free(stage->spv_data);
  }

  if (stage->graphics_metadata) {
    SDL_free(stage->graphics_metadata);
  }

  if (stage->compute_metadata) {
    SDL_free(stage->compute_metadata);
  }
}

static bool compile_stage_to_spirv(
    const nshader_compiler_config_t* config,
    const nshader_compiler_stage_setup_t* stage_setup,
    compiled_stage_t* out_stage,
    nshader_error_list_t* out_errors) {

  // Create properties for compilation
  SDL_PropertiesID props = SDL_CreateProperties();

  if (config->enable_debug) {
    SDL_SetBooleanProperty(props, SDL_SHADERCROSS_PROP_SHADER_DEBUG_ENABLE_BOOLEAN, true);

    if (config->debug_name) {
      SDL_SetStringProperty(props, SDL_SHADERCROSS_PROP_SHADER_DEBUG_NAME_STRING, config->debug_name);
    }
  }

  if (config->preserve_unused_bindings) {
    SDL_SetBooleanProperty(props, SDL_SHADERCROSS_PROP_SHADER_CULL_UNUSED_BINDINGS_BOOLEAN, true);
  }

  // Build defines array
  SDL_ShaderCross_HLSL_Define* sdl_defines = NULL;
  size_t total_defines = config->num_defines + stage_setup->num_defines;

  if (total_defines > 0) {
    sdl_defines = (SDL_ShaderCross_HLSL_Define*)nshader_calloc(
      total_defines + 1,
      sizeof(SDL_ShaderCross_HLSL_Define)
    );

    if (!sdl_defines) {
      nshader_error_list_push(out_errors, "Failed to allocate memory for defines");
      SDL_DestroyProperties(props);
      return false;
    }

    // Add global defines
    for (size_t i = 0; i < config->num_defines; ++i) {
      sdl_defines[i].name = (char*)config->defines[i].name;
      sdl_defines[i].value = (char*)config->defines[i].value;
    }

    // Add stage-specific defines
    for (size_t i = 0; i < stage_setup->num_defines; ++i) {
      sdl_defines[config->num_defines + i].name = (char*)stage_setup->defines[i].name;
      sdl_defines[config->num_defines + i].value = (char*)stage_setup->defines[i].value;
    }

    // Null terminator
    sdl_defines[total_defines].name = NULL;
    sdl_defines[total_defines].value = NULL;
  }

  // Setup HLSL info
  SDL_ShaderCross_HLSL_Info hlsl_info = {0};
  hlsl_info.source = stage_setup->source_code;
  hlsl_info.entrypoint = stage_setup->entry_point;
  hlsl_info.include_dir = config->include_dir;
  hlsl_info.defines = sdl_defines;
  hlsl_info.shader_stage = nshader_stage_to_sdl(stage_setup->stage_type);
  hlsl_info.props = props;

  // Compile to SPIRV
  size_t spirv_size = 0;
  void* spirv_data = SDL_ShaderCross_CompileSPIRVFromHLSL(&hlsl_info, &spirv_size);

  nshader_free(sdl_defines);
  SDL_DestroyProperties(props);

  if (!spirv_data) {
    const char* sdl_error = SDL_GetError();
    if (sdl_error && sdl_error[0] != '\0') {
      nshader_error_list_push(out_errors, sdl_error);
    } else {
      nshader_error_list_push(out_errors, "Failed to compile HLSL to SPIRV");
    }
    return false;
  }

  // Store SPIRV data
  out_stage->spirv_data = (uint8_t*)spirv_data;
  out_stage->spirv_size = spirv_size;
  out_stage->stage_type = stage_setup->stage_type;

  // Duplicate entry point
  size_t entry_len = strlen(stage_setup->entry_point);
  out_stage->entry_point = (char*)nshader_malloc(entry_len + 1);
  if (out_stage->entry_point) {
    memcpy(out_stage->entry_point, stage_setup->entry_point, entry_len + 1);
  }

  return true;
}

static bool compile_backends(
    const nshader_compiler_config_t* config,
    compiled_stage_t* stage,
    nshader_error_list_t* out_errors) {

  // Create SPIRV info
  SDL_ShaderCross_SPIRV_Info spirv_info = {0};
  spirv_info.bytecode = stage->spirv_data;
  spirv_info.bytecode_size = stage->spirv_size;
  spirv_info.entrypoint = stage->entry_point;
  spirv_info.shader_stage = nshader_stage_to_sdl(stage->stage_type);
  spirv_info.props = 0;

  // Compile to DXIL
  if (!config->disable_dxil) {
    size_t size = 0;
    void* data = SDL_ShaderCross_CompileDXILFromSPIRV(&spirv_info, &size);
    if (data) {
      stage->dxil_data = (uint8_t*)data;
      stage->dxil_size = size;
    } else {
      const char* sdl_error = SDL_GetError();
      char error_msg[512];
      SDL_snprintf(error_msg, sizeof(error_msg), "Failed to compile to DXIL: %s",
                   sdl_error ? sdl_error : "unknown error");
      nshader_error_list_push(out_errors, error_msg);
    }
  }

  // Compile to DXBC
  if (!config->disable_dxbc) {
    size_t size = 0;
    void* data = SDL_ShaderCross_CompileDXBCFromSPIRV(&spirv_info, &size);
    if (data) {
      stage->dxbc_data = (uint8_t*)data;
      stage->dxbc_size = size;
    } else {
      const char* sdl_error = SDL_GetError();
      char error_msg[512];
      SDL_snprintf(error_msg, sizeof(error_msg), "Failed to compile to DXBC: %s",
                   sdl_error ? sdl_error : "unknown error");
      nshader_error_list_push(out_errors, error_msg);
    }
  }

  // Compile to MSL
  if (!config->disable_msl) {
    void* data = SDL_ShaderCross_TranspileMSLFromSPIRV(&spirv_info);
    if (data) {
      size_t size = strlen((const char*)data) + 1;
      stage->msl_data = (uint8_t*)data;
      stage->msl_size = size;
    } else {
      const char* sdl_error = SDL_GetError();
      char error_msg[512];
      SDL_snprintf(error_msg, sizeof(error_msg), "Failed to transpile to MSL: %s",
                   sdl_error ? sdl_error : "unknown error");
      nshader_error_list_push(out_errors, error_msg);
    }
  }

  // Keep SPIRV
  if (!config->disable_spv) {
    stage->spv_data = (uint8_t*)nshader_malloc(stage->spirv_size);
    if (stage->spv_data) {
      memcpy(stage->spv_data, stage->spirv_data, stage->spirv_size);
      stage->spv_size = stage->spirv_size;
    }
  }

  return true;
}

static bool reflect_stage_metadata(
    compiled_stage_t* stage,
    nshader_error_list_t* out_errors) {

  if (stage->stage_type == NSHADER_STAGE_TYPE_COMPUTE) {
    // Reflect compute metadata
    stage->compute_metadata = SDL_ShaderCross_ReflectComputeSPIRV(
      stage->spirv_data,
      stage->spirv_size,
      0
    );

    if (!stage->compute_metadata) {
      nshader_error_list_push(out_errors, "Failed to reflect compute shader metadata");
      return false;
    }
  } else {
    // Reflect graphics metadata
    stage->graphics_metadata = SDL_ShaderCross_ReflectGraphicsSPIRV(
      stage->spirv_data,
      stage->spirv_size,
      0
    );

    if (!stage->graphics_metadata) {
      nshader_error_list_push(out_errors, "Failed to reflect graphics shader metadata");
      return false;
    }
  }

  return true;
}

// #############################################################################
// Main Compilation Function
// #############################################################################

NSHADER_API nshader_t* nshader_compiler_compile_hlsl(
    const nshader_compiler_config_t* config,
    nshader_error_list_t* out_errors) {

  if (!config || config->num_stages == 0) {
    if (out_errors) {
      nshader_error_list_push(out_errors, "Invalid compiler configuration");
    }
    return NULL;
  }

  // Initialize SDL_shadercross
  if (!SDL_ShaderCross_Init()) {
    if (out_errors) {
      nshader_error_list_push(out_errors, "Failed to initialize SDL_shadercross");
    }
    return NULL;
  }

  // Allocate compiled stages
  compiled_stage_t* stages = (compiled_stage_t*)nshader_calloc(
    config->num_stages,
    sizeof(compiled_stage_t)
  );

  if (!stages) {
    if (out_errors) {
      nshader_error_list_push(out_errors, "Failed to allocate memory for compiled stages");
    }
    SDL_ShaderCross_Quit();
    return NULL;
  }

  // Compile each stage
  bool compilation_failed = false;
  for (size_t i = 0; i < config->num_stages; ++i) {
    if (!compile_stage_to_spirv(config, &config->stages[i], &stages[i], out_errors)) {
      compilation_failed = true;
      break;
    }

    if (!compile_backends(config, &stages[i], out_errors)) {
      compilation_failed = true;
      break;
    }

    if (!reflect_stage_metadata(&stages[i], out_errors)) {
      compilation_failed = true;
      break;
    }
  }

  // If compilation failed, cleanup and return
  if (compilation_failed) {
    for (size_t i = 0; i < config->num_stages; ++i) {
      free_compiled_stage(&stages[i]);
    }
    nshader_free(stages);
    SDL_ShaderCross_Quit();
    return NULL;
  }

  // Create nshader_t object
  nshader_t* shader = (nshader_t*)nshader_calloc(1, sizeof(nshader_t));
  if (!shader) {
    if (out_errors) {
      nshader_error_list_push(out_errors, "Failed to allocate nshader_t object");
    }
    for (size_t i = 0; i < config->num_stages; ++i) {
      free_compiled_stage(&stages[i]);
    }
    nshader_free(stages);
    SDL_ShaderCross_Quit();
    return NULL;
  }

  // Determine shader type from first stage
  shader->info.type = nshader_stage_type_to_shader_type(stages[0].stage_type);

  // Allocate stage info
  shader->info.num_stages = config->num_stages;
  shader->info.stages = (nshader_stage_t*)nshader_calloc(
    config->num_stages,
    sizeof(nshader_stage_t)
  );

  if (!shader->info.stages) {
    if (out_errors) {
      nshader_error_list_push(out_errors, "Failed to allocate stage info");
    }
    nshader_free(shader);
    for (size_t i = 0; i < config->num_stages; ++i) {
      free_compiled_stage(&stages[i]);
    }
    nshader_free(stages);
    SDL_ShaderCross_Quit();
    return NULL;
  }

  // Determine available backends
  nshader_backend_t available_backends[NSHADER_BACKEND_COUNT];
  size_t num_backends = 0;

  if (!config->disable_dxil && stages[0].dxil_data) {
    available_backends[num_backends++] = NSHADER_BACKEND_DXIL;
  }
  if (!config->disable_dxbc && stages[0].dxbc_data) {
    available_backends[num_backends++] = NSHADER_BACKEND_DXBC;
  }
  if (!config->disable_msl && stages[0].msl_data) {
    available_backends[num_backends++] = NSHADER_BACKEND_MSL;
  }
  if (!config->disable_spv && stages[0].spv_data) {
    available_backends[num_backends++] = NSHADER_BACKEND_SPV;
  }

  shader->info.num_backends = num_backends;
  shader->info.backends = (nshader_backend_t*)nshader_malloc(
    num_backends * sizeof(nshader_backend_t)
  );

  if (!shader->info.backends) {
    if (out_errors) {
      nshader_error_list_push(out_errors, "Failed to allocate backend info");
    }
    nshader_free(shader->info.stages);
    nshader_free(shader);
    for (size_t i = 0; i < config->num_stages; ++i) {
      free_compiled_stage(&stages[i]);
    }
    nshader_free(stages);
    SDL_ShaderCross_Quit();
    return NULL;
  }

  memcpy(shader->info.backends, available_backends, num_backends * sizeof(nshader_backend_t));

  // Fill in stage metadata and blobs
  for (size_t stage_idx = 0; stage_idx < config->num_stages; ++stage_idx) {
    compiled_stage_t* compiled = &stages[stage_idx];
    nshader_stage_t* stage_info = &shader->info.stages[stage_idx];

    stage_info->type = compiled->stage_type;
    stage_info->entry_point = compiled->entry_point;
    compiled->entry_point = NULL; // Transfer ownership

    // Fill metadata based on stage type
    if (compiled->stage_type == NSHADER_STAGE_TYPE_COMPUTE) {
      SDL_ShaderCross_ComputePipelineMetadata* sdl_meta = compiled->compute_metadata;
      stage_info->metadata.compute.num_samplers = sdl_meta->num_samplers;
      stage_info->metadata.compute.num_readonly_storage_textures = sdl_meta->num_readonly_storage_textures;
      stage_info->metadata.compute.num_readonly_storage_buffers = sdl_meta->num_readonly_storage_buffers;
      stage_info->metadata.compute.num_readwrite_storage_textures = sdl_meta->num_readwrite_storage_textures;
      stage_info->metadata.compute.num_readwrite_storage_buffers = sdl_meta->num_readwrite_storage_buffers;
      stage_info->metadata.compute.num_uniform_buffers = sdl_meta->num_uniform_buffers;
      stage_info->metadata.compute.threadcount_x = sdl_meta->threadcount_x;
      stage_info->metadata.compute.threadcount_y = sdl_meta->threadcount_y;
      stage_info->metadata.compute.threadcount_z = sdl_meta->threadcount_z;
    } else {
      SDL_ShaderCross_GraphicsShaderMetadata* sdl_meta = compiled->graphics_metadata;

      // Common metadata for vertex and fragment
      if (compiled->stage_type == NSHADER_STAGE_TYPE_VERTEX) {
        stage_info->metadata.vertex.num_samplers = sdl_meta->resource_info.num_samplers;
        stage_info->metadata.vertex.num_storage_textures = sdl_meta->resource_info.num_storage_textures;
        stage_info->metadata.vertex.num_storage_buffers = sdl_meta->resource_info.num_storage_buffers;
        stage_info->metadata.vertex.num_uniform_buffers = sdl_meta->resource_info.num_uniform_buffers;

        // Copy inputs
        stage_info->metadata.vertex.input_count = sdl_meta->num_inputs;
        if (sdl_meta->num_inputs > 0) {
          stage_info->metadata.vertex.inputs = (nshader_stage_binding_t*)nshader_calloc(
            sdl_meta->num_inputs,
            sizeof(nshader_stage_binding_t)
          );

          for (uint32_t i = 0; i < sdl_meta->num_inputs; ++i) {
            SDL_ShaderCross_IOVarMetadata* sdl_var = &sdl_meta->inputs[i];
            nshader_stage_binding_t* binding = &stage_info->metadata.vertex.inputs[i];

            size_t name_len = strlen(sdl_var->name);
            binding->name = (char*)nshader_malloc(name_len + 1);
            memcpy(binding->name, sdl_var->name, name_len + 1);
            binding->location = sdl_var->location;
            binding->vector_size = sdl_var->vector_size;
            binding->type = sdl_iovar_type_to_nshader(sdl_var->vector_type);
          }
        }

        // Copy outputs
        stage_info->metadata.vertex.output_count = sdl_meta->num_outputs;
        if (sdl_meta->num_outputs > 0) {
          stage_info->metadata.vertex.outputs = (nshader_stage_binding_t*)nshader_calloc(
            sdl_meta->num_outputs,
            sizeof(nshader_stage_binding_t)
          );

          for (uint32_t i = 0; i < sdl_meta->num_outputs; ++i) {
            SDL_ShaderCross_IOVarMetadata* sdl_var = &sdl_meta->outputs[i];
            nshader_stage_binding_t* binding = &stage_info->metadata.vertex.outputs[i];

            size_t name_len = strlen(sdl_var->name);
            binding->name = (char*)nshader_malloc(name_len + 1);
            memcpy(binding->name, sdl_var->name, name_len + 1);
            binding->location = sdl_var->location;
            binding->vector_size = sdl_var->vector_size;
            binding->type = sdl_iovar_type_to_nshader(sdl_var->vector_type);
          }
        }
      } else if (compiled->stage_type == NSHADER_STAGE_TYPE_FRAGMENT) {
        stage_info->metadata.fragment.num_samplers = sdl_meta->resource_info.num_samplers;
        stage_info->metadata.fragment.num_storage_textures = sdl_meta->resource_info.num_storage_textures;
        stage_info->metadata.fragment.num_storage_buffers = sdl_meta->resource_info.num_storage_buffers;
        stage_info->metadata.fragment.num_uniform_buffers = sdl_meta->resource_info.num_uniform_buffers;

        // Copy inputs
        stage_info->metadata.fragment.input_count = sdl_meta->num_inputs;
        if (sdl_meta->num_inputs > 0) {
          stage_info->metadata.fragment.inputs = (nshader_stage_binding_t*)nshader_calloc(
            sdl_meta->num_inputs,
            sizeof(nshader_stage_binding_t)
          );

          for (uint32_t i = 0; i < sdl_meta->num_inputs; ++i) {
            SDL_ShaderCross_IOVarMetadata* sdl_var = &sdl_meta->inputs[i];
            nshader_stage_binding_t* binding = &stage_info->metadata.fragment.inputs[i];

            size_t name_len = strlen(sdl_var->name);
            binding->name = (char*)nshader_malloc(name_len + 1);
            memcpy(binding->name, sdl_var->name, name_len + 1);
            binding->location = sdl_var->location;
            binding->vector_size = sdl_var->vector_size;
            binding->type = sdl_iovar_type_to_nshader(sdl_var->vector_type);
          }
        }

        // Copy outputs
        stage_info->metadata.fragment.output_count = sdl_meta->num_outputs;
        if (sdl_meta->num_outputs > 0) {
          stage_info->metadata.fragment.outputs = (nshader_stage_binding_t*)nshader_calloc(
            sdl_meta->num_outputs,
            sizeof(nshader_stage_binding_t)
          );

          for (uint32_t i = 0; i < sdl_meta->num_outputs; ++i) {
            SDL_ShaderCross_IOVarMetadata* sdl_var = &sdl_meta->outputs[i];
            nshader_stage_binding_t* binding = &stage_info->metadata.fragment.outputs[i];

            size_t name_len = strlen(sdl_var->name);
            binding->name = (char*)nshader_malloc(name_len + 1);
            memcpy(binding->name, sdl_var->name, name_len + 1);
            binding->location = sdl_var->location;
            binding->vector_size = sdl_var->vector_size;
            binding->type = sdl_iovar_type_to_nshader(sdl_var->vector_type);
          }
        }
      }
    }

    // Store blobs for each backend
    for (size_t backend_idx = 0; backend_idx < num_backends; ++backend_idx) {
      nshader_backend_t backend = available_backends[backend_idx];
      nshader_blob_t* blob = (nshader_blob_t*)nshader_malloc(sizeof(nshader_blob_t));

      if (!blob) {
        continue;
      }

      uint8_t* data_to_copy = NULL;
      size_t size_to_copy = 0;

      switch (backend) {
        case NSHADER_BACKEND_DXIL:
          data_to_copy = compiled->dxil_data;
          size_to_copy = compiled->dxil_size;
          break;
        case NSHADER_BACKEND_DXBC:
          data_to_copy = compiled->dxbc_data;
          size_to_copy = compiled->dxbc_size;
          break;
        case NSHADER_BACKEND_MSL:
          data_to_copy = compiled->msl_data;
          size_to_copy = compiled->msl_size;
          break;
        case NSHADER_BACKEND_SPV:
          data_to_copy = compiled->spv_data;
          size_to_copy = compiled->spv_size;
          break;
        default:
          break;
      }

      if (data_to_copy && size_to_copy > 0) {
        uint8_t* blob_data = (uint8_t*)nshader_malloc(size_to_copy);
        if (blob_data) {
          memcpy(blob_data, data_to_copy, size_to_copy);
          blob->data = blob_data;
          blob->size = size_to_copy;
          shader->blobs[compiled->stage_type][backend] = blob;
        } else {
          nshader_free(blob);
        }
      } else {
        nshader_free(blob);
      }
    }
  }

  // Cleanup compiled stages
  for (size_t i = 0; i < config->num_stages; ++i) {
    free_compiled_stage(&stages[i]);
  }
  nshader_free(stages);

  SDL_ShaderCross_Quit();

  return shader;
}
