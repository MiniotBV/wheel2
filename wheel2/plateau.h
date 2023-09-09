unsigned long motorLoop;



float basis = 0;
float uitBuff;


float plateauP = 500;    //pid
float plateauI = 0.05;
float plateauD = 0;

int call;








float maakP(){
  return plateauP;
}   



float pid(float rpmIn){
  double uit = (targetRpm - rpmIn) * maakP();
  
  uit = limieteerF(uit, -4000, 4000);
  
  basis += uit * plateauI;            //breng basis voltage naar gemiddelde voor de juiste snelheid
  
  basis = limieteerF(basis, -4000, 4000);
  
  return  limieteerF(uit + basis,  -(PMAX-1),  PMAX-1);
}









void plateauFunc(){

  if(micros() - motorLoop > 10000){
    motorLoop = micros();

    uitBuff = pid(TLE5012.getVaart());//                  bereken motor kracht
    // uitBuff = pid(strobo.getVaart());//                  bereken motor kracht


    if(targetRpm != 0){             //staat de motor aan?
      if(uitBuff > 0){
        pwmWrite(motorP,   uint16_t(uitBuff) );
        pwmWrite(motorN,  0);
      }else{
        pwmWrite(motorP,  0);
        pwmWrite(motorN,  0);
        // pwmWrite(motorN,  uint16_t(-uitBuff));        
      }
        
      
    }else{
      pwmWrite(motorP,   0 );
      digitalWrite(motorN,  0);
      basis = 0; // reset I
    }

  }
}








