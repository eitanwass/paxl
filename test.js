import Paxl from "./build/paxl.js";

var paxl = await Paxl();

let handle = paxl._paxml(paxl.stringToNewUTF8("<Start>"));

console.log(globalThis.paxml_obj[handle]);
