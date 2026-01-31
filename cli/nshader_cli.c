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

#include <nshader.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// #############################################################################
// Version
// #############################################################################

#define NSHADER_CLI_VERSION "1.0.0"

// #############################################################################
// Utilities
// #############################################################################

static char* read_file_to_string(const char* filepath) {
  FILE* file = fopen(filepath, "rb");
  if (!file) {
    fprintf(stderr, "Error: Could not open file '%s'\n", filepath);
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char* buffer = (char*)malloc(size + 1);
  if (!buffer) {
    fprintf(stderr, "Error: Memory allocation failed\n");
    fclose(file);
    return NULL;
  }

  size_t read_size = fread(buffer, 1, size, file);
  buffer[read_size] = '\0';

  fclose(file);
  return buffer;
}

static bool write_blob_to_file(const char* filepath, const void* data, size_t size) {
  FILE* file = fopen(filepath, "wb");
  if (!file) {
    fprintf(stderr, "Error: Could not open file '%s' for writing\n", filepath);
    return false;
  }

  size_t written = fwrite(data, 1, size, file);
  fclose(file);

  if (written != size) {
    fprintf(stderr, "Error: Could not write complete data to file '%s'\n", filepath);
    return false;
  }

  return true;
}

// #############################################################################
// Help Messages
// #############################################################################

static void print_help(void) {
  printf("nshader - Cross-platform shader compiler and utility\n");
  printf("Version: %s\n\n", NSHADER_CLI_VERSION);
  printf("USAGE:\n");
  printf("  nshader <command> [options]\n\n");
  printf("COMMANDS:\n");
  printf("  compile <input.hlsl> -o <output.nshader>\n");
  printf("      Compile HLSL shader to nshader format\n\n");
  printf("  info <shader.nshader>\n");
  printf("      Display shader information\n\n");
  printf("  extract <shader.nshader> <backend> <stage> -o <output>\n");
  printf("      Extract a specific backend and stage to a file\n\n");
  printf("  help\n");
  printf("      Display this help message\n\n");
  printf("  version\n");
  printf("      Display version information\n\n");
  printf("For command-specific help, run: nshader <command> --help\n");
}

static void print_compile_help(void) {
  printf("nshader compile - Compile HLSL shader to nshader format\n\n");
  printf("USAGE:\n");
  printf("  nshader compile <input.hlsl> -o <output.nshader> [options]\n\n");
  printf("REQUIRED:\n");
  printf("  <input.hlsl>          Input HLSL shader file\n");
  printf("  -o <output.nshader>   Output nshader file\n\n");
  printf("SHADER STAGES:\n");
  printf("  --vertex <entry>      Vertex shader entry point\n");
  printf("  --fragment <entry>    Fragment shader entry point\n");
  printf("  --compute <entry>     Compute shader entry point\n\n");
  printf("OPTIONS:\n");
  printf("  -D <NAME[=VALUE]>     Add preprocessor define\n");
  printf("  -I <directory>        Include directory for shader code\n");
  printf("  --debug               Enable debug information\n");
  printf("  --debug-name <name>   Set debug name\n");
  printf("  --preserve-bindings   Don't cull unused resource bindings\n\n");
  printf("BACKEND CONTROL:\n");
  printf("  --disable-dxil        Disable DirectX IL backend\n");
  printf("  --disable-dxbc        Disable DirectX Bytecode backend\n");
  printf("  --disable-msl         Disable Metal Shading Language backend\n");
  printf("  --disable-spv         Disable SPIR-V backend\n");
}

static void print_info_help(void) {
  printf("nshader info - Display shader information\n\n");
  printf("USAGE:\n");
  printf("  nshader info <shader.nshader> [options]\n\n");
  printf("OPTIONS:\n");
  printf("  -v, --verbose         Show detailed information\n");
}

static void print_extract_help(void) {
  printf("nshader extract - Extract a specific backend and stage\n\n");
  printf("USAGE:\n");
  printf("  nshader extract <shader.nshader> <backend> <stage> -o <output>\n\n");
  printf("BACKENDS:\n");
  printf("  dxil                  DirectX Intermediate Language\n");
  printf("  dxbc                  DirectX Bytecode\n");
  printf("  msl                   Metal Shading Language\n");
  printf("  spv                   SPIR-V\n\n");
  printf("STAGES:\n");
  printf("  vertex                Vertex shader stage\n");
  printf("  fragment              Fragment shader stage\n");
  printf("  compute               Compute shader stage\n");
}

// #############################################################################
// Compile Command
// #############################################################################

typedef struct compile_args_t {
  const char* input_file;
  const char* output_file;
  const char* vertex_entry;
  const char* fragment_entry;
  const char* compute_entry;
  const char* include_dir;
  const char* debug_name;

  nshader_compiler_define_t* defines;
  size_t num_defines;

  bool debug;
  bool preserve_bindings;
  bool disable_dxil;
  bool disable_dxbc;
  bool disable_msl;
  bool disable_spv;
} compile_args_t;

static bool parse_define(const char* str, char** name, char** value) {
  const char* eq = strchr(str, '=');
  if (eq) {
    size_t name_len = eq - str;
    *name = (char*)malloc(name_len + 1);
    memcpy(*name, str, name_len);
    (*name)[name_len] = '\0';

    *value = strdup(eq + 1);
  } else {
    *name = strdup(str);
    *value = NULL;
  }
  return true;
}

static int cmd_compile(int argc, char** argv) {
  compile_args_t args = {0};

  // Parse arguments
  for (int i = 2; i < argc; i++) {
    if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
      print_compile_help();
      return 0;
    } else if (strcmp(argv[i], "-o") == 0) {
      if (++i >= argc) {
        fprintf(stderr, "Error: -o requires an argument\n");
        return 1;
      }
      args.output_file = argv[i];
    } else if (strcmp(argv[i], "--vertex") == 0) {
      if (++i >= argc) {
        fprintf(stderr, "Error: --vertex requires an argument\n");
        return 1;
      }
      args.vertex_entry = argv[i];
    } else if (strcmp(argv[i], "--fragment") == 0) {
      if (++i >= argc) {
        fprintf(stderr, "Error: --fragment requires an argument\n");
        return 1;
      }
      args.fragment_entry = argv[i];
    } else if (strcmp(argv[i], "--compute") == 0) {
      if (++i >= argc) {
        fprintf(stderr, "Error: --compute requires an argument\n");
        return 1;
      }
      args.compute_entry = argv[i];
    } else if (strcmp(argv[i], "-D") == 0) {
      if (++i >= argc) {
        fprintf(stderr, "Error: -D requires an argument\n");
        return 1;
      }
      args.defines = (nshader_compiler_define_t*)realloc(args.defines,
        sizeof(nshader_compiler_define_t) * (args.num_defines + 1));

      char* name, *value;
      parse_define(argv[i], &name, &value);
      args.defines[args.num_defines].name = name;
      args.defines[args.num_defines].value = value;
      args.num_defines++;
    } else if (strcmp(argv[i], "-I") == 0) {
      if (++i >= argc) {
        fprintf(stderr, "Error: -I requires an argument\n");
        return 1;
      }
      args.include_dir = argv[i];
    } else if (strcmp(argv[i], "--debug") == 0) {
      args.debug = true;
    } else if (strcmp(argv[i], "--debug-name") == 0) {
      if (++i >= argc) {
        fprintf(stderr, "Error: --debug-name requires an argument\n");
        return 1;
      }
      args.debug_name = argv[i];
    } else if (strcmp(argv[i], "--preserve-bindings") == 0) {
      args.preserve_bindings = true;
    } else if (strcmp(argv[i], "--disable-dxil") == 0) {
      args.disable_dxil = true;
    } else if (strcmp(argv[i], "--disable-dxbc") == 0) {
      args.disable_dxbc = true;
    } else if (strcmp(argv[i], "--disable-msl") == 0) {
      args.disable_msl = true;
    } else if (strcmp(argv[i], "--disable-spv") == 0) {
      args.disable_spv = true;
    } else if (argv[i][0] != '-') {
      if (!args.input_file) {
        args.input_file = argv[i];
      } else {
        fprintf(stderr, "Error: Unexpected argument '%s'\n", argv[i]);
        return 1;
      }
    } else {
      fprintf(stderr, "Error: Unknown option '%s'\n", argv[i]);
      return 1;
    }
  }

  // Validate required arguments
  if (!args.input_file) {
    fprintf(stderr, "Error: Input file required\n");
    print_compile_help();
    return 1;
  }

  if (!args.output_file) {
    fprintf(stderr, "Error: Output file (-o) required\n");
    print_compile_help();
    return 1;
  }

  if (!args.vertex_entry && !args.fragment_entry && !args.compute_entry) {
    fprintf(stderr, "Error: At least one shader stage entry point required\n");
    print_compile_help();
    return 1;
  }

  // Read source file
  printf("Reading source: %s\n", args.input_file);
  char* source = read_file_to_string(args.input_file);
  if (!source) {
    return 1;
  }

  // Build stage setups
  nshader_compiler_stage_setup_t stages[3];
  size_t num_stages = 0;

  if (args.vertex_entry) {
    stages[num_stages].stage_type = NSHADER_STAGE_TYPE_VERTEX;
    stages[num_stages].entry_point = args.vertex_entry;
    stages[num_stages].source_code = source;
    stages[num_stages].defines = NULL;
    stages[num_stages].num_defines = 0;
    num_stages++;
  }

  if (args.fragment_entry) {
    stages[num_stages].stage_type = NSHADER_STAGE_TYPE_FRAGMENT;
    stages[num_stages].entry_point = args.fragment_entry;
    stages[num_stages].source_code = source;
    stages[num_stages].defines = NULL;
    stages[num_stages].num_defines = 0;
    num_stages++;
  }

  if (args.compute_entry) {
    stages[num_stages].stage_type = NSHADER_STAGE_TYPE_COMPUTE;
    stages[num_stages].entry_point = args.compute_entry;
    stages[num_stages].source_code = source;
    stages[num_stages].defines = NULL;
    stages[num_stages].num_defines = 0;
    num_stages++;
  }

  // Build compiler config
  nshader_compiler_config_t config = {0};
  config.stages = stages;
  config.num_stages = num_stages;
  config.include_dir = args.include_dir;
  config.disable_dxil = args.disable_dxil;
  config.disable_dxbc = args.disable_dxbc;
  config.disable_msl = args.disable_msl;
  config.disable_spv = args.disable_spv;
  config.enable_debug = args.debug;
  config.debug_name = args.debug_name;
  config.preserve_unused_bindings = args.preserve_bindings;
  config.defines = args.defines;
  config.num_defines = args.num_defines;

  // Compile shader
  printf("Compiling shader...\n");
  nshader_error_list_t errors = {0};
  nshader_t* shader = nshader_compiler_compile_hlsl(&config, &errors);

  if (!shader) {
    fprintf(stderr, "Compilation failed:\n");
    for (size_t i = 0; i < errors.num_errors; i++) {
      fprintf(stderr, "  %s\n", errors.errors[i]);
    }
    nshader_error_list_free(&errors);
    free(source);

    // Free defines
    for (size_t i = 0; i < args.num_defines; i++) {
      free((char*)args.defines[i].name);
      if (args.defines[i].value) {
        free((char*)args.defines[i].value);
      }
    }
    free(args.defines);

    return 1;
  }

  nshader_error_list_free(&errors);

  // Write output
  printf("Writing output: %s\n", args.output_file);
  bool success = nshader_write_to_path(shader, args.output_file);

  if (!success) {
    fprintf(stderr, "Error: Failed to write output file\n");
    nshader_destroy(shader);
    free(source);

    // Free defines
    for (size_t i = 0; i < args.num_defines; i++) {
      free((char*)args.defines[i].name);
      if (args.defines[i].value) {
        free((char*)args.defines[i].value);
      }
    }
    free(args.defines);

    return 1;
  }

  printf("Compilation successful!\n");

  // Cleanup
  nshader_destroy(shader);
  free(source);

  // Free defines
  for (size_t i = 0; i < args.num_defines; i++) {
    free((char*)args.defines[i].name);
    if (args.defines[i].value) {
      free((char*)args.defines[i].value);
    }
  }
  free(args.defines);

  return 0;
}

