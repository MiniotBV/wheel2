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





float targetRpm = 0;

#define sampleMax 65500               //samples



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

    int dir;

    float pulsenPerRev;
    int teller = 0;
    int pin;


    float compVals[4096];
    float actualPuls[4096];
    float gemiddeldePuls[4096];

//
    int compFilterLen = 7;//80voor 12tanden en 52 voor 12     (6.6 samples per tand)
    int compFilterTeller = 0;
    float compFilter[7];



    COMPVAART(int samps, int p, float ppr){
      sampleNum = samps;
      pin = p;
      pulsenPerRev = ppr;

      

      clearSamples();
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
      

      shiftSamples(interval * dir);
      // shiftSamples(interval);

      // teller = limieteerI(teller + dir,   0,   pulsenPerRev);  
      
      teller += dir;
      
      if(teller >= pulsenPerRev){
        teller = 0;
        motorOffset = motorPos;
      }
      if(teller < 0){
        teller = pulsenPerRev - 1;
      }


      if(golven){
        float stappen = (motorPos - motorOffset) / (PI*2);
        plaatPos = (stappen * 4096) / stappenPerRev;

        
        // actualPuls[teller] = motorPos - motorOffset;plaatPos
        actualPuls[teller] = plaatPos;

        int tellerMinEen = teller - 1;
        if(teller == 0){
          tellerMinEen = 4095;
          compVals[teller] = (actualPuls[teller] + 4096) - actualPuls[tellerMinEen];
        }else{
          compVals[teller] = actualPuls[teller] - actualPuls[tellerMinEen];
        }
        

        // gemiddeldePuls += (compVals[teller] - gemiddeldePuls) / 10;

        compFilter[compFilterTeller++ % compFilterLen] = compVals[teller];


        // float compGefilterd = 0;
        // for(int i = 0; i < compFilterLen; i++){
        //   compGefilterd += compFilter[i];
        // }
        // compGefilterd /= compFilterLen;


        float compGefilterd = compVals[teller];


        gemiddeldePuls[teller] += (  compGefilterd  -  gemiddeldePuls[teller]  )  / 20;
        gemiddeldePuls[teller] += (  gemiddeldePuls[(teller + 1 ) % 4096]  -  gemiddeldePuls[teller]  )  / 5;
        gemiddeldePuls[teller] += (  gemiddeldePuls[tellerMinEen]  -  gemiddeldePuls[teller]  )  / 5;
      
        Serial.print(teller);
        // Serial.print(", ");
        // Serial.print(motorPos - motorOffset);
        Serial.print(", ");
        Serial.print(plaatPos);
        Serial.print(", ");
        Serial.print(teller - plaatPos);
        Serial.print(", ");
        Serial.print(compVals[teller]);
        Serial.print(", ");
        Serial.print(compGefilterd, 3);
        Serial.print(", ");
        Serial.print(gemiddeldePuls[teller], 3);

        Serial.println();
      }

    }



    



    void printSamples(){
 
      for(int i = 0;   i < pulsenPerRev;   i++){
        Serial.println(gemiddeldePuls[i], 3);
        delay(1);
      }
    }




    void saveSamples(){
      // int i;
      for(int i = 0;   i < pulsenPerRev;   i+=2){
        eepSchrijfFloat(i,   (gemiddeldePuls[i] + gemiddeldePuls[i+1])/2);
      }

      bool error = EEPROM.commit();
      Serial.println(error ? "opgeslagen: "   :   "faaal: ");
    }





    void recalSamples(){
      for(int i = 0;   i < pulsenPerRev;   i+=2){
        gemiddeldePuls[i] = eepLeesFloat(i);
        gemiddeldePuls[i+1] = gemiddeldePuls[i];
      }

      // Serial.println("gerecalt");
    }


    void clearSamples(){
      for(int i = 0; i < pulsenPerRev; i++){
        gemiddeldePuls[i] = 1;     
      }
      // Serial.println("gecleart");
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

      
      // vaart *= compensatieAct[teller / compensatiePerSample]; // compenseren
      
      // float b = targetRpm / vaart; //compenstaie berekenen
      // compensatie[teller / compensatiePerSample]    +=    (b - compensatie[teller / compensatiePerSample]) / 12;
      
      // return vaart * compensatieAct[teller / compensatiePerSample]; // compenseren

      return vaart;//niet compensenre
    }    






    float getGlad(){
      glad += (vaart - glad) / 10;
      return glad;
    }




    void shiftSamples(int samp){
      samples[ sampleTeller++ % sampleNum ] = samp;   
    }



    void lclearSamples(){
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






























