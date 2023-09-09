




#define displayLengte 80
float displayData[displayLengte];

int displayRefreshDelay  = 20000;
float displayRefreshDelayNu;




void displayInit(){
  pinMode(displayKLOK    , OUTPUT);
  pinMode(displayIN      , INPUT_PULLUP);
  pinMode(displayUIT     , OUTPUT);
  pinMode(displayLATCH   , OUTPUT);
  pinMode(displayPOTMETER, OUTPUT);
}



INTERVAL displayInt(1000, MICROS);

void displayUpdate(){
  if(displayInt.loop()){
    
  }
}







void displayPrint(){

  displayRefreshDelayNu =   float( micros()  %  displayRefreshDelay )   /   displayRefreshDelay; 
  
  for(int i = 0; i < displayLengte; i++){
    
    digitalWrite(displayIN, displayData[i] >  displayRefreshDelayNu);

    digitalWrite(displayKLOK, 1);
    digitalWrite(displayKLOK, 0);
  }
  
  digitalWrite(displayLATCH, 1);
  digitalWrite(displayLATCH, 0);
}