#include <e_ride_config.h>

#include <e_ride_err.h>
#include <e_ride_ps2.h>
#include <e_ride_ps2_utils.h>

#include <driver/gpio.h>

void signal_b_start(

    e_ride_ps2_handle_t* ps2Handle

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


void clear_handle(e_ride_ps2_handle_t* ps2Handle)
{
    uint8_t c_pin = ps2Handle->ps2Config.gpioConfig.clockPin;
    uint8_t d_pin = ps2Handle->ps2Config.gpioConfig.dataPin;

    ps2Handle->ps2Config.dataDrctn = PS2_DIRCN_RECV;
    ps2Handle->txTaskToNotift      = NULL;

    gpio_set_direction(d_pin, GPIO_MODE_INPUT);
    gpio_set_intr_type(c_pin, GPIO_INTR_NEGEDGE);
}

e_ride_err_t e_ride_ps2_send_byte(

    e_ride_ps2_handle_t* ps2Handle,
    uint8_t               byte

)
{
    ps2Handle->txTaskToNotift   = xTaskGetCurrentTaskHandle();
    ps2Handle->txPkt.frameIndex = 0;
    ps2Handle->txPkt.newStart   = 0;
    ps2Handle->txPkt.newByte    = byte;
    ps2Handle->txPkt.newParity  = e_ride_ps2_get_parity(byte);
    ps2Handle->txPkt.newStop    = 1;

    signal_b_start(ps2Handle);

    if (!ulTaskNotifyTake(
            pdTRUE,
            E_RIDE_PS2_PACKET_TIMEOUT_MS * 20 / portTICK_PERIOD_MS))
        {
            clear_handle(ps2Handle);
            return E_RIDE_PS2_ERR_TIMEOUT;
        }

    return E_RIDE_SUCCESS;
}
