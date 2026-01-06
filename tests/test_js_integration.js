import { parse } from "../paxl.js"

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
