#define AMAX 4095


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









int volt2pwm(float volt){
  return (volt * PMAX) / 3.3;
}