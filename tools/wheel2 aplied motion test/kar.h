



void karInit(){
  setPwm(stapperAP);
  setPwm(stapperAN);
  setPwm(stapperBP);
  setPwm(stapperBN);
}








float faseVerschuiving = PI/2;
float pos = 0;
float snelheid = 0;
float targetSnelheid = .2;
float accel = 0.1;
float kk = 1;//0.2;

#define conversie 1000000.0 / (PI*2)


Interval karMotorInt(100, MICROS);
Interval posInt(1, MICROS);



void karMotorFunc(){

      
  if(!karMotorInt.loop()){return;}
  //   if ( targetSnelheid != 0){
  //     targetSnelheid = 0;
  //   }else{
  //     targetSnelheid = 40;
  //   }
  // }
    
  snelheid += limieteerF(targetSnelheid - snelheid, -accel, accel);

  pos += (posInt.sinds() * snelheid * (PI*2) ) / 1000000.0;
  posInt.reset();

  // pos = pos > PI*2 ? pos - PI*2 : pos;

  

  pwmFase( sin(pos)*kk,                      stapperAP, stapperAN, true);
  // pwmFase( sin(pos + faseVerschuiving),  stapperBP, stapperBN, true);
  pwmFase( cos(pos)*kk,                      stapperBP, stapperBN, true);


  // pwmStapper(pos,   stapperAP, stapperAN,  stapperBP, stapperBN,  true);



}













