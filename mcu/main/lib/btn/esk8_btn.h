#ifndef _ESK8_BTN_H
#define _ESK8_BTN_H

#include <esk8_err.h>
#include <esk8_btn.h>


typedef enum
{
    ESK8_BTN_PRESS,
    ESK8_BTN_LONGPRESS
}
esk8_btn_press_t;

typedef struct
{
    int         btn_gpio;
    uint32_t    timeout_ms;
    uint32_t    debounce_ms;
    uint32_t    longpress_ms;
}
esk8_btn_cnfg_t;

typedef void*
esk8_btn_hndl_t;

esk8_err_t
esk8_btn_init(
    esk8_btn_hndl_t* out_hndl,
    esk8_btn_cnfg_t* btn_cnfg
);

esk8_err_t
esk8_btn_await_press(
    esk8_btn_hndl_t   btn_hndl,
    esk8_btn_press_t* out_press,
    uint32_t          timeout_ms
);

esk8_err_t
esk8_btn_deinit(
    esk8_btn_hndl_t hndl
);


#endif /* _ESK8_BTN_H */
