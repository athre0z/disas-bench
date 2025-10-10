#include <string.h>
#include "../load_bin.inc"

#include "bddisasm.h"

#ifdef DISASM_BENCH_USE_BDD_MINI
#define FmtFunc NdToTextMini
#else
#define FmtFunc NdToText
#endif

int main(int argc, char* argv[])
{
    NDSTATUS status;
    char text[ND_MIN_BUF_SIZE];

    uint8_t *code = NULL;
    size_t code_len = 0, loop_count = 0;
    if (!read_file(argc, argv, &code, &code_len, &loop_count))
    {
        return 1;
    }

    size_t num_valid_insns = 0;
    size_t num_bad_insn = 0;
    clock_t start_time = clock();
    for (size_t round = 0; round < loop_count; ++round)
    {
        for (size_t read_offs = 0; read_offs < code_len; )
        {
#ifdef DISASM_BENCH_USE_BDD_MINI
            INSTRUX_MINI ix;
            status = NdDecodeMini(
                &ix,
                code + read_offs,
                code_len - read_offs,
                ND_CODE_64
            );
#else
            INSTRUX ix;
            status = NdDecodeEx(
                &ix,
                code + read_offs,
                code_len - read_offs,
                ND_CODE_64,
                ND_DATA_64
            );
#endif
            if (!ND_SUCCESS(status)) {
                ++read_offs;
                ++num_bad_insn;
            }
            else
            {
#ifndef DISAS_BENCH_NO_FORMAT
                FmtFunc(&ix, 0, sizeof(text), text);
#endif

                read_offs += ix.Length;
                ++num_valid_insns;
            }
        }
    }
    clock_t end_time = clock();

    printf(
        "Disassembled %zu instructions (%zu valid, %zu bad), %.2f ms\n",
        num_valid_insns + num_bad_insn,
        num_valid_insns,
        num_bad_insn,
        (double)(end_time - start_time) * 1000.0 / CLOCKS_PER_SEC
    );

    return 0;
}
