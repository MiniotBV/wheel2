#define MILLIS 0
#define MICROS 1



class Interval{
	public:
		unsigned long interval = 0;
		unsigned long vorrigeTijd = 0;
		unsigned long vorrigeVorrigeTijd = 0;
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


		void offset(int ofst){
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
		

		unsigned long  sinds(){
			return tijd() - vorrigeTijd;
		}

		bool langerDan(){
			return tijd() - vorrigeTijd > interval;
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