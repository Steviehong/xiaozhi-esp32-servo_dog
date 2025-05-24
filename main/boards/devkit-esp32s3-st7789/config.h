
#pragma once
#include "sdkconfig.h"
#include "driver/gpio.h"
#include "esp_lcd_panel_io.h"

/* Board configuration for ESP32-S3-DevKitC-1 (N16R8) + 1.3" 240x240 ST7789 SPI LCD */

#define BOARD_NAME          "devkit-esp32s3-st7789"

/* ==== LCD ==== */
#define DISPLAY_HOST        SPI2_HOST
#define DISPLAY_HRES        240
#define DISPLAY_VRES        240

#define DISPLAY_CS_PIN      GPIO_NUM_10
#define DISPLAY_DC_PIN      GPIO_NUM_9
#define DISPLAY_SCLK_PIN    GPIO_NUM_12
#define DISPLAY_MOSI_PIN    GPIO_NUM_11
#define DISPLAY_RST_PIN     GPIO_NUM_NC
#define DISPLAY_BACKLIGHT_PIN GPIO_NUM_48   // uses LEDC for PWM brightness

/* ST7789 does not require MISO */
#define DISPLAY_MISO_PIN    GPIO_NUM_NC

/* ==== Audio (unused) ==== */
#define AUDIO_I2S_SPK_GPIO_BCLK   GPIO_NUM_NC
#define AUDIO_I2S_SPK_GPIO_LRCK   GPIO_NUM_NC
#define AUDIO_I2S_SPK_GPIO_DOUT   GPIO_NUM_NC
#define AUDIO_I2S_MIC_GPIO_SCK    GPIO_NUM_NC
#define AUDIO_I2S_MIC_GPIO_WS     GPIO_NUM_NC
#define AUDIO_I2S_MIC_GPIO_DIN    GPIO_NUM_NC

/* ==== Touch (none) ==== */
#define TOUCH_I2C_SCL_PIN  GPIO_NUM_NC
#define TOUCH_I2C_SDA_PIN  GPIO_NUM_NC

