
bool plateauLogica = true;
bool onbalansComp = true;

// float plateauP = 0.005;//plateau33P;    //pid
// float plateauI = 0.03;//plateau33I;
// float plateauD = 0;

// float plateauP = 1;//plateau33P;    //pid
// float plateauI = 0.1;//plateau33I;
// float plateauD = -3;

float plateauP = 1;//0.5;//plateau33P;    //pid
float plateauI = 0.02;//plateau33I;
float plateauD = 0;


float basis = 0;
float uitBuff;
float uitBuffPre;



// enum rpmStaats { AUTO, R33, R45, R78 }; 		is nu rpm.rpmSetting
// enum rpmStaats rpmStaat = AUTO;				is nu rpm.myRpm
// float autoRpm;								is nu rpm.autoRpm

void updatePlateauRpm()
{
	if(!isPlateauAan) return;
  rpm.setNewTargetRpm();

	// if(rpmStaat == AUTO){
	// 	targetRpm = autoRpm;
	// 	return;
	// }
	// if(rpmStaat == R33){
	// 	targetRpm = rpm33;
	// 	return;
	// }
	// if(rpmStaat == R45){
	// 	targetRpm = rpm45;
	// 	return;
	// }
}





bool plateauDebug = true;

void plateauPrint(String bericht)
{
	if(plateauDebug) { Serial.println("plateau: " + bericht); }
}


void setPlateauRpm(float newRpm)
{
	if(newRpm == rpm.targetRpm) return;  // no change=>return
	rpm.autoRpm = newRpm;

	updatePlateauRpm();
	
	strobo.clearCompSamples();
	draaienInterval.reset();
	// opsnelheid = false;
}



void plateauDraaien(){
	isPlateauAan = true;
	setPlateauRpm(rpm.r33);

	basis = 50;	//40;//60;//75;
	
	// strobo.clearCompSamples();
	// draaienInterval.reset();
	
	plateauPrint("aan");
}






void plateauStoppen(){
	isPlateauAan = false;
	rpm.targetRpm = 0;
	draaienInterval.reset();
	uitdraaien = true;
	opsnelheid = false;

	plateauPrint("uit");
}








void plateauInit(){
	setPwm(motorP);
	setPwm(motorN);
}













float vorrigeVaart = 0;
float pp, pd;


float pid(float rpmIn){

	float divTijd = rpmIn - vorrigeVaart;
	vorrigeVaart = rpmIn;


	float divTarget;
	if(strobo.plaatUitMiddenComp){
		divTarget = centerCompTargetRpm - rpmIn;
	}else{
		divTarget = rpm.targetRpm - rpmIn;
	}



	if(onbalansComp){
		pp = divTarget * plateauP;

    basis += divTarget * plateauI;            //breng basis voltage naar gemiddelde voor de juiste snelheid	
		basis = limitF(basis, 45, 80);

		pd = divTijd * plateauD;
	}
	

	return  pp + basis + pd;
}








void plateauStaatDingen(){

	if(!plateauLogica){return;}  
	
	float glad = strobo.glad;//gladglad;



	if(isPlateauAan){                   //staat de motor aan?

		if(staat == S_FOUTE_ORIENTATIE  ||
			staat == S_HOK ||
      staat == S_PARKEREN ||
	    staat == S_NAAR_HOK
		){
			plateauPrint("was nog aan 't draaien ofzo? IETS FOUT!!!!!");
			plateauStoppen();
			return;
		}
		

		if( opsnelheid == true){ //         tegen gehouden
			
			if(glad > rpm.targetRpm * 4 ){ //te snel 200%
				plateauPrint("te snel");
				stoppen();
				return;
			}
			else if(glad < rpm.targetRpm * 0.65 && draaienInterval.sinds() > 1000){ //te langzaam 70%
				plateauPrint("tegengehouden");
				stoppen();
				return;
			}
		}

		
		
		if(opsnelheid == false){ //                                                 tegen gehouden
			
			if(glad > rpm.targetRpm * 0.95){ //                       op snelheid (95% van de snelheid)
				plateauPrint("op snelheid");
				opsnelheid = true;
				return;
			}

			if(glad < rpm.targetRpm * 0.1 && draaienInterval.sinds() > 500){//   <5% target snelheid na een kort tijd
				
				plateauPrint("kon niet opgang komen");
				stoppen();//--------------------------------------------
				return;
			}
		}
	
	
	
	}else{
		if(draaienInterval.sinds() > 1000 && uitdraaien == false){//                              aangeslingerd
			if(glad > rpm.r33 * 0.50 && staat == S_HOK){                                                       //50% van de 33.3 snelheid
				plateauPrint("aangeslingerd");
				spelen();
				return;
			}
		}

		if(uitdraaien && glad < rpm.r33 * 0.05)			//	<5% speed               uitgedraaid
		{ 
			uitdraaien = false;
			draaienInterval.reset();
			plateauPrint("uitgedraaid");
			return;
		}
	}
	
}











Interval plateauInt(5000, MICROS);

void plateauFunc(){

	if(plateauInt.loop()){

		strobo.update();
		float vaart = strobo.glad;

		if(isPlateauAan){             //staat de motor aan?


			uitBuff = pid(vaart);//                  bereken motor kracht
			uitBuffPre = uitBuff;
			
      uitBuff += strobo.onbalansComp;
      // uitBuff /= strobo.onbalansComp;

			uitBuff = limitF(uitBuff, -100, 100);
			
			pwmFase(uitBuff / 100.0, motorN, motorP, false);
			
		}else{
			
			pwmFase(0, motorP, motorN, false);
			basis = 0; // reset I
		}


 
		plateauStaatDingen();

	}
}




