#define rpm33 33.333
#define rpm45 45
#define rpm78 78


#define AMAX 4095




int rondTrip(int n, int max){
  if( n < 0 ){
    return max - (n % max);
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




int volt2pwm(float volt){
  return (volt * PMAX) / 3.3;
}