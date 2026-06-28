// script.js
//
// paxl-browser.js here is the same browser build paxl publishes to npm
// (compiled from src/paxl-browser.ts, copied in by `make docs`) - not a
// hand-maintained duplicate. Importing it as an ES module means its
// top-level await (wasm fetch + instantiate + warm-up) is awaited
// automatically before this module's own top-level code continues, so
// there's no separate "is it ready yet" polling needed.
import { parse as paxlParse } from './paxl-browser.js';

// RapidX2J is intentionally not compared here: it's a native Node.js addon
// (compiled via node-gyp), so it has no browser build - esm.sh can't serve
// its compiled binary, and importing it 404s and aborts this whole module's
// evaluation (so every button on the page silently stops working). It's
// still compared against in the Node-only suite at benchmarks/benchmarks.js.
import { parse as txmlParse } from 'https://esm.sh/txml@5.2.1';

const CATEGORIES = ['Electronics', 'Books', 'Home', 'Toys', 'Sports', 'Garden'];

const SAMPLE_XML = `<catalog>
${Array.from({ length: 20 }, (_, i) => `  <product id="${i + 1}" category="${CATEGORIES[i % CATEGORIES.length]}" inStock="${i % 3 !== 0}">
    <name>Sample Product ${i + 1}</name>
    <price currency="USD">${(9.99 + i * 3.5).toFixed(2)}</price>
    <description>This is a description for product number ${i + 1}, used to pad out the sample XML.</description>
    <tags>
      <tag>tag-${i % 5}</tag>
      <tag>tag-${(i + 1) % 5}</tag>
    </tags>
  </product>`).join('\n')}
</catalog>`;

let xmlData = SAMPLE_XML;

document.getElementById('xmlInput').value = xmlData;

document.getElementById('fileInput').addEventListener('change', (e) => {
    const file = e.target.files[0];
    if (file) {
        const reader = new FileReader();
        reader.onload = (e) => {
            xmlData = e.target.result;
            document.getElementById('xmlInput').value = xmlData;
        };
        reader.readAsText(file);
    }
});

document.getElementById('xmlInput').addEventListener('input', (e) => {
    xmlData = e.target.value;
});

document.getElementById('loadSampleBtn').addEventListener('click', () => {
    xmlData = SAMPLE_XML;
    document.getElementById('xmlInput').value = xmlData;
});

// Time a parser fairly: a few throwaway warm-up calls (so JIT compilation
// doesn't get billed to whichever parser happens to run first), then repeat
// the real call enough times to get a stable average. A single performance.now()
// call around one invocation is too noisy to trust for comparisons - it can
// easily flip the ranking on small inputs.
function benchmark(fn, { warmupRuns = 5, targetMs = 150, minRuns = 20, maxRuns = 5000 } = {}) {
    for (let i = 0; i < warmupRuns; i++) fn();

    // Calibrate how many iterations are needed to run for ~targetMs.
    const calibrateStart = performance.now();
    let calibrateRuns = 0;
    while (performance.now() - calibrateStart < 20 && calibrateRuns < maxRuns) {
        fn();
        calibrateRuns++;
    }
    const calibrateElapsed = performance.now() - calibrateStart;
    const perCallMs = calibrateRuns > 0 ? calibrateElapsed / calibrateRuns : 0;
    let runs = perCallMs > 0 ? Math.round(targetMs / perCallMs) : minRuns;
    runs = Math.min(maxRuns, Math.max(minRuns, runs));

    const start = performance.now();
    for (let i = 0; i < runs; i++) fn();
    const elapsed = performance.now() - start;

    return { avgMs: elapsed / runs, runs };
}

function escapeHtml(str) {
    return str
        .replace(/&/g, '&amp;')
        .replace(/</g, '&lt;')
        .replace(/>/g, '&gt;');
}

document.getElementById('parseBtn').addEventListener('click', async () => {
    if (!xmlData.trim()) {
        alert('Please provide XML data.');
        return;
    }

    const resultsDiv = document.getElementById('results');
    resultsDiv.innerHTML = '<p>Benchmarking...</p>';

    // Each parser is timed on exactly what it does to hand back usable output:
    // Paxl's contract is "XML in, JSON string out" (see readme.md / tests/test_js_integration.js),
    // while TXML parses straight to a JS object. Calling JSON.parse() on Paxl's result during
    // the timed loop would bill it for an extra round-trip TXML never pays - that mismatch alone
    // was enough to flip the ranking in testing. JSON.parse() is only used below, once, purely
    // to pretty-print the result for display.
    const parsers = [
        { name: 'Paxl', func: (xml) => paxlParse(xml), toDisplayObject: JSON.parse },
        { name: 'TXML', func: (xml) => txmlParse(xml), toDisplayObject: (v) => v }
    ];

    const results = [];

    for (const parser of parsers) {
        try {
            const rawOutput = parser.func(xmlData);
            const displayObject = parser.toDisplayObject(rawOutput);
            const { avgMs, runs } = benchmark(() => parser.func(xmlData));
            results.push({ name: parser.name, output: JSON.stringify(displayObject, null, 2), avgMs, runs, error: null });
        } catch (e) {
            results.push({ name: parser.name, output: null, avgMs: Infinity, runs: 0, error: e.message });
        }
    }

    const ranked = [...results].sort((a, b) => a.avgMs - b.avgMs);
    const fastest = ranked.find(r => !r.error);
    const runnerUp = ranked.find(r => !r.error && r !== fastest);

    let summaryHtml = '';
    if (fastest && runnerUp) {
        const speedup = runnerUp.avgMs / fastest.avgMs;
        summaryHtml = `
            <div class="summary">
                🏆 <strong>${fastest.name}</strong> is fastest: ${fastest.avgMs.toFixed(4)} ms/parse,
                ${speedup.toFixed(2)}x faster than the runner-up (${runnerUp.name}).
            </div>`;
    }

    const cardsHtml = results.map(r => {
        const isWinner = fastest && r.name === fastest.name;
        const opsPerSec = r.error ? null : (1000 / r.avgMs);
        const preview = r.error
            ? `Error: ${escapeHtml(r.error)}`
            : escapeHtml(r.output.length > 5000 ? r.output.slice(0, 5000) + '\n... (truncated)' : r.output);

        return `
        <div class="result ${isWinner ? 'winner' : ''}">
            <h3>${r.name} ${isWinner ? '<span class="badge">FASTEST</span>' : ''}</h3>
            ${r.error
                ? `<p class="stats">${preview}</p>`
                : `<p class="stats">${r.avgMs.toFixed(4)} ms/parse avg &middot; ${opsPerSec.toFixed(0)} ops/sec &middot; averaged over ${r.runs} runs</p>`}
            <pre>${preview}</pre>
        </div>`;
    }).join('');

    resultsDiv.innerHTML = summaryHtml + `<div class="results-grid">${cardsHtml}</div>`;
});
