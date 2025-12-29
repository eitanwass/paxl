# Paxl

A high-performance XML to JSON parser implemented in WebAssembly.

Paxl parses XML documents and converts them into JSON objects with minimal overhead. It's designed for speed and efficiency, making it ideal for processing large XML files or high-throughput applications.

## Features

- **Fast**: Optimized C code compiled to WebAssembly for maximum performance
- **Lightweight**: Zero dependencies in the runtime (yyjson is statically linked)
- **Standards-compliant**: Supports standard XML syntax including attributes, nested elements, and text content
- **Node.js ready**: Easy to use JavaScript API with ES modules support

## Installation

```bash
npm install paxl
```

## Usage

```javascript
import { parse } from 'paxl';

const xml = `<root>
  <item id="1">
    <name>Example</name>
    <value>42</value>
  </item>
</root>`;

const json = parse(xml);
console.log(json);
// Output: {"children":[{"name":"item","attributes":{"id":"1"},"children":[{"name":"name","children":["Example"]},{"name":"value","children":["42"]}]}]}
```

## JSON Output Format

Paxl converts XML to a JSON structure where:

- Elements become objects with a `name` property and optional `attributes` and `children` properties
- Text content is represented as strings in the `children` array
- Attributes are collected in an `attributes` object
- Nested elements are placed in the `children` array

Example:

```xml
<book id="123" category="fiction">
  <title>Sample Book</title>
  <author>John Doe</author>
</book>
```

Becomes:

```json
{
  "children": [
    {
      "name": "book",
      "attributes": {
        "id": "123",
        "category": "fiction"
      },
      "children": [
        {
          "name": "title",
          "children": ["Sample Book"]
        },
        {
          "name": "author",
          "children": ["John Doe"]
        }
      ]
    }
  ]
}
```

## Performance

Paxl is designed for high performance and typically outperforms pure JavaScript XML parsers. Benchmarks show significant speedups compared to alternatives like `rapidx2j` and `txml`.

Run the included benchmarks to see performance on your system:

```bash
node tests/perf_test.js
```

## Building from Source

Paxl requires Emscripten to build. Make sure you have the Emscripten SDK installed and activated.

```bash
# Clone the repository
git clone https://github.com/eitanwass/paxl.git
cd paxl

# Build the WebAssembly module
make build

# Run tests
node tests/simple_test.js
```

### Build Options

- `DEBUG=y`: Build with debug symbols and no optimizations
- `W_ENTRY=y`: Include a main function for CLI applications using WASM
- `MAX_XML_DEPTH=256`: Maximum XML nesting depth (default: 256)

## Requirements

- Node.js >= 12.22.7
- Emscripten (for building from source)

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.

## License

ISC License - see the package.json for details.

## Credits

- Uses [yyjson](https://github.com/ibireme/yyjson) for JSON handling
- Built with [Emscripten](https://emscripten.org/)
