#ifndef _ESK8_BLEC_APP_BOARD_H
#define _ESK8_BLEC_APP_BOARD_H

#include <esk8_err.h>
#include <esk8_blec_apps.h>

#include <stdint.h>


extern esk8_blec_app_t
esk8_blec_app_board;

esk8_err_t
esk8_blec_app_ctrl_incr(
    uint8_t incr
);

esk8_err_t
esk8_blec_app_ctrl_pwr(
    int prw
);

#endif /* _ESK8_BLEC_APP_BOARD_H */
