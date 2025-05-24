#include "servo_control.h"
#include "driver/ledc.h"
#include "esp_err.h"

Servo::Servo(int gpio_num, int channel) : gpio_(gpio_num), channel_(channel) {}

void Servo::attach() {
    // Use a shared timer0 in low‑speed mode 50 Hz
    ledc_timer_config_t timer_conf = {};
    timer_conf.speed_mode = LEDC_LOW_SPEED_MODE;
    timer_conf.timer_num = LEDC_TIMER_0;
    timer_conf.duty_resolution = LEDC_TIMER_12_BIT;    // 12‑bit duty
    timer_conf.freq_hz = 50;
    timer_conf.clk_cfg = LEDC_AUTO_CLK;
    esp_err_t ret = ledc_timer_config(&timer_conf);
    (void)ret;

    ledc_channel_config_t ch_conf = {};
    ch_conf.gpio_num   = gpio_;
    ch_conf.speed_mode = LEDC_LOW_SPEED_MODE;
    ch_conf.channel    = static_cast<ledc_channel_t>(channel_);
    ch_conf.timer_sel  = LEDC_TIMER_0;
    ch_conf.duty       = 0;
    ch_conf.hpoint     = 0;
    ledc_channel_config(&ch_conf);
}

void Servo::setAngle(float angle_deg) {
    if (angle_deg < 0) angle_deg = 0;
    if (angle_deg > 180) angle_deg = 180;
    const float pulse_min_ms = 0.5f;
    const float pulse_max_ms = 2.5f;
    float pulse_ms = pulse_min_ms + (angle_deg / 180.0f) * (pulse_max_ms - pulse_min_ms);
    uint32_t duty_max = (1 << 12) - 1;                 // 12‑bit resolution
    uint32_t duty = static_cast<uint32_t>((pulse_ms / 20.0f) * duty_max);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, static_cast<ledc_channel_t>(channel_), duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, static_cast<ledc_channel_t>(channel_));
}