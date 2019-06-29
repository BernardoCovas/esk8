#include <esk8_config.h>
#include <esk8_err.h>
#include <esk8_ble.h>
#include <esk8_bms.h>
#include <esk8_ps2.h>
#include <esk8_pwm.h>

#include <app_ble_srvc.h>

#include <stdint.h>
#include <stdio.h>


void status_task()
{
    esk8_bms_status_t         bmsStatus       = {0};
    esk8_bms_deep_status_t    bmsDeepStatus   = {0};
    esk8_bms_config_t         bmsConfig;

    esk8_bms_init_from_config_h(&bmsConfig);

    printf("-----------------------------\n");
    printf("Listening to BMS...          \n");
    printf("-----------------------------\n");


    while(true)
    {
        for (int i=0; i<ESK8_UART_BMS_CONF_NUM; i++)
        {
            esk8_bms_set_rx(&bmsConfig, i);
            esk8_err_t errCodeS = esk8_bms_get_status(&bmsConfig, &bmsStatus);
            esk8_err_t errCodeDS = esk8_bms_get_deep_status(&bmsConfig, &bmsDeepStatus);

            if (errCodeS)
                printf("[ BMS ] Got error: %s reading BMS status at index: %d.\n",
                    esk8_err_to_str(errCodeS), i);
            if (errCodeDS)
                printf("[ BMS ] Got error: %s reading BMS deep status at index: %d.\n",
                    esk8_err_to_str(errCodeDS), i);

            app_srvc_status_update_bms_shallow(errCodeS, i, &bmsStatus);
            app_srvc_status_update_bms_deep(errCodeDS, i, &bmsDeepStatus);

            /* We might wait quite a long time here */
            vTaskDelay(10000 / portTICK_PERIOD_MS);
        }

    }

}


void ps2_task()
{
    esk8_ps2_handle_t ps2Handle;
    esk8_pwm_config_t pwm_Config;

    esk8_ps2_init_from_config_h(&ps2Handle);
    esk8_pwm_sgnl_init(&pwm_Config);

    printf("-----------------------------\n");
    printf("Listening to PS2 with PWM... \n");
    printf("-----------------------------\n");

    esk8_err_t errCode = esk8_ps2_send_cmd(&ps2Handle, ESK8_PS2_CMD_DATA_ENABLE, 1000);
    if (errCode)
    {
        printf("[ PS2 ] Got err: %d at init.\n", errCode);
        return;
    }

    esk8_ps2_mvmnt_t trckMvmnt;
    int speed = 0;
    while(1)
    {
        errCode = esk8_ps2_await_mvmnt(&ps2Handle, &trckMvmnt);
        if (errCode != ESK8_SUCCESS)
        {
            printf("[ PS2 ] Got err: %s\n", esk8_err_to_str(errCode));

            /**
             * For good measure let's enable the
             * PS2 device again. It might have disconnected.
             */
            errCode = esk8_ps2_send_cmd(&ps2Handle, ESK8_PS2_CMD_DATA_ENABLE, 100);
            if (errCode)
                printf("[ PS2 ] Got err: %s\n", esk8_err_to_str(errCode));

            continue;
        }

        if (trckMvmnt.lftBtn)
            speed = 0;

        speed += trckMvmnt.x;
        speed = speed>255?255:speed;
        speed = speed<  0?  0:speed;

        printf("[ PS2 ] Speed: %03d, X: %d %40s\n", speed, trckMvmnt.x, "");

        esk8_pwm_sgnl_set(&pwm_Config, (uint8_t) speed);
        app_srvc_status_update_speed((uint8_t)speed);

    }
}


void app_main()
{
    esk8_ble_config_t         bleCnfg;
    esk8_ble_app_t*           appSrvcList_p[] = APP_ALL_SRVC_LIST_P();

    esk8_ble_init(&bleCnfg);
    esk8_ble_register_apps((uint16_t) sizeof(appSrvcList_p) / sizeof(appSrvcList_p[0]), appSrvcList_p);

    // xTaskCreate(ps2_task   , "ps2_task"   , 2048, NULL, 1, NULL);
    xTaskCreate(status_task, "status_task", 2048, NULL, 1, NULL);
}
