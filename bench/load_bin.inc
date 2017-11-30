#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


#define XUL_TEXT_OFFS (    0x400)
#define XUL_TEXT_LEN  (0x2460400)


int read_xul_dll(uint8_t** buf, size_t* len)
{
    FILE* f = fopen("../../input/xul.dll", "rb");
    if (!f) return 0;

    // Seek to code section
    if (fseek(f, XUL_TEXT_OFFS, SEEK_SET)) 
    { 
        fclose(f);
        return 0; 
    }

    // Hardcoded length.
    uint8_t* code = malloc(XUL_TEXT_LEN);
    if (!code)
    {
        fclose(f);
        return 0;
    }

    if (fread(code, 1, XUL_TEXT_LEN, f) != XUL_TEXT_LEN)
    {
        fclose(f);
        free(code);
        return 0;
    }

    *buf = code;
    *len = XUL_TEXT_LEN;

    return 1;
}
