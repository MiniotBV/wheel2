#define sampleMax 65500               //samples



class VAART{
  public:
    volatile unsigned int vaartInterval;
    volatile unsigned int sampleNum;
    volatile unsigned int samples[100];
    volatile unsigned int sampleTeller = 0;
    volatile unsigned long tijd;
    volatile unsigned int interval;
    volatile float vaart;
    volatile int gemiddelde = sampleMax;
    volatile int gemiddeldePrev = sampleMax;

    float pulsenPerRev;
    int teller = 0;



    VAART(int samps, float ppr){
      sampleNum = samps;
      pulsenPerRev = ppr;
      for(int i = 0; i < sampleNum; i++){
        samples[i] = sampleMax;        
      }
    }

    void interrupt(){
      tijd = micros();
      interval = tijd - vaartInterval;
      vaartInterval = tijd;
      
      if(interval > sampleMax){interval = sampleMax;}
      
      shiftSamples(interval);

      teller = teller > 4096 ? 0 : teller + 1;  
    }







    void printSamples(){
      Serial.print("s: ");
      for(byte i = 0;   i < sampleNum;   i++){
        Serial.print(samples[i]);
        Serial.print(", ");
      }
      Serial.println();
    }


  float getVaart(){
    gemiddelde = gemiddeldeInterval();
    vaart =  huidigeVaart();
    return vaart;
  }    




    void shiftSamples(unsigned int samp){
      samples[ sampleTeller++ % sampleNum ] = samp;   
    }

    double gemiddeldeInterval(){
      double totaal = 0;
      
      for(byte i = 0;   i < sampleNum;   i++){
        totaal += samples[i];
      }
      return totaal / sampleNum;      
    }



    float huidigeVaart(){//                                                           RPM BEREKENEN
      // return (1000000 / gemiddeldeInterval())/4;  //  return totaal
      return ((1000000.0 / gemiddelde)*60) / pulsenPerRev;  //  return totaal
    }



};