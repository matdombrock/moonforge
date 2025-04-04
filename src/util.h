#pragma once
#include <stdio.h>
#include <stdarg.h>
#include "globals.h"

// ANSI color codes
enum {
    COLOR_NONE = 0,
    COLOR_BLACK = 30,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_BLUE,
    COLOR_MAGENTA,
    COLOR_CYAN,
    COLOR_WHITE
};

void printc(int color, const char *str) {
    printf("\033[0;%dm", color);
    printf("%s", str);
    printf("\033[0m");
}

void debug(const char *format, ...) {
    if (_sys.output_mode != 1) return;
    if (_sys.tick_num % 128 != 0) return;
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}
