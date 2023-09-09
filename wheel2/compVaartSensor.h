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
    volatile unsigned int sampleNum;
    volatile          int samples[200];
    volatile unsigned int sampleTeller = 0;
    volatile unsigned long tijd;
    volatile unsigned int interval;
    float gemiddelde = sampleMax;

    int filterOrde = 4;
    float filterWaarde = 1/20.0;
    float filter[12];
    
    float vaart;
    float vaartRuw;
    float gladNieuw;
    float glad;
    float div;
    float dav;

    int glitchTeller;

    int dir;

    float pulsenPerRev;
    int teller = 0;

    
    // float compSamples[4096];
    float compSamples[8192];
    bool compensatieMeten = false;


    COMPVAART(int samps, float ppr){
      sampleNum = samps;
      pulsenPerRev = ppr;
      
      clearSamples();
      clearCompSamples();
    }




	

    void interrupt(){
      tijd = micros();

      // bool A = gpio_get(plateauA);
      // bool B = gpio_get(plateauB);

      dir = 1;
      // if(!A && B   ||  A && !B){
      //   dir = -1;
      // }  
      
      
      interval = tijd - vaartInterval;
      vaartInterval = tijd;
      
      if(interval > sampleMax){interval = sampleMax;}
      

      
      // shiftSamples(interval);

      // teller = limieteerI(teller + dir,   0,   pulsenPerRev);  
      teller += dir;
      if(teller >= pulsenPerRev){teller = 0;}
      if(teller < 0){teller = pulsenPerRev - 1;}


      vaartRuw = huidigeVaart(interval);    
      vaart = getVaart();


      filter[0] = vaartRuw;
            
      for(int i = 1;   i < filterOrde + 1;   i++){
        filter[i] +=  ( filter[i-1] - filter[i]) * filterWaarde;
      }

      
      


      // div = getVaart() / strobo.getVaart();
      div = vaart / strobo.glad;
      
      if(compensatieMeten){
        

        if(isOngeveer(div, 1, 0.3)){
          // if(compInt.sinds() < 20000){//als er pas net compensatie modus is;
          //   compSamples[teller] += ( div - compSamples[teller] ) / 2;
          // }else{
          //   compSamples[teller] += ( div - compSamples[teller] ) / 100;
          // }
          compSamples[teller] += (div-1) / 10;
        }
        
        // shiftSamples((interval * dir));
        // shiftSamples((interval * dir) * compSamples[teller]);
      }
      

      gladNieuw = filter[filterOrde] / compSamples[teller];

      shiftSamples((interval * dir) * compSamples[teller]);
      
      dav = compSamples[teller];
      
      
    }


    void setCompensatieModus(bool c){
      compensatieMeten = c;
      compInt.reset();

      if(compensatieMeten){
        // clearCompSamples();
        Serial.println("meten aan");
      }else{
        Serial.println("meten uit");
      }
    }


    void toggleCompensatieModus(){
      setCompensatieModus(!compensatieMeten);

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






    float getGlad(){
      glad += (vaart - glad) / 10;
      return glad;
    }




    void shiftSamples(int samp){
      samples[ sampleTeller++ % sampleNum ] = samp;   
    }



    void clearSamples(){
      for(int i = 0; i < sampleNum; i++){
        samples[i] = sampleMax;        
      }
    }



    double gemiddeldeInterval(){
      double totaal = 0;
      
      for(byte i = 0;   i < sampleNum;   i++){
        totaal += samples[i];
      }
      return totaal / sampleNum;      
    }



    float huidigeVaart(float inter){//                                                           RPM BEREKENEN

      // return (1000000 / gemiddeldeInterval())/4;  //  return totaal
      // return (((1000000.0 / gemiddelde)*60) / pulsenPerRev;  //  return totaal
      return (((1000000.0 * 60) / inter)) / pulsenPerRev;  //  return totaal

    }



};