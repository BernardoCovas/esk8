#ifndef _ESK8_UART_H
#define _ESK8_UART_H

#include <esk8_err.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>


#define ESK8_UART_CONFIG_DEFAULT_ESP32()      \
{                                               \
    .baud_rate = 115200,                        \
    .data_bits = UART_DATA_8_BITS,              \
    .parity = UART_PARITY_DISABLE,              \
    .stop_bits = UART_STOP_BITS_1,              \
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE       \
}


#define ESK8_MSG_PKT_HEADER {0x5A, 0xA5}
#define ESK8_MSG_HEADER_SIZE 5
#define ESK8_MSG_MIN_SIZE 9


typedef struct esk8_uart_msg_t
{
    uint8_t  pld_length;
    uint8_t  src_address;
    uint8_t  dst_address;
    uint8_t  cmd_command;
    uint8_t  cmd_argment;
    uint8_t* payload;
    uint8_t  chk_sum[2];
} esk8_uart_msg_t;


typedef enum esk8_uart_reg_t
{
    /* Status */
    ESK8_REG_BMS_CAPACITY    = 0x32,
    ESK8_REG_BMS_CURRENT     = 0x33,
    ESK8_REG_BMS_VOLTAGE     = 0x34,
    ESK8_REG_BMS_TEMPRTR     = 0x35,
    ESK8_REG_BMS_HEALTH      = 0x3B,

    /* Deep Status */
    ESK8_REG_BMS_CELL0_V     = 0x40,
    ESK8_REG_BMS_CELL1_V     = 0x41,
    ESK8_REG_BMS_CELL2_V     = 0x42,
    ESK8_REG_BMS_CELL3_V     = 0x43,
    ESK8_REG_BMS_CELL4_V     = 0x44,
    ESK8_REG_BMS_CELL5_V     = 0x45,
    ESK8_REG_BMS_CELL6_V     = 0x46,
    ESK8_REG_BMS_CELL7_V     = 0x47,
    ESK8_REG_BMS_CELL8_V     = 0x48,
    ESK8_REG_BMS_CELL9_V     = 0x49,
    ESK8_REG_BMS_MANUFACTURE_DATE    = 0x10,
    ESK8_REG_BMS_SERIAL_NUMBER       = 0x10,
    ESK8_REG_BMS_FW_VERSION          = 0x17,
    ESK8_REG_BMS_FACTORY_CAPACITY    = 0x18,
    ESK8_REG_BMS_ACTUAL_CAPACITY     = 0x19,
    ESK8_REG_BMS_CHARGE_FULL_CYCLES  = 0x1B,
    ESK8_REG_BMS_CHARGE_COUNT       = 0x1C,
    ESK8_REG_BMS_CAPACITY_mAh        = 0x31,
} esk8_uart_reg_t;


typedef enum esk8_uart_addr_t
{
    ESK8_ADDR_ESC = 0x20,
    ESK8_ADDR_BLE = 0x21,
    ESK8_ADDR_BMS = 0x22,
    ESK8_ADDR_APP = 0x3e,
} esk8_uart_addr_t;


/**
 * Finds the first msg header in `buffer`.
 * If no header is found, returns -1.
 **/
int esk8_uart_msg_find_header(
    
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
 * Returns `ESK8_UART_MSG_SUCCESS`
 * if the message is valid and was parsed
 * properly. Any error does not allocate
 * resources.
 **/
esk8_err_t esk8_uart_msg_parse(

    uint8_t* buffer,
    size_t buf_length,
    esk8_uart_msg_t* outMsg

);


/**
 * Gets the length of the buffer needed
 * to hold the entire serialized message,
 * including the initial pkt header of
 * size 2, and the checksum of size 2.
 **/
size_t esk8_uart_msg_get_serialized_length(

    esk8_uart_msg_t msg

);


/**
 * Serializes `msg` into `buff`.
 * The length of the buffer needed
 * can be obtained with
 * `esk8_uart_msg_get_serialized_length`.
 **/
void esk8_uart_msg_serialize(

    esk8_uart_msg_t msg,
    uint8_t *buff

);


/**
 * Creates a new message for reading
 * the specified register of the device
 * with `dstAddr` address.
 * The new message asks for `readLen`
 * bytes from the specified register.
 **/
esk8_err_t esk8_uart_regread_msg_new(

    esk8_uart_addr_t dstAddr,
    esk8_uart_reg_t reg,
    uint8_t readLen,
    esk8_uart_msg_t *outMsg

);


/**
 * Cleans the resources of a message.
 **/
void esk8_uart_msg_free(

    esk8_uart_msg_t msg

);


/**
 * Calculates and writes the
 * buffer checksum to `chkSumBuf`.
 * This out buffer must have space
 * for at least 2 elements.
 **/
void esk8_uart_buff_chk_calc(

    uint8_t* buffer,
    size_t   bufferLen,
    uint8_t  chkSumBuf[2]

);


/**
 * Calculates the checksum for
 * a message.
 **/
void esk8_uart_msg_chk_calc(

    esk8_uart_msg_t msg,
    uint8_t chkSum[2]

);


/**
 * Calculates the checksum of the supplied
 * Overrides the `msg_chsum` field.
 **/
void esk8_uart_msg_set_chk(

    esk8_uart_msg_t* msg

);


/**
 * Verifies the integrity of the message.
 * Returns `ESK8_UART_MSG_SUCCESS` if
 * valid, any of the errors if not valid.
 **/
esk8_err_t esk8_uart_msg_chk(

    esk8_uart_msg_t msg

);


#endif /* _ESK8_UART_H */