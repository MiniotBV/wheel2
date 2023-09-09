


enum staats{
  S_STOPPEN,
  S_PARKEREN,
  S_NAAR_HOK,
  S_HOK,
  
  S_HOMEN_VOOR_SPELEN,
  S_NAAR_BEGIN_PLAAT,
  S_UITROLLEN_VOOR_SPELEN,
  S_SPELEN,

  S_PAUZE,
  S_NAAR_NUMMER,
  S_DOOR_SPOELEN,
  S_TERUG_SPOELEN,

  S_SCHOONMAAK,
  S_CALIBREER,
  S_HOMEN_GEFAALD,
  S_FOUTE_ORIENTATIE,
  S_ERROR,
};


// enum staats staat = S_STOPPEN;
enum staats staat = S_HOK;

Interval staatVeranderd(1000, MILLIS);






String printStaat(int s){
  if( s == S_STOPPEN                  ){ return "STOPPEN";}
  if( s == S_PARKEREN                 ){ return "PARKEREN";}
  if( s == S_NAAR_HOK                 ){ return "NAAR_HOK";}
  if( s == S_HOK                      ){ return "HOK";}
    
  if( s == S_HOMEN_VOOR_SPELEN        ){ return "HOMEN_VOOR_SPELEN";}
  if( s == S_NAAR_BEGIN_PLAAT         ){ return "NAAR_BEGIN_PLAAT";}
  if( s == S_UITROLLEN_VOOR_SPELEN    ){ return "UITROLLEN_VOOR_SPELEN";}
  if( s == S_SPELEN                   ){ return "SPELEN";}
    
  if( s == S_PAUZE                    ){ return "PAUZE";}
  if( s == S_NAAR_NUMMER              ){ return "NAAR_NUMMER";}
  if( s == S_DOOR_SPOELEN             ){ return "DOOR_SPOELEN";}
  if( s == S_TERUG_SPOELEN            ){ return "TERUG_SPOELEN";}
    
  if( s == S_SCHOONMAAK               ){ return "SCHOONMAAK";}
  if( s == S_CALIBREER                ){ return "CALIBREER";}
  if( s == S_HOMEN_GEFAALD            ){ return "HOMEN_GEFAALD";}
  if( s == S_ERROR                    ){ return "ERROR";}
  if( s == S_FOUTE_ORIENTATIE         ){ return "FOUTE_ORIENTATIE";}
  
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
  // plaatBegin = 0; // om display leeg te maken
  setStaat(S_STOPPEN);
  plateauStoppen();  
}

void spelen(){
  setStaat(S_HOMEN_VOOR_SPELEN);
  plateauDraaien();
}








