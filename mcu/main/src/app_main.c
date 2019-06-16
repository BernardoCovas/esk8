#include <e_ride_config.h>
#include <e_ride_err.h>
#include <e_ride_ble.h>
#include <e_ride_bms.h>
#include <e_ride_ps2.h>
#include <e_ride_pwm.h>

#include <app_ble_srvc.h>

#include <stdint.h>
#include <stdio.h>

void status_task()
{
    e_ride_bms_status_t         bmsStatus       = {0};
    e_ride_bms_deep_status_t    bmsDeepStatus   = {0};
    e_ride_bms_config_t         bmsConfig;
    e_ride_bms_init_from_config_h(&bmsConfig);

    printf("-----------------------------\n");
    printf("Listening to BMS...\n");
    printf("-----------------------------\n");


    while(true)
    {
        for (int i=0; i<E_RIDE_UART_BMS_CONF_NUM; i++)
        {
            e_ride_bms_set_rx(&bmsConfig, i);
            e_ride_err_t errCodeS = e_ride_bms_get_status(&bmsConfig, &bmsStatus);
            e_ride_err_t errCodeDS = e_ride_bms_get_deep_status(&bmsConfig, &bmsDeepStatus);

            if (errCodeS)
                printf("[BMS] Got error: %s reading BMS status at index: %d.\n",
                    e_ride_err_to_str(errCodeS), i);
            if (errCodeDS)
                printf("[BMS] Got error: %s reading BMS deep status at index: %d.\n",
                    e_ride_err_to_str(errCodeDS), i);

            app_srvc_status_update_bms_shallow(errCodeS, i, &bmsStatus);
            app_srvc_status_update_bms_deep(errCodeDS, i, &bmsDeepStatus);

            /* We might wait quite a long time here */
            vTaskDelay(10000 / portTICK_PERIOD_MS);
        }

    }

}


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

            /**
             * For good measure let's enable the
             * PS2 device again. It might have disconnected.
             */
            uint8_t _dummy;
            e_ride_ps2_send_byte(&ps2Handle, 0xF4);
            e_ride_ps2_await_byte(&ps2Handle, &_dummy, 100);

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
    e_ride_ble_config_t         bleCnfg;
    e_ride_ble_app_t*           appSrvcList_p[] = APP_ALL_SRVC_LIST_P();

    e_ride_ble_init(&bleCnfg);
    e_ride_ble_register_apps((uint16_t) sizeof(appSrvcList_p) / sizeof(appSrvcList_p[0]), appSrvcList_p);


    /**
     * The bluetooth stack seems
     * to either take priority over our
     * ISR's, or absolutely devour the
     * available CPU time. We can't have
     * ps2 and BLE on the same core.
     */
    xTaskCreatePinnedToCore(ps2_task, "ps2_task", 2048, NULL, 1, NULL, 1);
    xTaskCreate(status_task, "status_task", 2048, NULL, 2, NULL);
}
