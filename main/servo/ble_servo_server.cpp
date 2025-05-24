#include "ble_servo_server.h"
#include <cctype>   // toupper
#include <string>   // std::string

static const char* TAG_BLE = "BleServoServer";   // 如果后面会用 ESP_LOG*

BleServoServer::BleServoServer(Servo* s1, Servo* s2, Servo* s3, Servo* s4) {
    servos_[0] = s1;
    servos_[1] = s2;
    servos_[2] = s3;
    servos_[3] = s4;
}

void BleServoServer::begin() {
    NimBLEDevice::init("ServoDog");
    auto *server = NimBLEDevice::createServer();
    auto *service = server->createService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");

    auto *rx = service->createCharacteristic(
                   "6E400002-B5A3-F393-E0A9-E50E24DCCA9E",
                   NIMBLE_PROPERTY::WRITE  // 新枚举
               );
    rx->setCallbacks(this);
    service->start();

    auto *adv = NimBLEDevice::getAdvertising();
    adv->addServiceUUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
    adv->enableScanResponse(true);      // 新方法
    NimBLEDevice::startAdvertising();
}

void BleServoServer::stand() {
    for (int i=0;i<4;i++) servos_[i]->setAngle(90);
}
void BleServoServer::moveForward() {
    // Simple alternating gait
    servos_[0]->setAngle(60);
    servos_[1]->setAngle(120);
    servos_[2]->setAngle(60);
    servos_[3]->setAngle(120);
    vTaskDelay(pdMS_TO_TICKS(300));
    servos_[0]->setAngle(120);
    servos_[1]->setAngle(60);
    servos_[2]->setAngle(120);
    servos_[3]->setAngle(60);
    vTaskDelay(pdMS_TO_TICKS(300));
    stand();
}
void BleServoServer::moveBackward() {
    servos_[0]->setAngle(120);
    servos_[1]->setAngle(60);
    servos_[2]->setAngle(120);
    servos_[3]->setAngle(60);
    vTaskDelay(pdMS_TO_TICKS(300));
    servos_[0]->setAngle(60);
    servos_[1]->setAngle(120);
    servos_[2]->setAngle(60);
    servos_[3]->setAngle(120);
    vTaskDelay(pdMS_TO_TICKS(300));
    stand();
}
void BleServoServer::turnLeft() {
    servos_[0]->setAngle(60);
    servos_[2]->setAngle(60);
    vTaskDelay(pdMS_TO_TICKS(300));
    stand();
}
void BleServoServer::turnRight() {
    servos_[1]->setAngle(120);
    servos_[3]->setAngle(120);
    vTaskDelay(pdMS_TO_TICKS(300));
    stand();
}

void BleServoServer::onWrite(NimBLECharacteristic* pCharacteristic,
                             NimBLEConnInfo&       /*connInfo*/)
{
    const std::string& value = pCharacteristic->getValue();  // 取到写入的数据
    if (value.empty()) {
        return;
    }

    char cmd = std::toupper(static_cast<unsigned char>(value[0]));

    switch (cmd) {
        case 'F': moveForward();  break;
        case 'B': moveBackward(); break;
        case 'L': turnLeft();     break;
        case 'R': turnRight();    break;
        default:  stand();        break;
    }
}