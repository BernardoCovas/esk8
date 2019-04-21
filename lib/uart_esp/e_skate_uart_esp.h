#ifndef _E_SKATE_UART_ESP_h
#define _E_SKATE_UART_ESP_h

#include <e_skate_uart.h>

#include <driver/uart.h>

typedef enum e_skate_uart_esp_err_t
{
    E_SKATE_UART_SUCCESS,
} e_skate_uart_esp_err_t;

/**
 * Called when a valid message is received.
 **/
typedef void (*e_skate_uart_msg_cb_t)(e_skate_uart_msg_t msg);


typedef struct
{
    e_skate_uart_msg_cb_t msg_cb;
    uart_port_t uartNum;
} e_skate_uart_esp_t;


/**
 * 
 **/
e_skate_uart_esp_err_t e_skate_uart_esp_init(

);



#endif /* _E_SKATE_UART_ESP_h */
