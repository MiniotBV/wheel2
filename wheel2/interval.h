#define inMILLIS true   // #define MILLIS 0 #define MICROS 1
#define inMICROS false

class Interval
{
  public:
    Interval(int i, bool shouldBeInMillis = true) // int tijdMode)
    {
      interval = i;
      isInMillis = shouldBeInMillis;  //eenheid = tijdMode;
      lastTime = getTime();
    }

    unsigned int interval = 0;
    unsigned long lastTime = 0;
    bool isInMillis = true;   //int eenheid = MILLIS;

    bool loop()
    {
      if(getTime() - lastTime >= interval)
      {
          lastTime += interval;
          if(getTime() - lastTime >= interval) lastTime = getTime();
          return true;
      }
      return false;
    }


    void offset(int ofst) { lastTime += ofst; }
    void reset() { lastTime = getTime(); }
    int sinds() { return getTime() - lastTime; }
    bool langerDan() { return getTime() - lastTime > interval; }

    unsigned long getTime()
    {
      if(isInMillis) return millis();
      return micros();
    }

};