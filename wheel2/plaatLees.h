



float nummers[20] = {0.2, 0.3, 0.6, 0.68, 0.85};
int hoeveelNummers = 5;


float plaatLeesRuw;
float plaatLeesRuwOud;
float plaatLeesDiv;
float plaatLeesGefilterd;
float plaatLeesGefilterdBodem;




// bool plaatAanwezig = false;



unsigned int laatsteKnipperMeet;
unsigned int laatsteKnipperMeetLengte;
bool laatsteKnipperRichting;
bool knip;


int trackTresshold = 3200;
bool trackOnderTresh = true;
int minimumTrackSpacing = 0.05;


float karPos2trackPos(float kp){
  return mapF(kp, KAR_EINDE_PLAAT + SENSOR_NAALT_OFFSET,  KAR_BUITEN, 0, 1);
}

float trackPos2karPos(float tp){
  return mapF(tp, 0, 1, KAR_EINDE_PLAAT + SENSOR_NAALT_OFFSET, KAR_BUITEN);
}



void plaatLeesLedSetMilliAmp(float amp){
  amp /= 1000.0;
  pwmWrite(plaatLeesLed, volt2pwm(1 + (100 * amp)));  //100ohm + 1volt led drop
}




void plaatLeesInit(){
  setPwm(plaatLeesLed);
  plaatLeesLedSetMilliAmp(0);//10mA

  // pinMode(plaatLees, INPUT_PULLUP);
}




#define plaatKnipperInterval 100

void plaatDetectie(){
  knip =  ( millis()/plaatKnipperInterval ) % 2;
  if(knip){
    plaatLeesLedSetMilliAmp(20);
  }else{
    plaatLeesLedSetMilliAmp(0);
  }


  if(  plaatLeesRuw  >  plaatLeesGefilterd     ^   laatsteKnipperRichting ){ //is er een knipper zichtbaar
    laatsteKnipperRichting = !laatsteKnipperRichting;

    laatsteKnipperMeetLengte = millis() - laatsteKnipperMeet;   //bewaar de tijd van de flank
    laatsteKnipperMeet = millis();
  }
    

  
  plaatAanwezig = 
        (millis() - laatsteKnipperMeet) < plaatKnipperInterval* 1.1    
  &&    isOngeveer(laatsteKnipperMeetLengte,  plaatKnipperInterval,  plaatKnipperInterval*0.1);
}






















Interval plaatLeesInt(10000, MICROS);

void plaatLeesFunc(){
  if(plaatLeesInt.loop()){
    
    plaatLeesRuw = analogRead(plaatLees);
    
    plaatLeesDiv = plaatLeesRuwOud - plaatLeesRuw;
    plaatLeesRuwOud = plaatLeesRuw;

    plaatLeesGefilterd += (plaatLeesRuw - plaatLeesGefilterd) / 10;


    float sensorPos = karPos - SENSOR_NAALT_OFFSET;


    if(staat == S_HOK){
      hoeveelNummers = 0;
      nummers[hoeveelNummers] = 1;
    }
    

    else if(staat == S_NAAR_BEGIN_PLAAT){//--------------------------------                   TRACKS LEZEN

      plaatLeesLedSetMilliAmp(20);
      
      if(sensorPos > KAR_EINDE_PLAAT){
        
        if(plaatLeesRuw < trackTresshold && trackOnderTresh){
          trackOnderTresh = false;
        }

        if(plaatLeesRuw > trackTresshold && !trackOnderTresh){
          trackOnderTresh = true;

          Serial.print("nummer: ");
          Serial.print(sensorPos);
          Serial.print(" = ");
          Serial.println(  karPos2trackPos(sensorPos) );

          nummers[hoeveelNummers] = karPos2trackPos(sensorPos);
          hoeveelNummers++;
        }
      }
    }



    else{//---------------------------------------------------------------                    PLAAT DETECTIE
      
      

      if(knip){
        plaatLeesGefilterdBodem += (plaatLeesRuw - plaatLeesGefilterdBodem) / 20;
      }
      

      plaatDetectie();

      if(staat == S_PLAAT_AANWEZIG){
        if(staatsVeranderingInterval() > 300){
          if(plaatAanwezig){
            setStaat(S_NAAR_BEGIN_PLAAT);
          }else{
            setStaat(S_STOPPEN);
          }
        }
      }
    }


  }
}









