#include <esk8_err.h>
#include <esk8_ps2.h>
#include <esk8_pwm.h>
#include <esk8_ble.h>

#include <rmt_ble.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


void ps2_task()
{
    esk8_err_t        errCode;
    esk8_ps2_handle_t ps2Handle;
    esk8_ps2_init_from_config_h(&ps2Handle);

    printf("-----------------------------\n");
    printf("Listening to PS2 with PWM... \n");
    printf("-----------------------------\n");

    errCode = esk8_ps2_send_cmd(&ps2Handle, ESK8_PS2_CMD_DATA_ENABLE, 1000);
    if (errCode)
    {
        printf("[ PS2 ] Got err: %d at init.\n", errCode);
        return;
    }

    esk8_ps2_mvmnt_t trckMvmnt = {0};

    while(1)
    {
        errCode = esk8_ps2_await_mvmnt(&ps2Handle, &trckMvmnt);
        if (errCode)
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

        /**
         * Write speed to BLE Ctrl
         */
        app_ctrl_update_speed(trckMvmnt);
    }
}

void app_main()
{
    /**
     * TODO:
     * 
     * The base is still the server.
     * This controller connects to it, authenticates and is
     * able to controll the thing.
     */

    if (!xTaskCreate(ps2_task, "ps2_task", 2048, NULL, 2, NULL))
    {
        printf("Failed creating ps2 task\n");
        return;
    }
}