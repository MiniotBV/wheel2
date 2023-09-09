#include "api/Common.h"



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

		float onbalansFilterCurve[pprmax];
    float onbalansFilterCurveBreedte = 100;
    int onbalansFase = 50;
		float onbalansCompGewicht = 2;
    
		float onbalansCompensatie[pprmax];
		volatile float onbalansComp = 0;
		



		//------------------------------------------------debug
		unsigned int procesTijd;
		unsigned int procesInterval;

		bool golven = false;    
		bool onbalansCompAan = true;
		bool plaatUitMiddenComp = true;
		bool clearCompSamplesWachtrij = false;
    bool compRaportPerOmwenteling = false;

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


      maakOnbalansFilterCurve();
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




			
      if(teller == 0){// als er een omwenteling is geweest

        if(compRaportPerOmwenteling){

        }


        if(clearCompSamplesWachtrij){//--------------------------------------------------------T = 0 COMP RESET
          clearCompSamplesWachtrij = false;
          tellerSindsReset = 0;
          clearOnbalansCompSamples();
        }

      }



			

			//------------------------------------------------------------------------------------------------UIT HET MIDDEN COMPENSATIE
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
			




      //------------------------------------------------------------------------------te grote uitslag ERROR
			float sinBuff = karSinFilt / pulsenPerRev;
			float cosBuff = karCosFilt / pulsenPerRev;
			if( sinBuff * sinBuff  +  cosBuff * cosBuff  >  3 * 3){ // een uit het midden hijd van 6mm (3mm radius) triggerd error
				setError(E_TE_GROTE_UITSLAG);
        clearCompSamples();
				stoppen();
			}





      //---------------------------------------------------------------------------------------------------------gecompenseerde snelheden
			int leadTeller = rondTrip(teller - (8+9), pulsenPerRev); // fase verschuiving: 8 van de 16samples avg filter, en 9 van het gewonde filter er achteraan
			float uitMiddenSnelheidsComp = ( ( ( sinus[leadTeller] * karSinFilt )  +  ( cosin[leadTeller] * karCosFilt ) )  / pulsenPerRev  ) * 2;

      centerComp = (( karPosMidden - uitMiddenSnelheidsComp ) / karPosMidden );
			centerCompTargetRpm = targetRpm * centerComp;
      
      if(plaatUitMiddenComp){
        vaartCenterComp = vaart / centerComp;
      }else{
        vaartCenterComp = vaart;
      }








      //-------------------------------------------------------------------------------------------WOW FLUTTER METING
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
        tellerSindsReset = 0;
      }

      if(wow > 0.3 &&  wowEersteWeerLaag == false){
        wowEersteWeerLaag = true;
      }




			






			

			//-----------------------------------------------------------------------------ONBALANS COMPENSATIE
      procesTijd = micros();
      


			if( onbalansCompAan &&   //alle mementen waarom de compensatie niet mag werken, omdat er dan verschillen zijn met als de naald er egt op is
					plateauAan 
					&& draaienInterval.sinds() > 1000  //moet 1 seconden aan staan
					&& opsnelheid                       // en opsnelheid zijn     
					&& isOngeveer(vaart, targetRpm, 10)  //mag niet meer dan 10rpm van de target rpm afzijn

          && ((arm.isNaaldEropVoorZoLang(2000) && staat == S_SPELEN) )// ||  //)
          // staat == S_HOMEN_VOOR_SPELEN ||    
					// staat == S_NAAR_BEGIN_PLAAT)
			){ 
        float snelheidsError = vaartCenterComp - targetRpm;
        for(int i = 0; i < pulsenPerRev; i++){
          onbalansCompensatie[rondTrip(teller + i, pulsenPerRev)] += onbalansFilterCurve[i] * snelheidsError;
        }
        
				digitalWrite(ledWit, 1);//zet led aan
			}else{
        digitalWrite(ledWit, 0);//zet led uit
      }



			if(onbalansCompAan){
				onbalansComp = -onbalansCompensatie[rondTrip( teller + onbalansFase, pulsenPerRev)] * onbalansCompGewicht;
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








    void clearCompSamplesOpTellerNull(){
			clearCompSamplesWachtrij = true;
		}


		void clearCompSamples(){
			clearOnbalansCompSamples();
			clearCenterCompSamples();
		}



		void clearOnbalansCompSamples(){
			for(int i = 0; i < pulsenPerRev; i++){
				onbalansCompensatie[i] = 0;
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







    void maakOnbalansFilterCurve(){
      
      float totaal = 0;
      
      for(int i = 0; i < pulsenPerRev; i++){
        int verschovenI = i - (pulsenPerRev / 2) ;
        float j = float(verschovenI) / pulsenPerRev;
        onbalansFilterCurve[rondTrip(verschovenI, pulsenPerRev)]  =  exp( -onbalansFilterCurveBreedte * (j*j));
        totaal += onbalansFilterCurve[rondTrip(verschovenI, pulsenPerRev)];
      }

      for(int i = 0; i < pulsenPerRev; i++){
        onbalansFilterCurve[i] /= totaal;
      }
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