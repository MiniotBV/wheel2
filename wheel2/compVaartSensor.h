


Interval draaienInterval(10, MILLIS);

#define sampleMax 65500               //samples

#define pprmax 1000

 









Interval compInt(0, MILLIS);


class COMPVAART
{ 
  public:
    //-------------------------------------snelheid
    volatile unsigned int vaartInterval;
                      int sampleNum;
    volatile          int samples[100];
    volatile unsigned int sampleTeller = 0;
    volatile unsigned int tijd;
    volatile unsigned int interval;
    float gemiddelde = sampleMax;

    unsigned int procesTijd;
    unsigned int procesInterval;

    //-----------------------------------richting
    byte sens, sensPrev;
    int dir;
    int dirPrev;
    int glitchTeller;
    int pulsenPerRev;
    int teller = 0;

    
    //---------------------------------versamplede sinus en cos
    float sinus[pprmax];
    float cosin[pprmax];
    
    

    //---------------------------------------filter
    float vaart;
    float glad;
    float gladglad;





    //---------------------------------------uit Center Compensatie
    float karPosMiddenPre;
    float karUitCenterGolf[pprmax];
    float karUitCenterMidden;

    float karSinWaardes[pprmax];
    float karCosWaardes[pprmax];
    float karSin;
    float karCos;
    float karFourier;

    float karSinFilt;
    float karCosFilt;
    float karFourierFilt;




    //---------------------------------------------onbalans compensatie
    bool onbalansCompAan = true;
    int onbalansFase = 70;//90;//75;//90;  50 in pulsen per rev
    
    float gemiddeldeSnelheidPre, gemiddeldeSnelheid;
    
    float onbalansCompensatie[pprmax];

    volatile float onbalansComp = 0;
    
    float compVerval = 1.0;//0.6;//0.8;
    float onbalansCompGewicht = 0.8;


    float  onbalansSinTotaal[5];
    float  onbalansCosTotaal[5];
    float onbalansSinWaardes[5][pprmax];
    float onbalansCosWaardes[5][pprmax];
    int harmonisen = 3;
    volatile float onbalansCompFourier = 0;




    bool golven = false;    
    bool plaatUitMiddenComp = true;




