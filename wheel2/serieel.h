


bool golven = false;



Interval serieelInt(10000, MICROS);

void serieelFunc(){
  if(serieelInt.loop()){
    if(golven){
      // strobo.getDiv();
      Serial.print(strobo.vaart,3);
      Serial.print(", ");
      Serial.print(strobo.glad,3);
      Serial.print(", ");
      Serial.print(strobo.gladglad,3);

      Serial.print(", ");
      Serial.print(calibratieToon.vaart);

      Serial.print(", ");
      Serial.print(strobo.teller);

      // Serial.print(", ");
      // Serial.print(strobo.vaartRuw);
      // Serial.print(", ");
      // Serial.print(strobo.gladNieuw);
      // Serial.print(", ");
      // Serial.print(strobo.plateauComp);
      

      
      // Serial.print(", ");
      // Serial.print(calibratieToon.glad);
      
      // Serial.print(", ");
      // Serial.print(strobo.div, 3);
      // Serial.print(", ");
      // Serial.print(strobo.dav, 3);

      
      
      
      Serial.print(", ");
      Serial.print(plaatLeesRuw);

      // Serial.print(", ");
      // Serial.print(plaatLeesGefilterd);
      // Serial.print(", ");
      // Serial.print(plaatLeesGefilterdBodem);
      // Serial.print(", ");
      // Serial.print(trackTresshold);
      // Serial.print(", ");
      // Serial.print(plaatLeesDivTrack);
      // Serial.print(", ");
      // Serial.print(potVal);
      // Serial.print(", ");
      // Serial.print(potVolume);
      
      // Serial.print(", ");
      // Serial.print(uitBuff * 30);
      
      Serial.print(", ");
      Serial.print(armHoekSlow);//1696);
      
      Serial.print(", ");
      Serial.print(armHoekRuw);//1696);

      Serial.print(", ");
      Serial.print(armHoek);//1696);

      // Serial.print(", ");
      // Serial.print((karPos - karPosPrev)*1000.0);//1696);
      // karPosPrev = karPos;

      // Serial.print(", ");
      // Serial.print(karPos);
      
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
      else if(letter == '>'){    //set armhoekOfset
        naarVolgendNummer();
      }
      else if(letter == '<'){    //set armhoekOfset
        naarVorrigNummer();
      }
      else if(letter == 'H'){    //set armhoekOfset
        setStaat(S_NAAR_HOK);
      }
      else if(letter == 'S'){    //set armhoekOfset
        stoppen();
      }
      else if(letter == 'D'){    //set armhoekOfset
        spelen();
      }
      else if(letter == 'P'){    //set armhoekOfset
        pauze();
      }
      else if(letter == 'J'){    //set armhoekOfset
        setStaat(S_JOGGEN);
        karTargetPos = Serial.parseFloat();
        Serial.print("karTargetPos: ");
        Serial.println(karTargetPos);
      }
      else if(letter == 'O'){    //set armhoekOfset
        armHoekCalibreer();
      }
      else if(letter == 'k'){    //set karP
        karP = Serial.parseFloat();
        Serial.print("p: ");
        Serial.println(karP);
      }
      else if(letter == 'p'){    //set P
        plateauP = Serial.parseFloat();
        Serial.print("p: ");
        Serial.println(plateauP);
      }
      else if(letter == 'i'){    //set I
        plateauI = Serial.parseFloat();
        Serial.print("i: ");
        Serial.println(plateauI);
      }
      else if(letter == 'd'){    //set D
        plateauD = Serial.parseFloat();
        Serial.print("d: ");
        Serial.println(plateauD);
      }
      else if(letter == '='){    //set basisVaart
        float i = Serial.parseFloat();
        targetRpm = i;
        Serial.print("targetRpm: ");
        Serial.println(targetRpm);
      }
      // else if(letter == 'm'){    //armmotor uit
      //   karMotorEnable = !karMotorEnable;
      //   Serial.println(karMotorEnable?"aan":"uit");
      // }
      else if(letter == 'N'){    //arm motor aan
        setStaat(S_SCHOONMAAK);
      }
      else if(letter == 'A'){    //arm motor target
        armGewicht = Serial.parseFloat();
        armGewichtUpdate();
        Serial.print("armGewicht: ");
        Serial.print(armGewicht);
        Serial.print("  armTargetKracht: ");
        Serial.println(armTargetKracht);
      }
      else if(letter == 'v'){    //set volume
        int i = Serial.parseInt();
        volume = i;
        volumeOverRide = true;
        Serial.print("volume: ");
        Serial.println(i);
      }
      else if(letter == '?'){    //help
        Serial.print("p: ");
        Serial.println(plateauP);
        Serial.print("i: ");
        Serial.println(plateauI);
        Serial.print("d: ");
        Serial.println(plateauD);

        Serial.print("staat: ");
        printStaat(staat);
        
        Serial.println();
        printKnoppen();

        orientatie.print();
      }

      else if(letter == 'I'){    //golven uit
        toggleAudioFreqMeting();
      }
      else if(letter == 'C'){    //golven uit
        strobo.clearCompSamples();
      }
      else if(letter == 'r'){    //golven uit
        strobo.recalCompSamples();
      }
      else if(letter == 's'){    //golven uit
        strobo.saveCompSamples();
      }
      else if(letter == 'c'){    //golven uit
        strobo.toggleCompensatieModus();
      }
      else if(letter == '~'){    //golven uit
        strobo.printCompSamples();
      }


      else if(letter == 'Q'){    //golven uit
        strobo.plateauCompMeten = !strobo.plateauCompMeten;
        // strobo.clearPlateauSamples();
      }
      else if(letter == 'q'){    //golven uit
        // strobo.plateauCompMeten = !strobo.plateauCompMeten;
        strobo.clearPlateauSamples();
      }
    
    }
  }
}
