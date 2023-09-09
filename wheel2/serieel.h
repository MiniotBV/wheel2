#include <utility>

bool golven = false;
bool karPIDveranderen = true;


String zin = "";
String zinRuw = "";
String vorrigeCommando = "";

#define COMMANDO_INFO 1
#define WAARDE_INFO 2




void printMetMarge(String waarde, int lengte){
  
  Serial.print(waarde);

  int marge = lengte - waarde.length(); // padding
  if(marge < 1)marge = 1;

  for(int i = 0; i < marge; i++){
    Serial.print(" ");      
  }
}



void printCommandoMetPadding(String vergelijking, String beschrijving, bool padding){
	printMetMarge(vergelijking, 8);

	Serial.print(" " + beschrijving);
}



void printWaardeMetPadding(String vergelijking, String beschrijving, String waarde){
	printMetMarge(vergelijking, 6);
  printMetMarge(beschrijving, 26);
  Serial.println(" " + waarde);
}






bool checkZin(String vergelijking, String beschrijving, int infoPrinten){
	
	if(infoPrinten == COMMANDO_INFO){
		printCommandoMetPadding(vergelijking, beschrijving, true);
		Serial.println();
		return false;
	}
	
	vergelijking.toLowerCase();
	if( ! zin.startsWith(vergelijking) ) return false;

	vorrigeCommando = vergelijking;
	zin.replace(vergelijking, "");
	zin.trim();
	
	printCommandoMetPadding(vergelijking, beschrijving, false);
	return true;
}





bool checkZinFunc( String vergelijking, String beschrijving, int infoPrinten, void (*func)(void)){//void (*f)){
  if(infoPrinten == WAARDE_INFO){
		return false;
	}
  
  if(!checkZin(vergelijking, beschrijving, infoPrinten)) return false;
  
  if( zin.indexOf('?') != -1){ // als er een '?' is
		// Serial.println(" <commando>");
    Serial.println(int(func));
    return true;
	}
  
  func();
  Serial.println();
	return true;
}






bool checkZinCommando(String vergelijking, String beschrijving, int infoPrinten){
  if(infoPrinten == WAARDE_INFO){
		return false;
	}
  
  if(!checkZin(vergelijking, beschrijving, infoPrinten)) return false;
	
  Serial.println();
	return true;
}





bool checkZinInt(String vergelijking, String beschrijving, int infoPrinten, int& waarde){
  if(infoPrinten == WAARDE_INFO){
		printWaardeMetPadding(vergelijking, beschrijving, String(waarde));
		return false;
	}
  
  if(!checkZin(vergelijking, beschrijving, infoPrinten)) return false;

	if( zin.indexOf('?') == -1  &&  zin.length() != 0){ // als er een '?' geen waarde zoeken en als er geen 0 letters meer inzitten
		waarde = zin.toInt();
		Serial.print(" gezet: ");    
	}
	else{
		Serial.print(" opgevraagd: ");
	}
		
	Serial.println(String(waarde));
	return true;
}



bool checkZinFloat(String vergelijking, String beschrijving, int infoPrinten,  float& waarde){
  if(infoPrinten == WAARDE_INFO){
		printWaardeMetPadding(vergelijking, beschrijving, String(waarde, 5));
		return false;
	}

	if(!checkZin(vergelijking, beschrijving, infoPrinten)) return false;

	if( zin.indexOf('?') == -1  &&  zin.length() != 0){ // als er een '?' geen waarde zoeken en als er geen 0 letters meer inzitten
		waarde = zin.toFloat();
		Serial.print(" gezet: ");    
	}
	else{
		Serial.print(" opgevraagd: ");
	}
		
	Serial.println(String(waarde, 5));
	return true;
}




bool checkZinBool(String vergelijking, String beschrijving, int infoPrinten, bool& waarde){
  if(infoPrinten == WAARDE_INFO){
		printWaardeMetPadding(vergelijking, beschrijving, String(waarde));
		return false;
	}

	if(!checkZin(vergelijking, beschrijving, infoPrinten)) return false;

	if(zin.indexOf('?') != -1){// als er een '?' is
		Serial.print(" opgevraagd: ");
	}
	else if( isDigit( zin.charAt(0) ) ){ // als er een '?' geen waarde zoeken en als er geen 0 letters meer inzitten
		waarde = zin.toInt();
		Serial.print(" gezet: ");    
	}
	// if(zin.indexOf('t') != -1 ){ // togle
	else{
		waarde = !waarde;
		Serial.print(" geflipt: ");
	}

		
	Serial.println(String(waarde));
	return true;
}




