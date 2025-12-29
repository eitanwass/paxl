import rapidx2j from "rapidx2j";
import {parse} from "txml";

import {parse as paxl_parse} from "../dist/paxl.js";

const testCases = [
    {
        name: "Small (10 items)",
        xml: `<root>${Array.from({length: 10}, (_, i) => `<item id="${i}"><name>item${i}</name><value>${i*10}</value></item>`).join('')}</root>`,
        repeats: 10000
    },
    {
        name: "Medium (500 items)",
        xml: `<root>${
    Array.from({length: 500}, (_, i) => `<item id="${i}">
        <name>
            item${i}
        </name>
        <value>
            ${i * 10}
        </value>
    </item>`).join('')
}</root>`,
        repeats: 6000
    },
    {
        name: "Large (1000 items)",
        xml: `<root>${
    Array.from({length: 1000}, (_, i) => `<item id="${i}">
        <name>
            item${i}
        </name>
        <value>
            ${i * 10}
        </value>
    </item>`).join('')
}</root>`,
        repeats: 3000
    },
    {
        name: "Nested (100 items)",
        xml: `<root>${Array.from({length: 100}, (_, i) => `<item id="${i}"><nested><deep>value${i}</deep></nested></item>`).join('')}</root>`,
        repeats: 3000
    },
    {
        name: "Attributes (500 items)",
        xml: `<root>${Array.from({length: 500}, (_, i) => `<item id="${i}" name="item${i}" value="${i*10}" attr1="val1" attr2="val2" attr3="val3"/>`).join('')}</root>`,
        repeats: 6000
    }
];

for (const testCase of testCases) {
    const { name, xml, repeats } = testCase;

    console.log(`\nTesting: ${name}`);

    // Paxl
    const paxl_times = [];
    for (let i = 0; i < repeats; i++) {
        const start = performance.now();
        const result = paxl_parse(xml);
        const end = performance.now();
        paxl_times.push(end - start);
    }
    const paxl_avg = paxl_times.reduce((a, b) => a + b) / paxl_times.length;

    // RapidX2J
    const rapidx2j_times = [];
    for (let i = 0; i < repeats; i++) {
        const start = performance.now();
        const result = rapidx2j.parse(xml);
        const end = performance.now();
        rapidx2j_times.push(end - start);
    }
    const rapidx2j_avg = rapidx2j_times.reduce((a, b) => a + b) / rapidx2j_times.length;

    // TXML
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

    console.log(`  Speedup vs rapidx2j:  ${(rapidx2j_avg / paxl_avg).toFixed(2)}x faster`);
    console.log(`  Speedup vs txml:      ${(txml_avg / paxl_avg).toFixed(2)}x faster`);
}
