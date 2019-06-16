#ifndef _E_RIDE_PS2_H
#define _E_RIDE_PS2_H

#include <e_ride_err.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <driver/timer.h>

#include <stdint.h>


typedef enum
{
    E_RIDE_PS2_CMD_RESET                    = 0xFF,
    E_RIDE_PS2_CMD_RESEND                   = 0xFE,
    E_RIDE_PS2_CMD_DATA_DISABLE             = 0xF5,
    E_RIDE_PS2_CMD_DATA_ENABLE              = 0xF4,
    E_RIDE_PS2_CMD_GET_DEV_ID               = 0xF2,
    E_RIDE_PS2_CMD_GET_STATUS               = 0xE9,
    E_RIDE_PS2_CMD_SET_DEFAULT              = 0xF6,
    E_RIDE_PS2_CMD_SET_SMPL_RATE            = 0xF3,
    E_RIDE_PS2_CMD_SET_MODE_STREAM          = 0xEA,
    E_RIDE_PS2_CMD_SET_MODE_REMOTE          = 0xF0,
    E_RIDE_PS2_CMD_SET_MODE_WRAP            = 0xEE,
    E_RIDE_PS2_CMD_SET_MODE_NOWRAP          = 0xEC,
    E_RIDE_PS2_CMD_SET_SCALE_2_1            = 0xE7,
    E_RIDE_PS2_CMD_SET_SCALE_1_1            = 0xE6,
    E_RIDE_PS2_CMD_SET_RESOLUTION           = 0xE8,
    E_RIDE_PS2_CMD_MODE_REMOTE_DATA_READ    = 0xEB,

    E_RIDE_PS2_RES_ACK                      = 0xFA
} e_ride_ps2_cmd_t;


typedef struct
{
    uint8_t         clockPin;
    uint8_t         dataPin;
} e_ride_ps2_gpio_config_t;


typedef struct
{
    timer_idx_t     timerIdx;
    timer_group_t   timerGroup;
} e_ride_ps2_timer_config_t;


typedef enum
{
    PS2_DIRCN_RECV,
    PS2_DIRCN_SEND,
} e_ride_ps2_direction_t;


typedef struct
{
    e_ride_ps2_timer_config_t timerConfig;
    e_ride_ps2_gpio_config_t  gpioConfig;
    e_ride_ps2_direction_t    dataDrctn;
} e_ride_ps2_config_t;


typedef struct
{
    uint8_t                     newStart;
    uint8_t                     newByte;
    uint8_t                     newParity;
    uint8_t                     newStop;
    uint8_t                     frameIndex:5; /* A frame is 11 bits */
} e_ride_ps2_pkt_t;


typedef struct
{
    TaskHandle_t                txTaskToNotift;
    TaskHandle_t                rxTaskHandle;
    QueueHandle_t               rxBitQueueHandle;
    QueueHandle_t               rxByteQueueHandle;

    e_ride_ps2_pkt_t           rxPkt;
    e_ride_ps2_pkt_t           txPkt;

    e_ride_ps2_config_t        ps2Config;
} e_ride_ps2_handle_t;


typedef struct
{
    int x;
    int y;
    bool lftBtn;
} e_ride_ps2_mvmnt_t;


/**
 * Initializes the isr for the ps2
 * protocol. This function allways
 * returns `E_RIDE_SUCCESS`.
 * If something fails internally,
 * it panics.
 **/
e_ride_err_t e_ride_ps2_init(

    e_ride_ps2_handle_t* ps2Handle,
    e_ride_ps2_config_t* ps2Config

);


/**
 * Same as `e_ride_ps2_init` but with
 * values from e_ride_config.h.
 **/
e_ride_err_t e_ride_ps2_init_from_config_h(

    e_ride_ps2_handle_t* ps2Handle

);


/**
 * Clears everything, including the
 * isr handler and the timer.
 * This does NOT uninstall the isr
 * service, unless `withIsr` is true.
 **/
e_ride_err_t e_ride_ps2_deinit(

    e_ride_ps2_handle_t* ps2Handle,
    bool                  withIsr

);


/**
 * Waits for one byte of data to be
 * received.
 * Ps2 must be initialized.
 **/
e_ride_err_t e_ride_ps2_await_byte(

    e_ride_ps2_handle_t*   ps2Handle,
    uint8_t*                outByte,
    int                     timeOut_ms

);


/**
 * Waits for one movement packet
 * to be received.
 * Ps2 must be initialized.
 **/
e_ride_err_t e_ride_ps2_await_mvmnt(

    e_ride_ps2_handle_t* ps2Handle,
    e_ride_ps2_mvmnt_t * outMvmnt

);


/**
 * Sends one byte of data to the
 * device.
 **/
e_ride_err_t e_ride_ps2_send_byte(

    e_ride_ps2_handle_t* ps2Handle,
    uint8_t               byte

);


/**
 * Sends the command to the
 * device. The host has priority
 * over the device, so this
 * function returns when we receive
 * a valid ack response.
 **/
e_ride_err_t e_ride_ps2_send_cmd(

    e_ride_ps2_handle_t* ps2Handle,
    e_ride_ps2_cmd_t     cmd,
    int                  timeOut_ms

);


#endif /* _E_RIDE_PS2_H */
