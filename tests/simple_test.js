import {parse} from "../build/paxl.js"

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
