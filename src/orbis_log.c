// See include/orbis_log.h.
#include <orbis_log.h>

#include <stddef.h>

static orbis_log_fn orbis_logger;

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
