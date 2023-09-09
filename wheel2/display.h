




#define displayLengte 120
float displayData[displayLengte];

int nummersTeller = 0;

unsigned int displayDelay = 0;

int lichtLevel;





void displayInit(){
  pinMode(displayIN      , OUTPUT);
  pinMode(displayUIT     , INPUT);
  pinMode(displayKLOK    , OUTPUT);
  pinMode(displayLATCH   , OUTPUT);

  // setPwm(displayEN);

  pinMode(displayEN, OUTPUT);
  digitalWrite(displayEN, 1);

  // pwmWriteF(displayEN, 0.99);
  // pwmWriteF(displayEN, 0.5);

  pinMode(displayPOTMETER, INPUT);
}





int egtePos2displayPos(float pos){
  return mapF(pos, PLAAT_EINDE + SENSOR_OFFSET,  GROOTSTE_PLAAT_BEGIN, 0, displayLengte-1);
}






void displayPrint(float tijd){
  
  for(int i = 0; i < displayLengte; i++){
    
    
    // gpio_put(displayIN, displayData[i] >  tijd ? 1 : 0);

    int pix = (displayLengte - 1) - i;//flip display   
    pix = (pix + 7) - ((pix % 8) * 2);//flip byte
    
    
    gpio_put(displayIN, displayData[pix] >  tijd ? 1 : 0);//flip byte

    // gpio_put(displayIN, (i%16) != 0);//flip byte

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
    int naald = egtePos2displayPos(karPos);
    int target = egtePos2displayPos(targetNummerPos);
    int sensor = egtePos2displayPos(sensorPos);
    int sensorMaxBerijk = egtePos2displayPos(GROOTSTE_PLAAT_BEGIN - SENSOR_OFFSET)  +  3;

    int volumeMargin = displayLengte/5;//16;

    int dispHalf = displayLengte/2;





    if(staat == S_SCHOONMAAK){
      float verdeelPuntTeller = 0;

      for(int i = 0; i < displayLengte; i++){
        displayData[i] = 0;

        int volumePunt = mapF(armGewicht, 0, 4, (displayLengte-1) - volumeMargin,   volumeMargin);

        if(i < (displayLengte-1) - volumeMargin    &&    i > volumePunt){
          displayData[i] = 0.1;
        }


        int verdeelPunt = mapF(verdeelPuntTeller, 0, 4,  volumeMargin,  (displayLengte-1) - volumeMargin);
        if(i > verdeelPunt){
          verdeelPuntTeller += 0.5;          
        }

        if(i == verdeelPunt){
          displayData[i] = 0;
        }

        if(i == volumeMargin   ||   i == (displayLengte-1) - volumeMargin){ //eind van de schaal punt
          displayData[i] = 0.1;
        }

        if(i == volumePunt){
          displayData[i] = 0.9;
        }

      }
    }





    else if(staat == S_FOUTE_ORIENTATIE){
      float verdeelPuntTeller = 0;

      for(int i = 0; i < displayLengte; i++){
        displayData[i] = 0;

        float floatI = float(i) / displayLengte;

        if(isOngeveer(floatI, (orientatie.gefilterd*4) + 0.5, 0.1)){
          displayData[i] = 0.1;
        }


        
        if(i == int(dispHalf + displayLengte * 0.1 * 2 )){
          displayData[i] = 0.9;
        }

        if(i == int(dispHalf - displayLengte * 0.1 * 2 )){
          displayData[i] = 0.9;
        }

      }
    }





    else if(specialeDisplayActie.sinds() < 2000){
      for(int i = 0; i < displayLengte; i++){
        displayData[i] = 0;          

        // int volumePunt = mapF(volume, 0, 63, (displayLengte-1) - volumeMargin,   volumeMargin);

        // if(i < (displayLengte-1) - volumeMargin    &&    i > volumePunt){
        //   displayData[i] = 0.1;
        // }


        int volumePunt = mapF(volume, 0, 63, 0, 40);

        if(i < dispHalf + volumePunt    &&    i > dispHalf - volumePunt){
          displayData[i] = 0.1;
        }



        if( i == (displayLengte-1) - 2   &&   rpmStaat == AUTO){
          displayData[i] = 0.9;              
        }

        if(i == (displayLengte-1) - 7){
          if(isOngeveer(targetRpm, rpm33, 0.1)){
            displayData[i] = 0.1;
          }
          if(rpmStaat == R33){
            displayData[i] = 0.9;
          }        
        }

        if(i == (displayLengte-1) - 12){
          if(targetRpm == rpm45){
            displayData[i] = 0.1;
          }
          if(rpmStaat == R45){
            displayData[i] = 0.9;
          }        
        }    


      }
    
    
    
    
    }
    
    
    else{
      for(int i = 0; i < displayLengte; i++){


        int volgendeNummerDisplay  =  egtePos2displayPos(nummers[nummersTeller]);

        if( ( staat == S_NAAR_BEGIN_PLAAT  ||  staat == S_PLAAT_DIAMETER_METEN )    &&  i > sensor){
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
          
          
        }

        
        if(staat == S_NAAR_NUMMER  ||  staat ==  S_DOOR_SPOELEN  ||  staat == S_TERUG_SPOELEN  ||  staat == S_PAUZE){
          
          
          if(naald-1 == i || naald+1 == i){
            // displayData[i] = 0;
            if(millis()%1000 > 300){
              displayData[i] = 0.9;
            }
          }
          
          else if( i == target  &&  naald != i){
            displayData[i] = 0.9;
          }

          // if(naald == i){
          //   displayData[i] = 0;
          //   if(millis()%400 > 100){
          //     displayData[i] = 0.9;
          //   }
          // }

        }
        else if(naald == i){
          displayData[i] = 0.9;
        }


        
      }
    }






    displayPrint(0);
    
    commitDisplay();

    displayDelay = micros();
    while(micros() - displayDelay < 2){}
    
    digitalWrite(displayEN, 0);

    while(micros() - displayDelay < 200){}
    // delayMicroseconds(10);
    digitalWrite(displayEN, 1);


    displayPrint(0.5);
    // while(micros() - displayDelay < 75){}
    commitDisplay();


    digitalWrite(displayEN, 0);
    delayMicroseconds(4000);
    digitalWrite(displayEN, 1);

    // displayPrint(1);
    // // while(micros() - displayDelay < 400){}
    // commitDisplay();



    
    

    // if(lichtLevel == 0){
    //   displayPrint(0);//alles boven helderheid 0

    //   pwmWriteF(displayEN, 0.8); // display uit

    //   commitDisplay();

    //   lichtLevel++;
    // }
    // if(lichtLevel >= 1){
    //   displayPrint(0.5);//alles boven helderheid 0.5
      
    //   pwmWriteF(displayEN, 0.1); // display uit

    //   commitDisplay();

    //   lichtLevel = 0;
    // }


  }
}