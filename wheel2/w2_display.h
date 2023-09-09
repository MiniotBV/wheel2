#define D_LENGTH 80   // displayLengte 80
class Display
{
  public:

  float data[D_LENGTH];
  int songCounter = 0;      // nummersTeller = 0;
  unsigned int nDelay = 0;   // displayDelay = 0;
  Interval interval {10000, inMICROS};

  void init()
  {
    pinMode(displayKLOK    , OUTPUT);
    pinMode(displayIN      , OUTPUT);
    pinMode(displayUIT     , INPUT);
    pinMode(displayLATCH   , OUTPUT);
    pinMode(displayPOTMETER, INPUT);
  }

  int convertToDisplayPos(float pos)
  {
    return mapF(pos, PLAAT_EINDE + SENSOR_OFFSET, PLAAT_BEGIN, 0, D_LENGTH - 1);
  }

  void d_print(float tijd)    // displayPrint(float tijd)
  {
    for(int i = 0; i < D_LENGTH; i++)
    {
      gpio_put(displayIN, data[i] > tijd ? 1 : 0);
      gpio_put(displayKLOK, 1);
      gpio_put(displayKLOK, 0);
    }
    // delayMicroseconds(1);
  }

  void commitDisplay()
  {
    gpio_put(displayLATCH, 0);
    gpio_put(displayLATCH, 1);
  }


  void update()
  {
    if(!interval.loop()) return;  //if(displayInt.loop()){
    
    songCounter = 0;
    int naald = convertToDisplayPos(car.position);
    int target = convertToDisplayPos(car.targetPos);
    int sensor = convertToDisplayPos(car.sensorPos);
    int sensorMaxBerijk = convertToDisplayPos(PLAAT_BEGIN - SENSOR_OFFSET)  +  3;
    int volumeMargin = 16;

    if(staat == S_SCHOONMAAK)
    {
      float verdeelPuntTeller = 0;

      for(int i = 0; i < D_LENGTH; i++)
      {
        data[i] = 0.0f;

        int volumePunt = mapF(arm.weight, 0, 4, (D_LENGTH - 1) - volumeMargin,   volumeMargin);

        if(i < (D_LENGTH - 1) - volumeMargin && i > volumePunt)
        {
          data[i] = 0.1f;
        }

        int verdeelPunt = mapF(verdeelPuntTeller, 0, 4, volumeMargin, (D_LENGTH - 1) - volumeMargin);
        if(i > verdeelPunt)
        {
          verdeelPuntTeller += 0.5;          
        }

        if(i == verdeelPunt){
          data[i] = 0.0f;
        }

        if(i == volumeMargin   ||   i == (D_LENGTH - 1) - volumeMargin){ //eind van de schaal punt
          data[i] = 0.1f;
        }

        if(i == volumePunt){
          data[i] = 0.9f;
        }

      }
    }
    else if(specialeDisplayActie.sinds() < 2000)
    {
      for(int i = 0; i < D_LENGTH; i++)
      {
        data[i] = 0.0f;          
        int volumePunt = mapF(amp.volume, 0, 63, (D_LENGTH - 1) - volumeMargin,   volumeMargin);
        if(i < (D_LENGTH - 1) - volumeMargin    &&    i > volumePunt)
        {
          data[i] = 0.1f;
        }

        if(i == (D_LENGTH - 1) - 2 && rpmStaat == AUTO)
        {
          data[i] = 0.9f;              
        }

        if(i == (D_LENGTH - 1) - 7)
        {
          if(isOngeveer(targetRpm, rpm33, 0.1))
          {
            data[i] = 0.1f;
          }
          if(rpmStaat == R33)
          {
            data[i] = 0.9f;
          }        
        }

        if(i == (D_LENGTH - 1) - 12)
        {
          if(targetRpm == rpm45)
          {
            data[i] = 0.1f;
          }
          if(rpmStaat == R45)
          {
            data[i] = 0.9f;
          }        
        }    
      }
    
    }
      
    else
    {
      for(int i = 0; i < D_LENGTH; i++)
      {
        int volgendeNummerDisplay = convertToDisplayPos(nummers[songCounter]);
        if(staat == S_NAAR_BEGIN_PLAAT && i > sensor)
        {
          data[i] = 0.0f;
        }

        else if(i > sensorMaxBerijk){
          data[i] = 0.0f;
        }

        else if(volgendeNummerDisplay <= i && songCounter < hoeveelNummers)
        {
          songCounter++;
          data[i] = 0.0f;
        }
        
        else
        {
          if(songCounter == 0) data[i] = 0.0f;
          else data[i] = 0.1f;
        }
        
        if(staat == S_NAAR_NUMMER || staat == S_DOOR_SPOELEN ||
          staat == S_TERUG_SPOELEN || staat == S_PAUZE)
        {
          if(naald-1 == i || naald+1 == i){
            // displayData[i] = 0;
            if(millis()%1000 > 300){
              data[i] = 0.9f;
            }
          }
          
          else if( i == target  &&  naald != i){
            data[i] = 0.9f;
          }

          // if(naald == i){
          //   displayData[i] = 0.0f;
          //   if(millis()%400 > 100){
          //     displayData[i] = 0.9f;
          //   }
          // }

        }
        else if(naald == i){
          data[i] = 0.9f;
        }
      }
    }

    d_print(0);
    nDelay = micros();
    commitDisplay();

    d_print(0.5);
    while(micros() - nDelay < 75){}
    commitDisplay();

    d_print(1);
    while(micros() - nDelay < 400){}
    commitDisplay();
  }
  
};