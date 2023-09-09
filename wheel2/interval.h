#define MILLIS 0
#define MICROS 1



class Interval{
  public:
    unsigned int interval = 0;
    unsigned long vorrigeTijd = 0;
    int eenheid = MILLIS;

    Interval(int i, int tijdMode){
      interval = i;
      eenheid = tijdMode;
      vorrigeTijd = tijd();
    }



    bool loop(){
      if(tijd() - vorrigeTijd >= interval){
          vorrigeTijd += interval;

          if(tijd() - vorrigeTijd >= interval){
            vorrigeTijd = tijd();            
          }

          return true;
      }
      return false;
    }


    void offset(int ofst){
      vorrigeTijd += ofst;
    }



    unsigned long tijd(){
      if(eenheid == MILLIS){
        return millis();
      }
      if(eenheid == MICROS){
        return micros();
      }

      return 0;
    }

};