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


int volt2pwm(float volt){
  return (volt * PMAX) / 3.3;
}