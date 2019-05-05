#ifndef _E_SKATE_CONTROLLER_CONFIG_H
#define _E_SKATE_CONTROLLER_CONFIG_H


#define E_SKATE_UART_BMS_UPDATE_MS                  50
#define E_SKATE_UART_BMS_MSG_UPDATE_RETRIES         3
#define E_SKATE_UART_BMS_NUM                        UART_NUM_1
#define E_SKATE_UART_BMS_TX                         GPIO_NUM_12
#define E_SKATE_UART_BMS_TX_PINS                    { GPIO_NUM_25, GPIO_NUM_14, GPIO_NUM_4 , GPIO_NUM_5  }
#define E_SKATE_UART_BMS_RX_PINS                    { GPIO_NUM_26, GPIO_NUM_12, GPIO_NUM_16, GPIO_NUM_18 }
#define E_SKATE_UART_BMS_BUFF_SIZE                  1000

#define E_SKATE_PS2_BYTE_QUEUE_LENGTH               8
#define E_SKATE_PS2_BYTE_QUEUE_TIMEOUT_MS           10000
#define E_SKATE_PS2_PACKET_TIMEOUT_MS               5  /* Mouses, by default, broadcast 100 packets per second. */
#define E_SKATE_PS2_DATA_PIN                        GPIO_NUM_10
#define E_SKATE_PS2_CLOCK_PIN                       GPIO_NUM_0


#endif  /* _E_SKATE_CONTROLLER_CONFIG_H */
