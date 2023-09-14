#include "log.h"
#include "pwm.h"
#include "hardware/pwm.h"


void setPwm(int pin) {
  LOG_DEBUG("pwm.cpp", "[setPwm]");
  gpio_set_function(pin, GPIO_FUNC_PWM);
  pwm_set_enabled(pwm_gpio_to_slice_num(pin), true);
  pwm_set_wrap(pwm_gpio_to_slice_num(pin), PWM_PMAX + 1);
} // setPwm()


void pwmWriteFloat(int pin, float level) {
  pwmWrite(pin, level * PWM_PMAX);
} // pwmWriteFloat()


void pwmWrite(int pin, int level) {
  pwm_set_chan_level(pwm_gpio_to_slice_num(pin), pwm_gpio_to_channel(pin), level);
} // pwmWrite()


void pwmPhase(float force, int pinP, int pinN, bool reversed) {
  // LOG_DEBUG("pwm.cpp", "[pwmPhase]");
  int phase = force * PWM_PMAX;
  if (reversed) {
    if (phase > 0) {
      pwmWrite(pinP, PWM_PMAX - phase);
      pwmWrite(pinN, PWM_PMAX);
    } else {
      pwmWrite(pinP, PWM_PMAX);
      pwmWrite(pinN, PWM_PMAX + phase);
    }
  } else {
    if (phase > 0) {
      pwmWrite(pinP, abs(phase));
      pwmWrite(pinN, 0);
    } else {
      pwmWrite(pinP, 0);
      pwmWrite(pinN, abs(phase));
    }
  }
} // pwmPhase()


void pwmStepper(float angle, int pinAP, int pinAN, int pinBP, int pinBN, bool reversed) {
  pwmPhase(sin(angle), pinAP, pinAN, reversed);
  pwmPhase(cos(angle), pinBP, pinBN, reversed);
} // pwmStepper()


void pwmDisableStepper(int pinAP, int pinAN, int pinBP, int pinBN) {
  pwmPhaseDisable(pinAP, pinAN);
  pwmPhaseDisable(pinBP, pinBN);
} // pwmDisableStepper()


void pwmPhaseDisable(int pinP, int pinN) {
  pwmWrite(pinP, 0);
  pwmWrite(pinN, 0);
} // pwmPhaseDisable()
