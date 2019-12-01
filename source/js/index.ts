import fetchWASM from './fetchWebAssembly';

type WebAssemblyModule = {
    instance: {
        exports: {
            memory: {
                buffer: ArrayBuffer
            },
            __heap_base: {
                value: number
            }
        }
    }
};

async function init() {
    const {bytes, memory, table} = await fetchWASM("./playground.wasm");

    // Obvi not a UTF8 proper.  Assumes ascii.
    function getString(ptr) {
        const out = [];
        for (let i = ptr; buffer8[i]; ++i) {
            out.push(String.fromCharCode(buffer8[i]));
        }
        return out.join('');
    }

    let heapPtr;
    let heapStart;
    let buffer8;
    let buffer32;

    function resetMemoryGlobals() {
        buffer8 = new Uint8Array(instance.exports.memory.buffer);
        buffer32 = new Uint32Array(instance.exports.memory.buffer);
        heapStart = heapPtr = instance.exports.__heap_base.value;
    }

    const env = {
        /**
         * needed.  As we adjust memory the js can become out of date.
         */
        resetMemoryGlobals,

        getStartOfHeapPtr() {
            return heapStart;
        },

        println(ptr) {
            console.log(getString(ptr));
        },

        printi(numberToPrint) {
            console.log(numberToPrint);
        },

        table,
        memory,
    };

    // @ts-ignore
    const module = await WebAssembly.instantiate(bytes, {
        env
    }) as WebAssemblyModule;

    const { instance } = module;
    resetMemoryGlobals();
}

init();

