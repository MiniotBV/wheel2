float targetRpm = 0;

#define sampleMax 65500               //samples



 




void eepSchrijfInt(int addr, int val){
  EEPROM.write(addr,     (val >> 8) & 255);
  EEPROM.write(addr + 1,  val       & 255);
}

int eepLeesInt(int addr){
  return (EEPROM.read(addr) << 8) + EEPROM.read(addr + 1);
}

void eepSchrijfFloat(int addr, float val){
  eepSchrijfInt(addr, int( val * 10000.0 ) );
}

float eepLeesFloat(int addr){
  return eepLeesInt(addr) / 10000.0;
}







Interval compInt(0, MILLIS);


class COMPVAART{
  public:
    volatile unsigned int vaartInterval;
    volatile          int sampleNum;
    volatile          int samples[200];
    volatile unsigned int sampleTeller = 0;
    volatile unsigned long tijd;
    volatile unsigned int interval;
    float gemiddelde = sampleMax;

    // int filterOrde = 4;
    // float filterWaarde = 1/20.0;
    // float filter[12];
    
    float vaart;
    // float vaartRuw;
    // float gladNieuw;
    float glad;
    float gladglad;
    float div;
    float dav;

    // float divAvrg[64];
    // int divTeller;
    // int divSamps = 10;

    int glitchTeller;
    int terugdraaiTeller = 0;

    int dir;

    int pulsenPerRev;
    int teller = 0;

    
    // float compSamples[4096];
    float compSamples[9000];
    bool divCompMeten = false;


    float plateauCompensatie[9000];
    bool plateauCompMeten = false;
    float plateauComp = 0;









    COMPVAART(int samps, float ppr){
      sampleNum = samps;
      pulsenPerRev = ppr;
      
      clearSamples();
      clearCompSamples();
      clearPlateauSamples();
    }




	

    void interrupt(){
      tijd = micros();


      dir = 1;
      // if(!gpio_get(plateauB)){
      //   terugdraaiTeller++;
      //   if(terugdraaiTeller > 6){
      //     dir = -1;
      //     return;
      //   }     
      // }else{
      //   terugdraaiTeller = 0;
      // }


      
      
      interval = tijd - vaartInterval;
      vaartInterval = tijd;
      
      if(interval > sampleMax){interval = sampleMax;}
      
      shiftSamples((interval * dir) * compSamples[teller]);
      

      teller = rondTrip(teller + dir,  pulsenPerRev);
    
      
      dav = compSamples[teller];

      // getVaart();
      getDiv();

      glad += (vaart - glad) / 100;
      gladglad += (glad - gladglad) / 1000;




      if(divCompMeten){
        
        if(isOngeveer(div, 1, 0.3)){
          compSamples[rondTrip(teller - 10,  pulsenPerRev)] += ( div - 1 ) / 3;
        }
      }



      if(plateauCompMeten){
        plateauCompensatie[teller] += ( glad - targetRpm )*0.5 ;//* 0.2;// *0.8;
        // plateauCompensatie[teller] += ( gladglad - targetRpm )*0.5 ;//* 0.2;// *0.8;
        // plateauCompensatie[teller] -= (plateauCompensatie[teller] - plateauCompensatie[rondTrip(teller + 400,  pulsenPerRev)])*0.9;
      }

      plateauComp = plateauCompensatie[rondTrip(teller + random(200),  pulsenPerRev)];
      // plateauComp = plateauCompensatie[rondTrip(teller + 580,  pulsenPerRev)];
      
    }








    void clearPlateauSamples(){
      for(int i = 0; i < pulsenPerRev + 1000; i++){
        plateauCompensatie[i] = 0.0;
      }
    }


    void setCompensatieModus(bool c){
      divCompMeten = c;
      compInt.reset();

      if(divCompMeten){
        // clearCompSamples();
        Serial.println("meten aan");
      }else{
        Serial.println("meten uit");
      }
    }


    void toggleCompensatieModus(){
      setCompensatieModus(!divCompMeten);

    }





