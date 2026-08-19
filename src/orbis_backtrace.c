// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
/* backtrace(3) over the unwinder the toolchain already links. See include/execinfo.h. */
#include <execinfo.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unwind.h>
#include <unistd.h>

struct orbis_unwind_state {
    void **buffer;
    int size;
    int count;
    int skip; /* our own frames, which the caller did not ask about */
};

static _Unwind_Reason_Code orbis_unwind_step(struct _Unwind_Context *ctx, void *arg)
{
    struct orbis_unwind_state *st = arg;

    if (st->skip > 0) {
        st->skip--;
        return _URC_NO_REASON;
    }

    if (st->count >= st->size)
        return _URC_END_OF_STACK;

    /* A zero IP means the unwinder ran out of frame information rather than out of stack. */
    uintptr_t ip = (uintptr_t) _Unwind_GetIP(ctx);
    if (ip == 0)
        return _URC_END_OF_STACK;

    st->buffer[st->count++] = (void *) ip;
    return _URC_NO_REASON;
}

int orbis_unwind_collect(void **buffer, int size, int skip)
{
    if (buffer == NULL || size <= 0)
        return 0;

    struct orbis_unwind_state st = {buffer, size, 0, skip};
    _Unwind_Backtrace(orbis_unwind_step, &st);
    return st.count;
}

int backtrace(void **buffer, int size)
{
    /* Skip orbis_unwind_step's own frame and this one. */
    return orbis_unwind_collect(buffer, size, 1);
}

/* Hex by hand: this runs after a fault, where snprintf may already be unusable. */
static int orbis_write_hex(char *out, uintptr_t v)
{
    static const char digits[] = "0123456789abcdef";
    int n = 0;

    out[n++] = '0';
    out[n++] = 'x';

    int started = 0;
    for (int shift = (int) (sizeof(uintptr_t) * 8) - 4; shift >= 0; shift -= 4) {
        unsigned nibble = (unsigned) ((v >> shift) & 0xF);
        if (nibble != 0 || started || shift == 0) {
            out[n++] = digits[nibble];
            started = 1;
        }
    }

    out[n++] = '\n';
    return n;
}

void backtrace_symbols_fd(void *const *buffer, int size, int fd)
{
    if (buffer == NULL)
        return;

    for (int i = 0; i < size; ++i) {
        char line[2 + sizeof(uintptr_t) * 2 + 2];
        int n = orbis_write_hex(line, (uintptr_t) buffer[i]);

        /* Short writes are ignored on purpose: a crash dump is best-effort. */
        (void) !write(fd, line, (size_t) n);
    }
}

char **backtrace_symbols(void *const *buffer, int size)
{
    if (buffer == NULL || size <= 0)
        return NULL;

    const size_t stride = 2 + sizeof(uintptr_t) * 2 + 1;
    char **out = malloc((size_t) size * sizeof(char *) + (size_t) size * stride);
    if (out == NULL)
        return NULL;

    char *text = (char *) (out + size);
    for (int i = 0; i < size; ++i) {
        int n = orbis_write_hex(text, (uintptr_t) buffer[i]);
        text[n - 1] = '\0'; /* the newline orbis_write_hex appended */
        out[i] = text;
        text += stride;
    }

    return out;
}
