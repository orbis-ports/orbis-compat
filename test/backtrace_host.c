// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
/* Runs the unwinding logic on the host, because the algorithm is platform-independent even though
 * the reason it exists is not. Proves the frames are collected, in order, and bounded. */
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int fail;

#define CHECK(cond, msg)                                                                           \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            fprintf(stderr, "FAIL: %s\n", msg);                                                    \
            fail = 1;                                                                              \
        }                                                                                          \
    } while (0)

static void *frames[32];
static int   count;

static void innermost(void)
{
    count = backtrace(frames, 32);
}

static void middle(void) { innermost(); }
static void outermost(void) { middle(); }

int main(void)
{
    outermost();

    CHECK(count > 3, "fewer frames than the call chain that produced them");
    CHECK(count <= 32, "more frames than the buffer holds");
    for (int i = 0; i < count; ++i)
        CHECK(frames[i] != NULL, "a null return address was collected");

    /* The bound must be honoured exactly, not approximately. */
    void *small[2];
    CHECK(backtrace(small, 2) == 2, "the size limit was not respected");
    CHECK(backtrace(small, 0) == 0, "a zero-sized request returned frames");
    CHECK(backtrace(NULL, 8) == 0, "a null buffer was written to");

    char **syms = backtrace_symbols(frames, count);
    CHECK(syms != NULL, "backtrace_symbols returned nothing");
    if (syms) {
        CHECK(syms[0][0] == '0' && syms[0][1] == 'x', "an address is not formatted as hex");
        free(syms);
    }

    printf("%d frames, innermost %s\n", count, count ? "collected" : "MISSING");
    backtrace_symbols_fd(frames, count < 4 ? count : 4, STDOUT_FILENO);

    return fail;
}
