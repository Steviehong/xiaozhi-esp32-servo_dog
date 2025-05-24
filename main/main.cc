#include <esp_log.h>
#include <esp_err.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <driver/gpio.h>
#include <esp_event.h>
#include "servo/servo_control.h"
#include "servo/ble_servo_server.h"
#include "application.h"
#include "system_info.h"

#define TAG "main"

static Servo servo1(15, 0);   // GPIO, LEDC channel
static Servo servo2(16, 1);
static Servo servo3(17, 2);
static Servo servo4(18, 3);

extern "C" void app_main(void)
{
    // Initialize the default event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Initialize NVS flash for WiFi configuration
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "Erasing NVS flash to fix corruption");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Attach servos
    servo1.attach();
    servo2.attach();
    servo3.attach();
    servo4.attach();

    // Bring up BLE server in a separate task so it doesn't block
    static BleServoServer bleServer(&servo1, &servo2, &servo3, &servo4);
    bleServer.begin();

    // Optionally continue running existing application (if needed)
    Application::GetInstance().Start();
}