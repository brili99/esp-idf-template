// #include <stdio.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"


// void app_main(void)
// {
//     int i = 0;
//     while (1) {
//         printf("[%d] Hello world!\n", i);
//         i++;
//         vTaskDelay(5000 / portTICK_PERIOD_MS);
//     }
// }

#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_sntp.h"
#include "esp_voip.h"

static const char *TAG = "voip_example";

static const esp_voip_config_t voip_cfg = {
    .codec_type = ESP_VOIP_CODEC_TYPE_OPUS,
    .codec_param.opus_params = {
        .sample_rate = ESP_VOIP_OPUS_SAMPLE_RATE_16K,
        .bit_rate = 32000,
        .complexity = 10,
        .frame_size = 20
    }
};

static void voip_event_handler(esp_voip_event_t event, void *event_data)
{
    switch (event) {
        case ESP_VOIP_EVENT_CALL_INCOMING:
            ESP_LOGI(TAG, "Incoming call");
            esp_voip_answer_call();
            break;
        case ESP_VOIP_EVENT_CALL_CONNECTED:
            ESP_LOGI(TAG, "Call connected");
            break;
        case ESP_VOIP_EVENT_CALL_DISCONNECTED:
            ESP_LOGI(TAG, "Call disconnected");
            break;
        default:
            break;
    }
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_STA_CONNECTED) {
        ESP_LOGI(TAG, "Wi-Fi connected");
    } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Wi-Fi disconnected");
        esp_voip_hangup_call();
    }
}

void app_main()
{
    ESP_ERROR_CHECK(esp_voip_init(&voip_cfg));
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                                &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_voip_register_event_handler(&voip_event_handler));
    ESP_ERROR_CHECK(esp_wifi_start());
}
