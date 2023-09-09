


bool golven = false;



Interval serieelInt(10000, MICROS);
// Interval serieelInt(5000, MICROS);

void serieelFunc(){
  if(serieelInt.loop()){
    if(golven){
      
      
      Serial.print(strobo.vaart, 3);
      Serial.print(", ");
      Serial.print(strobo.glad, 3);
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
      // Serial.print(strobo.plateauCompFourier, 4);


      Serial.print(", ");
      Serial.print(uitBuff, 4);

      Serial.print(", ");
      Serial.print(basis, 4);

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

      

      
      // Serial.print(", ");
      // Serial.print(armHoekSlow);//1696);
      // Serial.print(", ");
      // Serial.print(armHoekRuw);//1696);
      Serial.print(", ");
      Serial.print(armHoek);//1696);



      Serial.print(", ");
      Serial.print(karPos, 4);

      Serial.print(", ");
      Serial.print(karPosFilter, 4);

      // Serial.print(", ");
      // Serial.print(karPosFilterSlow, 4);

      Serial.print(", ");
      Serial.print(plaatAanwezigGefilterd, 3);
      
      // Serial.print(", ");
      // Serial.print(afstandOmTeStoppen );
      
      // Serial.print(", ");
      // Serial.print(armKracht);


      // Serial.print(", ");
      // Serial.print(orientatie.x);
      // Serial.print(", ");
      // Serial.print(orientatie.y);
      // Serial.print(", ");
      // Serial.print(orientatie.z);





      Serial.println();




    }




    while(Serial.available() > 0){
      char letter = Serial.read();
      
      //------------------------------------------------------------------------SERIEEL PLATEAU MOTOR
      if(letter == 'G'){    //gol aan
        golven = true;
      }
      else if(letter == 'g'){    //golven uit
        golven = false;
      }
      else if(letter == 'y'){    //plaat lees golven
        plaatLeesGolven = !plaatLeesGolven;
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
        spelen();
      }
      else if(letter == 'P'){
        pauze();
      }
      else if(letter == 'O'){
        armHoekCalibreer();
      }
      
      else if(letter == 'F'){
        naaldErop();
      }
      else if(letter == 'f'){
        naaldEraf();
      }
      else if(letter == 'a'){    //arm motor target
        armTargetKracht = Serial.parseFloat();
        Serial.println("armTargetKracht: " + String(armTargetKracht));
      }
      else if(letter == 'A'){    //arm motor target
        armGewicht = Serial.parseFloat();
        armGewichtUpdate();
        Serial.println("armGewicht: " + String(armGewicht) + " armTargetKracht: " + String(armTargetKracht));
      }

      else if(letter == 'k'){    //set karP
        karP = Serial.parseFloat();
        Serial.println("karP: " + String(karP));
      }
      else if(letter == 'p'){    //set P
        plateauP = Serial.parseFloat();
        Serial.println("p: " + String(plateauP));
      }
      else if(letter == 'i'){    //set I
        plateauI = Serial.parseFloat();
        Serial.println("i: " + String(plateauI));
      }
      else if(letter == 'd'){    //set D
        plateauD = Serial.parseFloat();
        Serial.println("d: " + String(plateauD));
      }
      else if(letter == '='){    //set basisVaart
        float i = Serial.parseFloat();
        targetRpm = i;
        Serial.println("targetRpm: " + String(targetRpm));
      }
      else if(letter == 'N'){    //arm motor aan
        setStaat(S_SCHOONMAAK);
      }

      else if(letter == 'v'){    //set volume
        int i = Serial.parseInt();
        volume = i;
        volumeOverRide = true;
        Serial.println("volume: " + String(i));
      }
      else if(letter == '?'){    //help
        Serial.println("help----------------------------");   
        Serial.println("p: " + String(plateauP, 3));
        Serial.println("i: " + String(plateauI, 3));
        Serial.println("d: " + String(plateauD, 3));
        Serial.println();
        
        Serial.println("staat: " + printStaat(staat));
        Serial.println("volume: " + String(volume));
        Serial.println();
        
        Serial.println("armTargetKracht: " + String(armTargetKracht));
        Serial.println();
        
        Serial.println("V faseVerschuiving: " + String(strobo.faseVerschuiving));
        Serial.println("C compFilter: " + String(strobo.compFilter));
        Serial.println("c compVermenigvuldiging: " + String(strobo.compVermenigvuldiging));
        Serial.println("I compverval: " + String(strobo.compVerval));
        Serial.println();
        
        printKnoppen();
        orientatie.print();

        Serial.println("-----------------------------");  
      }



      else if(letter == '~'){    //
        setStaat(S_CALIBREER);
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
    
    }
  }
}



