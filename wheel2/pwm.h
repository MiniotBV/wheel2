#define PMAX 4000

void setPwm(int pin){
  gpio_set_function(pin, GPIO_FUNC_PWM);
  pwm_set_enabled(  pwm_gpio_to_slice_num(pin),  true);
  pwm_set_wrap(  pwm_gpio_to_slice_num(pin),   PMAX);
}

void pwmWrite(int pin, int level){
  pwm_set_chan_level(  pwm_gpio_to_slice_num(pin),  pwm_gpio_to_channel(pin), level);
}

