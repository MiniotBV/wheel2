#define KNOP_PLAY 6
#define KNOP_DOORSPOEL 5
#define KNOP_TERUGSPOEL 7


#define KNOP_LANG 700  //wat is een lange klik
#define KNOP_SUPER_LANG 3000  //een super lange klink voor naald stand
#define KNOP_BLINK 100 //hoelang blinkt de led


#define INGEDRUKT             1
#define LANG_INGEDRUKT        2
#define SUPER_LANG_INGEDRUKT  3
#define LOSGELATEN            0


int           knopIn[8];
int           knopStaat[8];
uint64_t      knopInterval[8];

Interval ledBlinkInterval(0, MILLIS);
Interval knopAlleInterval(0, MILLIS);
Interval volumeDisplayActie(0, MILLIS);
Interval rpmDisplayActie(0, MILLIS);


int potVal = 0;
int potValPrev = 0;

float riem = 0;
float riemPrev = 0;
float riemFilter;
float riemFilterPrev;

float riemDiv;


















void printKnoppen(){
	Serial.print("knoppen: ");
	for(int i = 0; i < 8; i++) { Serial.print(knopIn[i]);  Serial.print(' '); }
	Serial.println();
}

void ledBlink() { ledBlinkInterval.reset(); }

const char* knopNaam(int knop){
	if(knop == KNOP_PLAY) { return "play"; }
	if(knop == KNOP_DOORSPOEL) { return "doorspoel"; }
	if(knop == KNOP_TERUGSPOEL) { return "terugspoel"; }
  return "?";
}



bool knopDebug = false;

void knopLog(int knop, const char* actie){
	if(knopDebug){
		Serial.print(knopNaam(knop));
		Serial.println(actie);
	}
}




void getKnopData(){
		gpio_put(displayLATCH, 0);
		delayMicroseconds(1);
		gpio_put(displayLATCH, 1);

		for(int i = 0; i < 8; i++){
			
			delayMicroseconds(1);
			knopIn[i] = digitalRead(displayUIT);

			gpio_put(displayKLOK, 1);
			delayMicroseconds(1);
			gpio_put(displayKLOK, 0);
		}
}









int gecompenseerdeDoorspoel(){
	if(orientatie.isStaand){
		return KNOP_TERUGSPOEL;
	}
	return KNOP_DOORSPOEL;
}

int gecompenseerdeTerugspoel(){
	if(orientatie.isStaand){
		return KNOP_DOORSPOEL;
	}
	return KNOP_TERUGSPOEL;
}

bool isKnopDoorspoel(int knop){
	return knop == gecompenseerdeDoorspoel();
}

bool isKnopTerugspoel(int knop){
	return knop == gecompenseerdeTerugspoel();
}




