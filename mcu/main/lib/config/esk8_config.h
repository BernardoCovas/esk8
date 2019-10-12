#ifndef _ESK8_CONTROLLER_CONFIG_H
#define _ESK8_CONTROLLER_CONFIG_H


/* ========================================== BMS Configrations ========================================== */
#define ESK8_UART_BMS_UPDATE_MS                   50
#define ESK8_UART_BMS_MSG_UPDATE_RETRIES          3
#define ESK8_UART_BMS_NUM                         UART_NUM_1
#define ESK8_UART_BMS_TX_PINS                     { GPIO_NUM_14, GPIO_NUM_14, GPIO_NUM_14, GPIO_NUM_14 }
#define ESK8_UART_BMS_RX_PINS                     { GPIO_NUM_27, GPIO_NUM_26, GPIO_NUM_25, GPIO_NUM_33 }
#define ESK8_UART_BMS_CONF_NUM                    4               /* Number of BMS configured                                                 */
#define ESK8_UART_BMS_BUFF_SIZE                   1000


/* ========================================== PS2 Trackpad Configrations ================================= */
#define ESK8_PS2_DATA_PIN                         GPIO_NUM_22
#define ESK8_PS2_CLOCK_PIN                        GPIO_NUM_23
#define ESK8_PS2_MOVEMENT_TIMEOUT_MS              5000            /* Timeout, in mS, between movement pkt sequences.                       */
#define ESK8_PS2_QUEUE_LENGTH                     1024            /* Number of bytes a queue can handle. These are uint8_t's.               */
#define ESK8_PS2_TASK_PRIORITY                    2


/* ========================================== PWM Controll Configurations ================================ */
#define ESK8_PWM_GPIO                             GPIO_NUM_2      /* GPIO to where the PWM signal is routed.                                                                              */
#define ESK8_PWM_CHANNEL                          LEDC_CHANNEL_1  /* GPIO internal channel select. (ESP32: 0 to 7)                                                                        */
#define ESK8_PWM_TIMER_NUM                        1               /* Index of the ledc timer to use. (ESP32: 0 to 3)                                                                      */
#define ESK8_PWM_FREQ_HZ                          500             /* Frequency used when generating the PWM control signal. Not very restricted, but this plays with the precision bits.  */
#define ESK8_PWM_NUM_BITS                         8               /* Number of precision bits. This is actually not very restricted, but the ESP hardware timer must allow it.            */


/* ========================================== BLE Configurations ========================================= */
#define ESK8_BLE_DEV_NAME                         "Esk8"          /* Advertized device name                                                                                               */


/* ========================================== BTN Configurations ========================================= */
#define ESK8_BTN_GPIO                             GPIO_NUM_0
#define ESK8_BTN_LONGPRESS_ms                     3000
#define ESK8_BTN_DEBOUNCE_ms                      30


/* ========================================== RMT Configurations ========================================= */
#define ESK8_RMT_PS2_CMD_TIMEOUT_ms               200


#endif  /* _ESK8_CONTROLLER_CONFIG_H */
