#ifndef _E_RIDE_BMS_H
#define _E_RIDE_BMS_H

#include <e_ride_config.h>
#include <e_ride_uart.h>
#include <e_ride_err.h>

#include <driver/uart.h>


typedef struct __attribute__((__packed__))
{
    uint16_t capacity;
    uint16_t voltage;
    int16_t  current;
    uint8_t  temperature1;
    uint8_t  temperature2;
} e_ride_bms_status_t;


typedef struct __attribute__((__packed__))
{
    uint8_t  serialNumber[14];
    uint16_t firmwareVersion;
    uint16_t manufactureDate;
    uint16_t factoryCapacity_mAh;
    uint16_t actualCapacity_mAh;
    uint16_t remainingCapacity_prc; // Units in percentage.
    uint16_t remainingCapacity_mAh; // Units in mAh.
    uint16_t chargeFullCycles;
    uint16_t chargeCount;
    uint16_t packHeath_prc;
    uint16_t cellVoltage_mV[10];
    uint8_t  isCharging;
    uint8_t  isOverVoltage;
    uint8_t  isOverHeat;
} e_ride_bms_deep_status_t;


typedef void (*e_ride_bms_cb_t)(e_ride_bms_status_t* status, uint8_t nStatus);


typedef struct
{
    uart_port_t         bmsUartPort;
    uint8_t             numBat;
    uint8_t*            batRxPins;
    uint8_t*            batTxPins;
    uint8_t             batTxPin;
    uint32_t            bmsUpdateMs;
    uart_config_t       uartConfig;
} e_ride_bms_config_t;


e_ride_err_t e_ride_bms_init(

    e_ride_bms_config_t* bmsConfig

);

e_ride_err_t e_ride_bms_set_rx(

    e_ride_bms_config_t* eSkateUart,
    uint8_t uartRxI

);

/**
 * Same as `e_ride_bms_init()`, but uses
 * values from `e_ride_config.h`.
 **/
e_ride_err_t e_ride_bms_init_from_config_h(

    e_ride_bms_config_t* outConfig

);


/**
 * Asks the active BMS for all the useful
 * registers, and updates `outStatus`.
 * This function waits for all the
 * responses.
 * Returns `E_RIDE_BMS_SUCCESS` on
 * success, anything else on error.
 **/
e_ride_err_t e_ride_bms_get_status(

    e_ride_bms_config_t *bmsConfig,
    e_ride_bms_status_t *outStatus

);


/**
 * Reads the pack's deep status.
 * Takes a while, and the result is stored
 * in `outDeepStatus`.
 **/
e_ride_err_t e_ride_bms_get_deep_status(

    e_ride_bms_config_t      *bmsConfig,
    e_ride_bms_deep_status_t *outStatus

);


/**
 * Sends to the current active uart
 * the messages to read all the registers
 * with battery info.
 **/
e_ride_err_t e_ride_bms_get_all_status(

    e_ride_bms_config_t* bmsConfig

);


#endif /* _E_RIDE_BMS_H */
