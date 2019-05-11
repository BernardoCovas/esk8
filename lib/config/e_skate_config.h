#ifndef _E_SKATE_CONTROLLER_CONFIG_H
#define _E_SKATE_CONTROLLER_CONFIG_H


#define E_SKATE_UART_BMS_UPDATE_MS                  50
#define E_SKATE_UART_BMS_MSG_UPDATE_RETRIES         3
#define E_SKATE_UART_BMS_NUM                        UART_NUM_1
#define E_SKATE_UART_BMS_TX                         GPIO_NUM_12
#define E_SKATE_UART_BMS_TX_PINS                    { GPIO_NUM_25, GPIO_NUM_14, GPIO_NUM_4 , GPIO_NUM_5  }
#define E_SKATE_UART_BMS_RX_PINS                    { GPIO_NUM_26, GPIO_NUM_12, GPIO_NUM_16, GPIO_NUM_18 }
#define E_SKATE_UART_BMS_BUFF_SIZE                  1000

#define E_SKATE_PS2_RX_TASK_PRIORITY                1
#define E_SKATE_PS2_TX_TASK_PRIORITY                1
#define E_SKATE_PS2_BIT_QUEUE_LENGTH                1024 /* 1Kb */
#define E_SKATE_PS2_BYTE_QUEUE_LENGTH               1024 /* 1KB */
#define E_SKATE_PS2_BYTE_QUEUE_TIMEOUT_MS           2000
#define E_SKATE_PS2_PACKET_TIMEOUT_MS               1 /* Ms between clock cycles to be considered a lost packet. Clock should be in the 20/30 Khz range. */
#define E_SKATE_PS2_DATA_PIN                        GPIO_NUM_14
#define E_SKATE_PS2_CLOCK_PIN                       GPIO_NUM_27
#define E_SKATE_PS2_TIMER_GROUP                     TIMER_GROUP_0
#define E_SKATE_PS2_TIMER_IDX                       TIMER_0


#endif  /* _E_SKATE_CONTROLLER_CONFIG_H */
