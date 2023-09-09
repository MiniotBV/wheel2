#define PMAX 4000

void setPwm(int pin){
  gpio_set_function(pin, GPIO_FUNC_PWM);
  pwm_set_enabled(  pwm_gpio_to_slice_num(pin),  true);
  pwm_set_wrap(  pwm_gpio_to_slice_num(pin),   PMAX + 1);
}

void pwmWrite(int pin, int level){
  pwm_set_chan_level(  pwm_gpio_to_slice_num(pin),  pwm_gpio_to_channel(pin), level);
}



void stapperFase(float kracht, int pinP, int pinN){
  int fase = kracht * PMAX;
  
  if(fase > 0){
    pwmWrite(pinP,  PMAX - fase );
    pwmWrite(pinN,  PMAX);
  }else{
    pwmWrite(pinP,  PMAX);
    pwmWrite(pinN,  PMAX + fase );
  }

  // if(fase > 0){
  //   pwmWrite(pinP,  fase );
  //   pwmWrite(pinN,  0);
  // }else{
  //   pwmWrite(pinP,  0);
  //   pwmWrite(pinN,  fase - PMAX );
  // }

}

