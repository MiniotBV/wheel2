
bool golven = false;
bool karPIDveranderen = true;


String zin = "";
String vorrigeCommando = "";


void printCommandoMetPadding(String vergelijking, String beschrijving, bool padding){
  Serial.print(vergelijking);

  if(padding){
    int padding = 18 - vergelijking.length(); // padding
    for(int i = 0; i < padding; i++){
      Serial.print(" ");      
    }
  }  


  Serial.print(" " + beschrijving);
}



bool checkZin(String vergelijking, String beschrijving, bool infoPrinten){
  


  if(infoPrinten){
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



bool checkZinI(String vergelijking, String beschrijving, bool infoPrinten, int& waarde){
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



bool checkZinF(String vergelijking, String beschrijving, bool infoPrinten,  float& waarde){
  if(!checkZin(vergelijking, beschrijving, infoPrinten)) return false;

  if( zin.indexOf('?') == -1  &&  zin.length() != 0){ // als er een '?' geen waarde zoeken en als er geen 0 letters meer inzitten
    waarde = zin.toFloat();
    Serial.print(" gezet: ");    
  }
  else{
    Serial.print(" opgevraagd: ");
  }
    
  Serial.println(String(waarde));
  return true;
}




bool checkZinB(String vergelijking, String beschrijving, bool infoPrinten, bool& waarde){
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





void checkenVoorCommando(bool info){
  if(checkZinB("g", "golven", info, golven)){return;}
  if(checkZinB("PLG", "plaatLeesGolven", info, plaatLeesGolven)){return;}
  if(checkZinB("KG", "karGolven", info, karGolven)){return;}
  if(checkZinB("SG", "strobo.golven", info, strobo.golven)){return;}



  //-------------------------------------------------STAAT        
  if(checkZin(">>", "naarVolgendNummer()", info)){ naarVolgendNummer(); return;}
  if(checkZin("<<", "naarVorrigNummer()", info)){ naarVorrigNummer(); return;}
  if(checkZin("hok", "S_HOK", info)){ setStaat(S_HOK); return;}
  if(checkZin("stop", "stoppen()", info)){ stoppen(); return;}
  if(checkZin("spelen", "spelen()", info)){ spelen(); return;}
  if(checkZin("pauze", "pauze()", info)){ pauze(); return;}
  if(checkZin("schoonmaak", "S_SCHOONMAAK", info)){ setStaat(S_SCHOONMAAK); return;}
  if(checkZin("cal", "S_CALIBREER", info)){ setStaat(S_CALIBREER); return;}





  //----------------------------------------------------ARM
  if(checkZin("NE", "naaldErop()", info)){  naaldErop(); return;}
  if(checkZin("NA", "naaldEraf()", info)){  naaldEraf(); return;}
  if(checkZinF("ATG", "armTargetGewicht", info, armTargetGewicht)){return;}
  if(checkZin("AKL", "armKracht500mg calibreer", info)){    armKracht500mg = armKracht;   Serial.println("armKracht500mg: "  + String(armKracht500mg, 5));  return;}
  if(checkZin("AKH", "armKracht500mg calibreer", info)){  armKracht4000mg = armKracht; Serial.println("armKracht4000mg: " + String(armKracht4000mg, 5));   return;}


  //-------------------------------------------------------KAR SENSORS / TRACK SHIT
  if(checkZinF("PLS", "plaatLeesStroom", info, plaatLeesStroom)){return;}
  if(checkZinI("VOL", "volume", info, volume)){ volumeOverRide = true; return;}

  if(checkZinF("TO", "trackOffset", info, trackOffset)){return;}

  if(checkZin("AHCent", "armHoekCentreer()", info)){  armHoekCentreer(); return;}
  if(checkZin("AHCal", "armHoekCalibreer()", info)){ armHoekCalibreer(); return;}


  //--------------------------------------------------------KAR
  if(checkZinF("KP", "karP", info, karP)){return;}
  if(checkZinF("KI", "karI", info, karI)){return;}
  if(checkZinF("KD", "karD", info, karD)){return;}

  if(checkZinF("CNul", "antiCoggNul", info, antiCoggNul)){return;}
  if(checkZinF("CMacht", "antiCoggMacht", info, antiCoggMacht)){return;}
  if(checkZinF("CVerschuiving", "antiCoggVerschuiving", info, antiCoggVerschuiving)){return;}
  if(checkZinB("CAan", "antiCoggAan", info, antiCoggAan)){return;}
  if(checkZinB("CType", "antiCoggType", info, antiCoggType)){return;}

  //----------------------------------------------------PLATEAU
  if(checkZinF("PP", "plateauP", info, plateauP)){return;}
  if(checkZinF("PI", "plateauI", info, plateauI)){return;}
  if(checkZinF("PD", "plateauD", info, plateauD)){return;}

  if(checkZinF("TR", "targetRpm", info, targetRpm)){return;}

  if(checkZin("PA", "plateauDraaien()", info)){ plateauDraaien(); return;}
  if(checkZin("PS", "plateauStoppen()", info)){ plateauStoppen(); return;}
  if(checkZinB("PL", "plateauLogica", info, plateauLogica)){return;}
  if(checkZinB("PC", "plateauComp", info, plateauComp)){return;}

  if(checkZinI("SSN", "strobo.sampleNum", info, strobo.sampleNum)){return;}
  if(checkZinI("SMSN", "strobo.medianSampleNum", info, strobo.medianSampleNum)){return;}

  if(checkZinB("SCM", "strobo.compMeten", info, strobo.compMeten)){return;}
  if(checkZin( "SCC", "strobo.clearCompSamples()", info)){   strobo.clearCompSamples(); Serial.println("clearComp");  return;}
  if(checkZinI("SCFV", "strobo.faseVerschuiving", info, strobo.faseVerschuiving)){return;}
  if(checkZinF("SCF", "strobo.compFilter", info, strobo.compFilter)){return;}
  if(checkZinF("SCVermenigvuldiging", "strobo.compVermenigvuldiging", info, strobo.compVermenigvuldiging)){return;}
  if(checkZinF("SCVerval", "", info, strobo.compVerval)){return;}
  if(checkZinB("KC", "kar sinus comp", info, karUitMiddenComp)){return;}
  if(checkZinB("SKC", "plaatUitMiddenComp", info, strobo.plaatUitMiddenComp)){return;}


  //------------------------------------------------------OPSLAG
  if(checkZinF("EV", "eepromVersie", info, eepromVersie)){return;}
  if(checkZin("EO", "eepromOpslaan()", info)){   eepromOpslaan();  eepromShit = 1; return;}
  if(checkZin("EL", "eepromUitlezen()", info)){ eepromUitlezen();  return;}
  
  if(checkZin("OC", "orientatie.calibreerOrientatie()", info)){ orientatie.calibreerOrientatie(); return;}



  //------------------------------------------------------HELP
  if(checkZin("C?", "commandos", info)){ checkenVoorCommando(true);return;}

  if(checkZin("?", "help", info)){
    Serial.println("\n\n\nhelp---------------------------------------------");   
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
    Serial.println();
    
    Serial.println("armKracht: " + String(armKracht));
    Serial.println("armGewicht: " + String(armGewicht));
    Serial.println("isNaaldErop(): " + String(isNaaldErop()));
    Serial.println();

    Serial.println("trackOffset: " + String(trackOffset));
    Serial.println();



    eepromPrint();
    Serial.println();
    
    // Serial.println("V faseVerschuiving: " + String(strobo.faseVerschuiving));
    // Serial.println("C compFilter: " + String(strobo.compFilter));
    // Serial.println("c compVermenigvuldiging: " + String(strobo.compVermenigvuldiging));
    // Serial.println("I compverval: " + String(strobo.compVerval));
    // Serial.println();
    
    printKnoppen();
    orientatie.print();
    Serial.println();      

    Serial.println("antiCoggNul: " + String(antiCoggNul));
    Serial.println("antiCoggMacht: " + String(antiCoggMacht));
    Serial.println("antiCoggVerschuiving: " + String(antiCoggVerschuiving));     
    Serial.println();

    Serial.println("-------------------------------------------------\n\n\n");

    return;
  }


  Serial.println("fout command: \"" + zin + "\"");
}
















Interval serieelInt(10000, MICROS);
// Interval serieelInt(5000, MICROS);

void serieelFunc(){
  if(serieelInt.loop()){
    if(golven){
      
      
      
      Serial.print(strobo.vaart - targetRpm, 3);
      
      // Serial.print(", ");
      // Serial.print(strobo.glad, 3);
      // Serial.print(", ");
      // Serial.print(targetRpm, 2);

      Serial.print(", ");
      Serial.print(strobo.glad - targetRpm, 3);

      Serial.print(", ");
      Serial.print(strobo.glad - centerCompTargetRpm, 3);

      // Serial.print(", ");
      // Serial.print(strobo.gladglad - targetRpm, 3);

      Serial.print(", ");
      Serial.print(centerCompTargetRpm - targetRpm, 3);



      Serial.print(", ");
      Serial.print(strobo.teller);

      // Serial.print(", ");
      // Serial.print(strobo.preComp, 4);

      // Serial.print(", ");
      // Serial.print(strobo.plateauComp, 4);

      Serial.print(", ");
      Serial.print(uitBuff, 4);

      

      // Serial.print(", ");
      // Serial.print(armHoekRuw);//1696);
      // Serial.print(", ");
      // Serial.print(armHoek, 5);//1696);






      // Serial.print(", ");
      // Serial.print(karPos, 4);  
      Serial.print(", ");
      Serial.print(egteKarPos, 4);

      Serial.print(", ");
      Serial.print(karPosMidden, 4);


      Serial.print(", ");
      Serial.print(karPosMidden + strobo.karFourier, 4);  

      Serial.print(", ");
      Serial.print(karPosMidden + strobo.karFourierFilt, 4);  


      // Serial.print(", ");
      // Serial.print(nieuweHoek, 4);  
      // Serial.print(", ");
      // Serial.print(karSin, 4);  
      // Serial.print(", ");
      // Serial.print(karCos, 4); 
      

      // Serial.print(", ");
      // Serial.print(nieuwePos, 4);

      // Serial.print(", ");
      // Serial.print(plaatAanwezigGefilterd, 3);
      
      // Serial.print(", ");
      // Serial.print(afstandOmTeStoppen );
      
      // Serial.print(", ");
      // Serial.print(armKracht);
      // Serial.print(", ");
      // Serial.print(armGewicht);


      // Serial.print(", ");
      // Serial.print(orientatie.x);
      // Serial.print(", ");
      // Serial.print(orientatie.y);
      // Serial.print(", ");
      // Serial.print(orientatie.z);


      // Serial.print(", ");
      // Serial.print(potVal);





      Serial.println();




    }





    while(Serial.available() > 0){
    
      char letter = Serial.read();
      
      if( ( letter == '\n' || letter == '\r' )&& zin != ""){
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



