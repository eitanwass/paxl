import Paxl from "./paxl_core.js"

const paxl = await Paxl();
const { _parse, _free_json, stringToNewUTF8, UTF8ToString } = paxl;

export const parse = (xml) => {
    const ptr = _parse(stringToNewUTF8(xml));

    const res = UTF8ToString(ptr);

    _free_json(ptr);
    
    return res;
}
