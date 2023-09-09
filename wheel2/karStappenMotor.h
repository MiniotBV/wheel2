//motor heeft 48 stappen
//tandwiel heeft 12 tanden
//tandheugel heeft 1.5mm tand pitch 
//48 stappen / 12 tanden = 4 stappen per tand
//1.5mm / 4 stappen per tand = 0.375mm per stap

//PI = 2 stappen

float mmPerStap = 1.5 / ( 48 / 12 );
float stap2mm = ( 2 / PI ) * mmPerStap;  // 0.238732414637843
float mm2stap = 1 / stap2mm;             // 4.188790204786391

float karP = 0.00025;

int stapperFaseA = 0;
int stapperFaseB = 0;

bool karMotorEnable = true;
float karMotorPositie = 0;
float karMotorOffset = 0;
float karPositie = 0;
float karTargetPositie = 0;




float armHoekRuw = analogRead(hoekSensor);
float armHoek;
float armHoekFilterWaarde = 1;
float armHoekSlow = armHoek;
float armHoekSlowFilterWaarde = 1000;
float armHoekOffset = 1920;




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
INTERVAL karMotorInt(1000, MICROS);


void karMotorFunc(){
  if(armHoekInt.loop()){

    // armHoek = analogRead(hoekSensor);
    armHoekRuw += (analogRead(hoekSensor) - armHoekRuw) / armHoekFilterWaarde;
    armHoekSlow += (armHoekRuw - armHoekSlow) / armHoekSlowFilterWaarde;

    armHoek = armHoekRuw - armHoekOffset;
  }



    
      
  if(karMotorInt.loop()){

    if(staat == S_NAAR_HOK){
      if(armHoek < -600){
        karMotorOffset = karMotorPositie;
        karPositie = 0;
        setStaat(S_HOK);
      }else{
        if(armHoek < -100){
          karPositie -= 0.001;
        }else{
          karPositie -= 0.02;
        }
      }
    }

    else if(staat == S_HOK){
      if(karPositie < 1){
        karPositie += 0.002;
      }
    }

    else if(staat == S_BEGINNEN_SPELEN){
      if(karPositie < 96){
        karPositie += 0.02;
      }else{

      }

    }


    else if(staat == S_SPELEN){
      karPositie += limieteerF( -karP * armHoek , -0.02, 0.02);
    }

    // karMotorPositie += 0.001;

    // karMotorPositie = 40 * sin(micros()/1000000.0);
    // karMotorPositie += sin(micros()/1000000.0) * 0.01;

    // karMotorPositie += limieteerF( -karP * ( analogRead(hoekSensor) - 1890 ) , -0.02, 0.02);
    
    // karMotorPositie = karMotorPositieRuw - karMotorOffset;
    // karPositie = karMotorPositie * stap2mm;


    karMotorPositie = (karPositie * mm2stap) - karMotorOffset;






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






