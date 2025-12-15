import Paxl from './paxl_core.js';

const paxl = await Paxl();

const parse = (xml) => {
    const handle = paxl._parse(paxl.stringToNewUTF8(xml));
    return globalThis.paxl_obj[handle];
};

export default parse;
