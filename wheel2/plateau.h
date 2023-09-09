



// float targetRpm = 0;//staat nu in compVaartSensor.h

float basis = 0;
float uitBuff;


float plateauP = 0.1;    //pid
float plateauI = 0.05;
float plateauD = 0;

int call;




void plateauInit(){
  setPwm(motorP);
  pwmWrite(motorP, 0);

  setPwm(motorN);
  pwmWrite(motorN, 0);
}




void plateauDraaien(){
  targetRpm = 33.333;
}

void plateauStoppen(){
  targetRpm = 0;
}




float maakP(){
  return plateauP;
}   



float pid(float rpmIn){
  double uit = (targetRpm - rpmIn) * maakP();
  
  uit = limieteerF(uit, -1, 1);
  
  basis += uit * plateauI;            //breng basis voltage naar gemiddelde voor de juiste snelheid
  
  basis = limieteerF(basis, -1, 1);
  
  return  limieteerF(uit + basis,  -1,  1);
}







Interval plateauInt(10000, MICROS);

void plateauFunc(){

  if(plateauInt.loop()){

    uitBuff = pid(TLE5012.getVaart());//                  bereken motor kracht
    // uitBuff = pid(strobo.getVaart());//                  bereken motor kracht

    if(targetRpm != 0){             //staat de motor aan?
    
      pwmFase(uitBuff, motorP, motorN, false);
      
    }else{
      
      pwmWriteF(motorP,  1);
      pwmWriteF(motorN,  1);
      basis = 0; // reset I
    }

  }
}








