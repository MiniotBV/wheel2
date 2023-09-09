


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
  S_UITROLLEN_NA_SPOELEN,

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
  if( s == S_STOPPEN                  ){ return "S_STOPPEN";}
  if( s == S_PARKEREN                 ){ return "S_PARKEREN";}
  if( s == S_NAAR_HOK                 ){ return "S_NAAR_HOK";}
  if( s == S_HOK                      ){ return "S_HOK";}
    
  if( s == S_HOMEN_VOOR_SPELEN        ){ return "S_HOMEN_VOOR_SPELEN";}
  if( s == S_NAAR_BEGIN_PLAAT         ){ return "S_NAAR_BEGIN_PLAAT";}
  if( s == S_UITROLLEN_VOOR_SPELEN    ){ return "S_UITROLLEN_VOOR_SPELEN";}
  if( s == S_SPELEN                   ){ return "S_SPELEN";}
    
  if( s == S_PAUZE                    ){ return "S_PAUZE";}
  if( s == S_NAAR_NUMMER              ){ return "S_NAAR_NUMMER";}
  if( s == S_DOOR_SPOELEN             ){ return "S_DOOR_SPOELEN";}
  if( s == S_TERUG_SPOELEN            ){ return "S_TERUG_SPOELEN";}
  if( s == S_UITROLLEN_NA_SPOELEN     ){ return "S_UITROLLEN_NA_SPOELEN";}
    
  if( s == S_SCHOONMAAK               ){ return "S_SCHOONMAAK";}
  if( s == S_CALIBREER                ){ return "S_CALIBREER";}
  if( s == S_HOMEN_GEFAALD            ){ return "S_HOMEN_GEFAALD";}
  if( s == S_ERROR                    ){ return "S_ERROR";}
  if( s == S_FOUTE_ORIENTATIE         ){ return "S_FOUTE_ORIENTATIE";}
  
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








