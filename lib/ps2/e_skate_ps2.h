#ifndef _E_SKATE_PS2_H
#define _E_SKATE_PS2_H

#include <e_skate_err.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <driver/timer.h>

#include <stdint.h>


typedef struct
{
    uint8_t         clockPin;
    uint8_t         dataPin;
} e_skate_ps2_gpio_config_t;


typedef struct
{
    timer_idx_t     timerIdx;
    timer_group_t   timerGroup;
    uint8_t     	timerDivider;
} e_skate_ps2_timer_config_t;

typedef struct
{
    e_skate_ps2_timer_config_t timerConfig;
    e_skate_ps2_gpio_config_t  gpioConfig;
} e_skate_ps2_config_t;


typedef struct
{
    QueueHandle_t               byteQueueHandle;

    /* Packet Frame */
    uint8_t                     newStart;
    uint8_t                     newByte;
    uint8_t                     newParity;
    uint8_t                     newStop;
    
    uint8_t 	                frameIndex:5; /* A frame is 11 bits */
    e_skate_ps2_config_t        ps2Config;
} e_skate_ps2_handle_t;


/**
 * Initializes the isr for the ps2
 * protocol. This function allways
 * returns `E_SKATE_SUCCESS`.
 * If something fails internally,
 * it panics.
 **/
e_skate_err_t e_skate_ps2_init(

    e_skate_ps2_handle_t* ps2Handle,
    e_skate_ps2_config_t* ps2Config

);


/**
 * Same as `e_skate_ps2_init` but with
 * values from e_skate_config.h.
 **/
e_skate_err_t e_skate_ps2_init_from_config_h(

    e_skate_ps2_handle_t* ps2Handle

);


/**
 * Clears everything, including the
 * isr handler.
 * This does NOT uninstall the isr
 * service, unless `withIsr` is true.
 **/
e_skate_err_t e_skate_ps2_deinit(

    e_skate_ps2_handle_t* ps2Handle,
    e_skate_ps2_config_t* ps2Config,
    bool                  withIsr

);


/**
 * Resets the internal value and index
 * of `ps2Handle`.
 * This also resets the hardware
 * timer value, if `withTimer` is 
 * true.
 **/
void e_skate_ps2_reset_handle(

    e_skate_ps2_handle_t* ps2Handle,
    bool withTimer

);


/**
 * Waits for one byte of data to be
 * received.
 * Ps2 must be initialized.
 **/
e_skate_err_t e_skate_ps2_await_byte(

    e_skate_ps2_handle_t* ps2Handle,
    uint8_t*              outByte

);


#endif /* _E_SKATE_PS2_H */
