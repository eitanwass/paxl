import {parse} from "./build/paxl.js"

// const xml = `<root>${
//     Array.from({length: 100}, (_, i) => `<item id="${i}"><name>item${i}</name><value>${i * 10}</value></item>`).join('')
// }</root>`;

console.log("output:");
console.log(
    parse(`
        <xml>
            <child1>content1</child1>
            <child2>content2</child2>
            <emptyChild/>
            <childNoContent></childNoContent>
        </xml>
    `)
);
