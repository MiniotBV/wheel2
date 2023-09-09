




#define displayLengte 80
float displayData[displayLengte];

int displayRefreshDelay  = 20000;
float displayRefreshDelayNu;

int nummersTeller = 0;

float aa;




unsigned int displayDelay = 0;





void displayInit(){
  pinMode(displayKLOK    , OUTPUT);
  pinMode(displayIN      , OUTPUT);
  pinMode(displayUIT     , INPUT);
  pinMode(displayLATCH   , OUTPUT);
  pinMode(displayPOTMETER, INPUT);
}










void displayPrint(float tijd){
  
  for(int i = 0; i < displayLengte; i++){
    
    
    gpio_put(displayIN, displayData[i] >  tijd ? 1 : 0);

    // delayMicroseconds(1);
    gpio_put(displayKLOK, 1);
    // delayMicroseconds(1);
    gpio_put(displayKLOK, 0);
  }
  
  
  // delayMicroseconds(1);

}




void commitDisplay(){
  gpio_put(displayLATCH, 0);
  gpio_put(displayLATCH, 1);
}









Interval displayInt(10000, MICROS);

void displayUpdate(){
  if(displayInt.loop()){
  // if(true){

    nummersTeller = 0;
    int naald = karPos2trackPos(karPos) * (displayLengte-1);
    int sensor = karPos2trackPos(karPos - SENSOR_NAALT_OFFSET) * (displayLengte-1);
    int sensorMaxBerijk = (displayLengte-1) - (karPos2trackPos(SENSOR_NAALT_OFFSET) * (displayLengte-1));




    for(int i = 0; i < displayLengte; i++){


      int volgendeNummerDisplay  =  nummers[nummersTeller] * (displayLengte - 1);

      if(staat == S_NAAR_BEGIN_PLAAT  &&  i > sensor){
        displayData[i] = 0;
      }

      else if(i > sensorMaxBerijk){
        displayData[i] = 0;
      }

      else if(volgendeNummerDisplay == i){
        nummersTeller++;
        displayData[i] = 0;
      }
      
      else{
        if(nummersTeller == 0){
          displayData[i] = 0;
        }else{
          displayData[i] = 0.01;
        }
        
        // displayData[i] = 0;
      }

      // displayData[i] = (sin( (i / 0.02) + aa )+1)/4;
    }

    // aa += 0.1;

    
    // int naald = ((sin( millis()/2000.0)+1)/2) * (displayLengte-1);
    // int naald = ((sin( aa += 0.01 )+1.0)/2.0) * (displayLengte-1);

    for(int pixel = 0; pixel < displayLengte; pixel++){
      if(naald == pixel){
        displayData[pixel] = 0.9;
      }
    }


    displayPrint(0);
    displayDelay = micros();
    commitDisplay();


    displayPrint(0.5);
    while(micros() - displayDelay < 75){}
    commitDisplay();


    displayPrint(1);
    while(micros() - displayDelay < 400){}
    commitDisplay();



    // delayMicroseconds(10000);

  }
}