




#define displayLengte 80
float displayData[displayLengte];

int displayRefreshDelay  = 20000;
float displayRefreshDelayNu;


float nummers[20] = {0.2, 0.3, 0.6, 0.68, 0.85};
int hoeveelNummers = 5;
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

  // displayRefreshDelayNu =   ( micros()  %  displayRefreshDelay )   /   float(displayRefreshDelay); 
  
  // displayRefreshDelayNu  =  displayRefreshDelayNu  >=  1  ?  0  :   displayRefreshDelayNu + 0.025;
  displayRefreshDelayNu = tijd;

  
  for(int i = 0; i < displayLengte; i++){
    
    
    gpio_put(displayIN, displayData[i] >  displayRefreshDelayNu ? 1 : 0);

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








INTERVAL displayInt(50, MICROS);
INTERVAL displayUpInt(10000, MICROS);

void displayUpdate(){
  // if(displayInt.loop()){
  //   displayPrint(0);
  // }

  if(displayUpInt.loop()){
  // if(true){

    nummersTeller = 0;

    for(int i = 0; i < displayLengte; i++){

      int volgendeNummerDisplay  =  nummers[nummersTeller] * (displayLengte - 1);

      if(volgendeNummerDisplay == i){
        nummersTeller++;
        displayData[i] = 0;
      }else{
        displayData[i] = 0.01;
        // displayData[i] = 0;
      }

      // displayData[i] = (sin( (i / 0.02) + aa )+1)/4;
    }

    aa += 0.1;

    int naald = ((sin( millis()/2000.0)+1)/2) * (displayLengte-1);
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