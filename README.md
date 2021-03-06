# Esk8

This repo contains an app for the ESP32 SoC, paired with a
companion Android app. It`s main objective is to serve as
a complete electric skateboard manager.  
It manages the batteries, the speed controller, and has a Bluetooth low Energy server that allows you to both get information on the status of the device, and control it.  

There are two apps in this repo. One is meant to be on the skateboard, and the other on the remote controller.  

Some features include:  

- Complete support for any PS2 device, for example trackpads
- Complete implementation of the UART Battery Management System protocol for the Ninebot ESx Scooters
- Fully featured BLE Server with status and controll
- Non-volatile storage to remember previously connected devices and changed settings

The BMS protocol allows you to use some new/old/used/half-broken scooter battery(s) in your skateboard, and receive all it`s status through BLE. These can be bought for cheap on Ebay.

## Build

This is a raw ESP-IDF project, meaning that, as long as you  
follow the install instructions in their website, this should  
be as easy as:  

```
$ idf.py flash
```

This is a CMake project, so make sure to use the latest IDF and toolchain with CMake support.

## BLE

For the Bluetooth low energy, there are two services.
One for the status and one for the control. (WIP)  

The status service has notification support, and it works like this:  
There is a backgrould task running on the SoC that continuously updates the status
of the battery. After each battery update, it sends a notification to any connected device.
This notification contains a `uint32_t` and a `uint16_t`.
The `uint32_t` represents the error code from reading the battery info, and the `uint16_t`
represents the battery index that was updated.

An example value would be the following array:
```C
{0x07, 0x00, 0x00, 0x00, 0x01, 0x00}
```

This would mean error code 0x00000007 (`E_RIDE_BMS_ERR_NO_RESPONSE`) at BMS index 0x0001.  
Indexing starts at 0, and as such this would mean my second BMS is fried or disconnected.  
In my board I have 4 of these BMSs, and so this index ranges from 0 to 3.
  
After the notification, you can read the attribute value, if it there are no errors.
This will give you an array of

```C
size_of(e_ride_bms_deep_status_t) * number_of_bms
```
or 
```C
size_of(e_ride_bms_status_t) * number_of_bms
```
bytes, depending on the characteristic you are reading.
These bytes are the packed structs from the SoC:

For the shallow info:
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
#define ESK8_BTN_DEBOUNCE_ms                      5
#define ESK8_BTN_TIMEOUT_ms                       10000 /* Time to wait for a btn press before returning timeout */


/* ========================================== RMT Configurations ========================================= */
#define ESK8_RMT_PS2_CMD_TIMEOUT_ms               200


#endif  /* _ESK8_CONTROLLER_CONFIG_H */
```

Most important settings are the BMS, PS2 and button config. More specifically:

```C
#define ESK8_UART_BMS_TX_PINS
#define ESK8_UART_BMS_RX_PINS
#define ESK8_UART_BMS_CONF_NUM 

#define ESK8_BTN_GPIO

#define ESK8_PWM_GPIO
#define ESK8_PWM_FREQ_HZ

