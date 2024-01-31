
bool golven = false;
bool karPIDveranderen = true;


String zin = "";
String zinRuw = "";
String vorrigeCommando = "";

#define COMMANDO_INFO 1
#define WAARDE_INFO 2




String debugBericht = "";

void debug(String bericht){
  debugBericht += bericht + '\n';
}

void printDebug(){
  if(debugBericht.length() != 0){
    Serial.print(debugBericht);
    debugBericht = "";
  }
}






// void printMetMarge(String waarde, int lengte){
//   Serial.print(voegMargeToe(waarde, lengte));
// }



void printCommandoMetPadding(String vergelijking, String beschrijving, bool padding){
	Serial.print( voegMargeToe(vergelijking, 8) + " " + beschrijving);
}



void printWaardeMetPadding(String vergelijking, String beschrijving, String waarde){
	Serial.println(  voegMargeToe(vergelijking, 6) + voegMargeToe(beschrijving, 26) +  " " + waarde);
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
  if(checkZinCommando("RST",    "herstart",       info)){ rp2040.reboot(); return;}
  // if(checkZinCommando("BOOT",   "boot sel",       info)){ rp2040.reset_to_usb_boot(); return;}

  if(checkZinCommando("AT+",    "bluetooth commando",       info)){ bluetoothScrijf(zinRuw); return;}
  if(checkZinBool(    "BT",     "bluetoot uart",            info, bluetoothDebug)){return;}

	if(checkZinBool(    "g",      "golven",                   info, golven)){return;}
	if(checkZinBool(    "PLG",    "plaatLeesGolven",          info, plaatLeesGolven)){return;}
	if(checkZinBool(    "KG",     "karGolven",                info, karGolven)){return;}
	if(checkZinBool(    "SG",     "strobo.golven",            info, strobo.golven)){return;}
  if(checkZinBool(    "OG",     "orientatie.golven",            info, orientatie.golven)){return;}


	//-------------------------------------------------STAAT   
	infoPrintln(info);     
	if(checkZinCommando(">>",     "naarVolgendNummer()",      info)){naarVolgendNummer(); return;}
	if(checkZinCommando("<<",     "naarVorrigNummer()",       info)){ naarVorrigNummer(); return;}
	if(checkZinCommando("hok",    "setStaat(S_HOK)",          info)){ setStaat(S_HOK); return;}
	if(checkZinCommando("stop",   "stoppen()",                info)){ stoppen(); return;}
	if(checkZinCommando("speel",  "spelen()",                 info)){ spelen(); return;}
	if(checkZinCommando("pauze",  "pauze()",                  info)){pauze(); return;}
	if(checkZinCommando("naald",  "S_NAALD_SCHOONMAAK",             info)){ setStaat(S_NAALD_SCHOONMAAK); return;}
	if(checkZinCommando("cal",    "S_CALIBREER",              info)){ setStaat(S_CALIBREER); return;}
	if(checkZinBool(    "rep",    "herhaalDeHelePlaat",       info, herhaalDeHelePlaat)){return;}




	//----------------------------------------------------ARM
	infoPrintln(info);
	if(checkZinCommando("NE",     "naaldErop()",              info)){   arm.naaldErop(); return;}
	if(checkZinCommando("NA",     "naaldEraf()",              info)){   arm.naaldEraf(); return;}
	if(checkZinFloat(   "ATG",    "arm.targetGewicht",        info,     arm.targetGewicht)){       eepromNogOpslaan = true; return;}
  if(checkZinFloat(   "AG",     "arm.gewicht",              info,     arm.gewicht)){return;}
  if(checkZinCommando("AKHOK",  "armKrachtHok calibreer",   info)){   arm.netInHokGewicht = arm.gewicht; Serial.println("arm.netInHokGewicht: " + String(arm.netInHokGewicht, 5));   eepromNogOpslaan = true;   return;}
	if(checkZinCommando("AKL",    "armKracht500mg calibreer", info)){   arm.krachtLaag = arm.kracht;   Serial.println("armKracht500mg: "  + String(arm.krachtLaag, 5));     eepromNogOpslaan = true;  return;}
	if(checkZinCommando("AKH",    "armKracht4000mg calibreer",info)){   arm.krachtHoog = arm.kracht; Serial.println("armKracht4000mg: " + String(arm.krachtHoog, 5));       eepromNogOpslaan = true;  return;}
  if(checkZinFloat(   "AK",     "arm.kracht",               info,     arm.kracht)){ arm.kracht = limieteerF(arm.kracht, 0, 1); return;}



	//--------------------------------------------------------KAR
	infoPrintln(info);
	if(checkZinFloat(   "KP",     "karP",                     info, karP)){return;}
	if(checkZinFloat(   "KI",     "karI",                     info, karI)){return;}
	if(checkZinFloat(   "KD",     "karD",                     info, karD)){return;}
  if(checkZinFloat(   "TNP",    "targetNummerPos",          info, targetNummerPos)){ targetNummerPos = limieteerF(targetNummerPos, KAR_HOK, ELPEE_PLAAT_BEGIN);  return;}

	//----------------------------------------------------PLATEAU
	infoPrintln(info);
	if(checkZinFloat(   "PP",     "plateauP",                 info, plateauP)){return;}
	if(checkZinFloat(   "PI",     "plateauI",                 info, plateauI)){return;}
	if(checkZinFloat(   "PD",     "plateauD",                 info, plateauD)){return;}

	if(checkZinFloat(   "TR",     "targetRpm",                info, targetRpm)){draaienInterval.reset(); return;}

	if(checkZinCommando("PA",     "plateauDraaien()",         info)){ plateauDraaien(); return;}
	if(checkZinCommando("PS",     "plateauStoppen()",         info)){ plateauStoppen(); return;}
	if(checkZinBool(    "PL",     "plateauLogica",            info, plateauLogica)){return;}
	if(checkZinBool(    "PC",     "onbalansComp",             info, onbalansComp)){return;}


	//----------------------------------------------------------------------STROBO
	infoPrintln(info);
  

	// if(checkZinInt("SSN", "strobo.sampleNum", info, strobo.sampleNum)){return;}
	if(checkZinBool(    "SOC",    "strobo.onbalansCompAan",   info, strobo.onbalansCompAan)){return;}
  if(checkZinBool(    "SKC",    "strobo.plaatUitMiddenComp",info, strobo.plaatUitMiddenComp)){return;}
	if(checkZinBool(    "KC",     "karUitMiddenCompAan",      info, karUitMiddenCompAan)){return;}


	if(checkZinFloat(   "SOG",    "strobo.onbalansCompGewicht", info, strobo.onbalansCompGewicht)){return;}
  if(checkZinFloat(   "SOFB",  "strobo.onbalansFilterBreedte",  info, strobo.onbalansFilterBreedte)){strobo.maakOnbalansFilterCurve(); return;}
  if(checkZinInt(     "SOF",    "strobo.onbalansFase",      info, strobo.onbalansFase)){return;}

	if(checkZinCommando("SCZ",    "strobo.clearCompSamplesOpTellerNull()", info)){   strobo.clearCompSamplesOpTellerNull(); return;}
	if(checkZinCommando("SCC",    "strobo.clearCompSamples()", info)){   strobo.clearCompSamples(); return;}


	//------------------------------------------------------OPSLAG
	infoPrintln(info);
	if(checkZinFloat(   "EV",     "eepromVersie",             info, eepromVersie)){return;}
	if(checkZinCommando("EO",     "eepromOpslaan()",          info)){ eepromOpslaan();   return;}
	if(checkZinCommando("EL",     "eepromUitlezen()",         info)){ eepromUitlezen();  return;}
	if(checkZinCommando("OC",     "orientatie.calibreerOrientatie()", info)){ orientatie.calibreer(); eepromNogOpslaan = true;return;}
	if(checkZinFloat(   "TO",     "trackOffset",              info, trackOffset)){eepromNogOpslaan = true;   return;}
	if(checkZinCommando("AHCal",  "armHoekCalibreer()",       info)){ armHoekCalibreer(); eepromNogOpslaan = true;return;}



  	//-------------------------------------------------------KAR SENSORS / TRACK SHIT
	infoPrintln(info);
	// if(checkZinFloat("PLS",       "plaatLeesStroom",          info, plaatLeesStroom)){return;}
	if(checkZinInt(     "VOL",    "volume",                   info, volume)){ volumeOverRide = true; return;}
	if(checkZinCommando("AHCent", "armHoekCentreer()",        info)){ armHoekCentreer(); return;}





	//------------------------------------------------------HELP
	infoPrintln(info);
	if(checkZinCommando("C?",     "commandos",                info)){ checkenVoorCommando(COMMANDO_INFO);return;}
  if(checkZinCommando("CW",     "waardes",                  info)){ checkenVoorCommando(WAARDE_INFO);return;}

	if(checkZinCommando("?",      "help",                     info)){
		Serial.println("\nHELP ----- V" + String(versie) + " ----------------------------");
    Serial.println();

		// Serial.println("plateau P: " + String(plateauP, 5));
		// Serial.println("plateau I: " + String(plateauI, 5));
		// Serial.println("plateau D: " + String(plateauD, 5));
		// Serial.println();

		// Serial.println("kar P: " + String(karP, 5));
		// Serial.println("kar I: " + String(karI, 5));
		// Serial.println("kar D: " + String(karD, 5));
		// Serial.println();
		
		Serial.println("staat: " + printStaat(staat));
		Serial.println("volume: " + String(volume));
    Serial.println("draadlozeVersie: " + String(draadlozeVersie?"ja":"nee"));
    Serial.println("temperatuur rp2040: " + String( analogReadTemp() , 2));

		Serial.println();
		
		// Serial.println("kracht: " + String(kracht));
		// Serial.println("gewicht: " + String(gewicht));
		// Serial.println("isNaaldErop(): " + String(isNaaldErop()));
		// Serial.println();

		eepromPrint();
		Serial.println();
		
		
		// printKnoppen();
		orientatie.print();
		// Serial.println();

		// Serial.println("strobo.onbalansFase: " + String(strobo.onbalansFase));
    // Serial.println("strobo.onbalansCompGewicht: " + String(strobo.onbalansCompGewicht));
    // Serial.println("strobo.onbalansFilterBreedte: " + String(strobo.onbalansFilterBreedte));
    // Serial.println("strobo.onbalansFilterCurveBreedte: " + String(strobo.onbalansFilterCurveBreedte));
    

  
		// Serial.println("strobo.sampleNum: " + String(strobo.sampleNum));



		Serial.println("-----------------------------------------------");
		return;
	}


	Serial.println("fout commando:\"" + zin + "\"");
}







