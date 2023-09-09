




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


// void displayTekenBlok(int begin, int einde, float kleur){
//   int min = min(begin, einde);
//   int max = max(begin, einde);
//   for(int i = min; i < max; i++){
//     if( i >= 0 && i < displayLengte){
//       displayData[i] = kleur;
//     }
//   }  
// }






Interval displayInt(10000, MICROS);

void displayUpdate(){
	if(displayInt.loop()){
	// if(true){

		nummersTeller = 0;
		int naald = egtePos2displayPos(karPosFilter);//int naald = egtePos2displayPos(karPos);
		int target = egtePos2displayPos(targetNummerPos);
		int sensor = egtePos2displayPos(sensorPos);
		int sensorMaxBerijk = egtePos2displayPos(ELPEE_PLAAT_BEGIN - SENSOR_OFFSET)  +  3;
		int plaatGroote = egtePos2displayPos(plaatBegin);

		int volumeMargin = displayLengte/16;//16;

		int dispHalf = displayLengte/2;

    displayClear();



		//--------------------------------------------------------------------INTRO
    // if(0){
		if(millis()<4000){

      int pos = displayLengte - (millis()/10);
      int blokLengte = 10;
      int gatLengte = 3;

      char versieDisplay[500];
      for(int i = 0; i < 500; i++){versieDisplay[i] = 0;}
      int versieDisplayTeller = 0;

      int decimalen = 3;
      int versieDecimaal[decimalen];

      versieDecimaal[0] = (versie) % 10;
      versieDecimaal[1] = (versie / 10) % 10;
      versieDecimaal[2] = (versie / 100) % 10;

      for (int i = 0; i < decimalen; i++) {
        int dec = versieDecimaal[i];

        if (dec == 0) {
          for (int j = 0; j < gatLengte * 2; j++) {
            versieDisplay[versieDisplayTeller++] = (j < gatLengte) ? 0 : 1;
          }
        }

        while (dec > 0) {
          if (dec > 5) {
            dec -= 5;
            for (int j = 0; j < gatLengte + blokLengte * 2; j++) {
              versieDisplay[versieDisplayTeller++] = (j < gatLengte) ? 0 : 1;
            }
          }

          if (dec > 0) {
            dec -= 1;
            for (int j = 0; j < gatLengte + blokLengte; j++) {
              versieDisplay[versieDisplayTeller++] = (j < gatLengte) ? 0 : 1;
            }
          }
        }

        if (dec == 0) {
          for (int j = 0; j < blokLengte; j++) {
            versieDisplay[versieDisplayTeller++] = 0;
          }
        }
      }

      for (int i = 0; i < displayLengte; i++) {
        displayData[i] = (i >= pos) ? (versieDisplay[i - pos] == 1 ? 0.1 : 0) : 0;
      }
    }





    //----------------------------------------------------------------ERROR WEERGEVEN
		else if(errorVeranderd.sinds() < 10000  &&  error != E_GEEN){ // 10seonden knipperen

			if((millis()%1000) > 800){//                knipper


				int blokken = error;

				float blokBreedte = 0.1 * displayLengte;
				float totaleBreedte = blokBreedte * blokken;
				float halveBreedte = totaleBreedte / 2;
				float beginPos = dispHalf - halveBreedte;
				float eindPos = dispHalf + halveBreedte;


				for(int i = 0; i < displayLengte; i++){
					displayData[i] = 0;

					if(i > beginPos  &&  i < eindPos){
						displayData[i] = 0.1;

						if( (int(beginPos) + i) % int(blokBreedte) < 2 ){
							displayData[i] = 0;
						}
					}
				}
			}
		}

		//----------------------------------------------------------------SCHOONMAAK STAND
		else if(staat == S_SCHOONMAAK  ||  staat == S_HOMEN_VOOR_SCHOONMAAK){
			
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

			for(int i = 0; i < displayLengte; i++){
				displayData[i] = 0;

				if(i > volumePunt){
					displayData[i] = 0.1;
				}

        if(i == armHoekPunt  ||  i == krachtLaagPunt  ||  i == krachtHoogPunt){
					displayData[i] = 0.9;         
				}
			}
		}




		//-------------------------------------------------------------------------WATER PAS STAND
		else if(staat == S_FOUTE_ORIENTATIE){
			

      for(int i = 0; i < displayLengte; i++){
        float floatI = float(i) / displayLengte;

        if((  i < displayLengte/4  ||  i  > displayLengte - (displayLengte/4))  &&  (millis()%1000) < 800 ){
          displayData[i] = 0.1;
        }else{
          displayData[i] = 0;
        }


        if(isOngeveer(floatI, (-orientatie.y*3) + 0.5, 0.02)){ // belltje
					displayData[i] = 0.9;
				}
      }


      

		}


		

		//--------------------------------------------------------------RPM
		else if(rpmDisplayActie.sinds() < 2000){

			float blokken = 0;

			if(rpmStaat == R33){
				blokken = 3;//3.33;
			}  
			else if(rpmStaat == R45){
				blokken = 4;//4.5;
			}
			else if(rpmStaat == AUTO){
				blokken = 1;//4.5;
			} 

			float blokBreedte = 0.1 * displayLengte;
			float totaleBreedte = blokBreedte * blokken;
			float halveBreedte = totaleBreedte / 2;
			float beginPos = dispHalf - halveBreedte;
			float eindPos = dispHalf + halveBreedte;


			for(int i = 0; i < displayLengte; i++){
				displayData[i] = 0;

				if(i > beginPos  &&  i < eindPos){
					displayData[i] = 0.1;

					if( (int(beginPos) + i) % int(blokBreedte) < 2 ){
						displayData[i] = 0;
					}
				}
			}
		}


		//------------------------------------------------------------------------VOLUME
		else if(volumeDisplayActie.sinds() < 2000   &&  staat != S_DOOR_SPOELEN   &&  staat != S_TERUG_SPOELEN   &&  staat != S_NAAR_NUMMER   &&  staat != S_PAUZE){
			for(int i = 0; i < displayLengte; i++){
				displayData[i] = 0;          

				int volumePunt = mapF(volume, 0, 63, 2, dispHalf + 1);//40

				if(i < (dispHalf - 1) + volumePunt    &&    i > dispHalf - volumePunt){
					displayData[i] = 0.1;
				}
			}
		}
		
		
		//-----------------------------------------------------------------------------TRACK EN KAR DISPLAY
		else{
			for(int i = 0; i < displayLengte; i++){

				//-------------------------------------------------------------------------------TRACKS
				if( staat == S_STOPPEN  ||  staat == S_PARKEREN  ||  staat == S_NAAR_HOK  ||  staat == S_HOK){
					displayData[i] = 0;
				}else{
					
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


				//---------------------------------------------------------------------------------------------------------------CURSOR
				if(staat == S_NAAR_NUMMER  ||  staat ==  S_DOOR_SPOELEN  ||  staat == S_TERUG_SPOELEN  ||  staat == S_PAUZE){ 
					if(naald-1 == i || naald+1 == i){ // dubbele punt
						displayData[i] = 0.9;
					}
					
					else if( i == target  &&  naald != i){ // target puntje
						displayData[i] = 0.9;
					}
				}
        else if(staat == S_SPELEN  &&  !arm.isNaaldErop()  &&  (staatVeranderd.sinds() % 1000 < 250)   &&  !puristenMode){// knipperen voordat de naald erop is
          //niks
        }
				else if(herhaalDeHelePlaat){
					if(naald == i || naald-2 == i || naald+2 == i){ // 3 dubbele punt
						displayData[i] = 0.9;
					}
				}
        else if(puristenMode  &&  (millis() % 1000 < 500)){
					if(naald-1 == i || naald+1 == i){ // dubbele punt
						displayData[i] = 0.9;
					}
				}
				else if(naald == i){
					displayData[i] = 0.9;	//gewone cursor
				}

				
			}


      if(!orientatie.isStaand){
        flipDisplayData();
      }
		}


		//-----------------------------------------------------------KNOP BLINK
		if(ledBlinkInterval.sinds() < 100){
			
			int knopGroote = 6;
			int knopGrooteHalf = knopGroote/2;
			int knopUitMidden = 0.23 * displayLengte;

			if(knopStaat[KNOP_PLAY] != LOSGELATEN){
				for(int i = dispHalf - knopGrooteHalf; i < dispHalf + knopGrooteHalf; i++){
					displayData[i] = 0.9;
				}
			}

			if(knopStaat[KNOP_TERUGSPOEL] != LOSGELATEN){
				for(int i = dispHalf - knopUitMidden - knopGroote; i < dispHalf - knopUitMidden; i++){
					displayData[i] = 0.9;
				}
			}

			if(knopStaat[KNOP_DOORSPOEL] != LOSGELATEN){
				for(int i = dispHalf + knopUitMidden; i < dispHalf + knopUitMidden + knopGroote; i++){
					displayData[i] = 0.9;
				}
			}
		}






		displayPrint(0);
		
		commitDisplay();

		displayDelay = micros();
		while(micros() - displayDelay < 2){}
		
		digitalWrite(displayEN, 0);

		while(micros() - displayDelay < 60){}

		digitalWrite(displayEN, 1);


		displayPrint(0.5);
		// while(micros() - displayDelay < 75){}
		commitDisplay();


		digitalWrite(displayEN, 0);
		delayMicroseconds(1000);
		digitalWrite(displayEN, 1);

		// displayPrint(1);
		// // while(micros() - displayDelay < 400){}
		// commitDisplay();




	}
}