// #############################################################################
// Info Command
// #############################################################################

static void print_stage_bindings(const char* label, const nshader_stage_binding_t* bindings, size_t count) {
  if (count == 0) return;

  printf("    %s:\n", label);
  for (size_t i = 0; i < count; i++) {
    printf("      [%u] %s: %s (vec%u)\n",
      bindings[i].location,
      bindings[i].name,
      nshader_binding_type_to_string(bindings[i].type),
      bindings[i].vector_size);
  }
}

static int cmd_info(int argc, char** argv) {
  const char* input_file = NULL;
  bool verbose = false;

  // Parse arguments
  for (int i = 2; i < argc; i++) {
    if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
      print_info_help();
      return 0;
    } else if (strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v") == 0) {
      verbose = true;
    } else if (argv[i][0] != '-') {
      if (!input_file) {
        input_file = argv[i];
      } else {
        fprintf(stderr, "Error: Unexpected argument '%s'\n", argv[i]);
        return 1;
      }
    } else {
      fprintf(stderr, "Error: Unknown option '%s'\n", argv[i]);
      return 1;
    }
  }

  if (!input_file) {
    fprintf(stderr, "Error: Input file required\n");
    print_info_help();
    return 1;
  }

  // Read shader
  nshader_t* shader = nshader_read_from_path(input_file);
  if (!shader) {
    fprintf(stderr, "Error: Could not read shader file '%s'\n", input_file);
    return 1;
  }

  const nshader_info_t* info = nshader_get_info(shader);

  // Print basic info
  printf("Shader: %s\n", input_file);
  printf("Type: %s\n", info->type == NSHADER_SHADER_TYPE_GRAPHICS ? "Graphics" : "Compute");

  // Print backends
  printf("Backends (%zu):\n", info->num_backends);
  for (size_t i = 0; i < info->num_backends; i++) {
    printf("  - %s\n", nshader_backend_to_string(info->backends[i]));
  }

  // Print stages
  printf("Stages (%zu):\n", info->num_stages);
  for (size_t i = 0; i < info->num_stages; i++) {
    const nshader_stage_t* stage = &info->stages[i];
    printf("  %s:\n", nshader_stage_type_to_string(stage->type));
    printf("    Entry Point: %s\n", stage->entry_point);

    if (verbose) {
      switch (stage->type) {
        case NSHADER_STAGE_TYPE_VERTEX: {
          const nshader_stage_metadata_vertex_t* meta = &stage->metadata.vertex;
          printf("    Samplers: %u\n", meta->num_samplers);
          printf("    Storage Textures: %u\n", meta->num_storage_textures);
          printf("    Storage Buffers: %u\n", meta->num_storage_buffers);
          printf("    Uniform Buffers: %u\n", meta->num_uniform_buffers);
          print_stage_bindings("Inputs", meta->inputs, meta->input_count);
          print_stage_bindings("Outputs", meta->outputs, meta->output_count);
          break;
        }
        case NSHADER_STAGE_TYPE_FRAGMENT: {
          const nshader_stage_metadata_fragment_t* meta = &stage->metadata.fragment;
          printf("    Samplers: %u\n", meta->num_samplers);
          printf("    Storage Textures: %u\n", meta->num_storage_textures);
          printf("    Storage Buffers: %u\n", meta->num_storage_buffers);
          printf("    Uniform Buffers: %u\n", meta->num_uniform_buffers);
          print_stage_bindings("Inputs", meta->inputs, meta->input_count);
          print_stage_bindings("Outputs", meta->outputs, meta->output_count);
          break;
        }
        case NSHADER_STAGE_TYPE_COMPUTE: {
          const nshader_stage_metadata_compute_t* meta = &stage->metadata.compute;
          printf("    Samplers: %u\n", meta->num_samplers);
          printf("    Read-Only Storage Textures: %u\n", meta->num_readonly_storage_textures);
          printf("    Read-Only Storage Buffers: %u\n", meta->num_readonly_storage_buffers);
          printf("    Read-Write Storage Textures: %u\n", meta->num_readwrite_storage_textures);
          printf("    Read-Write Storage Buffers: %u\n", meta->num_readwrite_storage_buffers);
          printf("    Uniform Buffers: %u\n", meta->num_uniform_buffers);
          printf("    Thread Count: [%u, %u, %u]\n",
            meta->threadcount_x, meta->threadcount_y, meta->threadcount_z);
          break;
        }
        default:
          break;
      }
    }
  }

  nshader_destroy(shader);
  return 0;
}

