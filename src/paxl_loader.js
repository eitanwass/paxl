/**
 * API for Paxl when compiling as standalone wasm
 */

import { readFile } from "fs/promises";
import { fileURLToPath } from "url";
import { dirname, join } from "path";

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

const data = await readFile(join(__dirname, "./paxl_core.wasm"));

const { instance } = await WebAssembly.instantiate(data, {
  env: {}
});

const {
  parse: rawParse,
  memory: wasmMemory,
  malloc,
  free
} = instance.exports;

const jsStringToCString = (str, memory, malloc) => {
    const encoded = new TextEncoder().encode(str);
    const ptr = malloc(encoded.length + 1);

    const bytes = new Uint8Array(memory.buffer);
    bytes.set(encoded, ptr);
    bytes[ptr + encoded.length] = 0;

    return ptr;
}

const cStringToJs = (memory, resPtr) => {
  const data_view = new DataView(memory.buffer, resPtr, 8);

  const ptr = data_view.getUint32(0, true);
  const len = data_view.getUint32(4, true);

  const res = new TextDecoder("utf-8").decode(new Uint8Array(memory.buffer, ptr, len));

  free(ptr);

  return res;
}

const parse = (xml) => {
  const in_ptr = jsStringToCString(xml, wasmMemory, malloc);

  const resPtr = rawParse(in_ptr);

  const str = cStringToJs(wasmMemory, resPtr);
  
  free(in_ptr);
  free(resPtr);

  return str;
}

// Warming up the JIT
// Due to how JS loads wasm modules, some code paths may be compiled at runtime.
// Since we want the first run of `parse` to be as fast as can be, we will run a minimal XML here to pre-compile
// the necessary code.
// This will take longer in the import of the module but the actual parsing will be lightning speed.
parse("<xml x=1>a</xml>");

export {parse};
