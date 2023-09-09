



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

  
  pwmWriteF(stapperAP, sin(pos +   ( PI * 2.0 * (1.0/3.0) ) * 0  )  );
  pwmWriteF(stapperBP, sin(pos +   ( PI * 2.0 * (1.0/3.0) ) * 1  )  );
  pwmWriteF(plateauA , sin(pos +   ( PI * 2.0 * (1.0/3.0) ) * 2  )  );

  // pwmWriteF(stapperAN, 0  );
  // pwmWriteF(stapperBN, 0  );
  // pwmWriteF(plateauB , 0  );

  pwmWriteF(stapperAN, 1  );
  pwmWriteF(stapperBN, 1  );
  pwmWriteF(plateauB , 1  );

  // pwmFase( sin(pos)*kk,                      stapperAP, stapperAN, true);
  // // pwmFase( sin(pos + faseVerschuiving),  stapperBP, stapperBN, true);
  // pwmFase( cos(pos)*kk,                      stapperBP, stapperBN, true);


  // pwmStapper(pos,   stapperAP, stapperAN,  stapperBP, stapperBN,  true);



}













