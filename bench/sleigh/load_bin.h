#pragma once

#include <stdint.h>
#include <stdlib.h>

int read_file_data(uint8_t **buf, const char *filename, size_t file_offset, size_t bin_len);
int read_file(int argc, char *argv[], uint8_t **buf, size_t *bin_len, size_t *loop_count);