    // void recalCompSamples(){
    //   for(int i = 0;   i < pulsenPerRev;   i+=2){
    //     compSamples[i] = eepLeesFloat(i);
    //     compSamples[i+1] = compSamples[i];
    //   }

    //   Serial.println("gerecalt");
    // }



    void recalCompSamples(){
      int i = 0;
      for(int i = 0;   i < pulsenPerRev;   i += 4){
        // compSamples[i*2] = eepLeesFloat(i*2);
        // compSamples[(i*2)+1] = compSamples[i*2];
        
        compSamples[i] = eepLeesFloat(i/2);
        compSamples[i+1] = compSamples[i];
        compSamples[i+2] = compSamples[i];
        compSamples[i+3] = compSamples[i];
      }

      if(compSamples[10] < 2){
        Serial.println("recalt");
      }else{
        Serial.println("faal recalt");
        clearCompSamples();        
      }

      
    }




    // void saveCompSamples(){
    //   eepromPauze = true;
    //   delay(20);
    //   // enableInterupts(false);

    //   for(int i = 0;   i < pulsenPerRev;   i+=2){
    //     eepSchrijfFloat(i,   (compSamples[i] + compSamples[i+1])/2);
    //   }

    //   // bool error = EEPROM.commit();
    //   // Serial.println(error ? "opgeslagen: "   :   "faaal: ");

    //   eepromPauze = false;
    //   // enableInterupts(true);
    // }



    void saveCompSamples(){
      // noInterrupts();
      // rp2040.idleOtherCore();
      delay(10);      

      for(int i = 0;   i < pulsenPerRev;   i+=4){
        float inpol = ( compSamples[i+0] + 
                        compSamples[i+1] + 
                        compSamples[i+2] + 
                        compSamples[i+3]   )/4;
                        
        eepSchrijfFloat(i/2,   inpol);
      }

      bool geenError = EEPROM.commit();
      if(geenError){
        Serial.println("opgeslagen");
      }else{
        Serial.println("faaal save");
      }
    }











    void clearCompSamples(){
      for(int i = 0;   i < pulsenPerRev;   i++){
        compSamples[i] = 1;
      }
    }






    void printCompSamples(){
      for(int i = 0;   i < pulsenPerRev;   i+=4){
        Serial.println(compSamples[i], 3);
        delay(1);
      }
    }




    float getVaart(){

      if(micros() - tijd > sampleMax){
        
        glitchTeller++;

        if(glitchTeller > 6){
          glitchTeller = 0;
          clearSamples();        
        }
      
      }else{
        glitchTeller = 0;
      }


      
      gemiddelde = gemiddeldeInterval();
      vaart =  huidigeVaart(gemiddelde);


      return vaart;//niet compensenre
    }    






    // float getGlad(){
    //   glad += (vaart - glad) / 100;
    //   return glad;
    // }


    float getDiv(){
      div = getVaart() / strobo.getVaart();

      // getVaart();
      // strobo.getVaart();
      // div = (strobo.gemiddelde / strobo.sampleNum) / (gemiddelde / sampleNum);

      return div;
    }




    void shiftSamples(int samp){
      samples[ sampleTeller++ % sampleNum ] = samp;   
    }



    void clearSamples(){
      for(int i = 0; i < sampleNum; i++){
        samples[i] = sampleMax;        
      }
    }



    float gemiddeldeInterval(){
      int totaal = 0;
      
      for(byte i = 0;   i < sampleNum;   i++){
        totaal += samples[i];
      }
      return totaal / float(sampleNum);      
    }



    float huidigeVaart(float inter){//                                                           RPM BEREKENEN

      // return (1000000 / gemiddeldeInterval())/4;  //  return totaal
      // return (((1000000.0 / gemiddelde)*60) / pulsenPerRev;  //  return totaal
      return ((1000000.0 * 60) / inter) / pulsenPerRev;  //  return totaal
      return (inter * pulsenPerRev) / (1000000.0 * 60);

    }



};