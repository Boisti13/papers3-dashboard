#pragma once
// Host stub for rendering the UI without hardware.
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <algorithm>
using std::min; using std::max;
uint32_t millis();
inline void delay(uint32_t) {}
struct SerialStub {
    template <typename... A> void printf(const char *f, A... a) { ::printf(f, a...); }
    void print(const char *s) { fputs(s, stdout); }
    void println(const char *s = "") { puts(s); }
};
extern SerialStub Serial;
