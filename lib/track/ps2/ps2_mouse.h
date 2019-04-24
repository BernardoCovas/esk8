#ifndef _PS2_MOUSE_H
#define _PS2_MOUSE_H

#include <stdint.h>

typedef struct
{
    uint8_t clockPin;
    uint8_t dataPin;
} ps2_mouse_config_t;

#endif /* _PS2_MOUSE_H */
