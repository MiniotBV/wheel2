
unsigned long serialLoop;
bool golven = true;





void serieelFunc(){
  if(micros() - serialLoop > 10000){
    serialLoop = micros();

    Serial.print(TLE5012.getVaart());
    Serial.print(", ");
    Serial.print(TLE5012.vaart);
    Serial.print(", ");
    Serial.print(targetRpm);
    Serial.print(", ");
    Serial.print(armKracht);
    // Serial.print(", ");
    // Serial.print(strobo.getVaart());
    Serial.print(", ");
    Serial.print(uitBuff);
    Serial.print(", ");
    Serial.print(analogRead(hoekSensor) - 1696);
    Serial.print(", ");
    Serial.print(armHoek - 1696);
    Serial.print(", ");
    Serial.print(karPositie);

    Serial.println();


    while(Serial.available() > 0){
      char letter = Serial.read();
      
      //------------------------------------------------------------------------SERIEEL PLATEAU MOTOR
      if(letter == 'G'){    //gol aan
        golven = true;
      }
      else if(letter == 'g'){    //golven uit
        golven = false;
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
      else if(letter == 'k'){    //armmotor uit
        karMotorPositie = 0;
      }
      else if(letter == 'f'){    //armmotor uit
        armMotorAan = false;
        Serial.println("arm motor uit");
      }
      else if(letter == 'F'){    //arm motor aan
        armMotorAan = true;
        Serial.println("arm motor aan");
      }
      else if(letter == '?'){    //help
        Serial.print("p: ");
        Serial.println(plateauP);
        Serial.print("i: ");
        Serial.println(plateauI);
        Serial.print("d: ");
        Serial.println(plateauD);
      }
      else if(letter == 'S'){    //golven uit
        TLE5012.printSamples();
      }
    
    }
  }
}
