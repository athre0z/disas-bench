#include "../load_bin.inc"
#include <capstone.h>


int main() 
{
    csh handle = 0;
    cs_insn *insn = NULL;
    int ret = 0;
    uint8_t *xul_code = NULL;
    const uint8_t *xul_code_iter = NULL;
    size_t xul_code_len = 0;
    size_t xul_code_len_iter = 0;
    uint64_t ip = 0;
    size_t num_valid_insns = 0;
    size_t num_bad_insn = 0;
    size_t round;

    if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK) 
    {
        fputs("Unable to create Capstone handle\n", stderr);
        ret = 1;
        goto leave;
    }

    if (!read_xul_dll(&xul_code, &xul_code_len))
    {
        fputs("Can't read xul.dll\n", stderr);
        ret = 1;
        goto leave;
    }

    insn = cs_malloc(handle);
    if (!insn)
    {
        fputs("Failed to allocate memory\n", stderr);
        ret = 1;
        goto leave;
    }

    for (round = 0; round < 20; ++round)
    {
        xul_code_iter = xul_code;
        xul_code_len_iter = xul_code_len;
        while (xul_code_len_iter > 0)
        {
            if (!cs_disasm_iter(
                handle, 
                &xul_code_iter, 
                &xul_code_len_iter, 
                &ip, 
                insn
            ))
            {
                ++xul_code_iter;
                --xul_code_len_iter;
                ++num_bad_insn;
            }
            else
            {
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
    
leave:
    if (insn) cs_free(insn, 1);
    if (handle) cs_close(&handle);
    if (xul_code) free(xul_code);
    return ret;
}
