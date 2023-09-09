#include "api/Common.h"


#define TAU 2*PI


#define rpm33 33.333
#define rpm45 45
#define rpm78 78


#define AMAX 4095






#define NUMMER_TERUG_OPFSET 1 //hoeveel mm kan de kar bewegen voor er terug gespoeld kan worden naar het begin van het nummer ipv naar een vorrig nummer

#define ELPEE_PLAAT_BEGIN 146.5//147
#define TIEN_INCH_PLAAT_BEGIN 124//125
#define SINGLETJE_PLAAT_BEGIN 84//85

#define PLAAT_EINDE 52.5
// #define PLAAT_EINDE 54

// #define KAR_HOME 44//44.5 //met ouwe sparing
// #define KAR_HOK 45.5


#define KAR_HOK 44
#define KAR_HOME KAR_HOK - 1.5//2.5//2//

#define SCHOONMAAK_PLEK 75//100

#define SENSOR_OFFSET 7.5//mm

float trackOffset = 0.3;//0.7;



bool herhaalDeHelePlaat = false;
bool puristenMode = false;



float nummers[100];// = {0.2, 0.3, 0.6, 0.68, 0.85}; //staat nu in staat.h
int hoeveelNummers = 0;
float plaatBegin = 0;




bool plaatAanwezig = false;
float plaatAanwezigGefilterd = 0;

float targetNummerPos = 0;

float nieuwePos;
float karPos = KAR_HOK;
float egteKarPos = karPos;
float karPosFilter = karPos;
float karPosFilterSlow = karPos;
float karPosMinimaal = karPos;

float karPosMidden;
float spoorafstand;




float targetRpm = 0;
float centerCompTargetRpm;
bool plateauAan = false;

bool opsnelheid;
bool uitdraaien;











String voegMargeToe(String waarde, int lengte){
  
  while( waarde.length() < lengte){
    waarde += " "; 
  }

  return waarde;
}










int rondTrip(int n, int max){
	if( n < 0 ){
		return max + (n % max);
	}

	return n % max;
}



float limieteerF(float n, float min, float max){
	if(n < min){
		return min;
	}

	if(n > max){
		return max;
	}

	return n;
}



int limieteerI(int n, int min, int max){
	if(n < min){
		return min;
	}

	if(n > max){
		return max;
	}

	return n;
}



float mapF(float n, float nMin, float nMax, float min, float max){
	return ( ( (n - nMin) / (nMax - nMin) )  *  (max - min) ) + min;
}




bool isOngeveer(float waarde, float vergelijk, float marge){
	return (waarde > vergelijk - marge) && (waarde < vergelijk + marge);
}


bool isTussen(float waarde, float min, float max){
	return   waarde > min   &&   waarde < max;
}





int sign(float v) {
	return (v < 0) ? -1 : ((v > 0) ? 1 : 0);
}










void plateauStoppen();
void plateauDraaien();
void zetNummersAlsEenSingletje();

void naarBeginPlaat(); 
void gaNaarNummer(float pos);
void naarVorrigNummer();
void naarVolgendNummer();

void plaatLeesNaKijken();

void debug(String bericht);


