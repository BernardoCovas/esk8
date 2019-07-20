#ifndef _ESK8_BTN_H
#define _ESK8_BTN_H

#include <esk8_btn.h>
#include <esk8_err.h>

#include <driver/gpio.h>
#include <driver/timer.h>


typedef enum
{
    ESK8_BTN_PRESS,
    ESK8_BTN_LONGPRESS
} esk8_btn_press_t;


typedef struct
{
    gpio_num_t      btn_gpio;
    uint32_t        btn_longPress_ms;
    uint32_t        btn_postLock_ms;
    uint32_t        btn_debounce_ms;
    timer_group_t   btn_tmrGrp;
    timer_idx_t     btn_tmrIdx;

    void*       _que_hndlr;
    void*       _tsk_hndlr;
    void*       _smp_hndlr;
    uint32_t    _hndlr_ms;
    uint32_t    _hndlr_dir;
} esk8_btn_cnfg_t;


/**
 *
 */
esk8_err_t esk8_btn_init(

    esk8_btn_cnfg_t* btnCnfg

);


/**
 *
 */
esk8_err_t esk8_btn_await_press(

    esk8_btn_cnfg_t*    btnCnfg,
    esk8_btn_press_t*   pressType,
    uint32_t            timeOut_ms

);


/**
 *
 */
esk8_err_t esk8_btn_clear(

    esk8_btn_cnfg_t* btnCnfg

);


#endif /* _ESK8_BTN_H */
