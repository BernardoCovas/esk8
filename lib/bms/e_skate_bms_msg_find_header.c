#include <e_skate_bms.h>

#include <string.h>


int e_skate_bms_msg_find_header(
    uint8_t* buffer,
    int buf_length
)
{
    const static uint8_t msg_header[] = E_SKATE_MSG_HEADER;

    if (buf_length < 2)
        return -1;

    for(int i=0; i<buf_length-1; i++)
    {
        int memcmp_res = memcmp(
            buffer+i,
            msg_header,
            2 * sizeof(uint8_t)
            );

        if (memcmp_res == 0)
            return i;
    }

    return -1;
}
