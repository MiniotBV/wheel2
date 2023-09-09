
bool golven = false;
bool karPIDveranderen = true;


String zin = "";
String vorrigeCommando = "";



bool checkZin(String vergelijking){
  vergelijking.toLowerCase();

  if( ! zin.startsWith(vergelijking) ) return false;

  vorrigeCommando = zin;
  zin.replace(vergelijking, "");
  zin.trim();
  
  return true;
}



bool checkZinI(String vergelijking, int& waarde){
  if(!checkZin(vergelijking)) return false;

  Serial.print(vergelijking);

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



bool checkZinF(String vergelijking, float& waarde){
  if(!checkZin(vergelijking)) return false;

  Serial.print(vergelijking);

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




bool checkZinB(String vergelijking, bool& waarde){
  if(!checkZin(vergelijking)) return false;

  Serial.print(vergelijking);

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





void checkenVoorCommando(){
  if(checkZinB("g", golven)){return;}
  if(checkZinB("PLG", plaatLeesGolven)){return;}
  if(checkZinB("KG", karGolven)){return;}
  if(checkZinB("SG", strobo.golven)){return;}



  //-------------------------------------------------STAAT        
  if(checkZin(">>")){ naarVolgendNummer(); return;}
  if(checkZin("<<")){ naarVorrigNummer(); return;}
  if(checkZin("hok")){ setStaat(S_HOK); return;}
  if(checkZin("stop")){ stoppen(); return;}
  if(checkZin("spelen")){ spelen(); return;}
  if(checkZin("pauze")){ pauze(); return;}
  if(checkZin("schoonmaak")){ setStaat(S_SCHOONMAAK); return;}
  if(checkZin("cal")){ setStaat(S_CALIBREER); return;}





  //----------------------------------------------------ARM
  if(checkZin("naaldErop")){  naaldErop(); return;}
  if(checkZin("naaldEraf")){  naaldEraf(); return;}
  if(checkZinF("armTargetGewicht", armTargetGewicht)){return;}
  if(checkZin("AKL")){    armKracht500mg = armKracht;   Serial.println("armKracht500mg: "  + String(armKracht500mg, 5));  return;}
  if(checkZin("AKH")){  armKracht4000mg = armKracht; Serial.println("armKracht4000mg: " + String(armKracht4000mg, 5));   return;}


  //-------------------------------------------------------KAR SENSORS / TRACK SHIT
  if(checkZinF("PLS", plaatLeesStroom)){return;}
  if(checkZinI("vol", volume)){ volumeOverRide = true; return;}

  if(checkZinF("TO", trackOffset)){return;}

  if(checkZin("AHCent")){  armHoekCentreer(); return;}
  if(checkZin("AHCal")){ armHoekCalibreer(); return;}


  //--------------------------------------------------------KAR
  if(checkZinF("kP", karP)){return;}
  if(checkZinF("kI", karI)){return;}
  if(checkZinF("kD", karD)){return;}

  if(checkZinF("cogNul", antiCoggNul)){return;}
  if(checkZinF("cogMacht", antiCoggMacht)){return;}
  if(checkZinF("cogVerschuiving", antiCoggVerschuiving)){return;}
  if(checkZinB("cogAan", antiCoggAan)){return;}
  if(checkZinB("cogType", antiCoggType)){return;}

  //----------------------------------------------------PLATEAU
  if(checkZinF("pP", plateauP)){return;}
  if(checkZinF("pI", plateauI)){return;}  
  if(checkZinF("pD", plateauD)){return;}    

  if(checkZinF("targetRpm", targetRpm)){return;}

  if(checkZin("draaien")){ plateauDraaien(); return;}
  if(checkZinB("PL", plateauLogica)){return;}

  if(checkZinI("sampleNum", strobo.sampleNum)){return;}       

  if(checkZinB("compMeten", strobo.compMeten)){return;}
  if(checkZin( "clearCompSamples")){   strobo.clearCompSamples(); Serial.println("clearComp");  return;}
  if(checkZinI("faseVerschuiving", strobo.faseVerschuiving)){return;}
  if(checkZinF("compFilter", strobo.compFilter)){return;}
  if(checkZinF("compVermenigvuldiging", strobo.compVermenigvuldiging)){return;}
  if(checkZinF("compVerval", strobo.compVerval)){return;};


  //------------------------------------------------------OPSLAG
  if(checkZinF("eepVersie", eepromVersie)){return;}
  if(checkZin("eepOpslaan")){   eepromOpslaan();  eepromShit = 1; return;}
  if(checkZin("eepLees")){ eepromUitlezen();  return;}
  if(checkZin("calOri")){ orientatie.calibreerOrientatie(); return;}



  //------------------------------------------------------HELP
  if(checkZin("?")){
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
















// Interval serieelInt(10000, MICROS);
Interval serieelInt(5000, MICROS);

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
      Serial.print(strobo.teller);

      Serial.print(", ");
      Serial.print(strobo.preComp, 4);



      // Serial.print(", ");
      // Serial.print(strobo.plateauComp, 4);

      // Serial.print(", ");
      // Serial.print(uitBuff, 4);

      

      // Serial.print(", ");
      // Serial.print(armHoekRuw);//1696);
      // Serial.print(", ");
      // Serial.print(armHoek, 5);//1696);






      // Serial.print(", ");
      // Serial.print(karPos, 4);  
      // Serial.print(", ");
      // Serial.print(egteKarPos, 4);  


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

        checkenVoorCommando();
        

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



