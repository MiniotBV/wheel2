#define SAMPLE_MAX 65500               //samples

class StroboSpeed
{
  public:
  
  StroboSpeed(int samps, float ppr)
  {
    sampleNum = samps;
    pulsenPerRev = ppr;
    for(int i = 0; i < sampleNum; i++) samples[i] = SAMPLE_MAX;
  }

  volatile unsigned int vaartInterval;
  volatile unsigned int sampleNum;
  volatile unsigned int samples[100];
  volatile unsigned int sampleTeller = 0;
  volatile unsigned long tijd;
  volatile unsigned int interval;
  volatile float vaart;
  volatile int gemiddelde = SAMPLE_MAX;
  volatile int gemiddeldePrev = SAMPLE_MAX;

  float pulsenPerRev;
  int teller = 0;

  int filterOrde = 3;
  float filterWaarde = 0.4;
  float filter[5];   
  float glad;

  void interrupt()
  {
    tijd = micros();
    interval = tijd - vaartInterval;
    vaartInterval = tijd;
    
    if(interval > SAMPLE_MAX) interval = SAMPLE_MAX;

    shiftSamples(interval);

    // filter[0] = huidigeVaart(interval);
    // vaart = filter[0];
          
    // for(int i = 1;   i < filterOrde + 1;   i++){
    //   filter[i] +=  ( filter[i-1] - filter[i]) * filterWaarde;
    // }

    // glad = filter[filterOrde];

    teller = teller > 4096 ? 0 : teller + 1;  
  }


  void printSamples()
  {
    Serial.print("s: ");
    for(byte i = 0;   i < sampleNum;   i++){
      Serial.print(samples[i]);
      Serial.print(", ");
    }
    Serial.println();
  }

  float getVaart()
  {
    gemiddelde = gemiddeldeInterval();
    vaart =  huidigeVaart(gemiddelde);
    return vaart;
  }    

  void shiftSamples(unsigned int samp)
  {
    samples[ sampleTeller++ % sampleNum ] = samp;   
  }

  float gemiddeldeInterval()
  {
    double totaal = 0;
    
    for(byte i = 0;   i < sampleNum;   i++){
      totaal += samples[i];
    }
    return totaal / sampleNum;      
  }

  float huidigeVaart(float inter)
  {//                                                           RPM BEREKENEN
    // return (1000000 / gemiddeldeInterval())/4;  //  return totaal
    return ((1000000.0f / inter) * 60.0f) / pulsenPerRev;  //  return totaal
  }

};