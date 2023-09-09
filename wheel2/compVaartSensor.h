


Interval draaienInterval(10, MILLIS);

Interval naaldNaarVorenBewogen(1, MILLIS);

#define sampleMax 65500               //samples

#define pprmax 1000

 









Interval compInt(0, MILLIS);


class COMPVAART
{ 
	public:
		//-------------------------------------snelheid
		volatile unsigned int vaartInterval;
		volatile unsigned int tijd;
		volatile unsigned int interval;



		//-----------------------------------richting
		char sens, sensPrev;
		int dir;
		int dirPrev;
		int glitchTeller;
		int pulsenPerRev;
		int teller = 0;
		int tellerRuw = 0;

		
		//---------------------------------versamplede sinus en cos
		float sinus[pprmax];
		float cosin[pprmax];
		
		

		//---------------------------------------filter
		int sampleNum;
		volatile          int samples[100];
		volatile unsigned int sampleTeller = 0;

		float gemiddelde = sampleMax;

		float vaartRuw;
		float vaart;
    float vaartCenterComp;
		float vaartLowPass;
    float vaartHighPass;

    float lowpassRect;
    float wow;





		//---------------------------------------uit Center Compensatie
		float karPosMiddenPre;
		float karUitCenterGolf[pprmax];
		// float karUitCenterMidden;

		float karSinWaardes[pprmax];
		float karCosWaardes[pprmax];
		float karSin;
		float karCos;
		float karFourier;

		float karSinFilt;
		float karCosFilt;
		float karFourierFilt;

		float karPosMiddenGeschiedenis[pprmax];

    float centerComp;




		//---------------------------------------------onbalans compensatie

		int onbalansFase = 180;//90;//100;//70;//90;//75;//90;  50 in pulsen per rev
		int onbalansHarm = 120;
    int harmonisen = 3;
    int harmVerschuiving[10] = {0, 60, 120, 140};
		
		
		float onbalansCompensatie[pprmax];

		volatile float onbalansComp = 0;
		
		// float compVerval = 1.0;//0.6;//0.8;
		float onbalansCompGewicht = 1.9;//1.5;//2;//0.8;


		float  onbalansSinTotaal[10];
		float  onbalansCosTotaal[10];
		float onbalansSinWaardes[10][pprmax];
		float onbalansCosWaardes[10][pprmax];
		
		volatile float onbalansCompFourier = 0;



		//------------------------------------------------debug
		unsigned int procesTijd;
		unsigned int procesInterval;

		bool golven = false;    
		bool onbalansCompAan = true;
		bool plaatUitMiddenComp = true;
		bool clearCompSamplesWachtrij = false;

    bool wowEersteWeerLaag;
    int tellerSindsReset;







		COMPVAART(int samps, float ppr){
			sampleNum = samps;
			pulsenPerRev = ppr;
			
			clearSamples();
			clearCompSamples();

			for(int i = 0; i < pulsenPerRev; i++){
				float radiaalTeller = ( i * TAU ) / pulsenPerRev;
				sinus[i] = sin(radiaalTeller);
				cosin[i] = cos(radiaalTeller);
			}
		}




		void update(){
			
			if( micros() - vaartInterval > sampleMax ){
				if(glitchTeller > 3){
					shiftSamples(sampleMax * dir);
					vaartRuw = 0;
					vaart += (vaartRuw - vaart) / 10;
					vaartLowPass += (vaart - vaartLowPass) / 100;
				}else{
					glitchTeller++;
				}
			}else{
				glitchTeller = 0;
			}

			// if(!isOngeveer(centerCompTargetRpm, targetRpm, 5)){
			// 	centerCompTargetRpm = targetRpm;
			// }
		}




	

