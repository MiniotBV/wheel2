
bool golven = false;
bool karPIDveranderen = true;


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

      // Serial.print(", ");
      // Serial.print(calibratieToon.vaart);


      Serial.print(", ");
      Serial.print(strobo.teller);

      Serial.print(", ");
      Serial.print(strobo.preComp, 4);

      // Serial.print(", ");
      // Serial.print(strobo.mean, 4);

      Serial.print(", ");
      Serial.print(strobo.plateauComp, 4);

      Serial.print(", ");
      Serial.print(uitBuff, 4);

      // Serial.print(", ");
      // Serial.print(basis, 4);

      // Serial.print(", ");
      // Serial.print(karInterval);
      
      
      
      // Serial.print(", ");
      // Serial.print(plaatLeesRuw);

      // Serial.print(", ");
      // Serial.print(plaatLeesGefilterd);

      // Serial.print(", ");
      // Serial.print(plaatLeesDiv);

      // Serial.print(", ");
      // Serial.print(plaatLeesDivDiv);

      // // Serial.print(", ");
      // // Serial.print(plaatLeesTrack);

      // Serial.print(", ");
      // Serial.print(plaatLeesPre);
      // Serial.print(", ");
      // Serial.print(plaatLeesOmhoog);
      // Serial.print(", ");
      // Serial.print(plaatLeesOmlaag);
      
      // Serial.print(", ");
      // Serial.print(trackTresshold);
      // Serial.print(", ");
      // Serial.print(plaatLeesDivTrack);

      

      
      Serial.print(", ");
      Serial.print(armHoekSlow);//1696);
      Serial.print(", ");
      Serial.print(armHoekOffset);//1696);
      Serial.print(", ");
      Serial.print(armHoekRuw);//1696);
      Serial.print(", ");
      Serial.print(armHoek);//1696);





      // Serial.print(", ");
      // Serial.print(karDcomp, 4);

      Serial.print(", ");
      Serial.print(karPos, 4);  

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
      
      //------------------------------------------------------------------------SERIEEL PLATEAU MOTOR
      if(letter == 'G'){    //gol aan
        golven = true;
        Serial.println("golven aan");
      }
      else if(letter == 'g'){    //golven uit
        golven = false;
        Serial.println("golven uit");
      }
      else if(letter == 'y'){    //plaat lees golven
        plaatLeesGolven = !plaatLeesGolven;
        Serial.println("plaatLeesGolven: " + String(plaatLeesGolven));
      }
      else if(letter == 'Y'){    //kar golven
        karGolven = !karGolven;
        Serial.println("karGolven: " + String(karGolven));
      }

      
      else if(letter == '>'){
        naarVolgendNummer();
      }
      else if(letter == '<'){
        naarVorrigNummer();
      }
      else if(letter == 'H'){
        setStaat(S_NAAR_HOK);
      }
      else if(letter == 'S'){
        stoppen();
      }
      else if(letter == 'D'){
        // spelen();
        plateauDraaien();
      }
      else if(letter == 'P'){
        pauze();
      }
      else if(letter == 'N'){    //
        setStaat(S_SCHOONMAAK);
      }
      else if(letter == '~'){    //
        setStaat(S_CALIBREER);
      }
      
      
      
      else if(letter == 'O'){
        armHoekCentreer();
      }
      else if(letter == 'o'){
        armHoekMin = armHoekMinCall;
        armHoekMax = armHoekMaxCall;

        armHoekMinCall = AMAX;
        armHoekMaxCall = 0;
        Serial.println("armHoek gecalibreed en buffer waardes gereset");
      }
      


      else if(letter == 'F'){
        naaldErop();
      }
      else if(letter == 'f'){
        naaldEraf();
      }
      else if(letter == 'A'){    //arm motor target
        armTargetGewicht = Serial.parseFloat();
        Serial.println("armTargetGewicht: " + String(armTargetGewicht));
      }
      else if(letter == 'l'){
        armKracht500mg = armKracht;
        Serial.println("armKracht500mg: " + String(armKracht500mg, 5)); 
      }
      else if(letter == 'h'){
        armKracht4000mg = armKracht;
        Serial.println("armKracht4000mg: " + String(armKracht4000mg, 5)); 
      }



      else if(letter == 's'){    //plaatLeesStroom
        plaatLeesStroom = Serial.parseFloat();
        Serial.println("plaatLeesStroom: " + String(plaatLeesStroom));
      }
      else if(letter == 'v'){    //set volume
        int i = Serial.parseInt();
        volume = i;
        volumeOverRide = true;
        Serial.println("volume: " + String(i));
      }

      else if(letter == 't'){    //set track offset
        float i = Serial.parseFloat();
        trackOffset = i;
        Serial.println("trackOffset: " + String(i));
      }




      else if(letter == 'K'){    //set karP
        karPIDveranderen = !karPIDveranderen;
        if(karPIDveranderen){
          Serial.println("kar PID");
        }else{
          Serial.println("plateau PID");
        }
      }
      else if(letter == 'p'){    //set P
        if(karPIDveranderen){
          karP = Serial.parseFloat();
          Serial.println("kar P: " + String(karP, 5));
        }else{
          plateauP = Serial.parseFloat();
          Serial.println("plateau P: " + String(plateauP, 5));
        }
      }
      else if(letter == 'i'){    //set I
        if(karPIDveranderen){
          karI = Serial.parseFloat();
          Serial.println("kar I: " + String(karI, 5));
        }else{
          plateauI = Serial.parseFloat();
          Serial.println("plateau I: " + String(plateauI, 5));
        }
      }
      else if(letter == 'd'){    //set D
        if(karPIDveranderen){
          karD = Serial.parseFloat();
          Serial.println("kar D: " + String(karD, 5));
        }else{
          plateauD = Serial.parseFloat();
          Serial.println("plateau D: " + String(plateauD, 5));
        }
      }




      
      else if(letter == '='){    //set basisVaart
        float i = Serial.parseFloat();
        targetRpm = i;
        Serial.println("targetRpm: " + String(targetRpm));
      }












      else if(letter == 'e'){ 
        eepromVersie = Serial.parseFloat();
        EEPROM.put(EEPROM_VERSIE, eepromVersie);
        Serial.println("eepromVersie: " + String(eepromVersie, 5));
      }

      else if(letter == 'E'){ 
        Serial.println("opslaan...");
        eepromOpslaan();
        eepromShit = 1;
      }

      else if(letter == 'r'){ 
        eepromUitlezen();
      }

      else if(letter == 'a'){
        orientatie.gefilterdOffset = orientatie.gefilterd;
        Serial.println("orientatie.gefilterdOffset: " + String(orientatie.gefilterdOffset, 5)); 
      }






      else if(letter == 'Q'){    //
        strobo.compMeten = !strobo.compMeten;
        Serial.println("compMeten: " + String(strobo.compMeten));
        // strobo.clearCompSamples();
      }
      else if(letter == 'q'){    //
        // strobo.compMeten = !strobo.compMeten;
        strobo.clearCompSamples();
        Serial.println("clearComp");
      }

      else if(letter == 'V'){    //
        // strobo.compMeten = !strobo.compMeten;
        strobo.faseVerschuiving = Serial.parseInt();
        Serial.println("faseVerschuiving: " + String(strobo.faseVerschuiving));
      }
      else if(letter == 'C'){    //
        strobo.compFilter = Serial.parseFloat();
        Serial.println("C compFilter: " + String(strobo.compFilter));
      }
      else if(letter == 'c'){    //
        strobo.compVermenigvuldiging = Serial.parseFloat();
        Serial.println("c compVermenigvuldiging: " + String(strobo.compVermenigvuldiging));
      }
      else if(letter == 'I'){    //
        strobo.compVerval = Serial.parseFloat();
        Serial.println("I compverval: " + String(strobo.compVerval));
      }








      else if(letter == '?'){    //help
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

        Serial.println("-------------------------------------------------\n\n\n");  
      }
    
    }
  }
}



