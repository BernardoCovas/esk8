#ifndef _E_SKATE_BMS_H
#define _E_SKATE_BMS_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h>


#define E_SKATE_UART_CONFIG_DEFAULT_ESP32()     \
{                                               \
    .baud_rate = 115200,                        \
    .data_bits = UART_DATA_8_BITS,              \
    .parity = UART_PARITY_DISABLE,              \
    .stop_bits = UART_STOP_BITS_1,              \
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE       \
}


#define E_SKATE_MSG_PKT_HEADER {0x5A, 0xA5}
#define E_SKATE_MSG_HEADER_SIZE 5
#define E_SKATE_MSG_MIN_SIZE 9


typedef enum e_skate_uart_err_t
{
    E_SKATE_UART_MSG_SUCCESS,
    E_SKATE_UART_MSG_ERR_NO_HEADER,
    E_SKATE_UART_MSG_ERR_INVALID_PLDLEN,
    E_SKATE_UART_MSG_ERR_INVALID_CHKSUM,
    E_SKATE_UART_MSG_ERR_INVALID_BUFFER,
} e_skate_uart_err_t;


typedef struct e_skate_uart_msg_t
{
    uint8_t  pld_length;
    uint8_t  src_address;
    uint8_t  dst_address;
    uint8_t  cmd_command;
    uint8_t  cmd_argment;
    uint8_t* payload;
    uint8_t  chk_sum[2];
} e_skate_uart_msg_t;


typedef enum e_skate_uart_reg_t
{
    E_SKATE_REG_BMS_CAPACITY = 0x32,
    E_SKATE_REG_BMS_CURRENT  = 0x33,
    E_SKATE_REG_BMS_VOLTAGE  = 0x34,
    E_SKATE_REG_BMS_TEMPRTR  = 0x35,
    E_SKATE_REG_BMS_HEALTH   = 0x3B,
    E_SKATE_REG_BMS_CELL1_V  = 0x40,
} e_skate_uart_reg_t;


/**
 * Finds the first msg header in `buffer`.
 * If no header is found, returns -1.
 **/
int e_skate_uart_msg_find_header(
    uint8_t* buffer,
    int buf_length
);


/**
 * Creates a new message from a
 * `uint8_t` buffer.
 * Finds the header, and the returned
 * msg has a copy of the valid part of the
 * buffer, if the message is valid.
 * Allocates payload resources.
 * Returns `E_SKATE_UART_MSG_SUCCESS`
 * if the message is valid and was parsed
 * properly.
 **/
e_skate_uart_err_t e_skate_uart_msg_parse(
    uint8_t* buffer,
    size_t buf_length,
    e_skate_uart_msg_t* outMsg
);


/**
 * Gets the length of the buffer needed
 * to hold the entire serialized message,
 * including the initial pkt header of
 * size 2, and the checksum of size 2.
 **/
size_t e_skate_uart_msg_get_serialized_length(
    e_skate_uart_msg_t msg
);


/**
 * Serializes `msg` into `buff`.
 * The length of the buffer needed
 * can be obtained with
 * `e_skate_uart_msg_get_serialized_length`.
 **/
void e_skate_uart_msg_serialize(
    e_skate_uart_msg_t msg,
    uint8_t *buff
);


/**
 * Creates a new message for reading
 * the specified register.
 * The new message asks for `readLen`
 * bytes from the specified register.
 **/
e_skate_uart_err_t e_skate_uart_regread_msg_new(
    e_skate_uart_reg_t reg,
    uint8_t readLen,
    e_skate_uart_msg_t *outMsg
);


/**
 * Cleans the resources of a message.
 **/
void e_skate_uart_msg_free(
    e_skate_uart_msg_t msg
);


/**
 * Calculates and writes the
 * buffer checksum to `chkSumBuf`.
 * This out buffer must have space
 * for at least 2 elements.
 **/
void e_skate_uart_buff_chk_calc(
    uint8_t* buffer,
    size_t   bufferLen,
    uint8_t  chkSumBuf[static 2]
);


/**
 * Calculates the checksum for
 * a message.
 **/
void e_skate_uart_msg_chk_calc(
    e_skate_uart_msg_t msg,
    uint8_t chkSum[static 2]
);


/**
 * Calculates the checksum of the supplied
 * Overrides the `msg_chsum` field.
 **/
void e_skate_uart_msg_set_chk(
    e_skate_uart_msg_t* msg
);


/**
 * Verifies the integrity of the message.
 * Returns `E_SKATE_UART_MSG_SUCCESS` if
 * valid, any of the errors if not valid.
 **/
e_skate_uart_err_t e_skate_uart_msg_chk(
    e_skate_uart_msg_t msg
);


#endif /* _E_SKATE_BMS_H */