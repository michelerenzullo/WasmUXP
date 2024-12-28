// em++ WasmUXP.cpp -o WasmUXP.js -sWASM=1 -sWASM_ASYNC_COMPILATION=0 -sMODULARIZE=1 
// -sEXPORT_NAME="MyWasmUXP" -sEXPORTED_FUNCTIONS="['_malloc']" -sINITIAL_MEMORY=512MB 
// -sALLOW_MEMORY_GROWTH=1 -O3 --closure=1 -msimd128 -sMAXIMUM_MEMORY=4GB -std=c++2a 
// --bind -sEMBIND_AOT=1 -sDYNAMIC_EXECUTION=0 --no-entry
// =======================
#include <emscripten/bind.h>
#include <emscripten/emscripten.h>
#include <emscripten/val.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
typedef emscripten::val em_val;

struct free_delete {
  void operator()(void* ptr) const { free(ptr); }
};

em_val adjust_brightness(uintptr_t imageData_ptr, int width, int height,
                             int channels, int bitsPerChannel, int brightness) {
  const em_val Uint8Array = em_val::global("Uint8Array");
  const em_val Uint16Array = em_val::global("Uint16Array");
  //const em_val Float32Array = em_val::global("Float32Array");

  em_val file_copy = em_val::null();
  em_val result = em_val::object();

  int size = width * height * channels;        // Total elements (not bytes)
  int size_bytes = size * bitsPerChannel / 8;  // Total bytes

  auto start_0 = std::chrono::steady_clock::now();

  // Handle data dynamically based on bit depth
  if (bitsPerChannel == 8) {
    // 8-bit data
    auto imageData = std::unique_ptr<uint8_t[], free_delete>(
        reinterpret_cast<uint8_t*>(imageData_ptr));
    std::transform(imageData.get(), imageData.get() + size, imageData.get(),
                   [brightness](uint8_t pixel) {
                     uint8_t pixelValue =
                         pixel + static_cast<uint8_t>(brightness);
                     return std::clamp<uint8_t>(pixelValue, 0, 255);
                   });
    file_copy =
        Uint8Array.new_(emscripten::typed_memory_view(size, imageData.get()));
  } else if (bitsPerChannel == 16) {
    // 16-bit data
    auto imageData = std::unique_ptr<uint16_t[], free_delete>(
        reinterpret_cast<uint16_t*>(imageData_ptr));
    std::transform(
        imageData.get(), imageData.get() + size, imageData.get(),
        [brightness](uint16_t pixel) {
          uint16_t pixelValue = pixel + static_cast<uint16_t>(brightness);
          return std::clamp<uint16_t>(
              pixelValue, 0,
              32768);  // clamp to 15-bit range (Photoshop max supported)
        });
    file_copy =
        Uint16Array.new_(emscripten::typed_memory_view(size, imageData.get()));
    //  // Seems like the 32 bits images, that uses by default Display (Linear
    //  RGB) profile,
    //  // aren't supported when copying the data back to Photoshop layer even
    //  tho are correctly read. } else if (bitsPerChannel == 32) {
    //    // 32-bit data
    //    auto imageData = std::unique_ptr<uint32_t[], free_delete>(
    //        reinterpret_cast<uint32_t*>(imageData_ptr));
    //    std::vector<float> data(size);
    //    std::transform(imageData.get(), imageData.get() + size, data.begin(),
    //                   [](uint32_t pixel) {
    //                     float pixelValue = *reinterpret_cast<float*>(&pixel);
    //                     return pixelValue;
    //                   });
    //    file_copy =
    //        Float32Array.new_(emscripten::typed_memory_view(size,
    //        data.data()));
    //
  } else {
    // Unsupported bit depth
    std::fprintf(stderr, "Unsupported bitsPerChannel: %d\n", bitsPerChannel);
    return result;
  }

  std::printf("Execution time for wasm: %f ms\n",
              std::chrono::duration<double, std::milli>(
                  std::chrono::steady_clock::now() - start_0)
                  .count());

  result.set("file_output", file_copy);
  return result;
}

EMSCRIPTEN_BINDINGS(my_module) {
  emscripten::function("adjust_brightness", &adjust_brightness);
}
