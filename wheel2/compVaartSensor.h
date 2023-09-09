


Interval draaienInterval(10, MILLIS);

#define sampleMax 65500               //samples



 









Interval compInt(0, MILLIS);


class COMPVAART{
  public:
    volatile unsigned int vaartInterval;
                      int sampleNum;
    volatile          int samples[100];
    volatile unsigned int sampleTeller = 0;
    volatile unsigned long tijd;
    volatile unsigned int interval;
    float gemiddelde = sampleMax;

    
    int pulsenPerRev;
    int teller = 0;
    float radiaalTeller;

    float vaart;

    float glad;
    float gladglad;

    int medianSampleNum = 11;
    int medianTeller = 0;
    float medianSamples[100];
    float medianGesorteerd[100];    
    float median;


    float karPosMiddenPre;
    float karUitCenterGolf[1000];
    float karUitCenterMidden;  


    byte sens, sensPrev;



    int glitchTeller;

    int dir;
    int dirPrev;



    int faseVerschuiving = 90;//100;//90;//180;//90;  50 voor singletjes
    float plateauCompensatie[1000];
    bool compMeten = true;
    float preComp = 0;
    volatile float plateauComp = 0;
    float compFilter = 1.01;//2;
    float compVerval = 1.0;//0.6;//0.8;
    float compVermenigvuldiging = 0.8;

    float meanWaardehouder;
    float vorrigeWaarde;
    float mean;

    float sinTotaal[10];
    float cosTotaal[10];
    float sinWaardes[10][1000];
    float cosWaardes[10][1000];
    int harmonisen = 1;
    volatile float plateauCompFourier = 0;


    bool golven = false;    





    COMPVAART(int samps, float ppr){
      sampleNum = samps;
      pulsenPerRev = ppr;
      
      clearSamples();
      clearCompSamples();
    }




    void update(){
      if( micros() - vaartInterval > sampleMax ){
        if(glitchTeller > 3){
          shiftSamples(sampleMax * dir);
          vaart = 0;
          glad += (vaart - glad) / 10;
          gladglad += (glad - gladglad) / 10;
        }else{
          glitchTeller++;
        }
      }else{
        glitchTeller = 0;
      }
    }




	

