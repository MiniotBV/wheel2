#ifndef I2C_H
#define I2C_H

#include <Arduino.h>

void setI2CPins();
char i2cRead(byte adress, byte reg);
int i2cWrite(byte adress, byte reg, byte data);
float readAccelerationAxis(byte adress, uint8_t reg);

#endif
