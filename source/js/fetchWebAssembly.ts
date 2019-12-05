export type WasmModule = {
    bytes: ArrayBuffer;
    table: WebAssembly.Table;
    memory: WebAssembly.Memory;
};


const WASM_PAGE_SIZE = 65536;
const STATIC_BASE = 1024,
    STACK_BASE = 5246624,
    STACKTOP = STACK_BASE,
    STACK_MAX = 3744,
    DYNAMIC_BASE = 5246624,
    DYNAMICTOP_PTR = 3584;

const INITIAL_TOTAL_MEMORY = 16777216;

export default async function fetchWASM(path): Promise<WasmModule> {
    const bytes = await (await fetch(path)).arrayBuffer();
    const table = new WebAssembly.Table({
        'initial': 6,
        'element': 'anyfunc'
    });

    // these appear to do nothing....
    const memory = new WebAssembly.Memory({
      'initial': INITIAL_TOTAL_MEMORY / WASM_PAGE_SIZE,
      'maximum': INITIAL_TOTAL_MEMORY / WASM_PAGE_SIZE
    });

    return {
        bytes,
        memory,
        table
    };
};
