#ifndef _E_SKATE_PS2_H
#define _E_SKATE_PS2_H

#include <e_skate_err.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include <stdint.h>


typedef struct
{
    uint8_t         clockPin;
    uint8_t         dataPin;
} e_skate_ps2_config_t;

typedef struct
{
    QueueHandle_t   byteQueueHandle;
    uint8_t         newByte;
    uint8_t 	    newByteIndex:4;
} e_skate_ps2_handle_t;


/**
 * 
 **/
e_skate_err_t e_skate_ps2_init(

    e_skate_ps2_handle_t* ps2Handle,
    e_skate_ps2_config_t* ps2Config

);


/**
 * 
 **/
e_skate_err_t e_skate_ps2_deinit(

    e_skate_ps2_handle_t* ps2Handle,
    e_skate_ps2_config_t* ps2Config

);


#endif /* _E_SKATE_PS2_H */
