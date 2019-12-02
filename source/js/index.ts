import fetchWASM from './fetchWebAssembly';

type WebAssemblyModule = {
    instance: {
        exports: {
            add: (number) => number,
            testMemorySize: () => void,
            initMemoryPages: () => void,
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
        console.log("Heap Start", heapStart);
    }

    function println(ptr) {
        console.log(getString(ptr));
    }

    const env = {
        /**
         * needed.  As we adjust memory the js can become out of date.
         */
        resetMemoryGlobals,

        getStartOfHeapPtr() {
            return heapStart;
        },

        getStartOfHeapPtr2() {
            return heapStart;
        },

        printf(...args) {
            if (args.length === 1) {
                println(args[0]);
                return;
            }

            let currPos = 0;
            let currIdx = 1;
            const str: string = getString(args[0]);
            const out = [];

            do {
                const idx = str.indexOf("%", currPos);
                // TODO: Now I understand why I did this.
                // Clearly I think a head.
                if (idx === -1) {
                    out.push(str.substr(currPos));
                    break;
                }

                out.push(str.substring(currPos, idx));
                out.push(args[currIdx++]);

                const spaceIdx = str.indexOf(" ", idx + 1);
                const percIdx = str.indexOf("%", idx + 1);

                if (percIdx > 0 && spaceIdx > 0) {
                    currPos = Math.min(percIdx, spaceIdx);
                }

                else {
                    currPos = Math.max(percIdx, spaceIdx);
                }

            } while (currIdx < args.length);

            console.log(out.join(''));
        },

        println,

        printPtr(namePtr, ptr) {
            console.log(getString(namePtr), ptr);
        },

        printci(namePtr, integer) {
            console.log(getString(namePtr), integer);
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

    // @ts-ignore
    window.instance = instance;
}

init();

