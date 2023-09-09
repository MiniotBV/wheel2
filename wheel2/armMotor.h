#define MIN_ARMGEWICHT 0.5//gr
#define MAX_ARMGEWICHT 4//gr

#define HOK_ARMGEWICHT -10


float armTargetGewicht = 2.3;
float armGewicht = HOK_ARMGEWICHT;
float armKracht = 0;

float armSnelheidOp = 0.004;
float armSnelheidAf = 0.01;





// float armKracht500mg = 0.33;  //7de proto
// float armKracht4000mg = 0.66;

float armKracht500mg = 0.28;  //7de proto
float armKracht4000mg = 0.58;

//op de plaat
float netUitHokGewicht = -0.8;
float netOpDePlaatGewicht = 0.5;
//van de plaat af
float netVanDePlaatGewicht = 0;
float netInHokGewicht = -1;


bool armMotorAan = false;




void armInit(){
  setPwm(armMotor);
}




float armGewicht2pwm(float gewicht){
  float pwm = mapF(gewicht,   MIN_ARMGEWICHT,   MAX_ARMGEWICHT,    armKracht500mg, armKracht4000mg);
  return limieteerF(pwm, 0, 1);  
}

float pwm2armGewicht(float pwm){
  float gewicht = mapF(pwm,  armKracht500mg, armKracht4000mg,  MIN_ARMGEWICHT,   MAX_ARMGEWICHT);
  return gewicht;
}






Interval armInt(10, MILLIS);
Interval naaldEropInterval(0, MILLIS);

void armFunc(){
  if(armInt.loop()){

    if(staat == S_CALIBREER){
      armGewicht = pwm2armGewicht(armKracht);
      pwmWriteF(armMotor, armKracht);
      return;
    }


    

    if(armMotorAan == true){//moet de arm motor aan?

      if(armGewicht < netUitHokGewicht){//als de arm net aan staat jump meteen naar nognetInHokGewicht
        armGewicht = netUitHokGewicht;
      }

      if(armGewicht < armTargetGewicht){//is de arm al op het target gewicht?
        armGewicht += armSnelheidOp;
      }

      if(armGewicht > netOpDePlaatGewicht){//is de arm al op de plaat?
        armGewicht = armTargetGewicht;//zet dan de arm meteen op target gewicht
      }
    }
    

    if(armMotorAan == false){// moet de arm motor uit?
      
      if(armGewicht > netVanDePlaatGewicht){ //als de arm net is uitgezet
        armGewicht = netVanDePlaatGewicht; // zet haal dan meteen het meeste gewicht van de arm
      }

      if(armGewicht > HOK_ARMGEWICHT){ //is de arm nog niet helemaal uit
        armGewicht -= armSnelheidAf; // zet hem dan wat minder hard
      }
      
      if(armGewicht < netInHokGewicht){ //is de arm al van de plaat?
        armGewicht = HOK_ARMGEWICHT; // zet de arm dan meteen uit
      }
    }



    armKracht = armGewicht2pwm(armGewicht);

    pwmWriteF(armMotor, armKracht);





    if(armGewicht != armTargetGewicht){
      naaldEropInterval.reset();
    }
  }

}




bool isNaaldErop(){
  // return armGewicht == armTargetGewicht  &&  naaldEropInterval.sinds() > 250;
  return armGewicht == armTargetGewicht;
}

bool isNaaldEraf(){
  return armGewicht == HOK_ARMGEWICHT;
}

int isNaaldEropSinds(){
  return naaldEropInterval.sinds();
}

bool isNaaldEropVoorZoLang(int ms){
  return isNaaldErop() && naaldEropInterval.sinds() > ms;
}



bool naaldErop(){
  armMotorAan = true;
  return isNaaldErop();
}

bool naaldEraf(){
  armMotorAan = false;
  return isNaaldEraf();
}



bool naaldNoodStop(){
  armGewicht = HOK_ARMGEWICHT; // zet de arm dan meteen uit
  armMotorAan = false;
  return true;  
}





