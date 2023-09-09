




#define displayLengte 80
float displayData[displayLengte];

int displayRefreshDelay  = 20000;
float displayRefreshDelayNu;


float nummers[20] = {0.2, 0.3, 0.6, 0.68, 0.85};
int hoeveelNummers = 5;
int nummersTeller = 0;

float aa;

float delayStart = 500;


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
    
    digitalWrite(displayIN, displayData[i] >  displayRefreshDelayNu ? 1 : 0);

    // delayMicroseconds(1);
    digitalWrite(displayKLOK, 1);
    // delayMicroseconds(1);
    digitalWrite(displayKLOK, 0);
  }
  
  
  // delayMicroseconds(1);
  digitalWrite(displayLATCH, 0);

  digitalWrite(displayLATCH, 1);
}










INTERVAL displayInt(50, MICROS);
INTERVAL displayUpInt(10000, MICROS);

void displayUpdate(){
  // if(displayInt.loop()){
  //   displayPrint(0);
  // }

  if(displayUpInt.loop()){

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
      // displayData[i] = pow(  (  sin(  i/10.0   +   (millis()/100.0)  ) +1 ) /2   , 2);
      // displayData[i] = i > abs(sin( millis()/100.0)) * displayLengte ? 1.0 : 0;
      
      if(naald == pixel){
        displayData[pixel] = 1;
      }
      // displayData[i] = i + 1 > naald && i - 1 < naald? 1   : 0;
      // displayData[i] = 0;
      
    }

    // displayData[ int( abs(sin( millis()/100.0)) * displayLengte) ] = 10000;
    // displayData[ displayLengte/ 2 ] = 1.0;

    float lichtLevels = 2;
    float lichtLevel = 0;
    // float delayStart = 1;
    
    int delays[] = {0, 500};
    float delay = delayStart / pow(2, lichtLevels);

    displayPrint(0);
    displayPrint(0.5);
    // for(int i = 0;  i < lichtLevels;  i++){
    //   lichtLevel = i / lichtLevels;
    //   displayPrint(lichtLevel);
      
    //   delayMicroseconds(delays[i]);
    //   // delay *= 2;
    // }

    // displayPrint(2);


  }
}