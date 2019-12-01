export type WasmModule = {
    bytes: ArrayBuffer;
    table: WebAssembly.Table;
    memory: WebAssembly.Memory;
};

export default async function fetchWASM(path): Promise<WasmModule> {
    const bytes = await (await fetch(path)).arrayBuffer();
    const table = new WebAssembly.Table({
        'initial': 6,
        'element': 'anyfunc'
    });

    // these appear to do nothing....
    const memory = new WebAssembly.Memory({
        'initial': 1,
        'maximum': 50,
    });

    return {
        bytes,
        memory,
        table
    };
};