    COMPVAART(int samps, float ppr){
      sampleNum = samps;
      pulsenPerRev = ppr;
      
      clearSamples();
      clearCompSamples();

      for(int i = 0; i < pulsenPerRev; i++){
        float radiaalTeller = ( i * TAU ) / pulsenPerRev;
        sinus[i] = sin(radiaalTeller);
        cosin[i] = cos(radiaalTeller);
      }
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

      if(!isOngeveer(centerCompTargetRpm, targetRpm, 5)){
        centerCompTargetRpm = targetRpm;
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
        clearOnbalansCompSamples();
      }
      dirPrev = dir;


      //------------------------------------------------------SNELHEID
      interval = tijd - vaartInterval;
      vaartInterval = tijd;
      
      if(interval > sampleMax){interval = sampleMax;}


      teller = rondTrip(teller + dir,  pulsenPerRev);
      // cosTeller = rondTrip(teller + (pulsenPerRev / 4), pulsenPerRev);
      // radiaalTeller = ( teller * TAU ) / pulsenPerRev;

      shiftSamples(interval);// * dir);


      getVaart();

      glad += (vaart - glad) / 10;
      gladglad += (glad - gladglad) / 10;


      

      //-----------------------------------------------------------------------UIT HET MIDDEN COMPENSATIE
      karPosMiddenPre -= karUitCenterGolf[teller];
      karUitCenterGolf[teller] = egteKarPos;
      karPosMiddenPre += karUitCenterGolf[teller];
      karPosMidden = karPosMiddenPre / pulsenPerRev;

      float karPosUitMidden = egteKarPos - karPosMidden;

      if(isNaaldEropVoorZoLang(1000) && staat == S_SPELEN){ // naald moet er ff opzitten in spelen staat voor ie gaat rekenene
        karSin -= karSinWaardes[teller];
        karSinWaardes[teller] = sinus[teller]  *  karPosUitMidden;
        karSin += karSinWaardes[teller];
        
        karCos -= karCosWaardes[teller];
        karCosWaardes[teller] = cosin[teller]  *  karPosUitMidden;
        karCos += karCosWaardes[teller];
      }

      karFourier  = ( ( ( sinus[teller] * karSin )  +  ( cosin[teller] * karCos ) ) / pulsenPerRev ) * 2;

      karSinFilt += ( karSin - karSinFilt ) / 2000;
      karCosFilt += ( karCos - karCosFilt ) / 2000;

      karFourierFilt  = ( ( ( sinus[teller] * karSinFilt )  +  ( cosin[teller] * karCosFilt ) )  / pulsenPerRev  ) * 2;
      


      int leadTeller = rondTrip(teller - (8+9), pulsenPerRev);
      float uitMiddenSnelheidsComp = ( ( ( sinus[leadTeller] * karSinFilt )  +  ( cosin[leadTeller] * karCosFilt ) )  / pulsenPerRev  ) * 2;

      centerCompTargetRpm = targetRpm *  (( karPosMidden - uitMiddenSnelheidsComp ) / karPosMidden );

      

      float sinBuff = karSinFilt / pulsenPerRev;
      float cosBuff = karCosFilt / pulsenPerRev;
      if( sinBuff * sinBuff  +  cosBuff * cosBuff  >  3 * 3){ // een uit het midden hijd van 6mm (3mm radius) triggerd error
        setError(E_TE_GROTE_UITSLAG);
        stoppen();
      }





      procesTijd = micros();

      //-----------------------------------------------------------------------------UIT BALANS COMPENSATIE
      // gemiddeldeSnelheidPre -= onbalansCompensatie[teller];

      
      if( onbalansCompAan &&   //alle mementen waarom de compensatie niet mag werken, omdat er dan verschillen zijn met als de naald er egt op is
          plateauAan && 
          draaienInterval.sinds() > 1000 && //moet 1 seconden aan staan
          opsnelheid &&                      // en opsnelheid zijn     
          (staat == S_HOMEN_VOOR_SPELEN ||    
          staat == S_NAAR_BEGIN_PLAAT || 
          staat == S_SPELEN)

          // true
     
      ){ 
        if(isOngeveer(glad, targetRpm, 10)){
          if(plaatUitMiddenComp){
            onbalansCompensatie[teller] += ( glad - centerCompTargetRpm ) * onbalansCompGewicht; 
          }else{
            onbalansCompensatie[teller] += ( glad - targetRpm ) * onbalansCompGewicht; 
          }
          
        }
      }

      // onbalansCompensatie[teller] *= compVerval;

      float nieuweWaarde = onbalansCompensatie[teller];

      onbalansCompFourier = 0;

      for(int harm = 1; harm < harmonisen + 1; harm++){
        int hoek = rondTrip(teller * harm,  pulsenPerRev);

        onbalansSinTotaal[harm] -= onbalansSinWaardes[harm][teller];
        onbalansSinWaardes[harm][teller] = sinus[hoek]  *  nieuweWaarde;
        onbalansSinTotaal[harm] += onbalansSinWaardes[harm][teller];
        
        onbalansCosTotaal[harm] -= onbalansCosWaardes[harm][teller];
        onbalansCosWaardes[harm][teller] = cosin[hoek]  *  nieuweWaarde;
        onbalansCosTotaal[harm] += onbalansCosWaardes[harm][teller];

        hoek = rondTrip(hoek + onbalansFase,  pulsenPerRev);
        onbalansCompFourier += ( ( sinus[hoek] * onbalansSinTotaal[harm] ) + ( cosin[hoek] * onbalansCosTotaal[harm] ) ) / pulsenPerRev;
      }


      if(onbalansCompAan){
        onbalansComp = -onbalansCompFourier;//onbalansCompensatie[rondTrip(teller + onbalansFase,  pulsenPerRev)];
      }else{
        onbalansComp = 0;
      }
      




      procesInterval = micros() - procesTijd;




      if(golven){
        Serial.print(vaart, 3);
        Serial.print(",");
        Serial.print(glad, 3);
        Serial.print(",");
        Serial.print(karFourier, 3);
        Serial.println();        
      }


      
    }








    void clearCompSamples(){
      clearOnbalansCompSamples();
      clearCenterCompSamples();
    }




    void clearOnbalansCompSamples(){
      for(int i = 0; i < pulsenPerRev; i++){
        onbalansCompensatie[i] = 0;
      }

      for(int harm = 1; harm < harmonisen + 1; harm++){
        onbalansSinTotaal[harm] = 0;
        onbalansCosTotaal[harm] = 0;
        for(int i = 0; i < pulsenPerRev; i++){
          onbalansSinWaardes[harm][i] = 0;
          onbalansCosWaardes[harm][i] = 0;
        }        
      }

    }



    void clearCenterCompSamples(){
      for(int i = 0; i < pulsenPerRev; i++){
        karSinWaardes[i] = 0;
        karCosWaardes[i] = 0;
        karUitCenterGolf[i] = 0;
      }

      karSin = 0;
      karCos = 0;
      karPosMiddenPre = 0;

      karSinFilt = 0;
      karCosFilt = 0;
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