#define MILLIS 0
#define MICROS 1

class Interval
{
	public:
		unsigned int interval = 0;
		unsigned long vorrigeTijd = 0;
		unsigned long vorrigeVorrigeTijd = 0;
		int eenheid = MILLIS;

		Interval(int i, int tijdMode)
		{
			interval = i;
			eenheid = tijdMode;
			vorrigeTijd = tijd();
		}


		bool loop()
		{
			if(tijd() - vorrigeTijd < interval) return false;	// too soon

			vorrigeVorrigeTijd = vorrigeTijd;
			vorrigeTijd += interval;

			if(tijd() - vorrigeTijd >= interval)
			{
				vorrigeVorrigeTijd = vorrigeTijd;
				vorrigeTijd = tijd();
			}

			return true;
		}


		void offset(int ofst)	{ vorrigeTijd += ofst; }
		void reset() 			{ vorrigeTijd = tijd(); }
		int  sinds() 			{ return tijd() - vorrigeTijd; }
		bool langerDan()		{ return tijd() - vorrigeTijd > interval; }

		unsigned long tijd()
		{
			if(eenheid == MILLIS) return millis();
			if(eenheid == MICROS) return micros();
			return 0;
		}

};