    void interrupt(){
      tijd = micros();

      //------------------------------------------------------RICHTING
      dir = 1;
      sens = (gpio_get(plateauA) <<1)  |  gpio_get(plateauB);
      
      if( sens == 0b00 && sensPrev == 0b01 ||
          sens == 0b01 && sensPrev == 0b11 ||
          sens == 0b11 && sensPrev == 0b10 ||
          sens == 0b10 && sensPrev == 0b00
      ){
        dir = -1;
      }

      sensPrev = sens;


      if(dirPrev != dir){
        clearSamples();
      }
      dirPrev = dir;


      //------------------------------------------------------SNELHEID
      interval = tijd - vaartInterval;
      vaartInterval = tijd;
      
      if(interval > sampleMax){interval = sampleMax;}


      teller = rondTrip(teller + dir,  pulsenPerRev);
      radiaalTeller = ( teller * TAU ) / pulsenPerRev;

      shiftSamples(interval);// * dir);


      getVaart();

      glad += (vaart - glad) / 10;
      gladglad += (glad - gladglad) / 10;


      // getMedian();



      
      karPosMiddenPre -= karUitCenterGolf[teller];
      karUitCenterGolf[teller] = egteKarPos;
      karPosMiddenPre += karUitCenterGolf[teller];
      karPosMidden = karPosMiddenPre / pulsenPerRev;

      // sinTotaal -= sinWaardes[teller];
      // sinWaardes[teller] = sin(radiaalTeller * harm)  *  nieuweWaarde;
      // sinTotaal += sinWaardes[teller];
      
      // cosTotaal -= cosWaardes[teller];
      // cosWaardes[teller] = cos(radiaalTeller * harm)  *  nieuweWaarde;
      // cosTotaal += cosWaardes[teller];

      // // float hoek = (radiaalTeller * harm) + (faseShiftRad / harm);
      // float hoek = radiaalTeller;// + (faseShiftRad);
      // plateauCompFourier += ( sin(hoek) * sinTotaal ) / pulsenPerRev;        
      // plateauCompFourier += ( cos(hoek) * cosTotaal ) / pulsenPerRev;
      











      
      // plateauCompensatie[teller] += ( glad - targetRpm ) / 4;
      
      if( compMeten &&   //alle mementen waarom de compensatie niet mag werken, omdat er dan verschillen zijn met als de naald er egt op is
          plateauAan && 
          draaienInterval.sinds() > 1000 &&
          opsnelheid &&           
          (staat == S_HOMEN_VOOR_SPELEN ||
          staat == S_NAAR_BEGIN_PLAAT || 
          staat == S_SPELEN)

          // true
     
      ){ 
        if(isOngeveer(glad, targetRpm, 10)){
          plateauCompensatie[teller] += ( glad - targetRpm ) * compVermenigvuldiging; 
        }
      }

      // float vorrigeWaarde = plateauCompensatie[rondTrip(teller - random(20),  pulsenPerRev)];
      // plateauCompensatie[teller] *= compVerval;
      // plateauCompensatie[teller] = vorrigeWaarde + ((plateauCompensatie[teller] - vorrigeWaarde) / compFilter);

      // plateauComp = -plateauCompensatie[rondTrip(teller + faseVerschuiving,  pulsenPerRev)];

      preComp = plateauCompensatie[teller];
      float nieuweWaarde = plateauCompensatie[teller];
      plateauCompFourier = 0;
      
      float faseShiftRad = ( faseVerschuiving / (float)pulsenPerRev ) * (PI*2);

      meanWaardehouder -= vorrigeWaarde;
      meanWaardehouder += nieuweWaarde;
      vorrigeWaarde = nieuweWaarde;

      mean = meanWaardehouder / (float)pulsenPerRev;

      for(int harm = 1; harm < harmonisen + 1; harm++){
        sinTotaal[harm] -= sinWaardes[harm][teller];
        sinWaardes[harm][teller] = sin(radiaalTeller * harm)  *  nieuweWaarde;
        sinTotaal[harm] += sinWaardes[harm][teller];
        
        cosTotaal[harm] -= cosWaardes[harm][teller];
        cosWaardes[harm][teller] = cos(radiaalTeller * harm)  *  nieuweWaarde;
        cosTotaal[harm] += cosWaardes[harm][teller];

        // float hoek = (radiaalTeller * harm) + (faseShiftRad / harm);
        float hoek = (radiaalTeller * harm) + (faseShiftRad);
        plateauCompFourier += ( sin(hoek) * sinTotaal[harm] ) / pulsenPerRev;        
        plateauCompFourier += ( cos(hoek) * cosTotaal[harm] ) / pulsenPerRev;
      }


      plateauComp = -plateauCompFourier;//plateauCompensatie[rondTrip(teller + faseVerschuiving,  pulsenPerRev)];






      if(golven){
        Serial.print(vaart, 3);
        Serial.print(",");
        Serial.print(glad, 3);
        // Serial.print(",");
        // Serial.print(median, 3);
        Serial.println();        
      }
    }








    void clearCompSamples(){
      for(int i = 0; i < pulsenPerRev; i++){
        plateauCompensatie[i] = 0;
      }

      for(int harm = 1; harm < harmonisen + 1; harm++){
        sinTotaal[harm] = 0;
        cosTotaal[harm] = 0;
        for(int i = 0; i < pulsenPerRev; i++){
          sinWaardes[harm][i] = 0;
          sinWaardes[harm][i] = 0;
        }        
      }

      meanWaardehouder = 0;
    }





    void getMedian(){
      medianSamples[medianTeller++ % medianSampleNum] = vaart;

      for(int i = 0; i < medianSampleNum; i++){
        medianGesorteerd[i] = medianSamples[i];
      }

      
      int swapTeller = 1;
      while( swapTeller != 0 ){
        swapTeller = 0;
        
        for(int i = 0; i < medianSampleNum - 1; i++){
          
          if(medianGesorteerd[i] > medianGesorteerd[i + 1]){
            
            float buff = medianGesorteerd[i];  // swap de 2 waardes
            medianGesorteerd[i] = medianGesorteerd[i + 1];
            medianGesorteerd[i + 1] = buff;           
            
            swapTeller++;
          }                  
        }
      }

      median = medianGesorteerd[medianSampleNum / 2];
    }









    float getVaart(){

      gemiddelde = gemiddeldeInterval();
      vaart = huidigeVaart(gemiddelde) * dir;


      return vaart;//niet compensenre
    }    






    void shiftSamples(int samp){
      samples[ sampleTeller++ % sampleNum ] = samp;   
    }



    void clearSamples(){
      for(int i = 0; i < sampleNum; i++){
        samples[i] = sampleMax;// * dir;        
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

      float waarde = ((1000000.0 * 60) / inter) / pulsenPerRev;  //  return totaal
      return limieteerF(waarde, -300, 300);
    }



};