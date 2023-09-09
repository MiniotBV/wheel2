




#define displayLengte 120
float displayData[displayLengte];

int nummersTeller = 0;

unsigned int displayDelay = 0;

int lichtLevel;





void displayInit(){
	pinMode(displayIN      , OUTPUT);
	pinMode(displayUIT     , INPUT);
	pinMode(displayKLOK    , OUTPUT);
	pinMode(displayLATCH   , OUTPUT);

	

	pinMode(displayEN, OUTPUT);
	digitalWrite(displayEN, 1);

  // setPwm(displayEN);
	// pwmWriteF(displayEN, 0.99);

	pinMode(displayPOTMETER, INPUT);
}





int egtePos2displayPos(float pos){
	return mapF(pos, PLAAT_EINDE,  ELPEE_PLAAT_BEGIN, 0, displayLengte-1);
	// return mapF(pos, PLAAT_EINDE + SENSOR_OFFSET,  ELPEE_PLAAT_BEGIN, 0, displayLengte-1);
}






void displayPrint(float tijd){
	
	for(int i = 0; i < displayLengte; i++){
		
		int pix = i;
		pix = (pix + 7) - ((pix % 8) * 2);//flip byte
		
		
		gpio_put(displayIN, displayData[pix] >  tijd ? 1 : 0);
		delayMicroseconds(2);

		gpio_put(displayKLOK, 1);
		delayMicroseconds(2);
		gpio_put(displayKLOK, 0);
	}

}




void commitDisplay(){
	gpio_put(displayLATCH, 0);
  delayMicroseconds(2);
	gpio_put(displayLATCH, 1);
}




void flipDisplayData(){
	for(int i = 0; i < displayLengte/2; i++){
		float buff = displayData[i];
		displayData[i] = displayData[(displayLengte-1) - i];
		displayData[(displayLengte-1) - i] = buff;
	}
}




void displayClear(){
  for(int i = 0; i < displayLengte; i++){
    displayData[i] = 0;
  }    
}


void displayTekenBlok(int begin, int einde, float kleur){
  int start = min(begin, einde);
  int eind = max(begin, einde);

  // Use min and max to ensure the range of `i` is always between `0` and `displayLengte`.
  start = max(start, 0);
  eind = min(eind, displayLengte);

  for(int i = start; i < eind; i++){
    displayData[i] = kleur;
  }  
}



void displayTekenPunt(int pos, float kleur){
  if(pos < 0 || pos >= displayLengte){return;}
  displayData[pos] = kleur;  
}






Interval displayInt(10000, MICROS);

