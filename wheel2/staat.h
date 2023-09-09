

float autoRpm;

bool plateauAan = false;

bool opsnelheid;
bool uitdraaien;

float basis = 0;

Interval draaienInterval(10, MILLIS);


enum rpmStaats{
  AUTO,
  R33,
  R45,
  R78
};


enum rpmStaats rpmStaat = AUTO;






void updatePlateauRpm(){
  if(!plateauAan){
    return;    
  }

  if(rpmStaat == AUTO){
    targetRpm = autoRpm;
    return;
  }
  if(rpmStaat == R33){
    targetRpm = rpm33;
    return;
  }
  if(rpmStaat == R45){
    targetRpm = rpm45;
    return;
  }
}





void setPlateauRpm(float rpm){
  autoRpm = rpm;

  updatePlateauRpm();
  updatePlateauPID();
}






void plateauDraaien(){
  plateauAan = true;
  setPlateauRpm(rpm33);

  basis = 0.0;
  
  draaienInterval.reset();
  
  Serial.println("plateauStart()");
}






void plateauStoppen(){
  plateauAan = false;

  draaienInterval.reset();
  uitdraaien = true;
  opsnelheid = false;
}















float nummers[20];// = {0.2, 0.3, 0.6, 0.68, 0.85}; //staat nu in staat.h
int hoeveelNummers = 0;























enum staats{
  S_NAAR_HOK,
  S_HOK,
  S_STOPPEN,

  S_SCHOONMAAK,

  S_HOMEN_GEFAALD,



  S_HOMEN_VOOR_SPELEN,
  S_BEGINNEN_SPELEN,
  S_PLAAT_AANWEZIG,
  
  S_NAAR_BEGIN_PLAAT,
  // S_BEGIN_PLAAT,
  S_SPELEN,
  S_PLAAT_DIAMETER_METEN,
  S_NAALD_EROP,


  S_PAUZE,



  // S_VOLGEND_NUMMER,
  // S_VORRIG_NUMMER,
  S_NAAR_NUMMER,
  S_DOOR_SPOELEN,
  S_TERUG_SPOELEN,
  // S_JOGGEN,


  S_FOUTE_ORIENTATIE,
  S_ERROR,
};


// enum staats staat = S_STOPPEN;
enum staats staat = S_HOK;

Interval staatVeranderd(1000, MILLIS);






String printStaat(int s){
  if( s == S_NAAR_HOK             ){ return "NAAR_HOK";}
  if( s == S_HOK                  ){ return "HOK";}
  if( s == S_STOPPEN              ){ return "STOPPEN";}

  if( s == S_SCHOONMAAK           ){ return "SCHOONMAAK";}

  if( s == S_HOMEN_GEFAALD        ){ return "HOMEN_GEFAALD";}


  if( s == S_BEGINNEN_SPELEN      ){ return "BEGINNEN_SPELEN";}
  if( s == S_HOMEN_VOOR_SPELEN    ){ return "HOMEN_VOOR_SPELEN";}
  if( s == S_PLAAT_AANWEZIG       ){ return "PLAAT_AANWEZIG";}
  if( s == S_NAAR_BEGIN_PLAAT     ){ return "NAAR_BEGIN_PLAAT";}
  if( s == S_PLAAT_DIAMETER_METEN ){ return "PLAAT_DIAMETER_METEN";}
  // if( s == S_BEGIN_PLAAT          ){ return "BEGIN_PLAAT";}
  if( s == S_SPELEN               ){ return "SPELEN";}
  if( s == S_NAALD_EROP           ){ return "NAALD_EROP";}


  if( s == S_PAUZE                ){ return "PAUZE";}



  // if( s == S_VOLGEND_NUMMER       ){ return "VOLGEND_NUMMER";}
  // if( s == S_VORRIG_NUMMER        ){ return "VORRIG_NUMMER";}
  if( s == S_NAAR_NUMMER          ){ return "NAAR_NUMMER";}

  if( s == S_DOOR_SPOELEN         ){ return "DOOR_SPOELEN";}
  if( s == S_TERUG_SPOELEN        ){ return "TERUG_SPOELEN";}



  if( s == S_ERROR                ){ return "ERROR";}
  if( s == S_FOUTE_ORIENTATIE     ){ return "FOUTE_ORIENTATIE";}
  
  
  return "??????";

}










void setStaat(enum staats s){
  staatVeranderd.reset();
  // karRemmen = false;
  Serial.println(printStaat(staat) + " > " + printStaat(s));
  // printStaat(staat); Serial.print(" > "); printStaat(s); Serial.println();

  staat = s; // set staat
}






void stoppen(){
  setStaat(S_STOPPEN);
  plateauStoppen();
}



void spelen(){
  setStaat(S_HOMEN_VOOR_SPELEN);
  plateauDraaien();
}



// void naaldErop(){
  // setStaat(S_NAALD_EROP);
// }















