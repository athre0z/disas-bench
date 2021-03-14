#include "../load_bin.inc"
#include <xed-interface.h>


int main(int argc, char* argv[]) 
{
    xed_tables_init();

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
            xed_decoded_inst_t insn;
            xed_decoded_inst_zero(&insn);
            xed_decoded_inst_set_mode(
                &insn, 
                XED_MACHINE_MODE_LONG_64, 
                XED_ADDRESS_WIDTH_64b
            );
            
            if (xed_decode(
                &insn, 
                XED_STATIC_CAST(const xed_uint8_t*, code + read_offs),
                code_len - read_offs
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
