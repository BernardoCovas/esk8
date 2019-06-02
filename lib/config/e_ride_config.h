#ifndef _E_RIDE_CONTROLLER_CONFIG_H
#define _E_RIDE_CONTROLLER_CONFIG_H


/* ========================================== BMS Configrations ========================================== */
#define E_RIDE_UART_BMS_UPDATE_MS                   50
#define E_RIDE_UART_BMS_MSG_UPDATE_RETRIES          3
#define E_RIDE_UART_BMS_NUM                         UART_NUM_1
#define E_RIDE_UART_BMS_TX                          GPIO_NUM_12
#define E_RIDE_UART_BMS_TX_PINS                     { GPIO_NUM_25, GPIO_NUM_14, GPIO_NUM_4 , GPIO_NUM_5  }
#define E_RIDE_UART_BMS_RX_PINS                     { GPIO_NUM_26, GPIO_NUM_12, GPIO_NUM_16, GPIO_NUM_18 }
#define E_RIDE_UART_BMS_BUFF_SIZE                   1000


/* ========================================== PS2 Trackpad Configrations ================================= */
#define E_RIDE_PS2_DATA_PIN                         GPIO_NUM_14
#define E_RIDE_PS2_CLOCK_PIN                        GPIO_NUM_27
#define E_RIDE_PS2_MOVEMENT_TIMEOUT_MS              5000            /* Ms between two movement packets to be considered a lost pkt sequence.    */
#define E_RIDE_PS2_PACKET_TIMEOUT_MS                1               /* Ms between clock cycles to be considered a lost packet.                  */
#define E_RIDE_PS2_BIT_QUEUE_LENGTH                 1024            /* Number of bits a queue can handle. A 'bit' is actually a bool.           */
#define E_RIDE_PS2_BYTE_QUEUE_LENGTH                1024            /* Number of bytes a queue can handle. These are uint8_t's.                 */
#define E_RIDE_PS2_RX_TASK_PRIORITY                 1
#define E_RIDE_PS2_TX_TASK_PRIORITY                 1
#define E_RIDE_PS2_TIMER_GROUP                      TIMER_GROUP_0
#define E_RIDE_PS2_TIMER_IDX                        TIMER_0


/* ========================================== PWM Controll Configurations ================================ */
#define E_RIDE_PWM_GPIO                             GPIO_NUM_2      /* GPIO to where the PWM signal is routed.                                                                              */
#define E_RIDE_PWM_CHANNEL                          LEDC_CHANNEL_1  /* GPIO internal channel select. (ESP32: 0 to 7)                                                                        */
#define E_RIDE_PWM_TIMER_NUM                        1               /* Index of the ledc timer to use. (ESP32: 0 to 3)                                                                      */
#define E_RIDE_PWM_FREQ_HZ                          50              /* Frequency used when generating the PWM control signal. Not very restricted, but this plays with the precision bits.  */
#define E_RIDE_PWM_NUM_BITS                         8               /* Number of precision bits. This is actually not very restricted, but the ESP hardware timer must allow it.            */


/* ========================================== BLE Configurations ========================================= */
#define E_RIDE_BLE_DEV_NAME                         "E_Ride"        /* Advertized device name                                                                                               */


#endif  /* _E_RIDE_CONTROLLER_CONFIG_H */
