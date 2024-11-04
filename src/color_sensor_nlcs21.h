#pragma once

#ifndef EMAKEFUN_COLOR_SENSOR_NLCS21_H_
#define EMAKEFUN_COLOR_SENSOR_NLCS21_H_

#include <Wire.h>
#include <stdint.h>

namespace emakefun {
class ColorSensorNlcs21 {
 public:
  static constexpr uint32_t kVersionMajor = 2;
  static constexpr uint32_t kVersionMinor = 0;
  static constexpr uint32_t kVersionPatch = 1;
  static constexpr uint8_t kDefaultI2cAddress = 0x38;

  /**
   * @enum ErrorCode
   * @brief 错误码
   */
  enum ErrorCode : uint32_t {
    kOK = 0,                                  /**< 0：成功 */
    kI2cDataTooLongToFitInTransmitBuffer = 1, /**< 1：I2C数据太长，无法装入传输缓冲区 */
    kI2cReceivedNackOnTransmitOfAddress = 2,  /**< 2：在I2C发送地址时收到NACK */
    kI2cReceivedNackOnTransmitOfData = 3,     /**< 3：在I2C发送数据时收到NACK */
    kI2cOtherError = 4,                       /**< 4：其他I2C错误 */
    kI2cTimeout = 5,                          /**< 5：I2C通讯超时 */
    kInvalidParameter = 6,                    /**< 6：参数错误 */
    kUnknownError = 7,                        /**< 7：未知错误*/
  };

  enum Gain : uint8_t {
    kGain1X,
    kGain4X,
    kGain8X,
    kGain32X,
    kGain96X,
    kGain192X,
  };

  enum IntegrationTime : uint8_t {
    kIntegrationTime2ms,
    kIntegrationTime8ms,
    kIntegrationTime33ms,
    kIntegrationTime132ms,
  };

  struct Color {
    /* data */
    uint16_t r = 0;
    uint16_t g = 0;
    uint16_t b = 0;
    uint16_t c = 0;
  };

  explicit ColorSensorNlcs21(const Gain gain,
                             const IntegrationTime integration_time,
                             const uint8_t i2c_address = kDefaultI2cAddress,
                             TwoWire& wire = Wire);

  explicit ColorSensorNlcs21(const Gain gain, const IntegrationTime integration_time, TwoWire& wire)
      : ColorSensorNlcs21(gain, integration_time, kDefaultI2cAddress, wire) {
  }

  /**
   * @brief 初始化函数
   * @return 返回值请参考 @ref ErrorCode
   */
  ErrorCode Initialize();

  bool GetColor(Color* const color) const;

  uint8_t GetInterruptStatus() const;

  void ClearInterrupt() const;

  void SetThreshold(uint16_t threshold_low, uint16_t threshold_high) const;

 private:
  ColorSensorNlcs21(const ColorSensorNlcs21&) = delete;
  ColorSensorNlcs21& operator=(const ColorSensorNlcs21&) = delete;

  const Gain gain_ = kGain1X;
  const IntegrationTime integration_time_ = kIntegrationTime132ms;

  const uint8_t i2c_address_ = kDefaultI2cAddress;
  mutable uint64_t last_read_time_ = 0;
  TwoWire& wire_ = Wire;
};
}  // namespace emakefun
#endif