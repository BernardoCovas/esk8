#ifndef _E_SKATE_UART_ESP_h
#define _E_SKATE_UART_ESP_h

#include <e_skate_uart.h>

#include <driver/uart.h>

typedef enum e_skate_uart_esp_err_t
{
    E_SKATE_UART_SUCCESS,
    E_SKATE_UART_BAD_PARAM,
    E_SKATE_UART_FAIL,
} e_skate_uart_esp_err_t;


/**
 * Called when a valid message is received.
 **/
typedef void (*e_skate_uart_msg_cb_t)(e_skate_uart_msg_t msg);


typedef struct
{
    e_skate_uart_msg_cb_t   msg_cb;
    uart_port_t             uartNum;
    uint8_t                 curBmsPin;
    uint8_t                 numBat;
} e_skate_uart_esp_bms_t;


/**
 * 
 **/
e_skate_uart_esp_err_t e_skate_uart_esp_bms_init(
    e_skate_uart_esp_bms_t* eSkateUart,
    uart_port_t         uartPort,
    uint8_t             uartRxI
);

e_skate_uart_esp_err_t e_skate_uart_esp_bms_set_rx(
    e_skate_uart_esp_bms_t* eSkateUart,
    uint8_t uartRxI
);


#endif /* _E_SKATE_UART_ESP_h */
