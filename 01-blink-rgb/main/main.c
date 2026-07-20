#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_strip.h"
#include "esp_log.h"

// Standardowy pin dla wbudowanej diody RGB na ESP32-S3
#define BLINK_GPIO 48 

static const char *TAG = "MÓJ_PROJEKT";

void app_main(void)
{
    ESP_LOGI(TAG, "Inicjalizacja diody RGB na pinie %d", BLINK_GPIO);

    // Konfiguracja sprzętowa diody
    led_strip_handle_t led_strip;
    led_strip_config_t strip_config = {
        .strip_gpio_num = BLINK_GPIO,
        .max_leds = 1, // Mamy tylko jedną diodę na płytce
        .led_pixel_format = LED_PIXEL_FORMAT_GRB,
        .led_model = LED_MODEL_WS2812,
    };
    // Konfiguracja sygnału (używamy sprzętowego nadajnika RMT wbudowanego w ESP)
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz - standard dla WS2812
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));

    // Czyszczenie diody (wyłączenie) na start
    led_strip_clear(led_strip);

    bool led_state = false;

    // Nieskończona pętla (odpowiednik void loop() z Arduino)
    while (1) {
        if (led_state) {
            ESP_LOGI(TAG, "Dioda WŁĄCZONA (Fioletowy)");
            // Ustawiamy kolor. Wartości to (R, G, B) od 0 do 255. 
            // Dajemy 15 z 255 żeby oszczędzać wzrok.
            led_strip_set_pixel(led_strip, 0, 1, 0, 15);
            led_strip_refresh(led_strip); // Wyślij dane do diody
        } else {
            ESP_LOGI(TAG, "Dioda WYŁĄCZONA");
            led_strip_clear(led_strip);
        }
        
        led_state = !led_state; // Odwróć stan na przeciwny
        
        // Poczekaj przed kolejną zmianą
        vTaskDelay(2500 / portTICK_PERIOD_MS);
    }
}