void infoPrintln(int info){
	if(info){
		Serial.println();
	}
}




void checkenVoorCommando(int info){

	infoPrintln(info);

  if(checkZinCommando("AT+", "bluetooth commando", info)){ bluetoothScrijf(zinRuw); return;}
  if(checkZinBool("BT", "bluetoot uart", info, bluetoothDebug)){return;}

	if(checkZinBool("g", "golven", info, golven)){return;}
	if(checkZinBool("PLG", "plaatLeesGolven", info, plaatLeesGolven)){return;}
	if(checkZinBool("KG", "karGolven", info, karGolven)){return;}
	if(checkZinBool("SG", "strobo.golven", info, strobo.golven)){return;}



	//-------------------------------------------------STAAT   
	infoPrintln(info);     
	if(checkZinFunc(">>", "naarVolgendNummer()", info, naarVolgendNummer)){ return;}
	if(checkZinFunc("<<", "naarVorrigNummer()", info, naarVorrigNummer)){ return;}
	if(checkZinCommando("hok", "S_HOK", info)){ setStaat(S_HOK); return;}
	if(checkZinFunc("stop", "stoppen()", info, stoppen)){return;}
	if(checkZinFunc("spelen", "spelen()", info, spelen)){return;}
	if(checkZinFunc("pauze", "pauze()", info, pauze)){return;}
	if(checkZinCommando("schoonmaak", "S_SCHOONMAAK", info)){ setStaat(S_SCHOONMAAK); return;}
	if(checkZinCommando("cal", "S_CALIBREER", info)){ setStaat(S_CALIBREER); return;}
	if(checkZinBool("rep", "herhaalDeHelePlaat", info, herhaalDeHelePlaat)){return;}




	//----------------------------------------------------ARM
	infoPrintln(info);
	if(checkZinCommando("NE", "naaldErop()", info)){  arm.naaldErop(); return;}
	if(checkZinCommando("NA", "naaldEraf()", info)){  arm.naaldEraf(); return;}
	if(checkZinFloat("ATG", "arm.targetGewicht", info, arm.targetGewicht)){return;}
	if(checkZinCommando("AKL", "armKracht500mg calibreer", info)){    arm.krachtLaag = arm.kracht;   Serial.println("armKracht500mg: "  + String(arm.krachtLaag, 5));  return;}
	if(checkZinCommando("AKH", "armKracht500mg calibreer", info)){  arm.krachtHoog = arm.kracht; Serial.println("armKracht4000mg: " + String(arm.krachtHoog, 5));   return;}


	//-------------------------------------------------------KAR SENSORS / TRACK SHIT
	infoPrintln(info);
	if(checkZinFloat("PLS", "plaatLeesStroom", info, plaatLeesStroom)){return;}
	if(checkZinInt("VOL", "volume", info, volume)){ volumeOverRide = true; return;}

	if(checkZinFloat("TO", "trackOffset", info, trackOffset)){return;}

	if(checkZinFunc("AHCent", "armHoekCentreer()", info,  armHoekCentreer)){  return;}
	if(checkZinFunc("AHCal", "armHoekCalibreer()", info, armHoekCalibreer)){  return;}


	//--------------------------------------------------------KAR
	infoPrintln(info);
	if(checkZinFloat("KP", "karP", info, karP)){return;}
	if(checkZinFloat("KI", "karI", info, karI)){return;}
	if(checkZinFloat("KD", "karD", info, karD)){return;}


	//----------------------------------------------------PLATEAU
	infoPrintln(info);
	if(checkZinFloat("PP", "plateauP", info, plateauP)){return;}
	if(checkZinFloat("PI", "plateauI", info, plateauI)){return;}
	if(checkZinFloat("PD", "plateauD", info, plateauD)){return;}

	if(checkZinFloat("TR", "targetRpm", info, targetRpm)){draaienInterval.reset(); return;}

	if(checkZinFunc("PA", "plateauDraaien()", info, plateauDraaien)){  return;}
	if(checkZinFunc("PS", "plateauStoppen()", info, plateauStoppen)){  return;}
	if(checkZinBool("PL", "plateauLogica", info, plateauLogica)){return;}
	if(checkZinBool("PC", "onbalansComp", info, onbalansComp)){return;}


	//----------------------------------------------------------------------STROBO
	infoPrintln(info);
	if(checkZinInt("SSN", "strobo.sampleNum", info, strobo.sampleNum)){return;}
	if(checkZinBool("SOC", "strobo.onbalansCompAan", info, strobo.onbalansCompAan)){return;}

  if(checkZinInt("SHVA", "strobo.harmVerschuiving[1]", info, strobo.harmVerschuiving[1])){return;}
  if(checkZinInt("SHVB", "strobo.harmVerschuiving[2]", info, strobo.harmVerschuiving[2])){return;}
  if(checkZinInt("SHVC", "strobo.harmVerschuiving[3]", info, strobo.harmVerschuiving[3])){return;}

  if(checkZinFloat("SHEA", "strobo.harmEffect[1]", info, strobo.harmEffect[1])){return;}
  if(checkZinFloat("SHEB", "strobo.harmEffect[2]", info, strobo.harmEffect[2])){return;}
  if(checkZinFloat("SHEC", "strobo.harmEffect[3]", info, strobo.harmEffect[3])){return;}

  if(checkZinInt("SOFH", "strobo.onbalansHarm", info, strobo.onbalansHarm)){return;}
	if(checkZinInt("SOF", "strobo.onbalansFase", info, strobo.onbalansFase)){return;}
	if(checkZinFloat("SOG", "strobo.onbalansCompGewicht", info, strobo.onbalansCompGewicht)){return;}
  if(checkZinInt("SOH", "strobo.harmonisen", info, strobo.harmonisen)){return;}
	if(checkZinBool("SKC", "strobo.plaatUitMiddenComp", info, strobo.plaatUitMiddenComp)){return;}
	if(checkZinBool("KC", "karUitMiddenCompAan", info, karUitMiddenCompAan)){return;}
	
	if(checkZinCommando( "SCZ", "strobo.clearCompSamplesOpTellerNull()", info)){   strobo.clearCompSamplesOpTellerNull(); return;}
	if(checkZinCommando( "SCC", "strobo.clearCompSamples()", info)){   strobo.clearCompSamples(); return;}


	//------------------------------------------------------OPSLAG
	infoPrintln(info);
	if(checkZinFloat("EV", "eepromVersie", info, eepromVersie)){return;}
	if(checkZinFunc("EO", "eepromOpslaan()",  info, eepromOpslaan)){    return;}
	if(checkZinFunc("EL", "eepromUitlezen()", info, eepromUitlezen)){   return;}
	
	if(checkZinCommando("OC", "orientatie.calibreerOrientatie()", info)){ orientatie.calibreer(); return;}




	//------------------------------------------------------HELP
	infoPrintln(info);
	if(checkZinCommando("C?", "commandos", info)){ checkenVoorCommando(COMMANDO_INFO);return;}
  if(checkZinCommando("CW", "waardes", info)){ checkenVoorCommando(WAARDE_INFO);return;}

	if(checkZinCommando("?", "help", info)){
		Serial.println("\nhelp -- versie: " + String(versie) + " ----------------------------");
    Serial.println();

		Serial.println("plateau P: " + String(plateauP, 5));
		Serial.println("plateau I: " + String(plateauI, 5));
		Serial.println("plateau D: " + String(plateauD, 5));
		Serial.println();

		Serial.println("kar P: " + String(karP, 5));
		Serial.println("kar I: " + String(karI, 5));
		Serial.println("kar D: " + String(karD, 5));
		Serial.println();
		
		Serial.println("staat: " + printStaat(staat));
		Serial.println("volume: " + String(volume));
    Serial.println("draadlozeVersie: " + String(draadlozeVersie));

		Serial.println();
		
		// Serial.println("kracht: " + String(kracht));
		// Serial.println("gewicht: " + String(gewicht));
		// Serial.println("isNaaldErop(): " + String(isNaaldErop()));
		// Serial.println();

		// Serial.println("trackOffset: " + String(trackOffset));
		// Serial.println();


		eepromPrint();
		Serial.println();
		
		
		// printKnoppen();
		// orientatie.print();
		// Serial.println();

		// Serial.println("strobo.onbalansFase: " + String(strobo.onbalansFase));
    // Serial.println("strobo.onbalansHarm: " + String(strobo.onbalansHarm));
    Serial.println("strobo.harmVerschuiving[1]" + String(strobo.harmVerschuiving[1]));
    Serial.println("strobo.harmVerschuiving[2]" + String(strobo.harmVerschuiving[2]));
    Serial.println("strobo.harmVerschuiving[3]" + String(strobo.harmVerschuiving[3]));

    Serial.println("strobo.harmEffect[1]" + String(strobo.harmEffect[1]));
    Serial.println("strobo.harmEffect[2]" + String(strobo.harmEffect[2]));
    Serial.println("strobo.harmEffect[3]" + String(strobo.harmEffect[3]));
  
		Serial.println("strobo.sampleNum: " + String(strobo.sampleNum));
		// Serial.println();


		Serial.println("-----------------------------------------------");
		return;
	}


	Serial.println("fout commando:\"" + zin + "\"");
}







