#include <nshader/nshader_info.h>

// Convert backend enum to file extension
const char* nshader_backend_to_extension(nshader_backend_t backend) {
  switch (backend) {
    case NSHADER_BACKEND_DXIL:
      return ".dxil";
    case NSHADER_BACKEND_DXBC:
      return ".dxbc";
    case NSHADER_BACKEND_MSL:
      return ".msl";
    case NSHADER_BACKEND_SPV:
      return ".spv";
    default:
      return ".unknown";
  }
}

// Convert backend enum to string
const char* nshader_backend_to_string(nshader_backend_t backend) {
  switch (backend) {
    case NSHADER_BACKEND_DXIL:
      return "DXIL";
    case NSHADER_BACKEND_DXBC:
      return "DXBC";
    case NSHADER_BACKEND_MSL:
      return "MSL";
    case NSHADER_BACKEND_SPV:
      return "SPIR-V";
    default:
      return "Unknown";
  }
}

// Convert stage type enum to string
const char* nshader_stage_type_to_string(nshader_stage_type_t stage_type) {
  switch (stage_type) {
    case NSHADER_STAGE_TYPE_VERTEX:
      return "Vertex";
    case NSHADER_STAGE_TYPE_FRAGMENT:
      return "Fragment";
    case NSHADER_STAGE_TYPE_COMPUTE:
      return "Compute";
    default:
      return "Unknown";
  }
}

// Convert stage type to shader type
nshader_type_t nshader_stage_type_to_shader_type(nshader_stage_type_t stage_type) {
  switch (stage_type) {
    case NSHADER_STAGE_TYPE_VERTEX:
    case NSHADER_STAGE_TYPE_FRAGMENT:
      return NSHADER_SHADER_TYPE_GRAPHICS;
    case NSHADER_STAGE_TYPE_COMPUTE:
      return NSHADER_SHADER_TYPE_COMPUTE;
    default:
      return NSHADER_SHADER_TYPE_GRAPHICS;
  }
}

// Convert binding type enum to string
const char* nshader_binding_type_to_string(nshader_binding_type_t binding_type) {
  switch (binding_type) {
    case NSHADER_BINDING_TYPE_INT8:
      return "int8";
    case NSHADER_BINDING_TYPE_UINT8:
      return "uint8";
    case NSHADER_BINDING_TYPE_INT16:
      return "int16";
    case NSHADER_BINDING_TYPE_UINT16:
      return "uint16";
    case NSHADER_BINDING_TYPE_INT32:
      return "int32";
    case NSHADER_BINDING_TYPE_UINT32:
      return "uint32";
    case NSHADER_BINDING_TYPE_INT64:
      return "int64";
    case NSHADER_BINDING_TYPE_UINT64:
      return "uint64";
    case NSHADER_BINDING_TYPE_FLOAT16:
      return "float16";
    case NSHADER_BINDING_TYPE_FLOAT32:
      return "float32";
    case NSHADER_BINDING_TYPE_FLOAT64:
      return "float64";
    default:
      return "unknown";
  }
}