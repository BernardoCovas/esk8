#include <esk8_config.h>

#include <esk8_err.h>
#include <esk8_ps2.h>
#include <esk8_ps2_utils.h>

#include <driver/gpio.h>

void signal_b_start(

    esk8_ps2_handle_t* ps2Handle

)
{
    uint8_t c_pin = ps2Handle->ps2Config.gpioConfig.clockPin;
    uint8_t d_pin = ps2Handle->ps2Config.gpioConfig.dataPin;

    gpio_set_direction(c_pin, GPIO_MODE_OUTPUT);
    gpio_set_direction(d_pin, GPIO_MODE_OUTPUT);

    ps2Handle->ps2Config.dataDrctn = PS2_DIRCN_SEND;

    gpio_set_level(c_pin, 0);
    vTaskDelay(60 / portTICK_PERIOD_MS / 1000);

    gpio_set_intr_type(c_pin, GPIO_INTR_POSEDGE);
    gpio_set_direction(c_pin, GPIO_MODE_INPUT);
}


void clear_handle(

    esk8_ps2_handle_t* ps2Handle

)
{
    uint8_t c_pin = ps2Handle->ps2Config.gpioConfig.clockPin;
    uint8_t d_pin = ps2Handle->ps2Config.gpioConfig.dataPin;

    ps2Handle->ps2Config.dataDrctn = PS2_DIRCN_RECV;
    ps2Handle->txTaskToNotift      = NULL;

    gpio_set_direction(d_pin, GPIO_MODE_INPUT);
    gpio_set_intr_type(c_pin, GPIO_INTR_NEGEDGE);
}


esk8_err_t esk8_ps2_send_byte(

    esk8_ps2_handle_t* ps2Handle,
    uint8_t               byte

)
{
    ps2Handle->txTaskToNotift   = xTaskGetCurrentTaskHandle();
    ps2Handle->txPkt.frameIndex = 0;
    ps2Handle->txPkt.newStart   = 0;
    ps2Handle->txPkt.newByte    = byte;
    ps2Handle->txPkt.newParity  = esk8_ps2_get_parity(byte);
    ps2Handle->txPkt.newStop    = 1;

    signal_b_start(ps2Handle);

    esk8_err_t err = ESK8_SUCCESS;

    if (!ulTaskNotifyTake(
            pdTRUE,
            ESK8_PS2_BYTE_SEND_TIMEOUT_MS / portTICK_PERIOD_MS))
        {
            err = ESK8_PS2_ERR_TIMEOUT;
        }

    clear_handle(ps2Handle);
    return err;
}
