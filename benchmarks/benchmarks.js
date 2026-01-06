import Benchmark from 'benchmark';

import { parse as paxlParse } from '../paxl.js';
import rapidx2j from "rapidx2j";
import {parse} from "txml";

const colors = {
    reset: '\x1b[0m',
    bright: '\x1b[1m',
    red: '\x1b[31m',
    green: '\x1b[32m',
    yellow: '\x1b[33m',
    blue: '\x1b[34m',
    magenta: '\x1b[35m',
    cyan: '\x1b[36m',
};

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

const suite = new Benchmark.Suite;

const groups = {};

for (const testCase of testCases) {
    suite.add(`${testCase.name} - paxl`, () => {
        paxlParse(testCase.xml);
    })
    .add(`${testCase.name} - rapidx2j`, () => {
        rapidx2j.parse(testCase.xml);
    })
    .add(`${testCase.name} - txml`, () => {
        parse(testCase.xml);
    });
}

suite.on('cycle', (event) => {
    console.log(String(event.target));
    const bench = event.target;
    const [category, parser] = bench.name.split(' - ');
    if (!groups[category]) groups[category] = [];
    groups[category].push({ parser, hz: bench.hz });
    if (groups[category].length === 3) {
        const sorted = groups[category].sort((a, b) => b.hz - a.hz);
        const fastest = sorted[0];
        const second = sorted[1];
        const speedup = fastest.hz / second.hz;
        console.log(`${colors.cyan}${category}${colors.reset}: Fastest is ${colors.green}${fastest.parser}${colors.reset} at ${fastest.hz.toFixed(0)} ops/sec, ${speedup.toFixed(2)}x faster than the runner up (${second.parser})`);
    }
})
.on('complete', function() {
    // All done
})
.run({ 'async': true });