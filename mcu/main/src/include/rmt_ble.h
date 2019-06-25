#ifndef _RMT_BLE_H
#define _RMT_BLE_H

#include <e_ride_err.h>
#include <e_ride_ps2.h>


e_ride_err_t app_ctrl_init();
e_ride_err_t app_ctrl_update_speed(e_ride_ps2_mvmnt_t mvmtPacket);
e_ride_err_t app_ctrl_update_pwr(bool pwr);


#endif
