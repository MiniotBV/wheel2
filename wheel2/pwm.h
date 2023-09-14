#ifndef PWM_H
#define PWM_H

#define PWM_PMAX 4000

#include <Arduino.h>


void setPwm(int pin);
void pwmWriteFloat(int pin, float level);
void pwmWrite(int pin, int level);
void pwmPhase(float force, int pinP, int pinN, bool reversed = false);
void pwmStepper(float angle, int pinAP, int pinAN, int pinBP, int pinBN, bool reversed = false);
void pwmDisableStepper(int pinAP, int pinAN, int pinBP, int pinBN);
void pwmPhaseDisable(int pinP, int pinN);


#endif // PWM_H
