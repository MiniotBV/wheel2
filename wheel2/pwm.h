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



void pwmNietLinearFase(float kracht, int pinP, int pinN, bool omgekeerd){
  
  // int fase = pow(abs(kracht), logWaarde) * PMAX;
  int fase = abs(kracht) * PMAX * procentWaarde;
  
  if(omgekeerd){
    if(kracht > 0){
      pwmWrite(pinP,  PMAX - fase );
      pwmWrite(pinN,  PMAX);
    }else{
      pwmWrite(pinP,  PMAX);
      pwmWrite(pinN,  PMAX - fase );
    }
  }else{
    if(kracht > 0){
      pwmWrite(pinP,  fase );
      pwmWrite(pinN,  0);
    }else{
      pwmWrite(pinP,  0);
      pwmWrite(pinN,  fase );
    }
  }
}




void pwmFaseDisable(int pinP, int pinN){
  pwmWrite(pinP,  0);
  pwmWrite(pinN,  0);
}





void pwmStapper(float hoek, int pinAP, int pinAN, int pinBP, int pinBN, bool omgekeerd){
  pwmFase( sin(hoek),   pinAP, pinAN, omgekeerd);
  pwmFase( cos(hoek),   pinBP, pinBN, omgekeerd);
}


void pwmDisableStapper(int pinAP, int pinAN, int pinBP, int pinBN){
  pwmFaseDisable( pinAP, pinAN );
  pwmFaseDisable( pinBP, pinBN );
}























#define antiCoggSampNum  2000
float antiCoggSampels[antiCoggSampNum + 1];
float antiCoggFases[antiCoggSampNum][2];

bool antiCoggAan = true;
bool antiCoggType = true;

// float antiCoggNul = 1.5;
// float antiCoggMacht = 3;
// float antiCoggVerschuiving = 1.78;

float antiCoggNul = 1.7;
float antiCoggmacht = 3.5;
float antiCoggVerschuiving = 1.78;

void berekenAntiCogging(){
  antiCoggSampels[0] = 0;

  for(int i = 0; i < antiCoggSampNum; i++){
    
    float hoek = ( i * (PI*4) ) / antiCoggSampNum;
    hoek +=  antiCoggVerschuiving * PI;

    float grafiek = 0;
    if(antiCoggType){
      grafiek = pow( antiCoggNul - abs( sin(hoek) ) , antiCoggmacht);//   /   pow(vanNul, macht);
    }else{
      grafiek = antiCoggNul - pow( abs( sin(hoek) ) , antiCoggmacht);//   /   pow(vanNul, macht);
    }
    
    antiCoggSampels[i+1] = antiCoggSampels[i] + grafiek;
  }

  float deelsom = antiCoggSampels[antiCoggSampNum] / (PI * 2);

  for(int i = 0; i < antiCoggSampNum; i++){
      antiCoggSampels[i] /= deelsom;
  }

  for(int i = 0; i < antiCoggSampNum; i++){
    antiCoggFases[i][0] = sin(antiCoggSampels[i]);
    antiCoggFases[i][1] = cos(antiCoggSampels[i]);
  }
}




float nieuweHoek, karSin, karCos;

void pwmStapperAntiCogging(float hoek, int pinAP, int pinAN, int pinBP, int pinBN, bool omgekeerd){
  
  float andereSchaal =  hoek + (PI * 1000);
  int indexHoek = andereSchaal * ((antiCoggSampNum - 1) / (PI*2));
  nieuweHoek = antiCoggSampels[indexHoek % antiCoggSampNum];

  karSin = antiCoggFases[indexHoek % antiCoggSampNum][0];
  karCos = antiCoggFases[indexHoek % antiCoggSampNum][1];

  pwmFase( karSin,   pinAP, pinAN, omgekeerd);
  pwmFase( karCos,   pinBP, pinBN, omgekeerd);

  // pwmStapper(nieuweHoek, pinAP, pinAN,  pinBP, pinBN,  omgekeerd);
}





