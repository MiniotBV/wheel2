
bool plaatLeesGolven = false;



float plaatLeesRuw;
float plaatLeesRuwPrev;
float plaatLeesRuwDiv;
float plaatLeesAbsDiv;

float plaatLeesWaarde;
float plaatLeesWaardePrev;
float plaatLeesDiv;


float plaatLeesBuffer[2000][2];
int plaatLeesBufferTeller = 0;
int plaatLeesBufferLengte = 0;

bool knip;




float plaatLeesStroom = 10;
float plaatLedStroomP = 0.0006;
float plaatSensorTarget = 1500;









int volt2pwm(float volt){
	return (volt * PMAX) / 3.3;
}



void plaatLeesLedSetMilliAmp(float amp){
	amp /= 1000.0;
	pwmWrite(plaatLeesLed, volt2pwm(1 + (100 * amp)));  //100ohm + 1volt led drop
}

void plaatLeesLeduit(){
	pwmWrite(plaatLeesLed, 0);  //100ohm + 1volt led drop
}




void plaatLeesInit(){
	setPwm(plaatLeesLed);
	plaatLeesLedSetMilliAmp(0);//10mA
}



bool isPlaatAanwezig(){
	return plaatAanwezigGefilterd > 0.5;
}








// #define plaatKnipperInterval 50
#define plaatDetectieTreshold 150//200

void plaatDetectie(){
	
	plaatAanwezig = plaatLeesAbsDiv > plaatDetectieTreshold;//als er genoeg amplitude is is er een plaat

	plaatAanwezigGefilterd += (plaatAanwezig - plaatAanwezigGefilterd) / 10;


	if(!isPlaatAanwezig() &&  (staat == S_SPELEN  ||  staat == S_PAUZE)){   // is er nog een plaat aanwezig?
		Serial.println("plaat is verdrwedene??");
		stoppen();
		return;
	}
}









void clearNummers(){
  hoeveelNummers = 0;
  nummers[hoeveelNummers] = 1;
}



void zetNummersAlsEenSingletje(){
	hoeveelNummers = 1;
	nummers[1] = SINGLETJE_PLAAT_BEGIN;

	if(nummers[0] > SINGLETJE_PLAAT_BEGIN - 12){
		nummers[0] = 55;
		Serial.println("moest einde plaat aanpassen");
	}
}



void nieuwNummer(float pos){
	if(hoeveelNummers == 0){
		// Serial.println("einde plaat: " + String(pos));
	}else{
		float afstand = pos - nummers[hoeveelNummers-1];
		if(afstand < 2){return;}
		// Serial.println("track op: " + String(pos));
	}

	nummers[hoeveelNummers] = pos;
	hoeveelNummers++;  
}




#define MINIMALE_TRACK_AFSTAND 3//mm

int trackTresshold = 500;
bool trackOnderTresh = true;


void scannenVoorTracks(){
	
	if(sensorPos < PLAAT_EINDE + 2  ||  staat != S_NAAR_BEGIN_PLAAT){
    plaatLeesBufferTeller = 0;
		return;
	}

  float waarde = -plaatLeesDiv;

  plaatLeesBuffer[plaatLeesBufferTeller][0] = sensorPos; //   opslaan voor na check
  plaatLeesBuffer[plaatLeesBufferTeller][1] = waarde;
  plaatLeesBufferTeller++;

  if(waarde < trackTresshold / 2 &&   trackOnderTresh){  trackOnderTresh = false; }

	if(waarde > trackTresshold     &&  !trackOnderTresh){
		trackOnderTresh = true;
		nieuwNummer(sensorPos);
  }




	
}









void plaatLeesNaKijken(){
  plaatLeesBufferLengte = plaatLeesBufferTeller;
  plaatLeesBufferTeller = 0;


  //-----------------------------------------vind de grootste piek;
  float grootstePiek = 0; 

  for(int i = 0; i < plaatLeesBufferLengte; i++){
    if(plaatLeesBuffer[i][1] > grootstePiek){
      grootstePiek = plaatLeesBuffer[i][1]; 
    }
  }
  // Serial.println("grootste piek: " + String(grootstePiek));

  //------------------------------------------vind alle nummers;
  clearNummers();

  float nieuweTreshold = grootstePiek / 3;

  for(int i = 0; i < plaatLeesBufferLengte; i++){
    float waarde = plaatLeesBuffer[i][1];
    
    if(waarde < nieuweTreshold / 2 &&   trackOnderTresh){  trackOnderTresh = false; }

    if(waarde > nieuweTreshold     &&  !trackOnderTresh){
      trackOnderTresh = true;
      nieuwNummer(plaatLeesBuffer[i][0]);
    }
  }

}








Interval plaatLeesInt(10000, MICROS);

void plaatLeesFunc(){
	if(!plaatLeesInt.loop()){return;}
		
	if(staat == S_HOK){ // als de kar in hok is, plaat scanner uitzetten
		clearNummers();
		plaatLeesLeduit();
		return;
	}



	plaatLeesRuwPrev = plaatLeesRuw;
	plaatLeesRuw = analogRead(plaatLees);
	plaatLeesRuwDiv = plaatLeesRuw - plaatLeesRuwPrev;


	plaatLeesAbsDiv = abs(plaatLeesRuwDiv);

  plaatLeesWaarde = (plaatLeesAbsDiv - plaatSensorTarget);


	plaatDetectie();
	

	if(knip){
		plaatLeesLeduit();

		float ledStroomP = (plaatSensorTarget - plaatLeesAbsDiv) * plaatLedStroomP;
		plaatLeesStroom += limieteerF(ledStroomP,  -1, 1);

		plaatLeesStroom = limieteerF(plaatLeesStroom, 5, 30);

		plaatLeesDiv = plaatLeesWaarde - plaatLeesWaardePrev;
    plaatLeesWaardePrev = plaatLeesWaarde;

		scannenVoorTracks();//--------------------------------                   TRACKS LEZEN



	
	}else{
		plaatLeesLedSetMilliAmp(plaatLeesStroom);
	}


	knip = !knip; //toggle led





  if(plaatLeesGolven){
    Serial.print(plaatLeesWaarde);

    Serial.print(", ");
    Serial.print(plaatLeesAbsDiv);

        Serial.print(", ");
    Serial.print(plaatLeesDiv);


    Serial.print(", ");
    Serial.print(plaatLeesStroom * 100);


    Serial.println();
  }

}












