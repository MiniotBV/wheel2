//motor heeft 48 stappen
//tandwiel heeft 12 tanden
//tandheugel heeft 1.5mm tand pitch 
//48 stappen / 12 tanden = 4 stappen per tand
//1.5mm / 4 stappen per tand = 0.375mm per stap

//PI = 2 stappen
// #include "plaatLees.h"

int stapperFaseA = 0;
int stapperFaseB = 0;

bool karMotorEnable = true;
float motorPos = 0;
float motorOffset = 0;













void karInit(){
  setPwm(stapperAP);
  setPwm(stapperAN);
  setPwm(stapperBP);
  setPwm(stapperBN);
}







float plaatPos = 0;
float stappenPerRev = (12 / 12) * 620;
// float stappenPerRev = (12.0 / 8) * 620;



Interval karMotorInt(100, MICROS);


void karMotorFunc(){
  // if(karMotorInt.loop()){
    // motorPos = (micros()*PI*(48/12)*15)/1000000.0;
    if(golven){
      motorPos = (micros()*PI*(48/8)*4*2)/1000000.0;
    }
    
    // float stappen = (motorPos-motorOffset) / (PI*2);
    // plaatPos = (stappen * 4096) / stappenPerRev;

    

    if(karMotorEnable){

      pwmFase( sin(motorPos),  stapperAP, stapperAN, true);
      pwmFase( cos(motorPos),  stapperBP, stapperBN, true);
    
    }else{

      pwmFase( 0,  stapperAP, stapperAN, true);
      pwmFase( 0,  stapperBP, stapperBN, true);
    }

  // }
}













