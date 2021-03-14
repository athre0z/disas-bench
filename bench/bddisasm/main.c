#include <string.h>
#include "../load_bin.inc"

#include "disasmtypes.h"
#include "bddisasm.h"

int nd_vsnprintf_s(char *buffer, size_t sizeOfBuffer, size_t count, const char *format, va_list argptr)
{
    return vsnprintf(buffer, sizeOfBuffer, format, argptr);
}

void *
nd_memset(void *s, int c, size_t n)
{
    return memset(s, c, n);
}

int main(int argc, char* argv[])
{
    INSTRUX ix;
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
            status = NdDecodeEx(
                &ix,
                code + read_offs,
                code_len - read_offs,
                ND_CODE_64,
                ND_DATA_64
            );
            if (!ND_SUCCESS(status)) {
                ++read_offs;
                ++num_bad_insn;
            }
            else
            {
#ifndef DISAS_BENCH_NO_FORMAT
                NdToText(&ix, 0, sizeof(text), text);
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
