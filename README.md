
# WasmUXP – A Minimal Working Example of Adobe UXP Plugins with WebAssembly (WASM) via Emscripten

### Description:
**WasmUXP** is a lightweight, fully functional example project that demonstrates the development of Adobe UXP plugins using WebAssembly (WASM) compiled with Emscripten. It addresses the unique challenges of developing high-performance plugins within the Photoshop UXP sandbox while showcasing how to leverage WASM effectively. Although WebAssembly is not officially supported by Photoshop, this project provides a practical solution that works reliably with Photoshop 2025 using the recommended options.

---

### Key Features:
- **Synchronous WASM Compilation**: UXP does not allow asynchronous compilation of WebAssembly. This project ensures compatibility by using the `-sWASM_ASYNC_COMPILATION=0` flag, enabling synchronous loading and execution of WASM code.
- **No Web Workers or Pthreads**: The UXP environment restricts the use of multithreading via Web Workers or pthreads. This project is designed with these limitations in mind, focusing on single-threaded execution.
- **Targeting Emscripten-Generated WASM**: Specifically designed to work with WebAssembly code compiled via Emscripten, showcasing techniques such as:
  - Allocating and managing buffers in WASM memory.
  - Exporting pre-built `UintArray` objects for efficient data handling.
  - Utilizing Emscripten bindings (`EMBIND`) for seamless C++ and JavaScript integration.
- **Simplified Hybrid Development**: Write performance-critical code in C/C++ once, compile it to WASM with Emscripten, and use the same binary across all platforms supported by UXP without worrying about native architecture differences.
- **Sandbox-Compatible Implementation**: Fully adheres to UXP guidelines, including Content Security Policy (CSP) and restrictions on dynamic code execution (e.g., `eval`, `new Function`).

---

### Important Note:
While WebAssembly is a powerful tool for enhancing UXP plugin development, **it is not officially supported by Photoshop**. This means there is no guarantee that WASM-based plugins will work indefinitely. However, with the options and techniques demonstrated in this project, WASM integration works seamlessly in Photoshop 2025. Future updates may require adjustments to maintain compatibility.

---

### Why Use WebAssembly in UXP?
- **Cross-Platform Compatibility**: WASM enables high-performance code to run on any platform supported by UXP without requiring native cross-compilation.
- **Improved Performance**: WebAssembly brings near-native performance to computationally intensive tasks like image manipulation and data processing.
- **Seamless Integration**: Emscripten-generated WASM bridges the gap between high-performance native code and JavaScript flexibility in the UXP environment.
- **Designed for UXP’s Restrictions**: By adhering to UXP’s architectural and security constraints, this project ensures that your plugin will run smoothly in Adobe applications.

---

### Key Restrictions Addressed:
- **Synchronous WebAssembly Compilation**: Required for UXP; ensures WASM loads without asynchronous operations.
- **No Web Workers or Pthreads**: The project avoids multithreading and focuses on single-threaded execution within the UXP sandbox.
- **Dynamic Code Execution Disabled**: By using flags such as `-sDYNAMIC_EXECUTION=0` and `-sEMBIND_AOT=1`, the project avoids runtime-generated code, ensuring compatibility with UXP’s strict CSP.

---

### Use Cases:
- Learning how to build UXP plugins with WebAssembly.
- Understanding how to compile C/C++ libraries into WASM using Emscripten.
- Developing high-performance UXP plugins while adhering to sandbox constraints.
- Simplifying cross-platform development with a single WASM binary.
- Experimenting with hybrid plugin development, leveraging C++ for performance-critical tasks and JavaScript for UI and control logic.

---

### Getting Started:
1. Clone this repository:
   ```bash
   git clone https://github.com/michelerenzullo/WasmUXP.git
   ```
2.  Create build folder and build with cmake. Please ensure to have emscripten installed and activated in your system or you might need to define manually the path to `.../Emscripten.cmake` in `CMAKE_TOOLCHAIN_FILE`
    ```bash
    mkdir build && cd build
    cmake -S ..
    make install
    ```
3. The updated wasm an js example files will be installed in the current folder.

---

**WasmUXP** is your comprehensive guide to building efficient, scalable, and sandbox-compliant UXP plugins using WebAssembly. By targeting Emscripten-generated WASM and addressing UXP’s unique restrictions, this project empowers developers to create powerful plugins that run seamlessly across all supported platforms. While not officially supported by Photoshop, this solution works reliably with Photoshop 2025. Start building today! 
