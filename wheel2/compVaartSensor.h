float targetRpm = 0;
bool plateauAan = false;

bool opsnelheid;
bool uitdraaien;

Interval draaienInterval(10, MILLIS);







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
    volatile          int samples[1000];
    volatile unsigned int sampleTeller = 0;
    volatile unsigned long tijd;
    volatile unsigned int interval;
    float gemiddelde = sampleMax;


    
    float vaart;

    float glad;
    float gladglad;
    float div;

    byte sens, sensPrev;



    int glitchTeller;
    // int terugdraaiTeller = 0;

    int dir;
    int dirPrev;
    int andereDirTeller;

    int pulsenPerRev;
    int teller = 0;

    int faseVerschuiving = 90;//180;//90;  50 voor singletjes
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
    int harmonisen = 2;
    volatile float plateauCompFourier = 0;





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
      // dir = -1;
      dir = 1;
      sens = (gpio_get(plateauA) <<1)  |  gpio_get(plateauB);
      
      if( sens == 0b00 && sensPrev == 0b01 ||
          sens == 0b01 && sensPrev == 0b11 ||
          sens == 0b11 && sensPrev == 0b10 ||
          sens == 0b10 && sensPrev == 0b00
      ){
        // dir = 1;
        dir = -1;
      }

      sensPrev = sens;


      if(dirPrev != dir  &&  andereDirTeller < 2){
        andereDirTeller++;
        return;
      }
      andereDirTeller = 0;
      dirPrev = dir;


      //------------------------------------------------------SNELHEID
      interval = tijd - vaartInterval;
      vaartInterval = tijd;
      
      if(interval > sampleMax){interval = sampleMax;}


      teller = rondTrip(teller + dir,  pulsenPerRev);

      shiftSamples(interval);// * dir);


      getVaart();
      // getDiv();

      glad += (vaart - glad) / 10;
      gladglad += (glad - gladglad) / 10;






      
      // plateauCompensatie[teller] += ( glad - targetRpm ) / 4;
      
      if( compMeten && 
          plateauAan && 
          draaienInterval.sinds() > 1000 &&
          opsnelheid &&           
          (staat == S_HOMEN_VOOR_SPELEN ||
          // staat == S_BEGINNEN_SPELEN ||
          // staat == S_PLAAT_AANWEZIG ||
          staat == S_NAAR_BEGIN_PLAAT || 
          staat == S_NAALD_EROP)
     
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
      float radialen = ( teller / (float)pulsenPerRev ) * (PI*2);
      float faseShiftRad = ( faseVerschuiving / (float)pulsenPerRev ) * (PI*2);

      meanWaardehouder -= vorrigeWaarde;
      meanWaardehouder += nieuweWaarde;
      vorrigeWaarde = nieuweWaarde;

      mean = meanWaardehouder / (float)pulsenPerRev;

      for(int harm = 1; harm < harmonisen + 1; harm++){
        sinTotaal[harm] -= sinWaardes[harm][teller];
        sinWaardes[harm][teller] = sin(radialen * harm)  *  nieuweWaarde;
        sinTotaal[harm] += sinWaardes[harm][teller];
        
        cosTotaal[harm] -= cosWaardes[harm][teller];
        cosWaardes[harm][teller] = cos(radialen * harm)  *  nieuweWaarde;
        cosTotaal[harm] += cosWaardes[harm][teller];

        // float hoek = (radialen * harm) + (faseShiftRad / harm);
        float hoek = (radialen * harm) + (faseShiftRad);
        plateauCompFourier += ( sin(hoek) * sinTotaal[harm] ) / pulsenPerRev;        
        plateauCompFourier += ( cos(hoek) * cosTotaal[harm] ) / pulsenPerRev;
      }


      plateauComp = -plateauCompFourier;//plateauCompensatie[rondTrip(teller + faseVerschuiving,  pulsenPerRev)];
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
      vaart = huidigeVaart(gemiddelde);


      return vaart;//niet compensenre
    }    





    float getDiv(){
      div = getVaart() / calibratieToon.getVaart();

      return div;
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
      return (totaal / float(sampleNum)) * dir;      
    }



    float huidigeVaart(float inter){//                                                           RPM BEREKENEN

      float waarde = ((1000000.0 * 60) / inter) / pulsenPerRev;  //  return totaal
      return limieteerF(waarde, -300, 300);
    }



};