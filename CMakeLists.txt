# Minimum required version of CMake
cmake_minimum_required(VERSION 3.14)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

message(STATUS "Compiling to WebAssembly")
if(NOT CMAKE_TOOLCHAIN_FILE OR NOT CMAKE_TOOLCHAIN_FILE MATCHES "Emscripten.cmake")
    find_program(EMCC_EXE emcc)
    if (EMCC_EXE)
        message ("-- Found Emscripten")
        get_filename_component(EMSDK_DIR ${EMCC_EXE} DIRECTORY)
        set(CMAKE_TOOLCHAIN_FILE "${EMSDK_DIR}/cmake/Modules/Platform/Emscripten.cmake")
    else()
        message(FATAL_ERROR "-- Emscripten not found! Aborting...")
    endif()
endif()
message(STATUS "Using Emscripten toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
set(CMAKE_EXECUTABLE_SUFFIX ".wasm")

# Project name
project(WasmUXP)

message(STATUS "Building WebAssembly")

add_executable(WasmUXP "${CMAKE_SOURCE_DIR}/WasmUXP.cpp")
set(WASM_COMMON_LINK_OPTIONS 
    "SHELL: --closure 1"               # Shrink the js file minifying the code
    "-sALLOW_MEMORY_GROWTH=1"          # Allow memory growth
    "-sMAXIMUM_MEMORY=4GB"             # Allow max memory up to 4GB (default is 2GB)
    "-sINITIAL_MEMORY=314572800"       # Initial memory of 300MB
    "-O3"                              # Optimize the output
    "--bind"                           # Enable emscripten bindings
    "-sEXPORTED_FUNCTIONS=['_malloc']" # Export the malloc function in order to allocate the initial buffer from the JS side to the WASM side
    "-msimd128"                        # Enable SIMD support
    "-sEMBIND_AOT=1"                   # Ahead of time compilation for embindings fix: Code generation from strings disallowed for this context
    "-sDYNAMIC_EXECUTION=0"            # Disable dynamic execution allowing emscripten::val usage, fix: Code generation from strings disallowed for this context
    "-sWASM_ASYNC_COMPILATION=0"       # Disable async compilation of wasm, fix: wasm instantiation error
    "-sMODULARIZE=1"                   # Modularize the output
    "-sEXPORT_NAME='MyWasmUXP'"        # Export the module name
    "--no-entry"                       # Do not generate a main function, since is intended to be used as a reactor(library)
)

# Apply the options to the target
target_link_options(WasmUXP PRIVATE ${WASM_COMMON_LINK_OPTIONS})

# Install the WASM and JS file in the source directory (replacing the old ones)
install(FILES "${CMAKE_BINARY_DIR}/WasmUXP.wasm" "${CMAKE_BINARY_DIR}/WasmUXP.js" DESTINATION "${CMAKE_SOURCE_DIR}")