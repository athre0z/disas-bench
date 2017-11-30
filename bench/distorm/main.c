#include "../load_bin.inc"
#include <distorm.h>

int main()
{
    uint8_t *xul_code = NULL;
    size_t xul_code_len = 0;
    if (!read_xul_dll(&xul_code, &xul_code_len))
    {
        fputs("Can't read xul.dll\n", stderr);
        return 1;
    }

    size_t num_valid_insns = 0;
    size_t num_bad_insn = 0;
    for (int i = 0; i < 20; ++i)
    {
        _CodeInfo ci = {
            .codeOffset = 0,
            .code = xul_code,
            .codeLen = xul_code_len,
            .dt = Decode64Bits,
            .features = 0
        };

        for (;;)
        {
            unsigned used_insns = 0;
            _DInst insns[1024];

            switch (distorm_decompose64(
                &ci,
                insns,
                sizeof(insns) / sizeof(insns[0]),
                &used_insns
            ))
            {
            case DECRES_SUCCESS:
                goto next;
            case DECRES_MEMORYERR:
                break; 
            default:
                return 1;
            }

            for (size_t i = 0; i < used_insns; ++i)
            {
                if (insns[i].flags == FLAG_NOT_DECODABLE)
                {
                    ++num_bad_insn;
                }
                else 
                {
                    ++num_valid_insns;

#ifndef DISAS_BENCH_NO_FORMAT
                    _DecodedInst instr_fmt;
                    distorm_format64(
                        &ci,
                        insns + i,
                        &instr_fmt
                    );
                    /*
                    printf(
                        "%s %s\n", 
                        instr_fmt.mnemonic.p,
                        instr_fmt.operands.p
                    );
                    */
#endif
                }
            }

            size_t offs = (
                insns[used_insns - 1].addr + 
                insns[used_insns - 1].size
            );
            ci.code += offs;
            ci.codeLen -= offs;
        }

        next:;
    }

    printf(
        "Disassembled %zu instructions (%zu valid, %zu bad)\n", 
        num_valid_insns + num_bad_insn,
        num_valid_insns,
        num_bad_insn
    );

    free(xul_code);
    return 0;
}
