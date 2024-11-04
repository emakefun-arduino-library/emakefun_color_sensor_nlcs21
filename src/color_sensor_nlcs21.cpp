#include "color_sensor_nlcs21.h"

#include <Arduino.h>

namespace emakefun {
namespace {
constexpr float kIntegrationTimes[] = {2.0667, 8.2668, 33.0672, 132.2688};
}  // namespace

ColorSensorNlcs21::ColorSensorNlcs21(const Gain gain,
                                     const IntegrationTime integration_time,
                                     const uint8_t i2c_address,
                                     TwoWire& wire)
    : i2c_address_(i2c_address), wire_(wire), gain_(gain), integration_time_(integration_time) {
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

  // 开启中断
  wire_.beginTransmission(i2c_address_);
  wire_.write(0x01);
  wire_.write(0x03);
  wire_.endTransmission();
  delay(3);

  // 关闭通电复位中断
  wire_.beginTransmission(i2c_address_);
  wire_.write(0x02);
  wire_.write(0x00);
  wire_.endTransmission();
  delay(3);

  // persistence filter register
  wire_.beginTransmission(i2c_address_);
  wire_.write(0x0B);
  wire_.write(0x01);  // 0x01: 1次, 0x02: 2次, 0x03: 3次....16次
  wire_.endTransmission();
  delay(3);

  // 设置中断通道为透明通道
  wire_.beginTransmission(i2c_address_);
  wire_.write(0x16);
  wire_.write(0x08);
  wire_.endTransmission();
  delay(3);

  // 设置增益
  wire_.beginTransmission(i2c_address_);
  wire_.write(0x04);
  wire_.write(0x80 | (0x01 << gain_));
  wire_.endTransmission();
  delay(3);

  // 设置积分时间
  wire_.beginTransmission(i2c_address_);
  wire_.write(0x05);
  wire_.write(integration_time_);
  wire_.endTransmission();
  delay(3);

  return ret;
}

bool ColorSensorNlcs21::GetColor(Color* const color) const {
  if (color == nullptr) {
    return false;
  }

  if (last_read_time_ == 0) {
    last_read_time_ = millis();
    return false;
  }

  if (millis() - last_read_time_ < kIntegrationTimes[integration_time_]) {
    return false;
  }

  last_read_time_ = millis();

  wire_.beginTransmission(i2c_address_);
  wire_.write(0x1C);
  wire_.endTransmission();

  // 请求从传感器读取4个字节的数据
  wire_.requestFrom(i2c_address_, sizeof(*color));

  // 确认读取的数据大小是否正确
  if (wire_.available() == sizeof(*color)) {
    wire_.readBytes(reinterpret_cast<uint8_t*>(color), sizeof(*color));
  }

  if (color->c == 0) {
    *color = Color{};
    return false;
  }

  color->r = static_cast<uint16_t>((float)color->r / color->c * 255);
  color->g = static_cast<uint16_t>((float)color->g / color->c * 255);
  color->b = static_cast<uint16_t>((float)color->b / color->c * 255);

  return true;
}

uint8_t ColorSensorNlcs21::GetInterruptStatus() const {
  uint8_t interrupt = 0;
  wire_.beginTransmission(i2c_address_);
  wire_.write(0x02);
  wire_.endTransmission();
  // 请求从传感器读取1个字节的数据
  wire_.requestFrom(i2c_address_, 1);

  if (wire_.available() == 1) interrupt = wire_.read() & 0x01;
  return interrupt;
}

void ColorSensorNlcs21::SetThreshold(uint16_t threshold_low, uint16_t threshold_high) const {
  wire_.beginTransmission(i2c_address_);
  wire_.write(0x0C);
  wire_.write(threshold_low & 0xFF);
  wire_.endTransmission();
  delay(3);
  wire_.beginTransmission(i2c_address_);
  wire_.write(0x0D);
  wire_.write(threshold_low >> 8);
  wire_.endTransmission();
  delay(3);
  wire_.beginTransmission(i2c_address_);
  wire_.write(0x0E);
  wire_.write(threshold_high & 0xFF);
  wire_.endTransmission();
  delay(3);
  wire_.beginTransmission(i2c_address_);
  wire_.write(0x0F);
  wire_.write(threshold_high >> 8);
  wire_.endTransmission();
  delay(3);

  Serial.print("Threshold set to: ");
  // Serial.println(threshold);
}

void ColorSensorNlcs21::ClearInterrupt() const {
  wire_.beginTransmission(i2c_address_);
  wire_.write(0x02);
  wire_.write(0x80);
  wire_.endTransmission();
  delay(3);
}
}  // namespace emakefun