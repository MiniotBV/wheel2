int armTargetKracht = 1600;
int armKracht = 0;
int armSnelheidOp = 2;
int armSnelheidAf = 4;
unsigned int armLoop = 0;


//op de plaat
int netUitHokKracht = 800;
int netOpDePlaatKracht = 1500;


//van de plaat af
int netVanDePlaatKracht = 1000;
int netInHokKracht = 500;


bool armMotorAan = false;


void armInit(){
  setPwm(armMotor);
}




Interval armInt(10, MILLIS);

void armFunc(){
  if(armInt.loop()){

    if(armMotorAan == true){//moet de arm motor aan?

      if(armKracht < netUitHokKracht){//als de arm net aan staat jump meteen naar nogNetInHokKracht
        armKracht = netUitHokKracht;
      }

      if(armKracht < armTargetKracht){//is de arm al op het target gewicht?
        armKracht += armSnelheidOp;
      }

      if(armKracht > netOpDePlaatKracht){//is de arm al op de plaat?
        armKracht = armTargetKracht;//zet dan de arm meteen op target gewicht
      }
    }
    


    if(armMotorAan == false){// moet de arm motor uit?
      
      if(armKracht > netVanDePlaatKracht){ //als de arm net is uitgezet
        armKracht = netVanDePlaatKracht; // zet haal dan meteen het meeste gewicht van de arm
      }

      if(armKracht > 0){ //is de arm nog niet helemaal uit
        armKracht -= armSnelheidAf; // zet hem dan wat minder hard
      }
      
      if(armKracht < netInHokKracht){ //is de arm al van de plaat?
        armKracht = 0; // zet de arm dan meteen uit
      }
    }

    pwmWrite(armMotor, armKracht);
  }

}


bool isNaaldErop(){
  armMotorAan = true;
  return armKracht == armTargetKracht;
}

bool isNaaldEraf(){
  armMotorAan = false;
  return armKracht == 0;
}