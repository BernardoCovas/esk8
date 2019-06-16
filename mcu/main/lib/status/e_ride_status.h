#ifndef _E_RIDE_STATUS_H
#define _E_RIDE_STATUS_H

#include <e_ride_err.h>
#include <e_ride_config.h>
#include <e_ride_bms.h>


typedef struct
{
    e_ride_bms_status_t bmsStatus[E_RIDE_UART_BMS_CONF_NUM];
} e_ride_status_t;


e_ride_err_t e_ride_status_init(

    e_ride_status_t* status

);


#endif /* _E_RIDE_STATUS_H */
