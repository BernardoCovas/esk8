#ifndef _RMT_BLE_H
#define _RMT_BLE_H

#include <esk8_err.h>
#include <esk8_ps2.h>


esk8_err_t app_ctrl_init();
esk8_err_t app_ctrl_update_speed(esk8_ps2_mvmnt_t mvmtPacket);
esk8_err_t app_ctrl_update_pwr(bool pwr);


#endif
