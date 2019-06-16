# Esk8

## Welcome  
As of now this is a somewhat useful app for the ESP32 SoC, paired with a  
companion Android app.  

The main objective is to serve as a complete electric transport manager.  
It manages all the BMS, controls the speed and serves as a BLE server.  
  
It supports the PS2 protocol, allowing you to use any ps2 trackpad to control  
the device.  

It supports the entire UART protocol for the Ninebot es1/2/3/4 scooters, allowing you  
to use some new/old/used/half-broken scooter battery(s) in your skateboard, and receive  
all it's characteristics over BLE, using the included android (only) app.  

These include a shallow info:  

```C
typedef struct __attribute__((__packed__))
{
    uint16_t capacity;
    uint16_t voltage;
    int16_t  current;
    uint8_t  temperature1;
    uint8_t  temperature2;
} e_ride_bms_status_t;
```

and a more complete:

```C
typedef struct __attribute__((__packed__))
{
    uint8_t  serialNumber[14];
    uint16_t firmwareVersion;
    uint16_t manufactureDate;
    uint16_t factoryCapacity_mAh;
    uint16_t actualCapacity_mAh;
    uint16_t remainingCapacity_prc; // Units in percentage.
    uint16_t remainingCapacity_mAh; // Units in mAh.
    uint16_t chargeFullCycles;
    uint16_t chargeCount;
    uint16_t packHeath_prc;
    uint16_t cellVoltage_mV[10];
    uint8_t  isCharging;
    uint8_t  isOverVoltage;
    uint8_t  isOverHeat;
} e_ride_bms_deep_status_t;
```

## Build

This is a raw ESP-IDF project, meaning that, as long as you  
follow the install instructions in their website, this should  
be as easy as:  

```
$ idf.py flash
```

Keep in mind I have no idea how to use the Make build tool, and  
don't really care to learn it so this project only contains CMake files.  


## BLE
This is somewhat simple as of now.
What you have is one service (soon to be 2), and this with two characteristics.
One of which will give you the shallower info,
and the other the deeper.

You will receive a BLE notification when the BMS values have been updated,  
and the bytes you receive in this notif. are the error code and the index of  
the BMS updated.

The error code are the first 4 bytes, and the bms index the last two.
An example value would be:

```C
{0x07, 0x00, 0x00, 0x00, 0x01, 0x00}
```
This would mean error code 0x0007 (`E_RIDE_BMS_ERR_NO_RESPONSE`) at BMS index 0x0001.  
Since we are all sane here, indexing starts at 0, and as such this would mean  
my second BMS is fried or disconnected.  
In my board I have 4 of these BMSs, and so this index ranges from 0 to 3.
  
After the notification, you can read the attribute value, if it there are no errors.
This will give you an array of

```C
size(e_ride_bms_deep_status_t) * number_of_bms
```
or 
```C
size(e_ride_bms_status_t) * number_of_bms
```
bytes, of course each representing the matching value.
  
With 4 MBS´s, this array is `220` bytes long for the deep status, and `32` for the shallow.

## PWM

This uses
```C
#include <driver/ledc.h>
```

as a hardware PWM driver. All function wrappers are there, so hopefully easy to use.  
It's configured for a normal esc, as a 500 Hz pwm signal, but can easily be changed  
in "e_ride_config.h".

## Config

This alows easy configuring of some basic params:

```C
#ifndef _E_RIDE_CONTROLLER_CONFIG_H
#define _E_RIDE_CONTROLLER_CONFIG_H


/* ========================================== BMS Configrations ========================================== */
#define E_RIDE_UART_BMS_UPDATE_MS                   50
#define E_RIDE_UART_BMS_MSG_UPDATE_RETRIES          3
#define E_RIDE_UART_BMS_NUM                         UART_NUM_1
#define E_RIDE_UART_BMS_TX_PINS                     { GPIO_NUM_25, GPIO_NUM_25, GPIO_NUM_25, GPIO_NUM_25 }
#define E_RIDE_UART_BMS_RX_PINS                     { GPIO_NUM_26, GPIO_NUM_12, GPIO_NUM_16, GPIO_NUM_18 }
#define E_RIDE_UART_BMS_CONF_NUM                    4               /* Number of BMS configured                                                 */
#define E_RIDE_UART_BMS_BUFF_SIZE                   1000


/* ========================================== PS2 Trackpad Configrations ================================= */
#define E_RIDE_PS2_DATA_PIN                         GPIO_NUM_27
#define E_RIDE_PS2_CLOCK_PIN                        GPIO_NUM_14
#define E_RIDE_PS2_MOVEMENT_BYTE_TIMEOUT_MS         20              /* Ms between two movement packets to be considered a lost pkt sequence.    */
#define E_RIDE_PS2_MOVEMENT_TIMEOUT_MS              5000            /* Timeout, in mS, between movement pkt seequences.                         */
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
#define E_RIDE_PWM_FREQ_HZ                          500             /* Frequency used when generating the PWM control signal. Not very restricted, but this plays with the precision bits.  */
#define E_RIDE_PWM_NUM_BITS                         8               /* Number of precision bits. This is actually not very restricted, but the ESP hardware timer must allow it.            */


/* ========================================== BLE Configurations ========================================= */
#define E_RIDE_BLE_DEV_NAME                         "E_Ride"        /* Advertized device name                                                                                               */


#endif  /* _E_RIDE_CONTROLLER_CONFIG_H */

```

Most important maybe are the BMS and PS2 Config, more specifically:

```C
#define E_RIDE_UART_BMS_TX_PINS
#define E_RIDE_UART_BMS_RX_PINS
#define E_RIDE_UART_BMS_CONF_NUM

#define E_RIDE_PWM_FREQ_HZ
#define E_RIDE_PWM_GPIO

```

## BMS

So here is the deal.
If you have 1 BMS, you need 2 GPIOS.  
For more than 2 BMS, you can actually use one TX GPIO for multiple  
BMS's, and so you can repeat this same value for the config value.
For example, for me I can have:  

```C
#define E_RIDE_UART_BMS_TX_PINS                     { GPIO_NUM_25, GPIO_NUM_25, GPIO_NUM_25, GPIO_NUM_25 }
#define E_RIDE_UART_BMS_RX_PINS                     { GPIO_NUM_26, GPIO_NUM_12, GPIO_NUM_16, GPIO_NUM_18 }
```
And this seems to work just fine.

# Error Codes

This project uses a common e_ride_err_t enum.  
This means that if an error occurs in any of the lower level functions,  
it will be propagated and that is exactly what you will receive at the end.  
For example, in the BLE if you receive error code `E_RIDE_UART_MSG_ERR_INVALID_CHKSUM`,  
this actually might have more meaning than just a 'failed' kind of error. In fact,  
receiving this specific error would mean that the message checksum does not match,  
and so the response from the BMS was currupted.  
