#include <sys/_stdint.h>
#ifndef HELPER_H
#define HELPER_H

#include <Arduino.h>
#include "enums.h"


String padRight(String value, int length, char character = ' ');
int roundTrip(int n, int max);
float limitFloat(float n, float min, float max);
float mapFloat(float n, float nMin, float nMax, float min, float max);
int limitInt(int n, int min, int max);
bool isApprox(float value, float compare, float margin);
String getState(eStates state);
String getRpmState(eRpmMode rpm);
String getError(eErrors error);
void setBit(uint8_t *byte, uint8_t n, bool value);
bool getBit(uint8_t byte, uint8_t n);
uint64_t millisSinceBoot();


#endif // HELPER_H
