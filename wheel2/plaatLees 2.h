class PlaatLees
{
  public:
  bool showGolven = false;

  float ruw, ruwPrev, ruwDiv;
  float absDiv, waarde;
  float max, min;

  bool ledBlink;

  float stroom = 10;
  float ledStroomP = 0.0006;
  float sensorTarget = 1500;
  
  void init() { setPwm(plaatLeesLed); setLedStroom(0); } // 10 mA 

  int volt2pwm(float v) { return (v * PMAX) / 3.3; }



  void setLedStroom(float amp)
  {
    pwmWrite(plaatLeesLed, volt2pwm(1.0f + (0.1f * amp))); //100ohm + 1volt led drop
    // amp /= 1000.0;
    // pwmWrite(plaatLeesLed, volt2pwm(1 + (100 * amp)));  //100ohm + 1volt led drop
  }

  void ledUit() { pwmWrite(plaatLeesLed, 0); }  //100ohm + 1volt led drop
  
  bool isPlaatAanwezig() { return plaatAanwezigGefilterd > 0.5; }


  // #define plaatKnipperInterval 50
  #define plaatDetectieTreshold 100

  void plaatDetectie()
  {
    
    plaatAanwezig = absDiv > plaatDetectieTreshold;  // genoeg amplitude => plaat
    plaatAanwezigGefilterd += (plaatAanwezig - plaatAanwezigGefilterd) / 10;

    if(!isPlaatAanwezig() &&  (staat == S_SPELEN  ||  staat == S_PAUZE)){   // is er nog een plaat aanwezig?
      Serial.println("plaat is verdwenen ??");
      stoppen();
      return;
    }
  }


  void zetNummersAlsEenSingletje()
  {
    hoeveelNummers = 1;
    nummers[1] = SINGLETJE_PLAAT_BEGIN;

    if(nummers[0] > SINGLETJE_PLAAT_BEGIN - 12)
    {
      nummers[0] = 55;
      Serial.println("moest einde plaat aanpassen");
    }
  }



  void nieuwNummer(float pos)
  {
    if(hoeveelNummers == 0)
    {
      // Serial.println("einde plaat: " + String(pos));
    }
    else
    {
      float afstand = pos - nummers[hoeveelNummers-1];
      if(afstand < 2){return;}
      // Serial.println("track op: " + String(pos));
    }

    nummers[hoeveelNummers] = pos;
    hoeveelNummers++;  
  }

  #define MINIMALE_TRACK_AFSTAND 3//mm

  int trackTresshold = 500;
  bool trackOnderTresh = true;

  void scannenVoorTracks()
  {
    
    if(trackSensorPos < PLAAT_EINDE + 2  ||  staat != S_NAAR_BEGIN_PLAAT){
      return;
    }

    if(waarde < trackTresshold / 2 && trackOnderTresh){  trackOnderTresh = false; }

    if(waarde > trackTresshold && !trackOnderTresh){
      trackOnderTresh = true;

      nieuwNummer(trackSensorPos - trackOffset);
    }


    if(showGolven)
    {
      Serial.print(waarde); Serial.print(", ");
      Serial.print(absDiv); Serial.print(", ");
      Serial.print(max);    Serial.print(", ");
      Serial.print(min);    Serial.print(", ");
      Serial.print(stroom * 100);
      Serial.println();
    }
    
  }

  Interval interval {10000, MICROS};

  void loopFunc()
  {
    if(!interval.loop()) return;
      
    if(staat == S_HOK)  // kar in hok => scanner uit
    { 
      hoeveelNummers = 0;
      nummers[hoeveelNummers] = 1;
      ledUit();
      return;
    }



    ruwPrev = ruw;
    ruw = analogRead(plaatLees);
    ruwDiv = ruw - ruwPrev;
    absDiv = abs(ruwDiv);
    waarde = abs(absDiv - sensorTarget);

    plaatDetectie();

    if(ledBlink)
    {
      ledUit();

      float ledStroomP = (sensorTarget - absDiv) * ledStroomP;
      stroom += limitF(ledStroomP,  -1, 1);
      stroom = limitF(stroom, 5, 30);
      max = ruw;
      scannenVoorTracks();    //--------------------------------                   TRACKS LEZEN
    }
    else
    {
      setLedStroom(stroom);
      min = ruw;
    }

    
    ledBlink = !ledBlink; // toggle led

  }
};


