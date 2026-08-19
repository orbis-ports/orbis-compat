// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
// See include/orbis_log.h.
#include <orbis_log.h>

#include <stddef.h>

static orbis_log_fn orbis_logger;
static orbis_log_fn orbis_logger_fatal;
static orbis_fatal_action_fn orbis_fatal_fn;

void orbis_set_log(orbis_log_fn fn)
{
    orbis_logger = fn;
}

int orbis_log_enabled(void)
{
    return orbis_logger != NULL;
}

void orbis_log(const char *fmt, ...)
{
    orbis_log_fn fn = orbis_logger;
    if (fn == NULL)
        return;

    va_list ap;
    va_start(ap, fmt);
    fn(fmt, ap);
    va_end(ap);
}

void orbis_set_log_fatal(orbis_log_fn fn)
{
    orbis_logger_fatal = fn;
}

void orbis_log_fatal(const char *fmt, ...)
{
    /* The fatal channel if there is one, the ordinary one if not: half a report beats none. */
    orbis_log_fn fn = orbis_logger_fatal ? orbis_logger_fatal : orbis_logger;
    if (fn == NULL)
        return;

    va_list ap;
    va_start(ap, fmt);
    fn(fmt, ap);
    va_end(ap);
}

void orbis_set_fatal_action(orbis_fatal_action_fn fn)
{
    orbis_fatal_fn = fn;
}

void orbis_fatal_action(const char *what)
{
    orbis_fatal_action_fn fn = orbis_fatal_fn;
    if (fn != NULL)
        fn(what);
}