// #############################################################################
// Extract Command
// #############################################################################

static int cmd_extract(int argc, char** argv) {
  const char* input_file = NULL;
  const char* backend_str = NULL;
  const char* stage_str = NULL;
  const char* output_file = NULL;

  // Parse arguments
  for (int i = 2; i < argc; i++) {
    if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
      print_extract_help();
      return 0;
    } else if (strcmp(argv[i], "-o") == 0) {
      if (++i >= argc) {
        fprintf(stderr, "Error: -o requires an argument\n");
        return 1;
      }
      output_file = argv[i];
    } else if (argv[i][0] != '-') {
      if (!input_file) {
        input_file = argv[i];
      } else if (!backend_str) {
        backend_str = argv[i];
      } else if (!stage_str) {
        stage_str = argv[i];
      } else {
        fprintf(stderr, "Error: Unexpected argument '%s'\n", argv[i]);
        return 1;
      }
    } else {
      fprintf(stderr, "Error: Unknown option '%s'\n", argv[i]);
      return 1;
    }
  }

  if (!input_file || !backend_str || !stage_str || !output_file) {
    fprintf(stderr, "Error: Missing required arguments\n");
    print_extract_help();
    return 1;
  }

  // Parse backend
  nshader_backend_t backend;
  if (strcmp(backend_str, "dxil") == 0) {
    backend = NSHADER_BACKEND_DXIL;
  } else if (strcmp(backend_str, "dxbc") == 0) {
    backend = NSHADER_BACKEND_DXBC;
  } else if (strcmp(backend_str, "msl") == 0) {
    backend = NSHADER_BACKEND_MSL;
  } else if (strcmp(backend_str, "spv") == 0) {
    backend = NSHADER_BACKEND_SPV;
  } else {
    fprintf(stderr, "Error: Unknown backend '%s'\n", backend_str);
    return 1;
  }

  // Parse stage
  nshader_stage_type_t stage;
  if (strcmp(stage_str, "vertex") == 0) {
    stage = NSHADER_STAGE_TYPE_VERTEX;
  } else if (strcmp(stage_str, "fragment") == 0) {
    stage = NSHADER_STAGE_TYPE_FRAGMENT;
  } else if (strcmp(stage_str, "compute") == 0) {
    stage = NSHADER_STAGE_TYPE_COMPUTE;
  } else {
    fprintf(stderr, "Error: Unknown stage '%s'\n", stage_str);
    return 1;
  }

  // Read shader
  nshader_t* shader = nshader_read_from_path(input_file);
  if (!shader) {
    fprintf(stderr, "Error: Could not read shader file '%s'\n", input_file);
    return 1;
  }

  // Check if backend and stage exist
  if (!nshader_has_backend(shader, backend)) {
    fprintf(stderr, "Error: Shader does not have backend '%s'\n", backend_str);
    nshader_destroy(shader);
    return 1;
  }

  if (!nshader_has_stage(shader, stage)) {
    fprintf(stderr, "Error: Shader does not have stage '%s'\n", stage_str);
    nshader_destroy(shader);
    return 1;
  }

  // Get blob
  const nshader_blob_t* blob = nshader_get_blob(shader, stage, backend);
  if (!blob || !blob->data || blob->size == 0) {
    fprintf(stderr, "Error: Could not get blob for backend '%s' and stage '%s'\n",
      backend_str, stage_str);
    nshader_destroy(shader);
    return 1;
  }

  // Write output
  printf("Extracting %s %s to: %s (%zu bytes)\n",
    backend_str, stage_str, output_file, blob->size);

  bool success = write_blob_to_file(output_file, blob->data, blob->size);

  nshader_destroy(shader);

  if (!success) {
    return 1;
  }

  printf("Extraction successful!\n");
  return 0;
}

// #############################################################################
// Main
// #############################################################################

int main(int argc, char** argv) {
  if (argc < 2) {
    print_help();
    return 1;
  }

  const char* command = argv[1];

  if (strcmp(command, "help") == 0 || strcmp(command, "--help") == 0 || strcmp(command, "-h") == 0) {
    print_help();
    return 0;
  }

  if (strcmp(command, "version") == 0 || strcmp(command, "--version") == 0) {
    printf("nshader version %s\n", NSHADER_CLI_VERSION);
    return 0;
  }

  if (strcmp(command, "compile") == 0) {
    return cmd_compile(argc, argv);
  }

  if (strcmp(command, "info") == 0) {
    return cmd_info(argc, argv);
  }

  if (strcmp(command, "extract") == 0) {
    return cmd_extract(argc, argv);
  }

  fprintf(stderr, "Error: Unknown command '%s'\n", command);
  fprintf(stderr, "Run 'nshader help' for usage information\n");
  return 1;
}
