#include "../load_bin.inc"
#include <Zydis/Zydis.h>

int main(int argc, char* argv[])
{
#ifndef DISAS_BENCH_NO_FORMAT
    ZydisFormatter formatter;
    if (!ZYAN_SUCCESS(ZydisFormatterInit(
        &formatter,
        ZYDIS_FORMATTER_STYLE_INTEL
    )))
    {
        fputs("Unable to initialize instruction formatter\n", stderr);
        return 1;
    }
#endif

    uint8_t *code = NULL;
    size_t code_len = 0, loop_count = 0;
    if (!read_file(argc, argv, &code, &code_len, &loop_count))
    {
        return 1;
    }

    ZydisDecoder decoder;
    ZydisDecoderInit(
        &decoder,
        ZYDIS_MACHINE_MODE_LONG_64,
        ZYDIS_STACK_WIDTH_64
    );

#ifdef DISAS_BENCH_DECODE_MINIMAL
    ZydisDecoderEnableMode(
        &decoder,
        ZYDIS_DECODER_MODE_MINIMAL,
        ZYAN_TRUE
    );
#endif

    size_t num_valid_insns = 0;
    size_t num_bad_insn = 0;
    size_t read_offs;
    clock_t start_time = clock();
    for (int i = 0; i < loop_count; ++i)
    {
        read_offs = 0;

        ZyanStatus status;
        ZydisDecodedInstruction info;
        ZydisDecoderContext ctx;
        while ((status = ZydisDecoderDecodeInstruction(
            &decoder,
            &ctx,
            code + read_offs,
            code_len - read_offs,
            &info
        )) != ZYDIS_STATUS_NO_MORE_DATA)
        {
            if (!ZYAN_SUCCESS(status))
            {
                ++read_offs;
                ++num_bad_insn;
                continue;
            }

#ifndef DISAS_BENCH_DECODE_MINIMAL
            ZydisDecodedOperand ops[ZYDIS_MAX_OPERAND_COUNT_VISIBLE];
            if (ZYAN_FAILED(ZydisDecoderDecodeOperands(
                &decoder,
                &ctx,
                &info,
                ops,
                ZYDIS_MAX_OPERAND_COUNT_VISIBLE
            ))) {
                abort();
            }
#endif

#ifndef DISAS_BENCH_NO_FORMAT
            char print_buffer[256];
            if (ZYAN_FAILED(ZydisFormatterFormatInstruction(
                &formatter, 
                &info, 
                ops,
                ZYDIS_MAX_OPERAND_COUNT_VISIBLE,
                print_buffer, 
                sizeof(print_buffer), 
                read_offs,
                NULL
            ))) {
                abort();
            }
#endif

            read_offs += info.length;
            ++num_valid_insns;
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

    free(code);
    return 0;
}
