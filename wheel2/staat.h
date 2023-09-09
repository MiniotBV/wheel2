

bool herhaalDeHelePlaat = false;


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

  S_HOMEN_VOOR_SCHOONMAAK,  
  S_SCHOONMAAK,
  S_CALIBREER,
  S_HOMEN_GEFAALD,
  S_FOUTE_ORIENTATIE,
  S_ERROR,
};


// enum staats staat = S_STOPPEN;
enum staats staat = S_HOK;

Interval staatVeranderd(1000, MILLIS);

bool eersteKeerSindsStaatsVerandering = false;




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
    
  if( s == S_HOMEN_VOOR_SCHOONMAAK    ){ return "S_HOMEN_VOOR_SCHOONMAAK";}
  if( s == S_SCHOONMAAK               ){ return "S_SCHOONMAAK";}
  if( s == S_CALIBREER                ){ return "S_CALIBREER";}
  if( s == S_HOMEN_GEFAALD            ){ return "S_HOMEN_GEFAALD";}
  if( s == S_ERROR                    ){ return "S_ERROR";}
  if( s == S_FOUTE_ORIENTATIE         ){ return "S_FOUTE_ORIENTATIE";}
  
  return "??????";
}




void setStaat(enum staats s){
  staatVeranderd.reset();
  eersteKeerSindsStaatsVerandering = true;
  
  Serial.println(printStaat(staat) + " > " + printStaat(s));

  staat = s; // set staat
}




bool eenKeerSindsStaatsVerandering(){
  if(eersteKeerSindsStaatsVerandering){
    eersteKeerSindsStaatsVerandering = false;
    return true;
  }
  return false;
}




void stoppen(){
  // plaatBegin = 0; // om display leeg te maken
  if(staat == S_HOMEN_VOOR_SPELEN){ // omdat anders kan je een error triggeren als je precies tijdens de home bump stoppt jatoch
    staat = S_NAAR_HOK; // iet wat extreem , maar anders komen de staats verandering timers mee
  }else{
    setStaat(S_STOPPEN);
  }
  plateauStoppen();  
}

void spelen(){
  setStaat(S_HOMEN_VOOR_SPELEN); // eerst ff home
  plateauDraaien();
}

void schoonmaakStand(){
  setStaat(S_HOMEN_VOOR_SCHOONMAAK); // eerst ff home
}



void stoppenOfHerhalen(){
  if(herhaalDeHelePlaat){
    gaNaarNummer(plaatBegin);    
  }else{
    stoppen();
  }
}












enum errors{
  E_GEEN = 0,
  
  E_NAALD_TERUG_GELOPEN = 2,
  E_NAALD_NIET_BEWOGEN = 3,

  E_PLAAT_NIET_OPGANG = 4,

  E_KON_NIET_HOMEN = 5,

  E_ARMHOEK_LIMIET_POS = 6,//
  E_ARMHOEK_LIMIET_NEG = 7,//

  E_TE_GROTE_UITSLAG = 8,
  
};

enum errors error = E_GEEN;

Interval errorVeranderd(0, MILLIS);


String printError(enum errors s){
  if( s == E_GEEN                   ){ return "E_GEEN";}

  if( s == E_NAALD_TERUG_GELOPEN    ){ return "E_NAALD_TERUG_GELOPEN";}
  if( s == E_NAALD_NIET_BEWOGEN     ){ return "E_NAALD_NIET_BEWOGEN";}
  if( s == E_PLAAT_NIET_OPGANG      ){ return "E_PLAAT_NIET_OPGANG";}
  if( s == E_KON_NIET_HOMEN         ){ return "E_KON_NIET_HOMEN";}
  if( s == E_ARMHOEK_LIMIET_POS     ){ return "E_ARMHOEK_LIMIET_POS";}
  if( s == E_ARMHOEK_LIMIET_NEG     ){ return "E_ARMHOEK_LIMIET_NEG";}
  if( s == E_TE_GROTE_UITSLAG       ){ return "E_TE_GROTE_UITSLAG";}

  return "??????";
}


void setError(enum errors err){
  error = err;
  errorVeranderd.reset();
  Serial.println("error: " + printError(err));
}







