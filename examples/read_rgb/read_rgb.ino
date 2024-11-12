/**
 * @example read_rgb.ino
 */

#include <Adafruit_NeoPixel.h>

#include <color_sensor_nlcs21.h>

#define PIN 5               // 控制灯带的引脚
#define NUM_LEDS 12         // 灯带上灯的数量
#define THRESHOLD_HIGH 150  // 灯的亮度高阈值
#define THRESHOLD_LOW 0     // 灯的亮度低阈值
Adafruit_NeoPixel strip =
    Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

#define INFINITE_LOOP_ON_FAILURE InfiniteLoopOnFailure(__FUNCTION__, __LINE__)

namespace {
emakefun::ColorSensorNlcs21 g_color_sensor;
// (
//     emakefun::ColorSensorNlcs21::kGain1X,
//     emakefun::ColorSensorNlcs21::kIntegrationTime132ms);
void InfiniteLoopOnFailure(const char* function, const uint32_t line_number) {
  Serial.println(String(F("entering an infinite loop due to failure in ")) +
                 function + F(", at line number: ") + line_number);
  while (true) {
    yield();
  }
}

}  // namespace

void setup() {
  Serial.begin(115200);
  Serial.println(F("setup"));
  strip.begin();  // 初始化灯带
  strip.show();   // 所有LED灯关闭
  Wire.begin();
  Serial.println(String(F("color sensor lib version: ")) +
                 emakefun::ColorSensorNlcs21::kVersionMajor + "." +
                 emakefun::ColorSensorNlcs21::kVersionMinor + "." +
                 emakefun::ColorSensorNlcs21::kVersionPatch);

  const auto ret = g_color_sensor.Initialize();

  if (emakefun::ColorSensorNlcs21::kOK == ret) {
    Serial.println(F("color sensor initialization successful"));
  } else {
    Serial.println(String(F("color sensor device initialization failed: ")) +
                   ret);
    INFINITE_LOOP_ON_FAILURE;
  }

  Serial.println(F("setup successful"));
  g_color_sensor.SetThreshold(THRESHOLD_LOW,
                              THRESHOLD_HIGH);  // 设置灯的亮度阈值
}

void loop() {
  // const auto color = g_color_sensor.GetColor();
  emakefun::ColorSensorNlcs21::Color color;
  const auto interrupt = g_color_sensor.GetInterruptStatus();
  if (g_color_sensor.GetColor(&color)) {
    Serial.println(String("r: ") + color.r + ", g: " + color.g +
                   ", b: " + color.b);
    Serial.println(String("W_DATA: ") + color.c);
    // Serial.println(String("interrupt: ") + interrupt);
    for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, color.r, color.g, color.b);  // 设置颜色为红色
    strip.show();                                       // 展示颜色
  }
  }
  // Serial.println(String("W_DATA: ") + color.c);
  // Serial.println(String("interrupt: ") + interrupt);

  // 清除中断
  //  if (interrupt) {
  //    Serial.println(F("interrupt occurred"));
  //    g_color_sensor.ClearInterrupt();  // 清除中断标志位
  //    delay(1000);
  //    Serial.println(F("interrupt cleared"));
  //  }
 
}
