#include <e_skate_err.h>
#include <e_skate_config.h>
#include <e_skate_ps2.h>
#include <e_skate_ps2_utils.h>

#include <esp_intr_alloc.h>
#include <driver/gpio.h>


void IRAM_ATTR e_skate_ps2_isr(void* param)
{
    e_skate_ps2_handle_t* ps2Handle = (e_skate_ps2_handle_t*) param;
    e_skate_ps2_config_t* ps2Config = (e_skate_ps2_config_t*) &( ps2Handle->ps2Config );

    bool newBit = gpio_get_level(
        ps2Config->dataPin);

    /**
     * TODO: 100 packets per second.
     * If around 8/9 ms have passed 
     * since the last clock, 
     * reset and clear the values.
     **/

    e_skate_err_t bitErrCode = e_skate_ps2_add_bit(
        ps2Handle,
        newBit);

    if (bitErrCode == E_SKATE_PS2_ERR_VALUE_READY)
    {
        // Send the new byte to the queue
        xQueueSendFromISR(
            ps2Handle->byteQueueHandle,
            &( ps2Handle->newByte ),
            NULL);

        e_skate_ps2_reset_handle(ps2Handle);
    }
}


e_skate_err_t e_skate_ps2_init(

    e_skate_ps2_handle_t* ps2Handle,
    e_skate_ps2_config_t* ps2Config

)
{
    e_skate_ps2_reset_handle(ps2Handle);
    ps2Handle->ps2Config = *ps2Config;

    ps2Handle->byteQueueHandle = xQueueCreate(
        E_SKATE_PS2_BYTE_QUEUE_LENGTH,
        1);

    if (ps2Handle->byteQueueHandle == NULL)
        return E_SKATE_ERR_OOM;

    ESP_ERROR_CHECK(gpio_set_pull_mode(
        ps2Config->clockPin,
        GPIO_PULLUP_ONLY));

    ESP_ERROR_CHECK(gpio_set_pull_mode(
        ps2Config->dataPin,
        GPIO_PULLUP_ONLY));
   
    ESP_ERROR_CHECK(gpio_set_direction(
        ps2Config->dataPin,
        GPIO_MODE_INPUT));

    ESP_ERROR_CHECK(gpio_set_direction(
        ps2Config->clockPin,
        GPIO_MODE_INPUT));

    /* Falling edge intr, as per Ps2. */
    ESP_ERROR_CHECK(gpio_set_intr_type(
        ps2Config->clockPin,
        GPIO_INTR_NEGEDGE));

    esp_err_t isrErrCode = gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    
    /* Service might already be installed. */
    if  (isrErrCode != ESP_OK && isrErrCode != ESP_ERR_INVALID_STATE)
        ESP_ERROR_CHECK(isrErrCode);

    ESP_ERROR_CHECK(gpio_isr_handler_add(
        ps2Config->clockPin,
        e_skate_ps2_isr,
        ps2Handle));

    return E_SKATE_SUCCESS;
}