import Paxl from './paxl_core.js';

const paxl = await Paxl();

export const parse = (xml) => {
    const handle = paxl._parse(paxl.stringToNewUTF8(xml));
    return globalThis.paxl_obj[handle];
};

export const parse_new = (xml) => {
    const ptr = paxl._parse_new(paxl.stringToNewUTF8(xml));
    const jsonStr = paxl.UTF8ToString(ptr);

    paxl._free_json(ptr);

    return jsonStr;
}
