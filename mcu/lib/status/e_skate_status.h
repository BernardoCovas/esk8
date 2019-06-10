#ifndef _E_RIDE_STATUS_H
#define _E_RIDE_STATUS_H

#include <e_ride_bms.h>


typedef struct
{
    e_ride_bms_status_t *bmsStatus;
    uint8_t nStatus;
} e_ride_status_t;


#endif /* _E_RIDE_STATUS_H */