#ifndef _ESK8_BTN_PRIV_H
#define _ESK8_BTN_PRIV_H

#include <esk8_btn.h>

#include <stdint.h>


typedef enum
{
    ESK8_BTN_STATE_RELEASED,
    ESK8_BTN_STATE_PRESSED,
}
esk8_btn_state_t;

typedef struct
{
    esk8_btn_cnfg_t  btn_cnfg;
    esk8_btn_state_t hndl_state;
    void*            que_hndl;
    void*            tmr_hndl;
    uint64_t         delay_us;
}
esk8_btn_hndl_def_t;




#endif /* _ESK8_BTN_PRIV_H */
