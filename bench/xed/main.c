#include "../load_bin.inc"
#include <xed-interface.h>


int main() 
{
    xed_tables_init();

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
            xed_decoded_inst_t insn;
            xed_decoded_inst_zero(&insn);
            xed_decoded_inst_set_mode(
                &insn, 
                XED_MACHINE_MODE_LONG_64, 
                XED_ADDRESS_WIDTH_64b
            );
            
            if (xed_decode(
                &insn, 
                XED_STATIC_CAST(const xed_uint8_t*, xul_code + read_offs),
                xul_code_len - read_offs
            ))
            {
                ++read_offs;
                ++num_bad_insn;
            }
            else
            {
#ifndef DISAS_BENCH_NO_FORMAT
                char print_buf[256];
                xed_format_context(
                    XED_SYNTAX_INTEL,
                    &insn,
                    print_buf,
                    sizeof print_buf,
                    0,
                    NULL,
                    NULL
                );
#endif

                read_offs += xed_decoded_inst_get_length(&insn);
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