void displayUpdate(){
	if(displayInt.loop()){

		nummersTeller = 0;
		int naald = egtePos2displayPos(karPosFilter);//int naald = egtePos2displayPos(karPos);
		int target = egtePos2displayPos(targetNummerPos);
		int sensor = egtePos2displayPos(sensorPos);
		int sensorMaxBerijk = egtePos2displayPos(ELPEE_PLAAT_BEGIN - SENSOR_OFFSET)  +  3;
		int plaatGroote = egtePos2displayPos(plaatBegin);
		int dispHalf = displayLengte/2;

    displayClear();



		//--------------------------------------------------------------------INTRO
		if(millis()<4000){
      int pos = displayLengte - (millis()/10);
      int blokLengte = 10;
      int gatLengte = 3;

      int decimalen = 3;
      int versieDecimalen[decimalen] = {   (versie) % 10,     (versie / 10) % 10,       (versie / 100) % 10    };

      for (int i = 0; i < decimalen; i++) {
        int dec = versieDecimalen[i];

        if (dec == 0) {// 0 stipje
          int einde = pos + gatLengte;
          displayTekenBlok(pos, einde, 0.1);
          pos = einde + gatLengte;
        }

        while (dec > 0) {
          if (dec >= 5) {
            dec -= 5;
            int einde = pos + blokLengte * 2;// dikke streep voor 5
            displayTekenBlok(pos, einde, 0.1);
            pos = einde + gatLengte;
          }

          if (dec > 0) {
            dec -= 1;
            int einde = pos + blokLengte;
            displayTekenBlok(pos, einde, 0.1);
            pos = einde + gatLengte;
          }
        }

        pos += blokLengte; // tussen elk blokken een pauze
      }

    }





    //----------------------------------------------------------------ERROR WEERGEVEN
		else if(errorVeranderd.sinds() < 10000  &&  error != E_GEEN){ // 10seonden knipperen

			if((millis()%1000) < 800){// knipper
				
        int blokBreedte = 0.1 * displayLengte;
        int begin = dispHalf - (blokBreedte/2) * error;

        for(int i = 0; i < error; i++){
          displayTekenBlok(begin + 1, begin + blokBreedte - 1, 0.1);
          begin += blokBreedte;
        }
			}
		}



		//----------------------------------------------------------------SCHOONMAAK STAND
		else if(staat == S_NAALD_SCHOONMAAK && arm.armMotorAan){//}  ||  staat == S_HOMEN_VOOR_SCHOONMAAK){
			
			int volumePunt = mapF(arm.targetGewicht, 0, 4, 0, displayLengte) / 2.0;
      float verdeelPuntTeller = 0.5;//arm.targetGewicht / 2;

			for(int i = 0; i < displayLengte; i++){
				displayData[i] = 0;

				if(i > (dispHalf-1) - volumePunt   &&   i < dispHalf + volumePunt){
          displayData[i] = 0.1;
        }

				float verdeelPunt = mapF(verdeelPuntTeller, 0, 4,  (dispHalf-1) - volumePunt,  ((dispHalf-1) - volumePunt) + (displayLengte-1));
				if(i > verdeelPunt){
					verdeelPuntTeller += 0.5;
          if(verdeelPuntTeller >= 4){verdeelPuntTeller += 0.5;}

          displayData[i] = 0;         
				}


      }

      if(orientatie.isStaand){
        flipDisplayData();
      }
		}

	 
	 //----------------------------------------------------------------CALIBREER STAND
		else if(staat == S_CALIBREER){
			
			int volumePunt = mapF(arm.kracht, 0, 1, displayLengte - 1,   0);
      int krachtLaagPunt = mapF(arm.krachtLaag, 0, 1, displayLengte - 1,   0);
      int krachtHoogPunt = mapF(arm.krachtHoog, 0, 1, displayLengte - 1,   0);
      int armHoekPunt = mapF(armHoekCall, 1, -1, 0, displayLengte-1);

      displayTekenBlok(displayLengte, volumePunt, 0.1);
      displayTekenPunt(armHoekPunt, 0.9);
      displayTekenPunt(krachtLaagPunt, 0.9);
      displayTekenPunt(krachtHoogPunt, 0.9);
		}




		//-------------------------------------------------------------------------WATER PAS STAND
		else if(staat == S_FOUTE_ORIENTATIE){
			
      if((millis()%1000) < 800){
        displayTekenBlok(0, displayLengte/4, 0.1);
        displayTekenBlok(displayLengte - (displayLengte/4),   displayLengte, 0.1);
      }
      
      for(int i = 0; i < displayLengte; i++){
        float floatI = float(i) / displayLengte;

        if(isOngeveer(floatI, (-orientatie.y*3) + 0.5, 0.02)){ // belltje
					displayData[i] = 0.9;
				}
      }

		}


		

		//--------------------------------------------------------------RPM
		else if(rpmDisplayActie.sinds() < 2000){

			int blokken = 1; //rpmStaat == AUTO

			if(rpmStaat == R33){
				blokken = 3;//3.33;
			}  
			else if(rpmStaat == R45){
				blokken = 4;//4.5;
			}

      int blokBreedte = 0.1 * displayLengte;
      int begin = dispHalf - (blokBreedte/2) * blokken;

      for(int i = 0; i < blokken; i++){
        displayTekenBlok(begin + 1, begin + blokBreedte - 1, 0.1);
        begin += blokBreedte;
      }
		}


		//------------------------------------------------------------------------VOLUME
		else if(volumeDisplayActie.sinds() < 2000   &&  staat != S_DOOR_SPOELEN   &&  staat != S_TERUG_SPOELEN   &&  staat != S_NAAR_NUMMER   &&  staat != S_PAUZE){
			int volumePunt = mapF(volume, 0, 63, 1, dispHalf);//40
      displayTekenBlok(dispHalf + volumePunt, dispHalf - volumePunt, 0.1);
		}
		

		//-----------------------------------------------------------------------------TRACK EN KAR DISPLAY
		else{
			for(int i = 0; i < displayLengte; i++){

				//-------------------------------------------------------------------------------TRACKS
				if( !( staat == S_STOPPEN  ||  staat == S_PARKEREN  ||  staat == S_NAAR_HOK  ||  staat == S_HOK ) ){
					
					int volgendeNummerDisplay  =  egtePos2displayPos(nummers[nummersTeller]);

					if( i > plaatGroote){
						displayData[i] = 0;

					}else if( staat == S_NAAR_BEGIN_PLAAT   &&  i > sensor  &&  plaatBegin == 1000){
						displayData[i] = 0;
					
					}else if(i > sensorMaxBerijk){
						displayData[i] = 0;
					
					}else if(volgendeNummerDisplay <= i     &&    nummersTeller < hoeveelNummers){
						nummersTeller++;
						displayData[i] = 0;
					
					}else{
						if(nummersTeller == 0){
							displayData[i] = 0;
						}else{
							displayData[i] = 0.1;
						}
					}          
				}
			}

            //---------------------------------------------------------------------------------------------------------------CURSOR
      if(staat == S_NAAR_NUMMER  ||  staat ==  S_DOOR_SPOELEN  ||  staat == S_TERUG_SPOELEN  ||  staat == S_PAUZE){ 
        displayTekenPunt(naald-1, 0.9);
        displayTekenPunt(naald+1, 0.9);
        
        if(naald != target){ // target puntje
          displayTekenPunt(target, 0.9);
        }
      }
      else if(staat == S_SPELEN  &&  !arm.isNaaldErop()  &&  (staatVeranderd.sinds() % 1000 < 250)   &&  !puristenMode){// knipperen voordat de naald erop is
        //niks
      }
      else if(herhaalDeHelePlaat){
        displayTekenPunt(naald, 0.9);
        displayTekenPunt(naald-2, 0.9);
        displayTekenPunt(naald+2, 0.9);
      }
      else if(puristenMode  &&  (millis() % 1000 < 500)){
        displayTekenPunt(naald-1, 0.9);
        displayTekenPunt(naald+1, 0.9);
      }else{
        displayTekenPunt(naald, 0.9);
      }


      if(!orientatie.isStaand){
        flipDisplayData();
      }
		}





    if(eepromNogOpslaan){  // als de eeprom nog niet is opgeslagen, blijf dan lekker knipperen voor arco en floris
      if(millis()%1000>500){
        displayTekenBlok(0, displayLengte/20, 0.9);
        displayTekenBlok(displayLengte,  displayLengte   -  displayLengte/20, 0.9);
      }
    }







		//-----------------------------------------------------------KNOP BLINK
		if(ledBlinkInterval.sinds() < 100){
			
			int knopGroote = 6;
			int knopGrooteHalf = knopGroote/2;
			int knopUitMidden = 0.23 * displayLengte;

			if(knopStaat[KNOP_PLAY] != LOSGELATEN){
        displayTekenBlok(dispHalf - knopGrooteHalf,   dispHalf + knopGrooteHalf, 0.9);
			}

			if(knopStaat[KNOP_TERUGSPOEL] != LOSGELATEN){
        displayTekenBlok(dispHalf - knopUitMidden,      dispHalf - knopUitMidden - knopGroote, 0.9);
			}

			if(knopStaat[KNOP_DOORSPOEL] != LOSGELATEN){
        displayTekenBlok(dispHalf + knopUitMidden,      dispHalf + knopUitMidden + knopGroote, 0.9);
			}
		}






		displayPrint(0);
		
		commitDisplay();

		displayDelay = micros();
		while(micros() - displayDelay < 2){}
		
		digitalWrite(displayEN, 0);

		while(micros() - displayDelay < 200){}

		digitalWrite(displayEN, 1);


		displayPrint(0.5);
		// while(micros() - displayDelay < 75){}
		commitDisplay();


		digitalWrite(displayEN, 0);
		delayMicroseconds(1500);
		digitalWrite(displayEN, 1);

		// displayPrint(1);
		// // while(micros() - displayDelay < 400){}
		// commitDisplay();




	}
}



