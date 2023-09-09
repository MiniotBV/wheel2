//motor heeft 48 stappen
//tandwiel heeft 10 tanden
//tandheugel heeft 1.5mm tand pitch 
//48 stappen / 12 tanden = 4 stappen per tand
//1.5mm / 4 stappen per tand = 0.375mm per stap
//PI = 2 stappen

int karStapperTanden = 12;//8;

float mmPerStap = 1.5 / ( 48 / karStapperTanden );
float stap2mm = ( 2 / PI ) * mmPerStap;  // 0.238732414637843
float mm2stap = 1 / stap2mm;   


float karInterval;
bool karGolven;

bool karUitMiddenCompAan = true;
float karUitMiddenCompFilter;


float karP = 2;//4;//0.001;//0.0005; //0.00005;//0.00025;
float karI = 0;//0.005; //0.00005;//0.00025;
float karD = 1;//2;//1.5;//1.5;//0.0006;//-0.003;

float karDcomp = 0;








float sensorPos;
float karOffset = 0; // om te kijken wat de homing afwijking is



//armhoek sensor
float armHoekMin = 1200;
float armHoekMax = 3300;
float armHoekMinCall = AMAX;
float armHoekMaxCall = 0;
float armHoekCall;

float armHoekRuw = analogRead(hoekSensor);
float armHoekPrev;
float armHoekDiv;

float armHoek;
float armHoekSlow = armHoekRuw;
float armHoekOffset;







#define KAR_MAX_SNELHEID 0.02
// #define KAR_MAX_SNELHEID 0.0002
#define KAR_VERSNELLING  0.0001

bool karMotorEnable = true;
float karMotorPos = 0;

double karSnelHeid = 0;
// float karTargetPos = 0;
float versnelling = 0;
float afstandOmTeStoppen = 0;



bool beweegKarNaarPos(float target, float snelheid){

	versnelling = 0;
	
	float teGaan = abs(target - karPos);
	int teGaanRichting =  target - karPos > 0 ? 1 : -1;



	afstandOmTeStoppen = (karSnelHeid * karSnelHeid) / ( 2 * KAR_VERSNELLING );
	int afstandOmTeStoppenRichting = karSnelHeid > 0 ? 1 : -1;

	if(isOngeveer(teGaan,  0,  0.01)  &&  afstandOmTeStoppen < 0.1){
		karSnelHeid = 0;
		return true;
	}

	if(afstandOmTeStoppen >= teGaan){
		versnelling = -KAR_VERSNELLING * teGaanRichting;
	}
	else if(abs(karSnelHeid) < KAR_MAX_SNELHEID){
		versnelling = KAR_VERSNELLING * teGaanRichting;
	}

	
	karPos += karSnelHeid + (versnelling / 2);

	karSnelHeid += versnelling;

	karSnelHeid = limieteerF(karSnelHeid, -snelheid, snelheid);

	return false;
}



bool decelereerKar(){

	int richting = karSnelHeid > 0 ? 1 : -1;

	if(abs(karSnelHeid) < KAR_VERSNELLING){
		karSnelHeid = 0;
		return true;
	}

	versnelling = -KAR_VERSNELLING * richting;
	
	karPos += karSnelHeid + (versnelling / 2);

	karSnelHeid += versnelling;

	return false;
}




void karNoodStop(){
	karSnelHeid = 0;
}


















void armHoekCentreer(){
	armHoekOffset = armHoekSlow;
}


void armHoekCalibreer(){
	armHoekMin = armHoekMinCall;
	armHoekMax = armHoekMaxCall;

	armHoekMinCall = AMAX;
	armHoekMaxCall = 0;
	debug("armHoek gecalibreed en buffer waardes gereset");
}







void gaNaarNummer(float pos){
	targetNummerPos = pos;
	debug("naarpos: " + String(targetNummerPos));
	setStaat(S_NAAR_NUMMER);
}


void naarBeginPlaat(){
  gaNaarNummer(plaatBegin);
}



void naarVorrigNummer(){
	float pos = karPosFilter;

	if(staat == S_NAAR_NUMMER){//als er al word door gespoeld doe dan een extra nummer verder
		pos = targetNummerPos;
	}

	int nummer = 0;

	while(pos + NUMMER_TERUG_OPFSET >= nummers[nummer]){
		nummer++;
		if(nummer > hoeveelNummers - 1){
      naarBeginPlaat();
			return;
		}
	}

	if(nummers[nummer] > plaatBegin){
		naarBeginPlaat();
	}

	gaNaarNummer(nummers[nummer]);
}






