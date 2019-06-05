#ifndef _E_RIDE_PS2_H
#define _E_RIDE_PS2_H

#include <e_ride_err.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <driver/timer.h>

#include <stdint.h>


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
 * device. The host has priority
 * over the device, so this
 * function returns when
 * byte is ack.
 **/
e_ride_err_t e_ride_ps2_send_byte(

    e_ride_ps2_handle_t* ps2Handle,
    uint8_t               byte

);


#endif /* _E_RIDE_PS2_H */
