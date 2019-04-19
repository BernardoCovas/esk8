#include <e_skate_bms.h>

e_skate_bms_err_t e_skate_bms_msg_new(
    e_skate_bms_reg_t reg,
    e_skate_bms_msg_t *outMsg
)
{
    e_skate_bms_msg_t newMsg = {
        .pld_length = 1,
        .src_address = 0x20,
        .dst_address = 0x22,
        .cmd_command = 0x01,            // Read registers
        .cmd_argment = (uint8_t) reg,   // Reg to read
        .payload = (uint8_t*) malloc(1)
    };

    newMsg.payload[0] = 0x02;
    e_skate_bms_msg_set_chk(&newMsg);

    (*outMsg) = newMsg;

    return E_SKATE_BMS_MSG_SUCCESS;
}