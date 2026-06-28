# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

Paxl is a high-performance XML-to-JSON parser written in C, compiled to WebAssembly via Emscripten, and exposed to JavaScript/TypeScript through a thin wasm-loader API. The C parser does no allocation of its own data structures beyond yyjson nodes — it walks the XML string in place and builds a yyjson mutable document, which is then serialized to a JSON string returned to the caller.

## Build commands

Requires the Emscripten SDK (`emcc`) activated in the shell, plus a native `gcc` for tests.

```bash
make build          # compiles src/*.c -> dist/paxl_core.wasm, runs `npx tsc`, then `npm pack`
make test           # builds and runs all C unit tests (tests/test_*.c) via Unity, prints aggregate pass/fail
make format         # clang-format -i over src/**/*.c (Google style, see .clang-format)
make clean          # removes dist/ and built test binaries
make benchmark      # runs `npm run test` inside ./benchmarks (compares against rapidx2j, txml)
```

Build-time variables (pass as `make build VAR=val`):
- `DEBUG=y` — debug symbols, `-O0`, no optimization (default `n`, which uses `-O3 -ffast-math`)
- `W_ENTRY=y` — include a `main()` entrypoint for a CLI/standalone wasm binary (default `n`, builds with `--no-entry`/`-sINVOKE_RUN=0`)
- `MAX_XML_DEPTH=256` — sets `XML_DEPTH`, the size of the open-tag stack used during parsing

Build wiring lives in `Makefile.common` (compiler flags, source list, wasm export list) and `Makefile.test` (native test build/run loop). `Makefile` just includes both and defines the `build`/`benchmark`/`format`/`clean` targets.

### Running a single C test

Each `tests/test_*.c` file compiles to its own native binary (e.g. `tests/test_parser`). Build/run one directly:

```bash
make test_parser          # builds tests/test_parser from src + that test file, then runs it
./tests/test_parser        # re-run without rebuilding
```

The aggregate `make test` target sums Unity's "Tests/Failures/Ignored" counts across every `test_*` binary.

### JS/TS side

There is no JS test runner wired into `make test` — `tests/test_js_integration.js` is a manual smoke-test script (run with `node tests/test_js_integration.js` after `make build`, since it imports from `dist/paxl.js`). TypeScript compiles with plain `npx tsc` (see `tsconfig.json`: `src/` -> `dist/`, ESNext/ESM, strict mode).

## Architecture

### C parser pipeline (`src/`)

- `xml.c` / `xml.h` — public entrypoint `parse(char* xml)`. Allocates a yyjson mutable doc, calls `_parse_xml`, serializes with `yyjson_mut_write`, and returns a heap-allocated `parse_res{ json_ptr, json_len }` for the JS side to read and free. Exported via `EMSCRIPTEN_KEEPALIVE` when compiled with emcc. When built with `W_ENTRY=y`, also provides a `main()` for CLI use.
- `src/core/parser.c` (`_parse_xml`) — the actual scanning loop. It is a single pass over the input string with no recursion: a fixed-size stack (`yyjson_mut_val* stack[XML_DEPTH]`) of "currently open" JSON object nodes substitutes for a recursive-descent tree walk. For each `<...>` it dispatches on the byte after `<`: `/` pops the stack (closing tag), `?` skips processing instructions, `!` delegates to `parse_comment` (currently a no-op — comments are skipped but not represented in output), and anything else starts a new element: read tag name, loop over attributes (quoted or unquoted values), detect self-closing `/>`, and push/don't-push onto the stack accordingly. Text content between tags is trimmed of trailing whitespace and appended as a plain string into the current node's `children` array.
- `src/core/helpers.h` — tiny inline string-scanning primitives (`skip_ws`, `skip_to_tag_end`) shared by the parser.
- `src/parsers/tag_name_parser.h` — `parse_tag_name`, isolated so it can be unit tested independently of the main loop.
- `src/parsers/comment_parser.{c,h}` — `parse_comment` stub; comment parsing is on the roadmap but not implemented (see readme.md Roadmap).
- `libs/yyjson/` — vendored yyjson (JSON tree builder/writer), statically linked. Built with `YYJSON_DISABLE_READER`/`YYJSON_DISABLE_INCR_READER` since paxl only ever builds JSON, never parses it.

Output shape produced by the parser: each element becomes `{ tagName, attributes?, children? }`; `attributes` only appears if the tag had any; `children` holds nested element objects interleaved with plain strings for text nodes. The document root is an object whose own `children` array holds the top-level element(s) — there's no dedicated "root element" wrapper (see readme.md for example output).

### JS/TS bindings (`src/paxl.ts`, `src/paxl-browser.ts`)

Two near-identical entrypoints, kept manually in sync rather than sharing code, because module loading differs by environment:
- `paxl.ts` (Node): loads `paxl_core.wasm` via `fs/promises` + `WebAssembly.instantiate`.
- `paxl-browser.ts`: loads the same wasm via `fetch` + `WebAssembly.instantiate`.

Both implement the same low-level dance because the wasm module only exports `parse`, `malloc`, `free`, and `memory` (see `EXPORTS` in `Makefile.common`):
1. `jsStringToCString` — encode a JS string to UTF-8, `malloc` space in wasm memory, copy bytes in, NUL-terminate.
2. Call `rawParse(ptr)`, which returns a pointer to a `parse_res` struct (two `uint32`s: `json_ptr`, `json_len`) laid out in wasm linear memory.
3. `cStringToJs` — read those two fields via `DataView`, decode the JSON string, then `free` the inner `json_ptr`.
4. `free` the input string pointer and the `parse_res` pointer itself.

Both modules eagerly call `parse(...)` once at import time on a throwaway string to force JIT warm-up before real use (see comment in `paxl.ts`). Callers get back a JSON **string**, not a parsed object — call `JSON.parse(parse(xml))` to get a value (see `tests/test_js_integration.js`).

`dist/` is the compiled+packed output (`paxl_core.wasm`, `paxl.js`, `paxl.d.ts` + maps) — this is what gets published to npm (see `files` in `package.json`). `docs/` is the GitHub Pages demo bundle (separate `paxl-browser.js` build + `index.html`/`script.js`), unrelated to the npm package.

## Notes from the roadmap (readme.md) worth knowing before changing parser behavior

- XSD validation is explicitly out of scope.
- Comments are currently parsed-and-discarded (`parse_comment` no-ops); making comments appear in output is an open roadmap item, not a regression to fix.
- Attribute values are always strings today; "non-string attributes" and "parse booleans to native form" are both unimplemented roadmap items, not bugs.
- yyjson is a deliberate, possibly-temporary dependency ("remove yyjson for smaller in-house implementation" is on the roadmap) — don't assume its presence is permanent API surface to build heavily on top of.
