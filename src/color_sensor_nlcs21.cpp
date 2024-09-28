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
  delay(3);

  wire_.beginTransmission(i2c_address_);
  wire_.write(0x00);
  wire_.write(0x01);
  wire_.endTransmission();
  delay(3);

  wire_.beginTransmission(i2c_address_);
  wire_.write(0x05);
  wire_.write(0x03);
  wire_.endTransmission();
  delay(3);
  return ret;
}

ColorSensorNlcs21::Color ColorSensorNlcs21::GetColor() const {
  Color color;
  uint16_t value[4] = {0};
  uint16_t raw_r, raw_g, raw_b = 0;

  delay(132.2688);
  // 发送命令请求读取数据
  wire_.beginTransmission(i2c_address_);
  wire_.write(0x1C);
  wire_.endTransmission();
  // 请求从传感器读取3个字节的数据
  wire_.requestFrom(i2c_address_, sizeof(value));

  // 确认读取的数据大小是否正确
  if (wire_.available() == sizeof(value)) {
    wire_.readBytes(reinterpret_cast<uint8_t *>(value), sizeof(value));

    raw_r = value[0];
    raw_g = value[1];
    raw_b = value[2];
    color.c = value[3];
  }
  if (color.c == 0) {
    color.r = 0;
    color.g = 0;
    color.b = 0;
  } else {
    color.r = static_cast<uint16_t>((float)raw_r / color.c * 255);
    color.g = static_cast<uint16_t>((float)raw_g / color.c * 255);
    color.b = static_cast<uint16_t>((float)raw_b / color.c * 255);
  }
  return color;
}

}  // namespace emakefun