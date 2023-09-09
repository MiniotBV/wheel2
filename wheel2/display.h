




#define displayLengte 80
float displayData[displayLengte];

int nummersTeller = 0;

unsigned int displayDelay = 0;





void displayInit(){
  pinMode(displayKLOK    , OUTPUT);
  pinMode(displayIN      , OUTPUT);
  pinMode(displayUIT     , INPUT);
  pinMode(displayLATCH   , OUTPUT);
  pinMode(displayPOTMETER, INPUT);
}





int trackPos2displayPos(float kp){
  return mapF(kp, PLAAT_EINDE + SENSOR_OFFSET,  PLAAT_BEGIN, 0, displayLengte-1);
}






void displayPrint(float tijd){
  
  for(int i = 0; i < displayLengte; i++){
    
    
    gpio_put(displayIN, displayData[i] >  tijd ? 1 : 0);

    gpio_put(displayKLOK, 1);
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
    int naald = trackPos2displayPos(karPos);
    int sensor = trackPos2displayPos(sensorPos);
    int sensorMaxBerijk = trackPos2displayPos(PLAAT_BEGIN - SENSOR_OFFSET)  +  3;




    for(int i = 0; i < displayLengte; i++){


      int volgendeNummerDisplay  =  trackPos2displayPos(nummers[nummersTeller]);

      if(staat == S_NAAR_BEGIN_PLAAT  &&  i > sensor){
        displayData[i] = 0;
      }

      else if(i > sensorMaxBerijk){
        displayData[i] = 0;
      }

      else if(volgendeNummerDisplay <= i     &&    nummersTeller < hoeveelNummers){
        nummersTeller++;
        displayData[i] = 0;
      }
      
      else{
        if(nummersTeller == 0){
          displayData[i] = 0;
        }else{
          displayData[i] = 0.1;
        }
        
        // displayData[i] = 0;
      }
    }



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
  }
}