#include "esk8_ps2_utils.h"


int
esk8_ps2_get_bit(
    esk8_ps2_frame_t* frame,
    int               idx
)
{
    if (idx > 10)
        return -1;

    switch (idx)
    {
    case 0:
        return frame->start;
    case 9:
        return frame->parity;
    case 10:
        return frame->stop;

    default:
        break;
    }

    idx -= 1;
    return frame->byte & (1 << idx);
}


void
esk8_ps2_set_bit(
    esk8_ps2_frame_t* frame,
    int               idx,
    int               val
)
{
    if (idx > 10)
        return;

    val = val ? 1 : 0;

    switch (idx)
    {
    case 0:
        frame->start = val;
        return;
    case 9:
        frame->parity = val;
        return;
    case 10:
        frame->stop = val;
        return;

    default:
        break;
    }

    idx -= 1;
    if (val)
        frame->byte |= (1 << idx);
    else
        frame->byte &= ~(1 << idx)
}


int
esk8_ps2_get_parity(
    uint8_t byte
)
{
    x = (x & 0x0F)^(x >> 4);
    x = (x & 0x03)^(x >> 2);
    x = (x & 0x01)^(x >> 1);

    return !(x & 1);
}