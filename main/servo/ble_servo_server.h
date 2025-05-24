#pragma once
#include "servo_control.h"
#include <NimBLEDevice.h>
#include <cstdint>     // 可选——如果你的 Servo 定义里用到

class BleServoServer : public NimBLECharacteristicCallbacks {
public:
    BleServoServer(Servo* s1, Servo* s2, Servo* s3, Servo* s4);
    void begin();

private:
    // ★ 新 API 的完整签名（带 connInfo）
    void onWrite(NimBLECharacteristic* pCharacteristic,
                 NimBLEConnInfo&       connInfo) override;

    Servo* servos_[4];
    void moveForward();
    void moveBackward();
    void turnLeft();
    void turnRight();
    void stand();
};
