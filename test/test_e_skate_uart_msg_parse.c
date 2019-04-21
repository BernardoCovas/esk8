#include "test_e_skate.h"

#include <e_skate_uart.h>

#include <unity.h>
#include <stdint.h>


void test_e_skate_uart_msg_parse()
{
    uint8_t   valid_msg                 [] = {0x5A, 0xA5, 0x01, 0x3E, 0x20, 0x02, 0x78, 0x01, 0x25, 0xFF}; // Valid msg.
    uint8_t invalid_msg_pldlen          [] = {0x5A, 0xA5, 0x02, 0x3E, 0x20, 0x02, 0x78, 0x01, 0x24, 0xFF}; // Wrong advertized length, with correct checksum.
    uint8_t invalid_msg_cksum           [] = {0x5A, 0xA5, 0x01, 0x3E, 0x20, 0x02, 0x78, 0x01, 0x24, 0xFF}; // Wrong message checksum.
    uint8_t invalid_msg_no_hdr          [] = {0x5A, 0xA4, 0x01, 0x3E, 0x20, 0x02, 0x78, 0x01, 0x24, 0xFF}; // No valid header in buffer.
    uint8_t invalid_msg_buffer          [] = {0x01, 0x3E, 0x20, 0x02, 0x5A, 0xA5, 0x03, 0x01, 0x24, 0x34}; // No more space in buffer for advertized payload length.

    e_skate_uart_msg_t msg;
    e_skate_uart_msg_err_t errCode;

    errCode = e_skate_uart_msg_parse(valid_msg, 10, &msg);
    TEST_ASSERT_EQUAL(E_SKATE_UART_MSG_SUCCESS, errCode);
    if(errCode == E_SKATE_UART_MSG_SUCCESS)
        e_skate_uart_msg_free(msg);

    errCode = e_skate_uart_msg_parse(invalid_msg_pldlen, 10, &msg);
    TEST_ASSERT_EQUAL(E_SKATE_UART_MSG_ERR_INVALID_PLDLEN, errCode);
     if(errCode == E_SKATE_UART_MSG_SUCCESS)
        e_skate_uart_msg_free(msg);

    errCode = e_skate_uart_msg_parse(invalid_msg_cksum, 10, &msg);
    TEST_ASSERT_EQUAL(E_SKATE_UART_MSG_ERR_INVALID_CHKSUM, errCode);
    if(errCode == E_SKATE_UART_MSG_SUCCESS)
        e_skate_uart_msg_free(msg);

    errCode = e_skate_uart_msg_parse(invalid_msg_no_hdr, 10, &msg);
    TEST_ASSERT_EQUAL(E_SKATE_UART_MSG_ERR_NO_HEADER, errCode);
    if(errCode == E_SKATE_UART_MSG_SUCCESS)
        e_skate_uart_msg_free(msg);

    errCode = e_skate_uart_msg_parse(invalid_msg_buffer, 10, &msg);
    TEST_ASSERT_EQUAL(E_SKATE_UART_MSG_ERR_INVALID_BUFFER, errCode);
    if(errCode == E_SKATE_UART_MSG_SUCCESS)
        e_skate_uart_msg_free(msg);
}
