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
float armHoekFilter = 2;




void karInit(){
  setPwm(stapperAP);
  setPwm(stapperAN);
  setPwm(stapperBP);
  setPwm(stapperBN);
}





void stapperFase(float kracht, int pinP, int pinN){
  int fase = kracht * (PMAX-1);
  
  if(fase > 0){
    pwmWrite(pinP,  (PMAX-1) - fase );
    pwmWrite(pinN,  (PMAX-1));
  }else{
    pwmWrite(pinP,  (PMAX-1));
    pwmWrite(pinN,  (PMAX-1) + fase );
  }

  // if(fase > 0){
  //   pwmWrite(pinP,  fase );
  //   pwmWrite(pinN,  0);
  // }else{
  //   pwmWrite(pinP,  0);
  //   pwmWrite(pinN,  fase - (PMAX-1) );
  // }

}







INTERVAL armHoekInt(100, MICROS);
INTERVAL karMotorInt(200, MICROS);


void karMotorFunc(){
  if(armHoekInt.loop()){

    // armHoek = analogRead(hoekSensor);
    armHoek += (analogRead(hoekSensor) - armHoek) / armHoekFilter;

  }



    
      
  if(karMotorInt.loop()){

    // karMotorPositie += 0.001;

    karMotorPositie = 200 * sin(micros()/1000000.0);

    // karMotorPositie += limieteerF( -karP * ( analogRead(hoekSensor) - 1890 ) , -0.02, 0.02);
    // karMotorPositie += limieteerF( -karP * ( armHoek - 1890 ) , -0.02, 0.02);

    karPositie = karMotorPositie * karPositieVerhouding;

    // stapperFaseA = sin(karMotorPositie) * (PMAX-1);
    // stapperFaseB = cos(karMotorPositie) * (PMAX-1);

    if(karMotorEnable){

      stapperFase( sin(karMotorPositie),  stapperAP, stapperAN);
      stapperFase( cos(karMotorPositie),  stapperBP, stapperBN);
      
      // if(stapperFaseA > 0){
      //   pwmWrite(stapperAP,  (PMAX-1) - stapperFaseA );
      //   pwmWrite(stapperAN,  (PMAX-1));
      // }else{
      //   pwmWrite(stapperAP,  (PMAX-1));
      //   pwmWrite(stapperAN,  (PMAX-1) + stapperFaseA );
      // }

      // if(stapperFaseB > 0){
      //   pwmWrite(stapperBP,  (PMAX-1) - stapperFaseB );
      //   pwmWrite(stapperBN,  (PMAX-1));
      // }else{
      //   pwmWrite(stapperBP,  (PMAX-1));
      //   pwmWrite(stapperBN,  (PMAX-1) + stapperFaseB );
      // }
    
    }else{
      pwmWrite(stapperAP,  0 );
      pwmWrite(stapperAN,  0 );
      
      pwmWrite(stapperBP,  0 );
      pwmWrite(stapperBN,  0 );
    }

  }
}






