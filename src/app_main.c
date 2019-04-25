#include <e_skate_config.h>
#include <e_skate_err.h>
#include <e_skate_bms.h>

#include <stdint.h>
#include <stdio.h>


void app_main()
{
    e_skate_bms_config_t      bmsConfig;
    e_skate_bms_status_t      bmsStatus;
    e_skate_bms_deep_status_t bmsDeepStatus;

    e_skate_bms_init_from_config_h(
        &bmsConfig
    );

    while(1)
    {
        for (int i=0; i<bmsConfig.numBat; i++)
        {
            e_skate_bms_set_rx(&bmsConfig, i);
            e_skate_err_t errCodeS = e_skate_bms_get_status(
                &bmsConfig,
                &bmsStatus);
            
            e_skate_err_t errCodeDS = e_skate_bms_get_deep_status(
                &bmsConfig,
                &bmsDeepStatus);

            if (errCodeS == E_SKATE_SUCCESS)
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
                    e_skate_err_to_str(errCodeS));
            }

            if (errCodeDS == E_SKATE_SUCCESS)
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
                    e_skate_err_to_str(errCodeDS));
            }
        }

        printf("-----------------------------\n");

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}                