void naarVolgendNummer(){
	float pos = karPosFilter;
	
	if(staat == S_NAAR_NUMMER){//als er al word door gespoeld doe dan een extra nummer verder
		pos = targetNummerPos;
	}

	int nummer = hoeveelNummers - 1;

  if(nummer <= 0){ // nummer 0 is het begin van nummers (dus niet en egt nummer)
	stoppenOfHerhalen();//stoppen();
	return;
  }

	while(pos - 2 <= nummers[nummer]){ // 2mm lopen voordat hij hetzelfde nummer gaat herhalen
		nummer--;
		if(nummer <= 0){ // nummer 0 is het begin van nummers (dus niet en egt nummer)
			stoppenOfHerhalen();//stoppen();
			return;
		}
	}

	gaNaarNummer(nummers[nummer]);
}

























void staatDingen(){

  if(staat == S_HOK){
		armHoekCentreer();
		karMotorEnable = false;
		herhaalDeHelePlaat = false;
		return;
	}

	
	if(staat == S_STOPPEN){
		if(arm.naaldEraf() && decelereerKar()){
			setStaat(S_NAAR_HOK);
		}
		return;
	}



	if(staat == S_NAAR_HOK  ||  staat == S_HOMEN_VOOR_SPELEN  ||  staat == S_HOMEN_VOOR_SCHOONMAAK){
		
    if(staatVeranderd.sinds() < 100){return;}// ff uittrillen na aanzetten stappermotor

		bool aangekomen = beweegKarNaarPos(-150,  KAR_MAX_SNELHEID);

		if(aangekomen){
			karOffset -= KAR_HOME - karPos;
			karPos = KAR_HOME;
      
      if(error == E_KON_NIET_HOMEN){
        setError(E_KON_NIET_HOMEN);
        setStaat(S_PARKEREN);
      }else{
        setError(E_KON_NIET_HOMEN);
        setStaat(S_HOMEN_GEFAALD);
      }
      
			
			
			return;
		}

    // if(armHoek > 0.5){//-800){//-1000)
		if(armHoekCall > 0.75){//-800){//-1000)
			// debug("home verschill: " + String(karOffset + karPos));
			karOffset -= KAR_HOME - karPos;
			karPos = KAR_HOME;

			karNoodStop();
			strobo.clearCompSamples();// dit is een mooi moment om te stoppen

			if(staat == S_HOMEN_VOOR_SPELEN){
				setStaat(S_NAAR_BEGIN_PLAAT);

			}else if(staat == S_HOMEN_VOOR_SCHOONMAAK){
				setStaat(S_SCHOONMAAK);

			}else{
				setStaat(S_PARKEREN);
			}

			return;
		}
		
		return;
	}



	if(staat == S_PARKEREN){
		if(beweegKarNaarPos(KAR_HOK, KAR_MAX_SNELHEID)){
			setStaat(S_HOK);
		} 
		return;
	}







	if(staat == S_HOMEN_GEFAALD){
		if(beweegKarNaarPos(KAR_HOK+10, KAR_MAX_SNELHEID)){
      if(staatVeranderd.sinds() < 2000){
        armHoekCentreer();
        return;
      }
			setStaat(S_NAAR_HOK);
		} 
		return;
	}



	if(staat == S_FOUTE_ORIENTATIE){
		arm.naaldEraf();
		karMotorEnable = false;
		return;
	}







	if(staat == S_NAAR_BEGIN_PLAAT){
		if(eenKeerSindsStaatsVerandering()){ // 10ms om wat shit te doen
			plaatBegin = 1000;
		}

		if(!plaatAanwezig  &&  sensorPos > PLAAT_EINDE + 1){ // is er een plaat aanwezig
			
			float plaadDiaInch = (sensorPos / 25.4)*2;
			

			if(plaadDiaInch < 6){//kleiner dan 6" dan stoppen
				debug("geen plaat? plaatDia: " + String(plaadDiaInch));
				stoppen();
				return;
			}
			
			if(plaadDiaInch < 8){// ongeveer 
				debug("plaatDia: " + String(plaadDiaInch) + " : ±7\" ");
				setPlateauRpm(rpm45);
				plaatBegin = SINGLETJE_PLAAT_BEGIN;
				zetNummersAlsEenSingletje();
				// return;
			
			
			}else if(plaadDiaInch < 11){ 
				debug("plaatDia: " + String(plaadDiaInch) + " : ±10\" ");
				setPlateauRpm(rpm33);
				plaatBegin = TIEN_INCH_PLAAT_BEGIN;
			
			
			}else{
				debug("plaatDia: " + String(plaadDiaInch) + " : ???????\" ");
				plaatBegin = sensorPos;
				// setPlateauRpm(rpm33);
			}

			targetNummerPos = plaatBegin;
      plaatLeesNaKijken();

			setStaat(S_UITROLLEN_VOOR_SPELEN);
      

			return;
		}
		


		if(  beweegKarNaarPos(ELPEE_PLAAT_BEGIN,   KAR_MAX_SNELHEID)  ){ // als aangelokomen eind van de kar berijk en altijd een plaat gezien
			plaatBegin = ELPEE_PLAAT_BEGIN;
      targetNummerPos = plaatBegin;
			debug("plaatDia: 12inch");
			setPlateauRpm(rpm33);
      plaatLeesNaKijken();

			setStaat(S_SPELEN);
			return;
		}
		
		return;
	}





	if(staat == S_UITROLLEN_VOOR_SPELEN){
		if(beweegKarNaarPos(targetNummerPos, KAR_MAX_SNELHEID)){ 
			setStaat(S_SPELEN);
			return;
		}
		return;
	}

  



  //  ================================================================
  //      SPELEN
  //  ================================================================
	if(staat == S_SPELEN){
		if(staatVeranderd.sinds() < 1000){
      armHoekCentreer(); 
      return; 
    } // eerst ff centrere


		
		if(arm.naaldErop()){

      //-----------------------------------------------------------transport berekeningen
			nieuwePos = karPos + limieteerF(armHoek * karP, -3, 3);
			nieuwePos = limieteerF(nieuwePos, 0, plaatBegin);
			beweegKarNaarPos(nieuwePos, KAR_MAX_SNELHEID);
			

      //---------------------------------------------------------dingen die kunnen gebeuren tijdens spelen
			if(egteKarPos <= PLAAT_EINDE){
				debug("kar heeft de limiet berijkt");
				stoppenOfHerhalen();//stoppen();
				return;
			}

			if(egteKarPos < karPosFilter - 3){
				debug("waarschijnlijk uitloop groef");
				stoppenOfHerhalen();//stoppen();
				return;
			}

			if(egteKarPos > karPosFilter + 2.5){
				setError(E_NAALD_TERUG_GELOPEN);     
				stoppen();
				return;
			}

			if(naaldNaarVorenBewogen.sinds() > 4000){
        if(karPos < 60){//is het een uitloop groef? (54mm lijkt de verste van het midden)
          stoppenOfHerhalen();//stoppen();
          return;
        }else{
          setError(E_NAALD_NIET_BEWOGEN); //kar te lang niet bewogen
          naaldNaarVorenBewogen.reset(); // ff de timer reseten zodat hij niet straks weer triggerd
          gaNaarNummer(karPos - 0.25); // beweeg de kar 0.5mm naar binne om over de skip te skippen
          return;
        }
				
			} 

      if(puristenMode){
        if(strobo.wow < 0.15   ||  arm.isNaaldEropVoorZoLang(10000) ){// loop de plaat al geleleik of heeft het 10sec geduurd
          puristenMode = false;
          debug("loopt gelijk genoeg");
          // naarBeginPlaat();
          gaNaarNummer(targetNummerPos);
        }
      }    
		}
		return;
	}



  
  //  ================================================================
  //      NAAR NUMMER EN SPOELEN
  //  ================================================================
	if(staat == S_NAAR_NUMMER){
		if(arm.naaldEraf()){
			if(beweegKarNaarPos(targetNummerPos, KAR_MAX_SNELHEID)){
		    setStaat(S_SPELEN);
				return;
			}
		}
		return;
	}




	if(staat == S_DOOR_SPOELEN){
		if(arm.naaldEraf()){
			beweegKarNaarPos(PLAAT_EINDE, KAR_MAX_SNELHEID/4);  
		}
		targetNummerPos = karPos;//om het display opteschonene
	}




	if(staat == S_TERUG_SPOELEN){
		if(arm.naaldEraf()){
			beweegKarNaarPos(plaatBegin, KAR_MAX_SNELHEID/4);
		}
		targetNummerPos = karPos;//om het display opteschonene
	}




  if(staat == S_UITROLLEN_NA_SPOELEN){
		if(beweegKarNaarPos(targetNummerPos, KAR_MAX_SNELHEID/4)){ 
			setStaat(S_SPELEN);
			return;
		}
		return;
	}



	if(staat == S_PAUZE){
		if(arm.naaldEraf()){
			beweegKarNaarPos(targetNummerPos, KAR_MAX_SNELHEID);
		}
		return;
	}




	if(staat == S_SCHOONMAAK){
		if(beweegKarNaarPos(SCHOONMAAK_PLEK, KAR_MAX_SNELHEID)){
			arm.naaldErop();
		}

		if(sensorPos > PLAAT_EINDE + 2  &&  sensorPos < PLAAT_EINDE + 12  && plaatAanwezig){  // ligt er een plaat op? dan stoppen
      debug("kan niet in schoonmaak, er is een plaat of iets vergelijkbaars");
			stoppen();
		}
		return;
	}





	if(staat == S_CALIBREER){
    if(staatVeranderd.sinds() < 100){
      targetNummerPos = SCHOONMAAK_PLEK;
    }
		if(beweegKarNaarPos( targetNummerPos, KAR_MAX_SNELHEID)){}
		return;
	}

}














