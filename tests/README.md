# nshader Tests

This directory contains comprehensive test suites for the nshader library using the [Unity Test Framework](https://github.com/ThrowTheSwitch/Unity).

## Test Files

### [nshader_tests.c](nshader_tests.c)

Core unit tests covering all library modules:

#### Type Module Tests
- `test_backend_to_string` - Backend enum to string conversion
- `test_backend_to_extension` - Backend enum to file extension conversion
- `test_stage_type_to_string` - Stage type enum to string conversion
- `test_stage_type_to_shader_type` - Stage type to shader type mapping
- `test_binding_type_to_string` - Binding type enum to string conversion

#### Compiler Module Tests
- `test_compile_graphics_shader` - Compile vertex + fragment shader
- `test_compile_compute_shader` - Compile compute shader
- `test_compile_with_defines` - Compile with preprocessor defines
- `test_compile_invalid_shader` - Error handling for invalid shaders

#### Info Module Tests
- `test_get_shader_info` - Get shader metadata
- `test_has_backend` - Check available backends
- `test_has_stage` - Check available stages
- `test_get_blob` - Get compiled shader bytecode
- `test_compute_shader_metadata` - Verify compute shader metadata (threadcount, etc.)

#### Writer Module Tests
- `test_write_to_memory` - Write shader to memory buffer
- `test_write_to_file` - Write shader to FILE*
- `test_write_to_path` - Write shader to filepath

#### Reader Module Tests
- `test_read_from_memory` - Read shader from memory buffer
- `test_read_from_file` - Read shader from FILE*
- `test_read_from_path` - Read shader from filepath
- `test_read_invalid_data` - Error handling for invalid data

#### Reader/Writer Roundtrip Tests
- `test_roundtrip_memory` - Write to memory, read back, verify identical
- `test_roundtrip_file` - Write to file, read back, verify metadata

#### Base Module Tests
- `test_custom_memory_allocator` - Custom memory allocation functions

### [nshader_sample_tests.c](nshader_sample_tests.c)

Integration tests that compile all sample shaders from the `samples/` directory:

#### Compute Shader Tests (10 shaders)
- FillTexture
- GradientTexture
- LinearToSRGB
- LinearToST2084
- SpriteBatch
- TexturedQuad
- ToneMapACES
- ToneMapExtendedReinhardLuminance
- ToneMapHable
- ToneMapReinhard

#### Vertex Shader Tests (10 shaders)
- Fullscreen
- PositionColor
- PositionColorInstanced
- PositionColorTransform
- PullSpriteBatch
- RawTriangle
- Skybox
- TexturedQuad
- TexturedQuadColorWithMatrix
- TexturedQuadWithMatrix

#### Fragment Shader Tests (9 shaders)
- CustomSampling
- DepthOutline
- Skybox
- SolidColor
- SolidColorDepth
- TexturedQuad
- TexturedQuadArray
- TexturedQuadColor
- TexturedQuadWithMultiplyColor

#### Multi-Stage Tests
- `test_compile_Skybox_graphics` - Compile Skybox vertex + fragment
- `test_compile_TexturedQuad_graphics` - Compile TexturedQuad vertex + fragment

#### Persistence Tests
- `test_save_and_load_compute_shader` - Save compiled shader to file and load it back

## Building Tests

Tests are built automatically when the `NSHADER_BUILD_TESTS` option is enabled (ON by default):

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

To disable tests:

```bash
cmake -DNSHADER_BUILD_TESTS=OFF ..
```

## Running Tests

### Run all tests with CTest

```bash
cd build
ctest --verbose
```

### Run individual test executables

```bash
# Core unit tests
./nshader-tests

# Sample shader tests
./nshader-sample-tests
```

## Test Coverage

The test suite covers:

1. **Type System** - All enum conversions and type utilities
2. **Shader Compilation** - HLSL to cross-platform bytecode (DXIL, DXBC, MSL, SPIR-V)
3. **Shader Metadata** - Reflection info extraction
4. **Serialization** - Writing shaders to memory/files
5. **Deserialization** - Reading shaders from memory/files
6. **Roundtrip Testing** - Write-read-write verification
7. **Error Handling** - Invalid input handling
8. **Memory Management** - Custom allocator support
9. **Real-world Shaders** - All 29 sample shaders compile successfully

## Adding New Tests

To add new tests:

1. Add test function following Unity conventions:
   ```c
   void test_your_new_test(void) {
       // Setup
       // Execute
       // Assert
       TEST_ASSERT_*(...);
   }
   ```

2. Register the test in `main()`:
   ```c
   RUN_TEST(test_your_new_test);
   ```

3. Rebuild and run tests

## Dependencies

- [Unity Test Framework](https://github.com/ThrowTheSwitch/Unity) v2.6.0 (automatically fetched via CMake)
- nshader library
- Sample shaders in `samples/` directory
