import rapidx2j from "rapidx2j";
import {parse} from "txml";

import {parse as paxl_parse} from "./build/paxl.js";

const repeats = 100000;  // Reduce repeats for larger XML
const xml = `<root>${
    Array.from({length: 10}, (_, i) => `<item id="${i}"><name>item${i}</name><value>${i * 10}</value></item>`).join('')
}</root>`;
// const xml = `<xml>
//             <child1>content1</child1>
//             <child2>content2</child2>
//             <emptyChild/>
//             <childNoContent></childNoContent>
//         </xml>`;

const paxl_times = [];
for (let i = 0; i < repeats; i++) {
    const start = performance.now();
    const result = paxl_parse(xml);
    const end = performance.now();
    paxl_times.push(end - start);
}
const paxl_avg = paxl_times.reduce((a, b) => a + b) / paxl_times.length;

console.log(paxl_parse(xml));

const rapidx2j_times = [];
for (let i = 0; i < repeats; i++) {
    const start = performance.now();
    const result = rapidx2j.parse(xml);
    const end = performance.now();
    rapidx2j_times.push(end - start);
}
const rapidx2j_avg = rapidx2j_times.reduce((a, b) => a + b) / rapidx2j_times.length;

const txml_times = [];
for (let i = 0; i < repeats; i++) {
    const start = performance.now();
    const result = parse(xml);
    const end = performance.now();
    txml_times.push(end - start);
}
const txml_avg = txml_times.reduce((a, b) => a + b) / txml_times.length;

console.log(`  paxl (WASM):          ${paxl_avg.toFixed(3)}ms (avg of ${paxl_times.length} runs)`);
console.log(`  rapidx2j (C + JS):    ${rapidx2j_avg.toFixed(3)}ms (avg of ${rapidx2j_times.length} runs)`);
console.log(`  txml (JS):            ${txml_avg.toFixed(3)}ms (avg of ${txml_times.length} runs)`);

console.log(`Speedup vs rapidx2j:  ${(rapidx2j_avg / paxl_avg).toFixed(2)}x faster`);
console.log(`Speedup vs txml:      ${(txml_avg / paxl_avg).toFixed(2)}x faster`);
