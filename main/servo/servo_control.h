#pragma once
#include <stdint.h>
/**
 * Simple LEDC-based servo driver for ESP32-S3
 * SG90 requires 50 Hz PWM (20 ms period) with pulse width 0.5–2.5 ms => 0–180 degrees.
 */
class Servo {
public:
    /**
     * @param gpio_num GPIO connected to servo signal pin
     * @param channel  LEDC channel 0‑7
     */
    Servo(int gpio_num, int channel);
    /**
     * Configure LEDC timer+channel. Must be called once before use.
     */
    void attach();
    /**
     * Set angle in degrees (0–180).
     */
    void setAngle(float angle_deg);
private:
    int gpio_;
    int channel_;
};