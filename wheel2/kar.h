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


float karP = 4;//0.001;//0.0005; //0.00005;//0.00025;
float karI = 0;//0.005; //0.00005;//0.00025;
float karD = 2;//1.5;//1.5;//0.0006;//-0.003;

float karDcomp = 0;







float targetNummerPos = 0;
//float sensorPos; must be global, see .ino file, renamed trackSenorPos
float karOffset = 0; // om te kijken wat de homing afwijking is



//armhoek sensor
float armHoekMin = 1700;
float armHoekMax = 3340;
float armHoekMinCall = AMAX;
float armHoekMaxCall = 0;
float armHoekCall;

float armHoekRuw = analogRead(hoekSensor);
float armHoekPrev;
float armHoekDiv;

float armHoek;
float armHoekSlow = armHoekRuw;
float armHoekOffset;


Interval naaldNaarVorenBewogen(1, MILLIS);






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

	karSnelHeid = limitF(karSnelHeid, -snelheid, snelheid);

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
	Serial.println("armHoek gecalibreed en buffer waardes gereset");
}







void gaNaarNummer(float pos){
	targetNummerPos = pos;
	Serial.println("naarpos: " + String(targetNummerPos));
	setStaat(S_NAAR_NUMMER);
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
			gaNaarNummer(plaatBegin);
			return;
		}
	}

	if(nummers[nummer] > plaatBegin){
		gaNaarNummer(plaatBegin);
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
	
	if(staat == S_STOPPEN){
		if(arm.naaldEraf() && decelereerKar()){
			setStaat(S_NAAR_HOK);
		}
		return;
	}



	if(staat == S_NAAR_HOK  ||  staat == S_HOMEN_VOOR_SPELEN  ||  staat == S_HOMEN_VOOR_SCHOONMAAK){
		if(staatVeranderd.sinds() < 1000){//calibreren
			armHoekCentreer();
			return;
		}

		bool aangekomen = beweegKarNaarPos(-150,  KAR_MAX_SNELHEID);

		if(aangekomen){
			karOffset -= KAR_HOME - karPos;
			karPos = KAR_HOME;
			setStaat(S_HOMEN_GEFAALD);
			setError(E_KON_NIET_HOMEN);
			return;
		}

		if(armHoek < -0.25){//-800){//-1000)
			Serial.println("home verschill: " + String(karOffset + karPos));
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



	if(staat == S_HOK){
		armHoekCentreer();
		karMotorEnable = false;
		herhaalDeHelePlaat = false;
		return;
	}



	if(staat == S_HOMEN_GEFAALD){
		if(beweegKarNaarPos(KAR_HOK+10, KAR_MAX_SNELHEID)){
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

		if(!plaatAanwezig  &&  trackSensorPos > PLAAT_EINDE + 1){ // is er een plaat aanwezig
			
			float plaadDiaInch = (trackSensorPos / 25.4)*2;
			

			if(plaadDiaInch < 6){//kleiner dan 6" dan stoppen
				Serial.println("geen plaat? plaatDia: " + String(plaadDiaInch));
				stoppen();
				return;
			}
			
			if(plaadDiaInch < 8){// ongeveer 
				Serial.println("plaatDia: " + String(plaadDiaInch) + " : ±7\" ");
				setPlateauRpm(rpm.r45);
				plaatBegin = SINGLETJE_PLAAT_BEGIN;
				lees.zetNummersAlsEenSingletje();
				// return;
			
			
			}else if(plaadDiaInch < 11){ 
				Serial.println("plaatDia: " + String(plaadDiaInch) + " : ±10\" ");
				setPlateauRpm(rpm.r33);
				plaatBegin = TIEN_INCH_PLAAT_BEGIN;
			
			
			}else{
				Serial.println("plaatDia: " + String(plaadDiaInch) + " : ???????\" ");
				plaatBegin = trackSensorPos;
				// setPlateauRpm(rpm33);
			}

			targetNummerPos = plaatBegin;
			setStaat(S_UITROLLEN_VOOR_SPELEN);
			return;
		}
		


		if(beweegKarNaarPos(ELPEE_PLAAT_BEGIN, KAR_MAX_SNELHEID))
		{
			plaatBegin = ELPEE_PLAAT_BEGIN;
			Serial.println("plaatDia: 12inch");
			setPlateauRpm(rpm.r33);

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
		if(staatVeranderd.sinds() < 1000) { armHoekCentreer(); return; } // eerst ff centrere
		
		if(arm.naaldErop()){
			nieuwePos = karPos + limitF(armHoek * -karP, -3, 3);
			nieuwePos = limitF(nieuwePos, 0, plaatBegin);
			beweegKarNaarPos(nieuwePos, KAR_MAX_SNELHEID);
			
			if(karPos <= PLAAT_EINDE){
				Serial.println("kar heeft de limiet berijkt");
				stoppenOfHerhalen();//stoppen();
				return;
			}

			if(karPos < karPosFilter - 2.5){
				Serial.println("waarschijnlijk uitloop groef");
				stoppenOfHerhalen();//stoppen();
				return;
			}

			if(egteKarPos > karPosFilter + 2.5){
				setError(E_NAALD_TERUG_GELOPEN);  
				stoppen();
				return;
			}

			// if(naaldNaarVorenBewogen.sinds() > 6000){
			// 	setError(E_NAALD_NIET_BEWOGEN); //kar te lang niet bewogen
			// 	stoppen();
			// 	return;
			// }
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

		if(trackSensorPos > PLAAT_EINDE + 2  &&  plaatAanwezig  &&  arm.isNaaldEraf()){ 
			stoppen(); // als de naald erop is mag de plaat sensor wel afgaan     
		}
		return;
	}





	if(staat == S_CALIBREER){
		if(beweegKarNaarPos( SCHOONMAAK_PLEK, KAR_MAX_SNELHEID)){}
		return;
	}

}


  //  ================================================================
  //
  //              MOTOR
  //
  //  ================================================================


Interval karMotorInt(1000, MICROS);


bool karMotorUitvoeren()
{
	
	// karInterval = (micros() - karMotorInt.vorrigeVorrigeTijd) / 1000000.0;

	armHoekRuw += ( analogRead(hoekSensor) - armHoekRuw ) / 6;

  if(millis() > 1000){
    	if(armHoekRuw < armHoekMinCall){armHoekMinCall = armHoekRuw;}
      if(armHoekRuw > armHoekMaxCall){armHoekMaxCall = armHoekRuw;}
  }
	
	armHoekCall = mapF(armHoekRuw, armHoekMin, armHoekMax, 0, 1);
	
	armHoekDiv = armHoekCall - armHoekPrev;
	armHoekPrev = armHoekCall;

	armHoekSlow += (armHoekCall - armHoekSlow) / 100;

	armHoek = armHoekCall - armHoekOffset;

  
	if( staat == S_NAAR_BEGIN_PLAAT || 
      staat == S_UITROLLEN_VOOR_SPELEN || 
      staat == S_SPELEN || 
      staat == S_PAUZE || 
      staat == S_NAAR_NUMMER || 
      staat == S_DOOR_SPOELEN || 
      staat == S_TERUG_SPOELEN || 
      staat == S_UITROLLEN_NA_SPOELEN )
  {
		if(armHoekCall > 0.95){
			setError(E_ARMHOEK_LIMIET_POS);
			staat = S_HOK;
		}
		if(armHoekCall < 0.05){
			setError(E_ARMHOEK_LIMIET_NEG);
			staat = S_HOK;
		}
	}


	trackSensorPos = karPos - SENSOR_OFFSET;

	

	staatDingen();


	karDcomp *= 0.999;
	karDcomp += limitF(armHoekDiv * -karD, -KAR_MAX_SNELHEID, KAR_MAX_SNELHEID);//------------ Om oscilatie te voorkomen
	
	
	egteKarPos  = karPos + karDcomp;

	if(karUitMiddenCompAan)
	{
		karUitMiddenCompFilter +=  ( strobo.karFourierFilt - karUitMiddenCompFilter ) / 4;
		egteKarPos += karUitMiddenCompFilter;
	}


	karMotorPos = (egteKarPos + karOffset)  *  mm2stap;


	if(karMotorEnable)
	{
		if(antiCoggAan)
		{
			pwmStapperAntiCogging(-karMotorPos, stapperAP, stapperAN,  stapperBP, stapperBN, true);
		}
		else
		{
			pwmStapper(-karMotorPos, stapperAP, stapperAN,  stapperBP, stapperBN, true);
		}
	 
		
	
	}else{
		pwmDisableStapper(stapperAP, stapperAN,  stapperBP, stapperBN);
	}




	


	if(staat == S_SPELEN  &&  arm.isNaaldErop()){
		float div = karPos - karPosFilter;
		if(div < 0){
			karPosFilter += div / 1000;
						
		}

		if(karPos < karPosMinimaal){
			karPosMinimaal = karPos;
			naaldNaarVorenBewogen.reset();
		}
	}else{
		karPosFilter = karPos;
		karPosMinimaal = karPos;
		naaldNaarVorenBewogen.reset();
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






