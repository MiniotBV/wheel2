
#define rpm33 33.333
#define rpm45 45
#define rpm78 78


#define AMAX 4095






#define NUMMER_TERUG_OPFSET 2 //hoeveel mm kan de kar bewegen voor er terug gespoeld kan worden naar het begin van het nummer ipv naar een vorrig nummer

#define ELPEE_PLAAT_BEGIN 147
#define TIEN_INCH_PLAAT_BEGIN 125
#define SINGLETJE_PLAAT_BEGIN 85

// #define PLAAT_EINDE 52.5
#define PLAAT_EINDE 52.5

#define KAR_HOME 44//44.5
#define KAR_HOK 45.5

#define SCHOONMAAK_PLEK 100

#define SENSOR_OFFSET 7.5//mm






bool plaatAanwezig = false;
float plaatAanwezigGefilterd = 0;












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














// void pauze();
bool isPlaatOngeveer7Inch();
void updatePlateauPID();
void plateauStoppen();
void plateauDraaien();