		void interrupt(){
			tijd = micros();

			//------------------------------------------------------RICHTING
			dir = 1;
			sens = (gpio_get(plateauA) <<1)  |  gpio_get(plateauB);
			
			if( sens == 0b00 && sensPrev == 0b01 ||
					sens == 0b01 && sensPrev == 0b11 ||
					sens == 0b11 && sensPrev == 0b10 ||
					sens == 0b10 && sensPrev == 0b00
			){
				dir = -1;
			}

			sensPrev = sens;


			if(dirPrev != dir){
				clearOnbalansCompSamples();
			}
			dirPrev = dir;


			//------------------------------------------------------SNELHEID
			interval = tijd - vaartInterval;
			vaartInterval = tijd;
			
			if(interval > sampleMax){interval = sampleMax;}

			tellerRuw += dir;
      tellerSindsReset += dir;
			teller = rondTrip(teller + dir,  pulsenPerRev);


      shiftSamples(interval);// * dir);


			getVaart();

			vaart += (vaartRuw - vaart) / 10;




			//--------------------------------------------------------T = 0 COMP RESET
			if(clearCompSamplesWachtrij  && teller == 0){
				clearCompSamplesWachtrij = false;
        tellerSindsReset = 0;
				clearOnbalansCompSamples();
				// clearCompSamples();
			}


			

			//-----------------------------------------------------------------------UIT HET MIDDEN COMPENSATIE
			karPosMiddenPre -= karUitCenterGolf[teller];
			karUitCenterGolf[teller] = egteKarPos;
			karPosMiddenPre += karUitCenterGolf[teller];

			karPosMidden = karPosMiddenPre / pulsenPerRev;

			spoorafstand = karPosMiddenGeschiedenis[teller] - karPosMidden;
			karPosMiddenGeschiedenis[teller] = karPosMidden;

      if(spoorafstand > 0.01  ||  !(staat == S_SPELEN  &&  arm.isNaaldEropVoorZoLang(2000))){
        naaldNaarVorenBewogen.reset();
      }



			float karPosUitMidden = egteKarPos - karPosMidden;

			if(arm.isNaaldEropVoorZoLang(1000) && staat == S_SPELEN){ // naald moet er ff opzitten in spelen staat voor ie gaat rekenene
				karSin -= karSinWaardes[teller];
				karSinWaardes[teller] = sinus[teller]  *  karPosUitMidden;
				karSin += karSinWaardes[teller];
				
				karCos -= karCosWaardes[teller];
				karCosWaardes[teller] = cosin[teller]  *  karPosUitMidden;
				karCos += karCosWaardes[teller];

        karSinFilt += ( karSin - karSinFilt ) / 2000;
        karCosFilt += ( karCos - karCosFilt ) / 2000;
			}

			karFourier  = ( ( ( sinus[teller] * karSin )  +  ( cosin[teller] * karCos ) ) / pulsenPerRev ) * 2;

			karFourierFilt  = ( ( ( sinus[teller] * karSinFilt )  +  ( cosin[teller] * karCosFilt ) )  / pulsenPerRev  ) * 2;
			


			int leadTeller = rondTrip(teller - (8+9), pulsenPerRev); // fase verschuiving: 8 van de 16samples avg filter, en 9 van het gewonde filter er achteraan
			float uitMiddenSnelheidsComp = ( ( ( sinus[leadTeller] * karSinFilt )  +  ( cosin[leadTeller] * karCosFilt ) )  / pulsenPerRev  ) * 2;

      centerComp = (( karPosMidden - uitMiddenSnelheidsComp ) / karPosMidden );
			centerCompTargetRpm = targetRpm * centerComp;
      





      //-----------------------------------------------------------------------------gecompenseerde snelheden
      if(plaatUitMiddenComp){
        vaartCenterComp = vaart / centerComp;
      }else{
        vaartCenterComp = vaart;
      }

      vaartLowPass += (vaartCenterComp - vaartLowPass) / 100;
      vaartHighPass = vaartCenterComp - vaartLowPass;

      lowpassRect = abs(vaartLowPass - targetRpm);
      if(lowpassRect > wow){
        wow = lowpassRect;
      }else{
        wow += (lowpassRect - wow) / 1000;
      }


      if(wow < 0.1 && wowEersteWeerLaag == true){
        wowEersteWeerLaag = false;
        Serial.println("loopt weer geleik na: " + String(tellerSindsReset / float(pulsenPerRev)) + " omwentelingen");
      }

      if(wow > 0.3 &&  wowEersteWeerLaag == false){
        wowEersteWeerLaag = true;
      }




			
      //------------------------------------------------------------------------------te grote uitslag ERROR
			float sinBuff = karSinFilt / pulsenPerRev;
			float cosBuff = karCosFilt / pulsenPerRev;
			if( sinBuff * sinBuff  +  cosBuff * cosBuff  >  3 * 3){ // een uit het midden hijd van 6mm (3mm radius) triggerd error
				setError(E_TE_GROTE_UITSLAG);
        clearCompSamples();
				stoppen();
			}





			procesTijd = micros();

			//-----------------------------------------------------------------------------UIT BALANS COMPENSATIE

			
      digitalWrite(ledWit, 0);//zet led aan

			if( onbalansCompAan &&   //alle mementen waarom de compensatie niet mag werken, omdat er dan verschillen zijn met als de naald er egt op is
					plateauAan && 
					draaienInterval.sinds() > 1000 && //moet 1 seconden aan staan
					opsnelheid &&                      // en opsnelheid zijn     
					
          ((arm.isNaaldEropVoorZoLang(2000) && staat == S_SPELEN) )
          //  ||
          // staat == S_HOMEN_VOOR_SPELEN ||    
					// staat == S_NAAR_BEGIN_PLAAT)
		 
			){ 
				if(isOngeveer(vaart, targetRpm, 10)){
					onbalansCompensatie[teller] += ( vaartCenterComp - targetRpm ) * onbalansCompGewicht;
				}
        digitalWrite(ledWit, 1);//zet led aan
			}

			float nieuweWaarde = onbalansCompensatie[teller];

			onbalansCompFourier = 0;

			for(int harm = 1; harm < harmonisen + 1; harm++){
				int hoek = rondTrip(teller * harm,  pulsenPerRev); // hoek vermenigvuldigd met de uidige harmonici

				onbalansSinTotaal[harm] -= onbalansSinWaardes[harm][teller]; // haal de ouwe waarde uit het totaal
				onbalansSinWaardes[harm][teller] = sinus[hoek]  *  nieuweWaarde; // bereken de nieuwe waarde
				onbalansSinTotaal[harm] += onbalansSinWaardes[harm][teller]; // voeg de nieuwe to aan het totaal
				
				onbalansCosTotaal[harm] -= onbalansCosWaardes[harm][teller];
				onbalansCosWaardes[harm][teller] = cosin[hoek]  *  nieuweWaarde;
				onbalansCosTotaal[harm] += onbalansCosWaardes[harm][teller];

				// hoek = rondTrip(hoek + onbalansFase - (onbalansHarm / harm),  pulsenPerRev); // hoek ofset om de compensati wat voorspellender te maken
        hoek = rondTrip(hoek + harmVerschuiving[harm],  pulsenPerRev); // hoek ofset om de compensati wat voorspellender te maken
        onbalansCompFourier += ( ( sinus[hoek] * onbalansSinTotaal[harm] ) + ( cosin[hoek] * onbalansCosTotaal[harm] ) ) / pulsenPerRev;
			}


			if(onbalansCompAan){
				onbalansComp = -onbalansCompFourier;
			}else{
				onbalansComp = 0;
			}
			



			procesInterval = micros() - procesTijd;




			if(golven){
				Serial.print(vaartRuw, 3);
				Serial.print(",");
				Serial.print(vaart, 3);
				Serial.print(",");
				Serial.print(karFourier, 3);
				Serial.println();        
			}


			
		}