Interval karMotorInt(1000, MICROS);


bool karMotorUitvoeren()
{
	
  //--------------------------------------------------------------------arm hoek
	armHoekRuw += ( analogRead(hoekSensor) - armHoekRuw ) / 6;

  if(millis() > 1000){
    	if(armHoekRuw < armHoekMinCall){armHoekMinCall = armHoekRuw;}
      if(armHoekRuw > armHoekMaxCall){armHoekMaxCall = armHoekRuw;}
  }
	
	armHoekCall = mapF(armHoekRuw, armHoekMin, armHoekMax, 1, -1);
	
	armHoekDiv = armHoekCall - armHoekPrev;
	armHoekPrev = armHoekCall;

	armHoekSlow += (armHoekCall - armHoekSlow) / 100;

	armHoek = armHoekCall - armHoekOffset;

  //------------------------------limiet error
	if( (staat == S_NAAR_BEGIN_PLAAT   &&   staatVeranderd.sinds() > 500)    || staat == S_UITROLLEN_VOOR_SPELEN || staat == S_SPELEN || staat == S_PAUZE || staat == S_NAAR_NUMMER || staat == S_DOOR_SPOELEN || staat == S_TERUG_SPOELEN || staat == S_UITROLLEN_NA_SPOELEN ){
		if(armHoekCall > 0.9){
			setError(E_ARMHOEK_LIMIET_POS);
      arm.naaldEraf();
      setStaat(S_HOK);
		}
		if(armHoekCall < -0.9){
			setError(E_ARMHOEK_LIMIET_NEG);
      arm.naaldEraf();
			setStaat(S_HOK);
		}
	}


	sensorPos = ( karPos - SENSOR_OFFSET )  - trackOffset;


	staatDingen();


	karDcomp *= 0.999;
	karDcomp += limieteerF(armHoekDiv * karD, -KAR_MAX_SNELHEID, KAR_MAX_SNELHEID);//------------ Om oscilatie te voorkomen
	
	
	egteKarPos  = karPos + karDcomp;

	if(karUitMiddenCompAan){
		karUitMiddenCompFilter +=  ( strobo.karFourierFilt - karUitMiddenCompFilter ) / 4;
		egteKarPos += karUitMiddenCompFilter;   
	}


	karMotorPos = (egteKarPos + karOffset)  *  mm2stap;


	if(karMotorEnable){
    // pwmStapper(-karMotorPos,   stapperAP, stapperAN,  stapperBP, stapperBN,  true);	
    pwmStapper(karMotorPos,   stapperAP, stapperAN,  stapperBP, stapperBN,  true);	
	}else{
		pwmDisableStapper(stapperAP, stapperAN,  stapperBP, stapperBN);
	}




	


	if(staat == S_SPELEN  &&  arm.isNaaldErop()){ // kar pos filter voor display
		float div = karPos - karPosFilter;
		if(div < 0){
			karPosFilter += (div) / 1000;				
		}
	}else{
		karPosFilter = karPos;
	}



	karMotorEnable = true;  




	if(karGolven)
  {
		Serial.print(karDcomp, 5); Serial.print(',');
		Serial.print(armHoekRuw); Serial.print(',');
		Serial.print(armHoekCall, 2);
		Serial.println();
	}
	return 1;
}





void karMotorFunc(){  
	if(karMotorInt.loop()){
		karMotorUitvoeren();
	}
}



void karInit(){
	setPwm(stapperAP);
	setPwm(stapperAN);
	setPwm(stapperBP);
	setPwm(stapperBN);

	// struct repeating_timer timer;
	// add_repeating_timer_us(1, karMotorUitvoeren, NULL, &timer);
	// add_repeating_timer_us(1000, karMotorUitvoeren, NULL, &karTimer);
}






