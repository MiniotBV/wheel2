#define PMAX 4000

float procentWaarde = 0.5;
float dollarWaarde = 0;
float hashtagWaarde = 0;

float faseA, faseB;


void setPwm(int pin){
	gpio_set_function(pin, GPIO_FUNC_PWM);
	pwm_set_enabled(  pwm_gpio_to_slice_num(pin),  true);
	pwm_set_wrap(  pwm_gpio_to_slice_num(pin),   PMAX + 1);
}

void pwmWrite(int pin, int level){
	pwm_set_chan_level(  pwm_gpio_to_slice_num(pin),  pwm_gpio_to_channel(pin), level);
}


void pwmWriteF(int pin, float level){
	pwmWrite(pin, level * PMAX);
}


// void pwmFase(float kracht, int pinP, int pinN, bool omgekeerd){
  
//   if(kracht == 0){
//     pwmWrite(pinP,  0);
//     pwmWrite(pinN,  0);
//     return;
//   }
  
//   int fase = kracht * PMAX;
  
//   if(omgekeerd){
//     if(fase > 0){
//       pwmWrite(pinP,  PMAX - fase );
//       pwmWrite(pinN,  PMAX);
//     }else{
//       pwmWrite(pinP,  PMAX);
//       pwmWrite(pinN,  PMAX + fase );
//     }
//   }else{
//     if(fase > 0){
//       pwmWrite(pinP,  fase );
//       pwmWrite(pinN,  0);
//     }else{
//       pwmWrite(pinP,  0);
//       pwmWrite(pinN,  fase - PMAX );
//     }
//   }




// }





void pwmFase(float kracht, int pinP, int pinN, bool omgekeerd){
	
	int fase = kracht * PMAX;
	
	if(omgekeerd){
		if(fase > 0){
			pwmWrite(pinP,  PMAX - fase );
			pwmWrite(pinN,  PMAX);
		}else{
			pwmWrite(pinP,  PMAX);
			pwmWrite(pinN,  PMAX + fase );
		}
	}else{
		if(fase > 0){
			pwmWrite(pinP,  abs(fase) );
			pwmWrite(pinN,  0);
		}else{
			pwmWrite(pinP,  0);
			pwmWrite(pinN,  abs(fase) );
		}
	}
}

















void pwmStapper(float hoek, int pinAP, int pinAN, int pinBP, int pinBN, bool omgekeerd){
	pwmFase( sin(hoek),   pinAP, pinAN, omgekeerd);
	pwmFase( cos(hoek),   pinBP, pinBN, omgekeerd);
}

