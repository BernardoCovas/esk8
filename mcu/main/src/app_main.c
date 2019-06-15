#include <e_ride_config.h>
#include <e_ride_err.h>
#include <e_ride_ble.h>
#include <e_ride_bms.h>
#include <e_ride_ps2.h>
#include <e_ride_pwm.h>

#include <app_ble_srvc.h>

#include <stdint.h>
#include <stdio.h>

void ps2_task()
{
    e_ride_ps2_handle_t         ps2Handle;
    e_ride_ps2_init_from_config_h(&ps2Handle);

    printf("-----------------------------\n");
    printf("Listening to PS2 with PWM...\n");
    printf("-----------------------------\n");

    e_ride_pwm_config_t pwm_Config;
    e_ride_pwm_sgnl_init(&pwm_Config);

    uint8_t cmdList[] = {
        0xF4
    };

    for (int i=0; i<sizeof(cmdList); i++)
    {
        uint8_t byte = cmdList[i];
        e_ride_ps2_send_byte(&ps2Handle, byte);
        e_ride_ps2_await_byte(&ps2Handle, &byte, 100);
    }

    e_ride_ps2_mvmnt_t trckMvmnt;

    int speed = 0;
    while(1)
    {
        if (e_ride_ps2_await_mvmnt(&ps2Handle, &trckMvmnt) != E_RIDE_SUCCESS)
        {
            printf("[ PS2 ]TIMEOUT%40s\n", "");
            continue;
        }

        if (trckMvmnt.lftBtn)
            speed = 0;

        speed += trckMvmnt.x;
        speed = speed>255?255:speed;
        speed = speed<  0?  0:speed;

        printf("[ PS2 ] Speed: %03d, X: %d %40s\n", speed, trckMvmnt.x, "");
        e_ride_pwm_sgnl_set(&pwm_Config, (uint8_t) speed);
        app_srvc_status_update_speed((uint8_t)speed);
    }

}

void app_main()
{
    e_ride_bms_config_t         bmsConfig;
    e_ride_bms_status_t         bmsStatus;
    e_ride_bms_deep_status_t    bmsDeepStatus;

    e_ride_ble_config_t         bleCnfg;
    e_ride_ble_app_t*           appSrvcList_p[] = APP_ALL_SRVC_LIST_P();

    e_ride_bms_init_from_config_h(&bmsConfig);
    e_ride_ble_init(&bleCnfg);
    e_ride_ble_register_apps((uint16_t) sizeof(appSrvcList_p) / sizeof(appSrvcList_p[0]), appSrvcList_p);


    printf("-----------------------------\n");
    printf("Listening to BMS...\n");
    printf("-----------------------------\n");


    for (int i=0; i<bmsConfig.numBat; i++)
    {
        e_ride_bms_set_rx(&bmsConfig, i);
        e_ride_err_t errCodeS = e_ride_bms_get_status(&bmsConfig, &bmsStatus);
        e_ride_err_t errCodeDS = e_ride_bms_get_deep_status(&bmsConfig, &bmsDeepStatus);

        if (errCodeS == E_RIDE_SUCCESS)
        {
            printf("(BMS Status) Got status: %.2fV, %d%%, %.2fA, %dºC, %dºC\n",
                (double)    bmsStatus.voltage / 100,
                (int)       bmsStatus.capacity,
                (double)    bmsStatus.current / 100,
                (int)       bmsStatus.temperature1 - 20,
                (int)       bmsStatus.temperature2 - 20);
        } else
        {
            printf("(BMS Status) Got error %d: %s.\n",
                errCodeS,
                e_ride_err_to_str(errCodeS));
        }

        if (errCodeDS == E_RIDE_SUCCESS)
        {
            printf( "(BMS Deep Status) Got Deep status:\n"
                    "   Serial Number: ");
            for (int i=0; i<14; i++)
            {
                printf(i<13?"%d-":"%d\n", bmsDeepStatus.serialNumber[i]);
            }

            printf( "   Firmware Version: %d\n"
                    "   Factory Capacity: %d\n"
                    "   Actual capacity: %d\n"
                    "   Charge Full Cycles: %d\n"
                    "   Charge Count: %d\n"
                    "   Manufacture Date: %d\n"
                    "   Cell 0 Voltage, mV: %d\n"
                    "   Cell 1 Voltage, mV: %d\n"
                    "   Cell 2 Voltage, mV: %d\n"
                    "   Cell 3 Voltage, mV: %d\n"
                    "   Cell 4 Voltage, mV: %d\n"
                    "   Cell 5 Voltage, mV: %d\n"
                    "   Cell 6 Voltage, mV: %d\n"
                    "   Cell 7 Voltage, mV: %d\n"
                    "   Cell 8 Voltage, mV: %d\n"
                    "   Cell 9 Voltage, mV: %d\n",
                    bmsDeepStatus.firmwareVersion,
                    bmsDeepStatus.factoryCapacity_mAh,
                    bmsDeepStatus.actualCapacity_mAh,
                    bmsDeepStatus.chargeFullCycles,
                    bmsDeepStatus.chargeCount,
                    bmsDeepStatus.manufactureDate,
                    bmsDeepStatus.cellVoltage_mV[0],
                    bmsDeepStatus.cellVoltage_mV[1],
                    bmsDeepStatus.cellVoltage_mV[2],
                    bmsDeepStatus.cellVoltage_mV[3],
                    bmsDeepStatus.cellVoltage_mV[4],
                    bmsDeepStatus.cellVoltage_mV[5],
                    bmsDeepStatus.cellVoltage_mV[6],
                    bmsDeepStatus.cellVoltage_mV[7],
                    bmsDeepStatus.cellVoltage_mV[8],
                    bmsDeepStatus.cellVoltage_mV[9]
                    );
        } else
        {
            printf("(BMS Deep Status) Got error %d: %s.\n",
                errCodeDS,
                e_ride_err_to_str(errCodeDS));
        }
    }

    /**
     * The bluetooth stack seems
     * to either take priority over our
     * ISR's, or absolutely devour the
     * available CPU time. We can't have
     * ps2 and BLE on the same core.
     */
    xTaskCreatePinnedToCore(
        ps2_task,
        "ps2_task",
        2048, NULL, 1, NULL, 1);
}
