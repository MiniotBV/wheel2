//motor heeft 48 stappen
//tandwiel heeft 12 tanden
//tandheugel heeft 1.5mm tand pitch 
//48 stappen / 12 tanden = 4 stappen per tand
//1.5mm / 4 stappen per tand = 0.375mm per stap

//PI = 2 stappen
// #include "plaatLees.h"

bool plaatAanwezig = false;

#define KAR_SNELHEID 0.02

#define KAR_BUITEN 97
#define KAR_HOK 1
#define KAR_EINDE_PLAAT 5
#define SENSOR_NAALT_OFFSET 7.5//mm

float mmPerStap = 1.5 / ( 48 / 12 );
float stap2mm = ( 2 / PI ) * mmPerStap;  // 0.238732414637843
float mm2stap = 1 / stap2mm;             // 4.188790204786391

float karP = 0.0001;//0.00025;

int stapperFaseA = 0;
int stapperFaseB = 0;

bool karMotorEnable = true;
float karMotorPos = 0;
float karMotorOffset = 0;
float karPos = 0;
float karTargetPos = 0;




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


void armHoekCalibreer(){
  armHoekOffset = armHoekSlow;
  Serial.print("ofset: ");
  Serial.println(armHoekOffset);
}











Interval armHoekInt(100, MICROS);
Interval karMotorInt(1000, MICROS);


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
        Serial.print("div; ");
        Serial.println(  (karMotorOffset - karMotorPos)  * stap2mm  );
        karMotorOffset = karMotorPos;

        karPos = 0;
        setStaat(S_HOK);
      }else{
        if(armHoek < -100){
          karPos -= KAR_SNELHEID/10;
        }else{
          karPos -= KAR_SNELHEID;
        }
      }
    }

    else if(staat == S_HOK){
      if(karPos < KAR_HOK){
        karPos += KAR_SNELHEID;
      }
    }

    else if(staat == S_BEGINNEN_SPELEN){
      if(karPos - SENSOR_NAALT_OFFSET < KAR_EINDE_PLAAT){
        karPos += KAR_SNELHEID;
      }else{
        setStaat(S_PLAAT_AANWEZIG);
      }

    }

    else if(staat == S_NAAR_BEGIN_PLAAT){
      if(karPos < KAR_BUITEN){
        karPos += KAR_SNELHEID;
      }else{
        setStaat(S_BEGIN_PLAAT);
      }

    }


    else if(staat == S_SPELEN){
      karPos += limieteerF( -karP * armHoek , -KAR_SNELHEID / 2, KAR_SNELHEID / 2);
      karPos = limieteerF( karPos, 0, KAR_BUITEN + 2);
      if(karPos <= KAR_EINDE_PLAAT){
        setStaat(S_STOPPEN);
      }
    }



    else if(staat == S_JOGGEN){
      karPos += limieteerF( karTargetPos - karPos , -KAR_SNELHEID, KAR_SNELHEID);
      // karPos = limieteerF( karPos, 0, KAR_BUITEN);

      if(karPos == karTargetPos){
        setStaat(S_PAUZE);
      }
    }






    karMotorPos = (karPos * mm2stap) + karMotorOffset;


    if(karMotorEnable){

      stapperFase( sin(karMotorPos),  stapperAP, stapperAN);
      stapperFase( cos(karMotorPos),  stapperBP, stapperBN);
    
    }else{
      pwmWrite(stapperAP,  0 );
      pwmWrite(stapperAN,  0 );
      
      pwmWrite(stapperBP,  0 );
      pwmWrite(stapperBN,  0 );
    }

  }
}













