float targetRpm = 0;

#define sampleMax 65500               //samples

#include <EEPROM.h>

 




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










class COMPVAART{
  public:
    volatile unsigned int vaartInterval;
    volatile unsigned int sampleNum;
    volatile          int samples[100];
    volatile unsigned int sampleTeller = 0;
    volatile unsigned long tijd;
    volatile unsigned int interval;
    float vaart;
    float gemiddelde = sampleMax;

    float glad;

    int glitchTeller;

    bool Anu;
    bool Aoud;
    int dir;

    float pulsenPerRev;
    int teller = 0;
    int pin;

    float compSamples[4096];
    bool compAan = true;


    COMPVAART(int samps, int p, float ppr){
      sampleNum = samps;
      pin = p;
      pulsenPerRev = ppr;
      clearSamples();

      clearCompSamples();

    }






    void interrupt(){
      tijd = micros();

      dir = 1;
      if(gpio_get(plateauB)){
        dir = -1;
      }  
      
      
      interval = tijd - vaartInterval;
      vaartInterval = tijd;
      
      if(interval > sampleMax){interval = sampleMax;}
      

      
      // shiftSamples(interval);

      // teller = limieteerI(teller + dir,   0,   pulsenPerRev);  
      teller += dir;
      if(teller >= pulsenPerRev){teller = 0;}
      if(teller < 0){teller = pulsenPerRev - 1;}

      shiftSamples((interval * dir) / compSamples[teller]);
    }






    void recalCompSamples(){
      for(int i = 0;   i < pulsenPerRev;   i+=2){
        compSamples[i] = eepLeesFloat(i);
        compSamples[i+1] = compSamples[i];
      }

      // Serial.println("gerecalt");
    }

    void clearCompSamples(){
      for(int i = 0;   i < pulsenPerRev;   i++){
        compSamples[i] = 1;
      }
    }






    void printSamples(){
      for(int i = 0;   i < pulsenPerRev;   i++){
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
      vaart =  huidigeVaart();


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



    float huidigeVaart(){//                                                           RPM BEREKENEN
      if(gemiddelde >= sampleMax){
        return 0;          
      }
      // return (1000000 / gemiddeldeInterval())/4;  //  return totaal
      // return (((1000000.0 / gemiddelde)*60) / pulsenPerRev;  //  return totaal
      return (((1000000.0 * 60) / gemiddelde)) / pulsenPerRev;  //  return totaal

    }



};