```

You can try different configurations with the PWM control, but keep in mind the 
SoC hardware must support it. Not all frequency / precision are supported by the hardware,
but this tends to be a problem on the 10´s of KHz.


## BMS

Every BMS UART port has a TX and RX line, and so one single BMS needs two GPIOs.
However, if you have more than one BMS, you can choose to join the TX lines into one
GPIO. The software is expecting this, and so it will work fine.
There seems to be an issue with pullup though, but this will be fixed soon.

For my BMS settup, this works:  

```C
#define ESK8_UART_BMS_TX_PINS   { GPIO_NUM_14, GPIO_NUM_14, GPIO_NUM_14, GPIO_NUM_14 }
#define ESK8_UART_BMS_RX_PINS   { GPIO_NUM_27, GPIO_NUM_26, GPIO_NUM_25, GPIO_NUM_33 }
```

Notice the repeated GPIO in the TX config.
This is entirely up to you, but keep in mind not all pins work as outputs, and so not all pins can be used for the TX line.

# Error Codes

This project uses a common `esk8_err_t` enum.  
This means that if an error occurs in any of the lower level functions,  
it will be propagated and that is exactly what you will receive at the end.  
For example, in the BLE if you receive error code `E_RIDE_UART_MSG_ERR_INVALID_CHKSUM`,  
this actually might have more meaning than just a 'failed' kind of error. In fact,  
receiving this specific error would mean that the message checksum does not match,  
and so the response from the BMS was currupted.  

The enum is the following:  

```C
typedef enum esk8_err_t
{
    ESK8_OK,
    ESK8_ERR_INVALID_PARAM,
    ESK8_ERR_OOM,
    ESK8_BMS_ERR_INVALID_DATA,
    ESK8_BMS_ERR_INVALID_LEN,
    ESK8_BMS_ERR_WRONG_ADDRESS,           /* Address was not what was expected */
    ESK8_BMS_ERR_WRONG_RESPONSE,          /* Response was not what was expected */
    ESK8_BMS_ERR_NO_RESPONSE,
    ESK8_UART_MSG_ERR_NO_HEADER,
    ESK8_UART_MSG_ERR_INVALID_PLDLEN,     /* Payload was either bigger or smaller than advertized in header */
    ESK8_UART_MSG_ERR_INVALID_CHKSUM,
    ESK8_UART_MSG_ERR_INVALID_BUFFER,
    ESK8_PS2_ERR_VALUE_READY,
    ESK8_PS2_ERR_INVALID_STATE,
    ESK8_PS2_ERR_TIMEOUT,
    ESK8_PS2_ERR_RESP_TIMEOUT,            /* There was no response from device. */
    ESK8_PS2_ERR_MVMT_TIMEOUT,            /* Timeout waiting for mvmt packet */
    ESK8_PS2_ERR_BAD_MVMT,                /* The mvmt signal bit was 0. It is always one. This should cause a ps2 resync / reset */
    ESK8_PS2_ERR_BAD_PCK_STRT,            /* Start bit was not 0 */
    ESK8_PS2_ERR_BAD_PCK_STOP,            /* Stop bit was not 1 */
    ESK8_PS2_ERR_BAD_PCK_PRTY,            /* Parity bit is wrong */
    ESK8_PS2_ERR_BAD_PCK_ACKN,            /* Missing ack bit */
    ESK8_PS2_ERR_NO_ACK,                  /* Device has not ack the received command */
    ESK8_PS2_ERR_RESEND,                  /* Device received the command but did not understand it */
    ESK8_PS2_ERR_ERROR,                   /* Device received the command but there was an error */
    ESK8_BLE_INIT_NOINIT,                 /* Not initialized */
    ESK8_BLE_INIT_REINIT,                 /* Already had been initialized */
    ESK8_BLE_INIT_FAILED,
    ESK8_BLE_INIT_MAXREG,                 /* Maximum number of registed apps reached */
    ESK8_BLE_NOTF_TIMEOUT,
    ESK8_BLE_APP_NOREG,                   /* App not registered */
    ESK8_NVS_NOT_INIT,                    /* NVS was not initialized */
    ESK8_NVS_NOT_AVAILABLE,               /* Could not be initialized in any way. Probably broken */
    ESK8_NVS_FULL,                        /* No space left on NVS */
    ESK8_NVS_NO_SETTINGS,                 /* No settigs stored */
    ESK8_NVS_NO_IDX,                      /* NVS index out of range */
    ESK8_NVS_NO_VAL,                      /* Value not stored */
    ESK8_NVS_WRONG_SIZE,                  /* Stored value returned an unexpected size. Might also mean non existent value */
    ESK8_NVS_ERR_WRITE,                   /* An NVS write opperatin failed. */
    ESK8_BTN_ERR_NOINIT,
    ESK8_BTN_ERR_TIMEOUT,
    ESK8_AUTH_ERR_HASH,                   /* An error occured in the tls module. */
    ESK8_AUTH_ERR_AUTH,                   /* Wrong key supplied.                 */
    ESK8_AUTH_ERR_CONN,                   /* Wrong connection authenticating.    */
    ESK8_ERR_OBRD_NOINIT,
    ESK8_ERR_REMT_NOINIT,
    ESK8_ERR_REMT_REINIT,
    ESK8_ERR_REMT_BAD_STATE,
}
esk8_err_t;
```

If you make changes to the code, make sure to propagate the error if your code uses some lower functions.
For this, to prevent multiple
```C
if (err_code)
    return err_code;
```

you can use the provided definition:
```C
#define ESK8_ERRCHECK_THROW(X) do { esk8_err_t __err_c = (X); if (__err_c) return __err_c; } while(0)

```
This can be use as follows:
```C
ESK8_ERRCHECK_THROW(esk8_ble_conn_clear());
```
This will return if there is an error, propagating it.
