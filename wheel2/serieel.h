bool shouldShowWaves = false;

Interval serieelInt(10000, inMICROS);

void printWaves();      // proto
void handleSerialIn();  // proto

// ===================================
//    serial loop
// ===================================
void serieelFunc()
{
  if(!serieelInt.loop()) return;
  if(shouldShowWaves) printWaves();
  handleSerialIn();
}

void printCommaSpace() { Serial.print(", ");}

void printWaves()
{
  Serial.print(TLE5012.vaart); printCommaSpace();
  Serial.print(TLE5012.glad); printCommaSpace();
  // Serial.print(", "); // Serial.print(TLE5012.vaartRuw); 
  // Serial.print(", "); // Serial.print(TLE5012.gladNieuw);
  
  Serial.print(strobo.vaart); printCommaSpace();
  // Serial.print(", "); // Serial.print(strobo.glad);
  Serial.print(TLE5012.div, 3); printCommaSpace();
  Serial.print(TLE5012.dav, 3); printCommaSpace();
  
  // Serial.print(", "); // Serial.print(plaatLeesRuw);

  // Serial.print(", "); // Serial.print(plaatLeesGefilterd);
  // Serial.print(", "); // Serial.print(plaatLeesGefilterdBodem);
  // Serial.print(", "); // Serial.print(trackTresshold);
  // Serial.print(", "); // Serial.print(plaatLeesDivTrack);
  // Serial.print(", "); // Serial.print(potVal);
  // Serial.print(", "); // Serial.print(potVolume);
  
  // Serial.print(", "); // Serial.print(uitBuff * 30);
  
  Serial.print(car.armHoekSlow); printCommaSpace(); //1696);
  Serial.print(car.armHoekRuw); //1696);
  // Serial.print(", "); // Serial.print(car.position);
  // Serial.print(", "); // Serial.print(armKracht);
  Serial.println();
}
void handleSerialIn()
{ 
    while(Serial.available() > 0)
    {
      char cmd = Serial.read();
      
      if(cmd == 'G') { shouldShowWaves = true;  return; }
      if(cmd == 'g') { shouldShowWaves = false; return; }
      if(cmd == '>') { car.gotoNextSong();      return; } // set armhoekOfset
      if(cmd == '<') { car.gotoPrevSong();      return; }
      if(cmd == 'H') { setStaat(S_NAAR_HOK);    return; }
      if(cmd == 'S') { stoppen(); return; }
      if(cmd == 'D') { spelen();  return; }
      if(cmd == 'P') { car.pauze();   return; }
      if(cmd == 'O') { car.armHoekCalibreer(); return; }
      if(cmd == 'N') { setStaat(S_SCHOONMAAK); return; }   //arm motor aan
      if(cmd == '-') { TLE5012.clearCompSamples(); return; }
      if(cmd == 'r') { TLE5012.recalCompSamples(); return; }
      if(cmd == 's') { TLE5012.saveCompSamples(); return;  }
      if(cmd == 'c') { TLE5012.toggleCompensatieModus(); return; }
      if(cmd == '~') { TLE5012.printCompSamples(); return; }
      
      if(cmd == 'J'){   
        setStaat(S_JOGGEN);
        car.targetPos = Serial.parseFloat();
        Serial.print("car targetPos: ");
        Serial.println(car.targetPos);
        return;
      }
      // ===================
      //  Set P, I, D
      // ===================
      if(cmd == 'p')
      {
        plateauP = Serial.parseFloat();
        Serial.print("p: ");
        Serial.println(plateauP);
        return;
      }
      if(cmd == 'i')
      {
        plateauI = Serial.parseFloat();
        Serial.print("i: ");
        Serial.println(plateauI);
        return;
      }
      if(cmd == 'd')
      {
        plateauD = Serial.parseFloat();
        Serial.print("d: ");
        Serial.println(plateauD);
        return;
      }

      if(cmd == '=')
      {    //set basisVaart
        float i = Serial.parseFloat();
        targetRpm = i;
        Serial.print("targetRpm: ");
        Serial.println(targetRpm);
        return;
      }
      
      if(cmd == 'm')
      {    //armmotor uit
        car.isMotorEnabled = !car.isMotorEnabled;
        Serial.println(car.isMotorEnabled ? "aan" : "uit");
        return;
      }

      if(cmd == 'A')
      {    //arm motor target
        arm.weight = Serial.parseFloat();
        arm.updateWeight();
        Serial.print("armGewicht: ");
        Serial.print(arm.weight);
        Serial.print("  arm.targetWeight: ");
        Serial.println(arm.targetWeight);
        return;
      }
      if(cmd == 'v')
      {    //set volume
        amp.volume = Serial.parseInt(); // int i = Serial.parseInt(); volume = i;
        Serial.println(amp.volume);     //Serial.println(i);
        return;
      }
      if(cmd == '?')
      {    //help
        Serial.print("p: ");
        Serial.println(plateauP);
        Serial.print("i: ");
        Serial.println(plateauI);
        Serial.print("d: ");
        Serial.println(plateauD);
        
        printKnoppen();
        return;
      }
    } // end while
}