Interval serieelInt(10000, MICROS);
// Interval serieelInt(5000, MICROS);

void serieelFunc(){
	if(serieelInt.loop()){
		if(golven){
			
			
			
			Serial.print(strobo.vaartRuw - targetRpm, 3);
      Serial.print(", ");
			Serial.print(strobo.vaart - centerCompTargetRpm, 3);

      Serial.print(", ");
			Serial.print(strobo.vaart, 3);
      Serial.print(", ");
			Serial.print((float)strobo.teller / strobo.pulsenPerRev, 3);
      // Serial.print(", ");
			// Serial.print(strobo.vaartLowPass, 3);
			


			// Serial.print(", ");
			// Serial.print(strobo.vaartCenterComp - targetRpm, 3);


      // for(int i = 1; i < strobo.harmonisen + 1; i++){
      //   Serial.print(", ");
      //   Serial.print(strobo.onbalansSinTotaal[i], 3);
      //   Serial.print(", ");
      //   Serial.print(strobo.onbalansCosTotaal[i], 3);
      // }


      for(int i = 1; i < strobo.harmonisen + 1; i++){
        Serial.print(", ");
        Serial.print(atan2(strobo.onbalansCosTotaal[i], strobo.onbalansSinTotaal[i]) * 114.6, 3);
        Serial.print(", ");
        Serial.print(sqrt( strobo.onbalansSinTotaal[i] * strobo.onbalansSinTotaal[i]   +  strobo.onbalansCosTotaal[i] * strobo.onbalansCosTotaal[i]), 3);
      }



      


			// Serial.print(", ");
			// Serial.print(strobo.vaartLowPass - targetRpm, 3);
      // Serial.print(", ");
			// Serial.print(strobo.lowpassRect, 3);
      // Serial.print(", ");
			// Serial.print(strobo.wow, 3);

      // Serial.print(", ");
			// Serial.print(strobo.vaartHighPass, 3);

			Serial.print(", ");
			Serial.print(centerCompTargetRpm - targetRpm, 3);



			Serial.print(", ");
			Serial.print(strobo.teller / float(strobo.pulsenPerRev));

			// Serial.print(", ");
			// Serial.print(strobo.preComp, 4);

			// Serial.print(", ");
			// Serial.print(strobo.onbalansComp, 4);

			Serial.print(", ");
			Serial.print(uitBuff, 2);

			// Serial.print(", ");
			// Serial.print(uitBuffPre, 2);

			

			// Serial.print(", ");
			// Serial.print(armHoekRuw);//1696);
			Serial.print(", ");
			Serial.print(armHoekCall, 4);//1696);
			// Serial.print(", ");
			// Serial.print(armHoekSlow, 5);//1696);
			// Serial.print(", ");
			// Serial.print(armHoekOffset, 5);//1696);






			// Serial.print(", ");
			// Serial.print(karPos, 3);  
			// Serial.print(", ");
			// Serial.print(egteKarPos, 3);

			// // Serial.print(", ");
			// // Serial.print(karPosMidden, 3);

			// // Serial.print(", ");
			// // Serial.print(karPosMidden + strobo.karFourier, 3);  

			// Serial.print(", ");
			// Serial.print(karPosMidden + strobo.karFourierFilt, 3);  


      // Serial.print(", ");
			// Serial.print(spoorafstand, 3);





			Serial.println();
		}





		while(Serial.available() > 0){
		
			char letter = Serial.read();
			
			if( ( letter == '\n' || letter == '\r' )&& zin != ""){
				
        zinRuw = zin;
        zin.trim();
				zin.toLowerCase();

				if(zin.startsWith("l")){zin.replace("l", vorrigeCommando);} // 'L' is laatste commando voeg laatste commando toe aan zin

				checkenVoorCommando(false);
				

				zin = "";

			}else{
				zin += letter;
			}
		}





	}
}










//             bytes   cycles                
// LD (HL),d8      2   12
// INC L           1   4
//                 3   16

// LD (HL),d8      2   12
// INC HL          1   8
//                 3   20

// LD (HL+),A      1   8
// LD A,d8         2   8
//                 3   16                

// LD A, [DE]      1   8
// LD (HL+),A      1   8
//                 2   16

// LD SP,d16       3   12
// LD (a16),SP     3   20
//                 6   32