Interval serieelInt(10000, MICROS);
// Interval serieelInt(5000, MICROS);

void serieelFunc(){
	if(serieelInt.loop()){
		printDebug();
    
    if(golven){
			
			
			
			Serial.print(strobo.vaartRuw - targetRpm, 3);
      Serial.print(", ");
			Serial.print(strobo.vaart - centerCompTargetRpm, 3);

      // Serial.print(", ");
			// Serial.print(strobo.vaart, 3);
      Serial.print(", ");
			Serial.print((float)strobo.teller / strobo.pulsenPerRev, 3);

      // // Serial.print(", ");
			// // Serial.print(strobo.onbalansFilterCurve[strobo.teller]);


      
      // // Serial.print(", ");
			// // Serial.print(strobo.vaartLowPass, 3);


      
      // // Serial.print(", ");
			// // Serial.print(strobo.procesInterval);
      


			// // Serial.print(", ");
			// // Serial.print(strobo.vaartLowPass - targetRpm, 3);
      // // Serial.print(", ");
			// // Serial.print(strobo.lowpassRect, 3);
      // // Serial.print(", ");
			// // Serial.print(strobo.wow, 3);

      // // Serial.print(", ");
			// // Serial.print(strobo.vaartHighPass, 3);

			// Serial.print(", ");
			// Serial.print(centerCompTargetRpm - targetRpm, 3);



			// // Serial.print(", ");
			// // Serial.print(strobo.teller / float(strobo.pulsenPerRev));

			// // Serial.print(", ");
			// // Serial.print(strobo.preComp, 4);

			// Serial.print(", ");
			// Serial.print(strobo.onbalansComp, 4);

			// // Serial.print(", ");
			// // Serial.print(uitBuff, 2);

			// // Serial.print(", ");
			// // Serial.print(uitBuffPre, 2);

			

			// // Serial.print(", ");
			// // Serial.print(armHoekRuw);//1696);
      
			// // Serial.print(", ");
			// // Serial.print(karDcomp, 4);//1696);
      // Serial.print(", ");
			// Serial.print(armHoekCall, 4);//1696);
      
			// // Serial.print(", ");
			// // Serial.print(armHoekSlow, 5);//1696);
			// // Serial.print(", ");
			// // Serial.print(armHoekOffset, 5);//1696);






			// // Serial.print(", ");
			// // Serial.print(karPos, 3);  
			// Serial.print(", ");
			// Serial.print(egteKarPos, 3);

			// // // Serial.print(", ");
			// // // Serial.print(karPosMidden, 3);

			// // // Serial.print(", ");
			// // // Serial.print(karPosMidden + strobo.karFourier, 3);  

			// // Serial.print(", ");
			// // Serial.print(karPosMidden + strobo.karFourierFilt, 3);  


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



