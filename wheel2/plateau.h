
bool plateauLogica = true;
bool onbalansComp = true;

float plateauP = 1;//0.5;//plateau33P;    //pid
float plateauI = 0.02;//plateau33I;
float plateauD = 0;


float basis = 0;
float uitBuff;
float uitBuffPre;




enum rpmStaats{
	AUTO,
	R33,
	R45,
	R78
};


enum rpmStaats rpmStaat = AUTO;



float autoRpm;

void updatePlateauRpm(){
	if(!plateauAan){
		return;    
	}

	if(rpmStaat == AUTO){
		targetRpm = autoRpm;
		return;
	}
	if(rpmStaat == R33){
		targetRpm = rpm33;
		return;
	}
	if(rpmStaat == R45){
		targetRpm = rpm45;
		return;
	}
}





bool plateauDebug = true;

void plateauPrint(String bericht){
	if(plateauDebug){
		debug("plateau: " + bericht);
	}
}





void setPlateauRpm(float rpm){
	if(rpm == targetRpm){return;}//als er nisk veranderd is, hoeft er niks gerestet te worden
	autoRpm = rpm;

	updatePlateauRpm();
	
	strobo.clearCompSamples();
	draaienInterval.reset();
	// opsnelheid = false;
}






void plateauDraaien(){
	plateauAan = true;
	setPlateauRpm(rpm33);

	basis = 30;//50;//40;//60;//75;
	
	// strobo.clearCompSamples();
	// draaienInterval.reset();
	
	plateauPrint("aan");
}






void plateauStoppen(){
	plateauAan = false;
	targetRpm = 0;
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


	float divTarget = targetRpm - rpmIn;


	if(onbalansComp){
		pp = divTarget * plateauP;
    


    // if(rpmIn > 15){
    //   basis += divTarget * plateauI;            //breng basis voltage naar gemiddelde voor de juiste snelheid	
    // }else{
    //   basis += divTarget * plateauI/20;            //breng basis voltage naar gemiddelde voor de juiste snelheid	
    // }

    basis += divTarget * plateauI;            //breng basis voltage naar gemiddelde voor de juiste snelheid	
    basis = limieteerF(basis, 40, 80);

		pd = divTijd * plateauD;
	}

	return  pp + basis + pd;
}








void plateauStaatDingen(){

	if(!plateauLogica){return;}
  if(staat == S_PLAAT_SCHOONMAAK){return;} 
	
	float vaart = strobo.vaart;//gladglad;



	if(plateauAan){                   //staat de motor aan?

		if(staat == S_FOUTE_ORIENTATIE  ||
			staat == S_HOK// ||
      // staat == S_PARKEREN ||
	    // staat == S_NAAR_HOK
		){
			plateauPrint("was nog aant draaien ofzo? IETS FOUT!!!!!");
			plateauStoppen();
			return;
		}
		

		if( opsnelheid == true){ //         tegen gehouden
			
			// if(vaart > targetRpm * 4 ){ //te snel 200%
			// 	plateauPrint("tesnel");
			// 	stoppen();
			// 	return;
			// }
      if(vaart  <  targetRpm * 0.65  &&   draaienInterval.sinds() > 1500){ //te langzaam 70%
				plateauPrint("tegengehouden");
				stoppen();
				return;
			}
		}

		
		
		if(  opsnelheid == false    ){ //                                                 tegen gehouden
			
			if(vaart  >  targetRpm * 0.95){ //                       op snelheid (95% van de snelheid)
				plateauPrint("opsnelheid");
				opsnelheid = true;
				return;
			}

			if(vaart  <  targetRpm * 0.1   &&     draaienInterval.sinds() > 750){//   <5% target snelheid na een kort tijd
				
				plateauPrint("kon niet opgang komen");
				stoppen();//--------------------------------------------
				return;
			}
		}
	
	
	
	}else{
		if(draaienInterval.sinds() > 1000   &&   uitdraaien == false){//                              aangeslingerd
			if(vaart  >  rpm33 * 0.666   &&   (staat == S_HOK || staat == S_NAAR_HOK || staat == S_PARKEREN)){                                                       //50% van de 33.3 snelheid
				plateauPrint("aangeslingerd");
				spelen();
				return;
			}
		}

		if(uitdraaien == true && vaart < rpm33 * 0.05){ //     <5% van de 33.3 snelheid                                                    uitgedraaid
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

    // if(staat == S_PLAAT_SCHOONMAAK){ // als de plaat schoonmaak stand aanstaad kan de motor gewoon een voltage krijgen
    //   pwmFase(0.6, motorN, motorP, false);
    //   return;
    // }

		float vaart = strobo.vaartCenterComp;

		if(plateauAan){             //staat de motor aan?


			uitBuff = pid(vaart);//                  bereken motor kracht
			uitBuffPre = uitBuff;
			
      uitBuff += strobo.onbalansComp;

			uitBuff = limieteerF(uitBuff, -100, 100);
			
			pwmFase(uitBuff / 100.0, motorN, motorP, false); // ouwe motor
      // pwmFase(uitBuff / 100.0, motorP, motorN, false); //nieuwe motor
			
		}else{
			
			pwmFase(0, motorP, motorN, false);
			basis = 0; // reset I
		}


 
		plateauStaatDingen();

	}
}








