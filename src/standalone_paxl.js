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

const cStringToJs = (ptr, memory) => {
    const bytes = new Uint8Array(memory.buffer);
    let end = ptr;

    // find null terminator
    while (bytes[end] !== 0) end++;

    // slice & decode
    return new TextDecoder("utf-8").decode(bytes.subarray(ptr, end));
}

export const parse = (xml) => {
  const inPtr = jsStringToCString(xml, wasmMemory, malloc);

  const outPtr = rawParse(inPtr);

  const str = cStringToJs(outPtr, wasmMemory);
  
  free(inPtr);
  free(outPtr);

  return str;
}
