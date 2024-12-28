
const imaging = require('photoshop').imaging;
const executeAsModal = require('photoshop').core.executeAsModal;
const app = require('photoshop').app;
const button = document.getElementById('adjustBrightnessBtn');

let wasmModule;

async function loadWasm() {
  return new Promise((resolve, reject) => {
    const script = document.createElement('script');
    script.src = 'WasmUXP.js';
    console.log(process.versions.v8);  // Added code V8 version
    console.log(process.version);      // Added code: Node.js version

    script.onload = async () => {
      try {
        // Fetch the wasm binary by ourself and provide it to the
        // js Module synchronously. This is necessary for UXP CSP.
        const response = await fetch('WasmUXP.wasm');
        const wasmBinary = await response.arrayBuffer();

        // Initialize the Module with the wasmBinary
        const Module = {
          wasmBinary: wasmBinary,  // here the magic
          onRuntimeInitialized: () => resolve(Module),
        };

        MyWasmUXP(Module);
      } catch (error) {
        reject(error);
      }
    };

    script.onerror = reject;
    document.head.appendChild(script);
  });
}

async function adjustBrightness(brightnessValue) {
  let imageObj;
  let layer;
  try {
    imageObj = await executeAsModal(async () => {
      return await imaging.getPixels({applyAlpha: true, colorSpace: 'RGB'});
    });

    let width = imageObj.imageData.width;
    let height = imageObj.imageData.height;
    let channels = imageObj.imageData.components;
    let bitsPerChannel = imageObj.imageData.componentSize;
    let size = width * height * channels;
    let size_bytes = size * bitsPerChannel / 8;
    let colorProfile = imageObj.imageData.colorProfile;
    let colorSpace = imageObj.imageData.colorSpace;
    const imageBuffer = await imageObj.imageData.getData();

    console.log('Properties are: ');
    console.log('size: ' + size);
    console.log('size_bytes: ' + size_bytes);
    console.log('width: ' + width);
    console.log('height: ' + height);
    console.log('channels: ' + channels);
    console.log('bitsPerChannel: ' + bitsPerChannel);
    console.log('colorProfile: ' + colorProfile);
    console.log('colorSpace: ' + colorSpace);
    if (!wasmModule) wasmModule = await loadWasm();

    // Allocate memory for the image data
    const ptr = wasmModule._malloc(size_bytes);
    if (!ptr) {
      throw new Error('Memory allocation failed');
    }
    if (bitsPerChannel === 8) {
      wasmModule.HEAPU8.set(imageBuffer, ptr);
    } else if (bitsPerChannel === 16) {
      // align the ptr to 2 bytes
      wasmModule.HEAPU16.set(imageBuffer, ptr >>> 1);
      // // Seems like 32 bit are correctly read and written
      // // but when putPixels is called, an error about the Color Profile
      // // (untouched) is thrown, like isn't supported Display (Linear RGB)
      //} else if (bitsPerChannel === 32) {
      //  // align the ptr to 4 bytes
      //  wasmModule.HEAPF32.set(imageBuffer, ptr >>> 2);
    } else {
      throw new Error('Unsupported bits per channel');
    }
    imageObj.imageData.dispose();

    const result = wasmModule.adjust_brightness(
        ptr, width, height, channels, bitsPerChannel, brightnessValue);

    const finalArray =
        await imaging.createImageDataFromBuffer(result.file_output, {
          width: width,
          height: height,
          components: channels,
          componentSize: bitsPerChannel,
          colorProfile: colorProfile,
          colorSpace: colorSpace
        });

    await executeAsModal(async () => {
      layer = await app.activeDocument.createLayer();
      await imaging.putPixels({layerID: layer.id, imageData: finalArray});
    });
  } catch (e) {
    console.log(e);
  } finally {
    if (imageObj) imageObj.dispose();
  }
}

// Event listener for the "Adjust Brightness" button click
button.onclick = async function() {
  start = performance.now();  // Added code start time
  const brightnessInput = document.getElementById('brightnessInput');
  const brightnessValue = parseInt(brightnessInput.value, 10);
  await adjustBrightness(brightnessValue);
};