import Paxl from "./paxl_core.js"

const paxl = await Paxl();
const { _parse, _free_json, stringToNewUTF8, UTF8ToString } = paxl;

export const parse = (xml) => {
    const ptr = _parse(stringToNewUTF8(xml));

    const res = UTF8ToString(ptr);

    _free_json(ptr);
    
    return res;
}

// Warming up the JIT
// Due to how JS loads wasm modules, some code paths may be compiled at runtime.
// Since we want the first run of `parse` to be as fast as can be, we will run a minimal XML here to pre-compile
// the necessary code.
// This will take longer in the import of the module but the actual parsing will be lightning speed.
parse("<xml x=1>a</xml>");

export {parse};
