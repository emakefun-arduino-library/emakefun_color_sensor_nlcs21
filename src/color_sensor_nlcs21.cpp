#include "color_sensor_nlcs21.h"

#include <Arduino.h>

namespace emakefun {
namespace {
constexpr uint16_t kMaxRawR = 500;
constexpr uint16_t kMaxRawG = 1100;
constexpr uint16_t kMaxRawB = 800;

float Map(const float value, const float in_min, const float in_max, const float out_min, const float out_max) {
  if (value <= in_min) {
    return out_min;
  } else if (value >= in_max) {
    return out_max;
  } else {
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }
}
}  // namespace

ColorSensorNlcs21::ColorSensorNlcs21(const uint8_t i2c_address, TwoWire &wire) : i2c_address_(i2c_address), wire_(wire) {
  // do somethings
}

ColorSensorNlcs21::ErrorCode ColorSensorNlcs21::Initialize() {
  ErrorCode ret = kOK;
  wire_.beginTransmission(i2c_address_);
  wire_.write(0x00);
  wire_.write(0x80);
  ret = static_cast<ErrorCode>(wire_.endTransmission());

  delay(100);

  wire_.beginTransmission(i2c_address_);
  wire_.write(0x00);
  wire_.write(0x01);
  wire_.endTransmission();

  // wire_.beginTransmission(i2c_address_);
  // wire_.write(0x04);
  // wire_.write(0x20);
  // wire_.endTransmission();

  return ret;
}

ColorSensorNlcs21::Color ColorSensorNlcs21::GetColor() const {
  Color color;
  uint16_t value[3] = {0};

  // 发送命令请求读取数据
  wire_.beginTransmission(i2c_address_);
  wire_.write(0x1C);
  wire_.endTransmission();

  // 请求从传感器读取3个字节的数据
  wire_.requestFrom(i2c_address_, sizeof(value));

  // 确认读取的数据大小是否正确
  if (wire_.available() == sizeof(value)) {
    wire_.readBytes(reinterpret_cast<uint8_t *>(value), sizeof(value));

    color.r = value[0];
    color.g = value[1];
    color.b = value[2];
  }
  return color;
}

}  // namespace emakefun