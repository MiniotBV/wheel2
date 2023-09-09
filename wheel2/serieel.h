


bool golven = false;



Interval serieelInt(10000, MICROS);

void serieelFunc(){
  if(serieelInt.loop()){

    if(golven){
      Serial.print(TLE5012.getVaart());
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
      Serial.print(", ");
      Serial.print(potVolume);
      Serial.print(", ");
      Serial.print(uitBuff);
      Serial.print(", ");
      // Serial.print(armHoekSlow);//1696);
      // Serial.print(", ");
      Serial.print(armHoek);//1696);
      Serial.print(", ");
      Serial.print(karPos);
      // Serial.print(", ");
      // Serial.print(armKracht);

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
      else if(letter == 'm'){    //armmotor uit
        karMotorEnable = !karMotorEnable;
        Serial.println(karMotorEnable?"aan":"uit");
      }
      else if(letter == 'f'){    //armmotor uit
        armMotorAan = false;
        Serial.println("arm motor uit");
      }
      else if(letter == 'F'){    //arm motor aan
        armMotorAan = true;
        Serial.println("arm motor aan");
      }
      else if(letter == 'v'){    //set volume
        int i = Serial.parseInt();
        volume = i;
        Serial.println(i);
      }
      else if(letter == '?'){    //help
        Serial.print("p: ");
        Serial.println(plateauP);
        Serial.print("i: ");
        Serial.println(plateauI);
        Serial.print("d: ");
        Serial.println(plateauD);
        
        printKnoppen();
        
      
      }
      // else if(letter == 'S'){    //golven uit
      //   TLE5012.printSamples();
      // }
    
    }
  }
}
