#include "esk8_log.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#ifndef ESK8_LOG_LEVEL
#define ESK8_LOG_LEVEL 0
#endif


const char* esk8_tag_l[ESK8_TAG_MAX] = {
    "MAIN", "PS2", "BTN", "BLE", "NVS", "ATH", "ONB", "RMT"
};

const char esk8_lvl_l[ESK8_LOG_LVL_MAX] = {
    'D', 'I', 'W', 'E'
};


void
esk8_log(
    esk8_log_level_t lvl,
    esk8_log_tag_t   tag,
    const char*      fmt,
    ...
)
{
    if (tag >= ESK8_TAG_MAX)
        return;

    if (lvl < ESK8_LOG_LEVEL)
        return;

    const char* tag_str = esk8_tag_l[tag];
    const char  lvl_str = esk8_lvl_l[lvl];

    va_list args;
    va_start(args, fmt);

    printf("%c:[%s] - ", lvl_str, tag_str);
    vprintf(fmt, args);

    va_end(args);
}
