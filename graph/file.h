#pragma once
#include <stdio.h>
#include "sys-defines.h"

void close_file (char *filename, FILE *stream);
void open_file_for_reading (char *filename, FILE **input);
