#ifndef _E_SKATE_BMS_H
#define _E_SKATE_BMS_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define E_SKATE_MSG_HEADER {0x5A, 0xA5}

typedef enum e_skate_bms_err_t
{
    E_SKATE_BMS_MSG_SUCCESS,
    E_SKATE_BMS_MSG_ERR_INVALID_CHKSUM,
    E_SKATE_BMS_MSG_ERR_INVALID_PLDLEN,
} e_skate_bms_err_t;


typedef struct e_skate_bms_msg_t {
    uint8_t  raw_buffer_len;
    uint8_t *raw_buffer;
    uint8_t *pkt_header;
    uint8_t *pld_length;
    uint8_t *src_address;
    uint8_t *dst_address;
    uint8_t *cmd_command;
    uint8_t *cmd_argment;
    uint8_t *msg_payload;
    uint8_t *msg_cheksum;
} e_skate_bms_msg_t;


/**
 * Finds the first msg header in `buffer`.
 * If no header is found, returns -1.
 **/
int e_skate_bms_msg_find_header(
    uint8_t* buffer,
    int buf_length
);


/**
 * Creates a new message from a
 * `uint8_t` buffer.
 * Allocates payload resources.
 * Returns `E_SKATE_BMS_MSG_SUCCESS`
 * if the message is valid and was parsed
 * properly.
 **/
e_skate_bms_err_t e_skate_bms_msg_new(
    uint8_t* buffer,
    size_t buf_length,
    e_skate_bms_msg_t* outMsg
);


/**
 * Cleans the resources of a message.
 **/
void e_skate_bms_msg_free(
    e_skate_bms_msg_t msg
);


/**
 * Calculates and writes the
 * message checksum to `chkSumBuf`.
 * This buffer must have at least
 * 2 elements. 
 **/
void e_skate_bms_msg_chk_calc(
    e_skate_bms_msg_t msg,
    uint8_t chkSumBuf[static 2]
);


/**
 * Verifies the integrity of the message.
 * Returns `E_SKATE_BMS_MSG_SUCCESS` if
 * valid, any of the errors if not valid.
 **/
e_skate_bms_err_t e_skate_bms_msg_chk(
    e_skate_bms_msg_t msg
);


#endif /* _E_SKATE_BMS_H */