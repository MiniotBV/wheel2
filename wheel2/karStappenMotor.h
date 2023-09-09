//motor heeft 48 stappen
//tandwiel heeft 12 tanden
//tandheugel heeft 1.5mm tand pitch 
//48 stappen / 12 tanden = 4 stappen per tand
//1.5mm / 4 stappen per tand = 0.375mm per stap

//PI = 2 stappen

float mmPerStap = 1.5 / ( 48 / 12 );
float karPositieVerhouding = ( 2 / PI ) * mmPerStap;

float karP = 0.001;

int stapperFaseA = 0;
int stapperFaseB = 0;

bool karMotorEnable = true;
float karMotorPositie = 0;
float karPositie = 0;




float armHoek = analogRead(hoekSensor);
float armHoekFilterWaarde = 2;
float armHoekSlow = armHoek;
float armHoekSlowFilterWaarde = 1000;
float armHoekOffset = 1890;




void karInit(){
  setPwm(stapperAP);
  setPwm(stapperAN);
  setPwm(stapperBP);
  setPwm(stapperBN);
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







INTERVAL armHoekInt(100, MICROS);
INTERVAL karMotorInt(500, MICROS);


void karMotorFunc(){
  if(armHoekInt.loop()){

    // armHoek = analogRead(hoekSensor);
    armHoek += (analogRead(hoekSensor) - armHoek) / armHoekFilterWaarde;
    armHoekSlow += (armHoek - armHoekSlow) / armHoekSlowFilterWaarde;
  }



    
      
  if(karMotorInt.loop()){

    // karMotorPositie += 0.001;

    // karMotorPositie = 40 * sin(micros()/1000000.0);
    // karMotorPositie += sin(micros()/1000000.0) * 0.01;

    // karMotorPositie += limieteerF( -karP * ( analogRead(hoekSensor) - 1890 ) , -0.02, 0.02);
    karMotorPositie += limieteerF( -karP * ( armHoek - armHoekOffset ) , -0.02, 0.02);

    karPositie = karMotorPositie * karPositieVerhouding;


    if(karMotorEnable){

      stapperFase( sin(karMotorPositie),  stapperAP, stapperAN);
      stapperFase( cos(karMotorPositie),  stapperBP, stapperBN);
    
    }else{
      pwmWrite(stapperAP,  0 );
      pwmWrite(stapperAN,  0 );
      
      pwmWrite(stapperBP,  0 );
      pwmWrite(stapperBN,  0 );
    }

  }
}