void knopLogica(int knop){
	if( knopStaat[knop] == LOSGELATEN   &&    knopIn[knop] == INGEDRUKT ){//-------------------------------------------------------------KORT INGEDRUKT
		knopStaat[knop] = INGEDRUKT;
		
		knopAlleInterval.reset();
		knopInterval[knop] = nieuweMillis();

		ledBlink();  //led blink
		
		knopLog( knop, " in ");

    if(knop == KNOP_PLAY){
      if( staat == S_HOMEN_VOOR_SPELEN  ||  staat == S_NAAR_BEGIN_PLAAT){
        puristenMode = true;
        Serial.println("puristen modus aan");
      }

      if( staat == S_HOK ){
        spelen();
        knopStaat[knop] = LANG_INGEDRUKT; // zodat de lange druk hem niet kan stoppen
      }
    }

		
		if( staat == S_NAALD_SCHOONMAAK || staat == S_PLAAT_SCHOONMAAK ){//   SCHOONMAAK STAND STOPPEN
			stoppen();
		}
		return;
	}
	
	
  //----------------------------------------------------KORT LOSGELATEN
	if( knopStaat[knop] == INGEDRUKT    &&    knopIn[knop] == LOSGELATEN )
  {
		knopStaat[knop] = LOSGELATEN;
		knopAlleInterval.reset();
		knopLog( knop, " los ");
		
		if(isKnopDoorspoel(knop)){
			if((staat == S_SPELEN || staat == S_PAUZE || staat == S_NAAR_NUMMER)){
				naarVolgendNummer();
			}
		}

		if(isKnopTerugspoel(knop)){
			if((staat == S_SPELEN || staat == S_PAUZE || staat == S_NAAR_NUMMER)){
				naarVorrigNummer();
			}
		}

		if(knop == KNOP_PLAY){
			if(staat == S_PAUZE || staat == S_SPELEN) { pauze(); }
      // else if(staat == S_HOK) { spelen(); }
		}


		if(staat == S_HOK   &&   knop == KNOP_TERUGSPOEL){ //-----rpm
    
			if(rpmStaat == AUTO) { rpmStaat = R33; }
      else if(rpmStaat == R33) { rpmStaat = R45; }
      else { rpmStaat = AUTO; }

      updatePlateauRpm();
      rpmDisplayActie.reset();
		}


    // if(staat == S_HOK   &&   knop == KNOP_DOORSPOEL){ //-----bluetooth knop
		// 	bluetoothScrijf("AT+DELVMLINK");
		// }


		return;
	}
	
	
	
	
	if( knopStaat[knop] == INGEDRUKT    &&    nieuweMillis() - knopInterval[knop]  >  KNOP_LANG ){//------------------------------------------LANG INGEDRUKT
		knopStaat[knop] = LANG_INGEDRUKT;
		knopAlleInterval.reset();

		knopLog( knop," lang ");


		
		
		if( (staat == S_SPELEN  ||  staat == S_PAUZE  ||  staat == S_NAAR_NUMMER)  &&   isKnopDoorspoel(knop)){ //      >> DOOR SPOELEN
			setStaat( S_DOOR_SPOELEN );
		}

		if( (staat == S_SPELEN  ||  staat == S_PAUZE  ||  staat == S_NAAR_NUMMER)   &&   isKnopTerugspoel(knop)){//      << TERUG SPOELEN
			setStaat( S_TERUG_SPOELEN );
		}

		



		if(knop == KNOP_PLAY){
			if(staat != S_HOK){
        stoppen();
				// spelen();
			}
		}

    // if(staat == S_HOK   &&   knop == KNOP_DOORSPOEL){ //-----bluetooth reset
		// 	bluetoothScrijf("AT+REST");
		// }
		return;
	}
	
	
	
	//------------------------------------LANG LOSGELATEN
	if( knopStaat[knop] == LANG_INGEDRUKT && knopIn[knop] == LOSGELATEN )
  {
		knopStaat[knop] = LOSGELATEN;
		knopAlleInterval.reset();
		
		knopLog(  knop," lang-los ");



		if( (staat == S_DOOR_SPOELEN || staat == S_TERUG_SPOELEN) &&
        (knop == KNOP_DOORSPOEL  ||  knop == KNOP_TERUGSPOEL  )  )
    { //                                                            WEER BEGINNEN NA SPOELEN
			targetNummerPos = karPos;
			setStaat(S_UITROLLEN_NA_SPOELEN);
		}
		return;
	}
	
	
	
	
	if( knopStaat[knop] == LANG_INGEDRUKT    &&    nieuweMillis() - knopInterval[knop]  >  KNOP_SUPER_LANG ){//--------------SUPER LANG INGEDRUKT
		knopStaat[knop] = SUPER_LANG_INGEDRUKT;
		knopAlleInterval.reset();   //led blink
		
		knopLog(  knop, " super-lang ");

		if(  knop == KNOP_PLAY  ){//               NAALD TEST STAND
			if(staat == S_HOMEN_VOOR_SPELEN ||  staat == S_NAAR_BEGIN_PLAAT ){
				herhaalDeHelePlaat = true;
			}
		}


		if(  staat == S_HOK  &&  knop == KNOP_TERUGSPOEL  ){//               NAALD TEST STAND
			schoonmaakStand();
      ledBlink();
		}
		return;
	}
	
	
	
	
	if( knopStaat[knop] == SUPER_LANG_INGEDRUKT  &&    knopIn[knop] == LOSGELATEN ){//-------------------------------------------------SUPER LANG LOSGELATEN
		knopStaat[knop] = LOSGELATEN;
		knopAlleInterval.reset();  //led blink
		
		knopLog(  knop, " super-lang-los ");
		
		
		if(  (  staat == S_DOOR_SPOELEN   ||   staat == S_TERUG_SPOELEN  )  &&  (  knop == KNOP_DOORSPOEL  ||  knop == KNOP_TERUGSPOEL  )  ){//WEER BEGINNEN NA SPOELEN
			targetNummerPos = karPos;
			setStaat(S_UITROLLEN_NA_SPOELEN);
		}
		return;
	}
	
	
	
	
	if( knopIn[knop] == LOSGELATEN ){ //nog ff checken of de knop is losgelaten zodat en lange druk geen loop maakt
		knopStaat[knop] = LOSGELATEN;

		return;
	}
}












Interval knoppenInt(10000, MICROS);

void knoppenUpdate(){
	if(knoppenInt.loop()){

		getKnopData();


		for(int knop = 5; knop < 8; knop++){
			knopLogica(knop);
		}






		potVal = analogRead(displayPOTMETER);

		if(!isOngeveer(potVal, potValPrev, AMAX/2)){
			if(potVal > potValPrev){
				potValPrev += AMAX;
			}else{
				potValPrev -= AMAX;        
			}
		}

		riem    +=     (float(potVal - potValPrev) * 80 ) / AMAX;
		potValPrev = potVal;
		
		riemFilter += (riem - riemFilter)/3;




		if( !isOngeveer(riemFilter, riemFilterPrev, 1) ){
			riemDiv = riemFilter - riemFilterPrev;
			riemFilterPrev = riemFilter;
			
      if(nieuweMillis() < 1000){return;}//de band doet pas wat na 1seconden


			

			if(staat == S_NAALD_SCHOONMAAK  &&  arm.armMotorAan){
				arm.targetGewicht += riemDiv * 0.0333;
				arm.targetGewicht = limieteerF(arm.targetGewicht,   MIN_GEWICHT,   MAX_GEWICHT);
			}




      if(!orientatie.isStaand){
				riemDiv = -riemDiv; // flip
			}



			if(staat == S_CALIBREER){
				arm.kracht += riemDiv * 0.001;
				arm.kracht = limieteerF(arm.kracht, 0, 1);
				
			}

			else if(staat == S_PAUZE){
				targetNummerPos -= riemDiv * 0.25;
				targetNummerPos = limieteerF(targetNummerPos, PLAAT_EINDE, plaatBegin);      
			}
		
			else{
				if(staat != S_DOOR_SPOELEN   &&  staat != S_TERUG_SPOELEN   &&  staat != S_NAAR_NUMMER   &&  staat != S_PAUZE){ //zodat niet volume oppopt na een knop indruk met een doorspoel ofzo
					volumeDisplayActie.reset();
				}
				
				volume += round(riemDiv);
				volume = limieteerI(volume, 0, 63);        
			}
		}
		






	}
}

























