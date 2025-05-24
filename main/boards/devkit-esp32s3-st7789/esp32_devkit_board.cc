
#include "board.h"
#include "display/display.h"
#include "display/backlight.h"
#include "display/esp_lcd_st7789.h"
#include "esp_lcd_panel_io_additions.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "config.h"
#include "audio/audio_codec.h"

class DevKitS3Board : public WifiBoard {
public:
    DevKitS3Board() : lcd_(nullptr), backlight_(nullptr) {}
    void InitializeLcdDisplay() override {
        spi_bus_config_t buscfg = {
            .mosi_io_num = DISPLAY_MOSI_PIN,
            .miso_io_num = DISPLAY_MISO_PIN,
            .sclk_io_num = DISPLAY_SCLK_PIN,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = DISPLAY_HRES * DISPLAY_VRES * 2 + 8,
        };
        ESP_ERROR_CHECK(spi_bus_initialize(DISPLAY_HOST, &buscfg, SPI_DMA_CH_AUTO));

        esp_lcd_panel_io_spi_config_t io_config = {
            .cs_gpio_num = DISPLAY_CS_PIN,
            .dc_gpio_num = DISPLAY_DC_PIN,
            .pclk_hz = 40 * 1000 * 1000,
            .spi_mode = 0,
            .trans_queue_depth = 10,
            .on_color_trans_done = nullptr,
            .user_ctx = nullptr,
        };
        esp_lcd_panel_io_handle_t io_handle;
        ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)DISPLAY_HOST, &io_config, &io_handle));

        esp_lcd_panel_st7789_config_t panel_config = {
            .reset_gpio_num = DISPLAY_RST_PIN,
            .rgb_endian = LCD_RGB_ENDIAN_RGB,
            .bits_per_pixel = 16,
        };
        esp_lcd_panel_handle_t panel_handle;
        ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

        lcd_ = new Display(panel_handle, DISPLAY_HRES, DISPLAY_VRES);

        if (DISPLAY_BACKLIGHT_PIN != GPIO_NUM_NC) {
            backlight_ = new BacklightPWM(DISPLAY_BACKLIGHT_PIN);
            backlight_->on();
        }
    }

    Display* GetDisplay() override { return lcd_; }
    Backlight* GetBacklight() override { return backlight_; }
    AudioCodec* GetAudioCodec() override { return nullptr; }

private:
    Display* lcd_;
    Backlight* backlight_;
};

/* Factory */
extern "C" WifiBoard* create_board() {
    static DevKitS3Board board;
    return &board;
}
