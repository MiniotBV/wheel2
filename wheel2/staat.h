float nummers[20];// = {0.2, 0.3, 0.6, 0.68, 0.85}; //staat nu in staat.h
int hoeveelNummers = 0;


enum staats{
  S_NAAR_HOK,
  S_HOK,
  S_STOPPEN,

  S_BEGINNEN_SCHOONMAAK,
  S_SCHOONMAAK_BEWEGEN,
  S_SCHOONMAAK,





  S_HOMEN_VOOR_SPELEN,
  S_BEGINNEN_SPELEN,
  S_PLAAT_AANWEZIG,
  
  S_NAAR_BEGIN_PLAAT,
  // S_BEGIN_PLAAT,
  // S_SPELEN,
  S_PLAAT_DIAMETER_METEN,
  S_NAALD_EROP,


  S_PAUZE,



  S_VOLGEND_NUMMER,
  S_VORRIG_NUMMER,
  S_NAAR_NUMMER,

  S_DOOR_SPOELEN,
  S_TERUG_SPOELEN,


  S_ERROR,

  S_JOGGEN,
};


enum staats staat = S_STOPPEN;

Interval staatVeranderd(1000, MILLIS);






void printStaat(int s){
  if( s == S_NAAR_HOK             ){ Serial.print("NAAR_HOK");            return;}
  if( s == S_HOK                  ){ Serial.print("HOK");                 return;}
  if( s == S_STOPPEN              ){ Serial.print("STOPPEN");             return;}

  if( s == S_BEGINNEN_SCHOONMAAK  ){ Serial.print("BEGINNEN_SCHOONMAAK"); return;}
  if( s == S_SCHOONMAAK_BEWEGEN   ){ Serial.print("SCHOONMAAK_BEWEGEN");  return;}
  if( s == S_SCHOONMAAK           ){ Serial.print("SCHOONMAAK");          return;}



  if( s == S_BEGINNEN_SPELEN      ){ Serial.print("BEGINNEN_SPELEN");     return;}
  if( s == S_PLAAT_AANWEZIG       ){ Serial.print("PLAAT_AANWEZIG");      return;}
  if( s == S_NAAR_BEGIN_PLAAT     ){ Serial.print("NAAR_BEGIN_PLAAT");    return;}
  if( s == S_PLAAT_DIAMETER_METEN ){ Serial.print("PLAAT_DIAMETER_METEN");    return;}
  // if( s == S_BEGIN_PLAAT          ){ Serial.print("BEGIN_PLAAT");         return;}
  // if( s == S_SPELEN               ){ Serial.print("SPELEN");              return;}
  if( s == S_NAALD_EROP           ){ Serial.print("NAALD_EROP");          return;}


  if( s == S_PAUZE                ){ Serial.print("PAUZE");               return;}



  if( s == S_VOLGEND_NUMMER       ){ Serial.print("VOLGEND_NUMMER");      return;}
  if( s == S_VORRIG_NUMMER        ){ Serial.print("VORRIG_NUMMER");       return;}
  if( s == S_NAAR_NUMMER          ){ Serial.print("NAAR_NUMMER");         return;}

  if( s == S_DOOR_SPOELEN         ){ Serial.print("DOOR_SPOELEN");        return;}
  if( s == S_TERUG_SPOELEN        ){ Serial.print("TERUG_SPOELEN");       return;}



  if( s == S_ERROR                ){ Serial.print("ERROR");               return;}

  
  
  Serial.print("??????");

}










void setStaat(enum staats s){
  staatVeranderd.reset();
  // karRemmen = false;

  printStaat(staat); Serial.print(" > "); printStaat(s); Serial.println();

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



void naaldErop(){
  setStaat(S_NAALD_EROP);
}








Interval staatInt(10000, MICROS);


void staatFunc(){
  if(staatInt.loop()){





  }
}













