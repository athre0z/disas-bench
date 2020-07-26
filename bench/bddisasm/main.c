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

int main()
{
    INSTRUX ix;
    NDSTATUS status;
    char text[ND_MIN_BUF_SIZE];

    uint8_t *xul_code = NULL;
    size_t xul_code_len = 0;
    if (!read_xul_dll(&xul_code, &xul_code_len))
    {
        fputs("Can't read xul.dll\n", stderr);
        return 1;
    }

    size_t num_valid_insns = 0;
    size_t num_bad_insn = 0;
    for (size_t round = 0; round < 20; ++round)
    {
        for (size_t read_offs = 0; read_offs < xul_code_len; )
        {
            status = NdDecodeEx(
                &ix,
                xul_code + read_offs,
                xul_code_len - read_offs,
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
    
    printf(
        "Disassembled %zu instructions (%zu valid, %zu bad)\n", 
        num_valid_insns + num_bad_insn,
        num_valid_insns,
        num_bad_insn
    );
    
    return 0;
}