		void clearCompSamples(){
			clearOnbalansCompSamples();
			clearCenterCompSamples();
      // Serial.println("clearComp"); 
		}




    void clearCompSamplesOpTellerNull(){
			clearCompSamplesWachtrij = true;
		}



		void clearOnbalansCompSamples(){
			for(int i = 0; i < pulsenPerRev; i++){
				onbalansCompensatie[i] = 0;
			}

			for(int harm = 1; harm < harmonisen + 1; harm++){
				onbalansSinTotaal[harm] = 0;
				onbalansCosTotaal[harm] = 0;
				for(int i = 0; i < pulsenPerRev; i++){
					onbalansSinWaardes[harm][i] = 0;
					onbalansCosWaardes[harm][i] = 0;
				}
			}

		}



		void clearCenterCompSamples(){

			float pos = egteKarPos;

			for(int i = 0; i < pulsenPerRev; i++){
				karSinWaardes[i] = 0;
				karCosWaardes[i] = 0;
				karUitCenterGolf[i] = pos;

				karPosMiddenGeschiedenis[i] = pos;
			}

			karSinFilt = 0;
			karCosFilt = 0;
			karSin = 0;
			karCos = 0;
			
			karPosMiddenPre = pos * pulsenPerRev;
			karPosMidden = pos;
		}









		float getVaart(){

			gemiddelde = gemiddeldeInterval();
			vaartRuw = huidigeVaart(gemiddelde) * dir;

			return vaartRuw;//niet compensenre
		}







		void shiftSamples(int samp){
			samples[ sampleTeller++ % sampleNum ] = samp;   
		}




		void clearSamples(){
			for(int i = 0; i < sampleNum; i++){
				samples[i] = sampleMax;// * dir;        
			}
		}



		float gemiddeldeInterval(){
			int totaal = 0;
			
			for(byte i = 0;   i < sampleNum;   i++){
				totaal += samples[i];
			}
			return totaal / float(sampleNum);      
		}



		float huidigeVaart(float inter){//                                                           RPM BEREKENEN

			float waarde = ((1000000.0 * 60) / inter) / pulsenPerRev;  //  return totaal
			return limieteerF(waarde, -300, 300);
		}



};