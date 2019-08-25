#ifndef _ESK8_PS2_H
#define _ESK8_PS2_H

#include <esk8_err.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <driver/timer.h>

#include <stdint.h>


typedef enum
{
    ESK8_PS2_CMD_RESET                    = 0xFF,
    ESK8_PS2_CMD_RESEND                   = 0xFE,
    ESK8_PS2_CMD_DATA_DISABLE             = 0xF5,
    ESK8_PS2_CMD_DATA_ENABLE              = 0xF4,
    ESK8_PS2_CMD_GET_DEV_ID               = 0xF2,
    ESK8_PS2_CMD_GET_STATUS               = 0xE9,
    ESK8_PS2_CMD_SET_DEFAULT              = 0xF6,
    ESK8_PS2_CMD_SET_SMPL_RATE            = 0xF3,
    ESK8_PS2_CMD_SET_MODE_STREAM          = 0xEA,
    ESK8_PS2_CMD_SET_MODE_REMOTE          = 0xF0,
    ESK8_PS2_CMD_SET_MODE_WRAP            = 0xEE,
    ESK8_PS2_CMD_SET_MODE_NOWRAP          = 0xEC,
    ESK8_PS2_CMD_SET_SCALE_2_1            = 0xE7,
    ESK8_PS2_CMD_SET_SCALE_1_1            = 0xE6,
    ESK8_PS2_CMD_SET_RESOLUTION           = 0xE8,
    ESK8_PS2_CMD_MODE_REMOTE_DATA_READ    = 0xEB,

    ESK8_PS2_RES_ACK                      = 0xFA
} esk8_ps2_cmd_t;


typedef struct
{
    uint8_t         clockPin;
    uint8_t         dataPin;
} esk8_ps2_gpio_config_t;


typedef struct
{
    timer_idx_t     timerIdx;
    timer_group_t   timerGroup;
} esk8_ps2_timer_config_t;


typedef enum
{
    PS2_DIRCN_RECV,
    PS2_DIRCN_SEND,
} esk8_ps2_direction_t;


typedef struct
{
    esk8_ps2_timer_config_t timerConfig;
    esk8_ps2_gpio_config_t  gpioConfig;
    esk8_ps2_direction_t    dataDrctn;
} esk8_ps2_config_t;


typedef struct
{
    uint8_t                     newStart;
    uint8_t                     newByte;
    uint8_t                     newParity;
    uint8_t                     newStop;
    uint8_t                     frameIndex:5; /* A frame is 11 bits */
} esk8_ps2_pkt_t;


typedef struct
{
    TaskHandle_t                txTaskToNotift;
    TaskHandle_t                rxTaskHandle;
    QueueHandle_t               rxByteQueueHandle;

    esk8_ps2_pkt_t           rxPkt;
    esk8_ps2_pkt_t           txPkt;

    esk8_ps2_config_t        ps2Config;
} esk8_ps2_handle_t;


typedef struct
{
    int x;
    int y;
    bool lftBtn;
} esk8_ps2_mvmnt_t;


/**
 * Initializes the isr for the ps2
 * protocol. This function allways
 * returns `ESK8_SUCCESS`.
 * If something fails internally,
 * it panics.
 **/
esk8_err_t esk8_ps2_init(

    esk8_ps2_handle_t* ps2Handle,
    esk8_ps2_config_t* ps2Config

);


/**
 * Same as `esk8_ps2_init` but with
 * values from esk8_config.h.
 **/
esk8_err_t esk8_ps2_init_from_config_h(

    esk8_ps2_handle_t* ps2Handle

);


/**
 * Clears everything, including the
 * isr handler and the timer.
 * This does NOT uninstall the isr
 * service, unless `withIsr` is true.
 **/
esk8_err_t esk8_ps2_deinit(

    esk8_ps2_handle_t* ps2Handle,
    bool                  withIsr

);


/**
 * Waits for one byte of data to be
 * received.
 * Ps2 must be initialized.
 **/
esk8_err_t esk8_ps2_await_byte(

    esk8_ps2_handle_t*   ps2Handle,
    uint8_t*                outByte,
    int                     timeOut_ms

);


/**
 * Waits for one movement packet
 * to be received.
 * Ps2 must be initialized.
 **/
esk8_err_t esk8_ps2_await_mvmnt(

    esk8_ps2_handle_t* ps2Handle,
    esk8_ps2_mvmnt_t * outMvmnt

);


/**
 * Sends one byte of data to the
 * device.
 **/
esk8_err_t esk8_ps2_send_byte(

    esk8_ps2_handle_t* ps2Handle,
    uint8_t               byte

);


/**
 * Sends the command to the
 * device. The host has priority
 * over the device, so this
 * function returns when we receive
 * a valid ack response.
 **/
esk8_err_t esk8_ps2_send_cmd(

    esk8_ps2_handle_t* ps2Handle,
    esk8_ps2_cmd_t     cmd,
    int                  timeOut_ms

);


#endif /* _ESK8_PS2_H */
