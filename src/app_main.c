#include <e_skate_config.h>
#include <e_skate_err.h>
#include <e_skate_bms.h>

#include <stdint.h>
#include <stdio.h>


void app_main()
{
    e_skate_bms_config_t    bmsConfig;
    e_skate_bms_status_t    bmsStatus;

    e_skate_bms_init_from_config_h(
        &bmsConfig
    );

    while(1)
    {
        printf("-----------------------------\n");
        for (int i=0; i<bmsConfig.numBat; i++)
        {
            e_skate_bms_set_rx(&bmsConfig, i);
            e_skate_err_t errCode = e_skate_bms_get_status(
            &bmsConfig,
            &bmsStatus);

            if (errCode == E_SKATE_SUCCESS)
            {
                printf("Got status: %.2fV, %d%%, %.2fA, %dºC, %dºC\n",
                    (double)    bmsStatus.voltage / 100,
                    (int)       bmsStatus.capacity,
                    (double)    bmsStatus.current / 100,
                    (int)       bmsStatus.temperature1 - 20,
                    (int)       bmsStatus.temperature2 - 20);
            } else
            {
                printf("Got error %d: %s.\n",
                    errCode,
                    e_skate_err_to_str(errCode));
            }
        }


    }
}                