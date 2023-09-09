#include "hardware/timer.h"
#define MILLIS 0
#define MICROS 1



class Interval{
	public:
		uint32_t interval = 0;
		uint32_t vorrigeTijd = 0;
		uint32_t vorrigeVorrigeTijd = 0;
		int eenheid = MILLIS;
    bool eenKeerLatch = true;

		Interval(int i, int tijdMode){
			interval = i;
			eenheid = tijdMode;
			vorrigeTijd = tijd();
		}



		bool loop(){
			if(tijd() - vorrigeTijd >= interval){
					vorrigeVorrigeTijd = vorrigeTijd;
					vorrigeTijd += interval;

					if(tijd() - vorrigeTijd >= interval){
						vorrigeVorrigeTijd = vorrigeTijd;
						vorrigeTijd = tijd();            
					}

					return true;
			}
			return false;
		}


		void offset(uint32_t ofst){
			vorrigeTijd += ofst;
		}


		void reset(){
			vorrigeTijd = tijd(); 
      eenKeerLatch = true;
		}


    bool eenKeer(){
      if(eenKeerLatch   &&   tijd() - vorrigeTijd  > interval){
        eenKeerLatch = false;
        return true;
      }
      return false;
    }
		

		uint32_t  sinds(){
			return tijd() - vorrigeTijd;
		}

		bool langerDan(){
			return tijd() - vorrigeTijd > interval;
		}


		uint32_t tijd(){
			if(eenheid == MILLIS){
				return millis();
			}
			if(eenheid == MICROS){
				return micros(); //
        // time_us_32()
        // time_us_64();
        
			}

			return 0;
		}

};