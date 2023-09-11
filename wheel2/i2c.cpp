#include "log.h"
#include "i2c.h"
#include <Wire.h>
#include "pins.h"

void setI2CPins() {
  LOG_DEBUG("i2c.cpp", "[setI2CPins]");
  // Set i2c pins
  Wire1.setSDA(I2C_SDA_PIN);
  Wire1.setSCL(I2C_SCL_PIN);
} // setI2CPins()

char i2cRead(byte adress, byte reg) {
  int err = 0;

  Wire1.begin();
  Wire1.beginTransmission(adress);
  Wire1.write(reg);
  err = Wire1.endTransmission(false);
  if (err) {
    return -111;
  }
  if (Wire1.requestFrom(adress, 1) <= 0) {
    return -111;
  }
  return Wire1.read();
} // i2cRead()

int i2cWrite(byte adress, byte reg, byte data) {
  Wire1.begin();
  Wire1.beginTransmission(adress);
  Wire1.write(reg);
  Wire1.write(data);

  // if (Wire1.endTransmission()) {
  //   Serial.print("err@: ");
  //   Serial.print(adress);
  //   Serial.print(" er:");
  //   Serial.println(Wire1.endTransmission());
  // }
  return Wire1.endTransmission();
} // i2cWrite()

float readAccelerationAxis(byte adress, uint8_t reg) {
  Wire1.begin();
  Wire1.beginTransmission(adress);
  Wire1.write(reg);
  Wire1.endTransmission(false);
  Wire1.requestFrom(adress, 2);
  int16_t data = (Wire1.read() & 0b11111100) | (Wire1.read() << 8); // dump into a 16 bit signed int, so the sign is correct
  data = data / 4;                                                  // divide the result by 4 to maintain the sign, since the data is 14 bits

  float buf = data / 4096.0;
  return buf;
} // readAccelerationAxis()
