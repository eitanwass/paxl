import { parse } from "../dist/paxl.js"

const sample_xml = `
<root id="first">
    <child id="first_child" index=0>
        first_child_name
    </child>
    <child id="second_child" index=1>
        <is_important>false</is_important>
    </child>
</root>
`;

const result = JSON.parse(parse(sample_xml));
console.log(JSON.stringify(result, null, 2));

// singleRoot defaults to true: a single root element is unwrapped.
console.assert(result.tagName === "root", "expected singleRoot default to unwrap the root element");

const wrapped = JSON.parse(parse(sample_xml, { singleRoot: false }));
console.assert(Array.isArray(wrapped.children), "expected singleRoot: false to keep the children wrapper");
