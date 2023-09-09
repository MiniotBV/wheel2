
float limieteerF(float n, float min, float max){
  if(n < min){
    return min;
  }

  if(n > max){
    return max;
  }

  return n;
}


