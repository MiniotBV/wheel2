#define MIN_ARMGEWICHT 0.5//gr
#define MAX_ARMGEWICHT 4//gr

float armGewicht = 3;
float armTargetKracht;

float armKracht = 0;
float armSnelheidOp = 0.00025;
float armSnelheidAf = 0.001;



// float armKracht500mg = 0.22; //derde proto
// float armKracht4000mg = 0.56;
// //op de plaat
// float netUitHokKracht = 0.15;
// float netOpDePlaatKracht = 0.2;
// //van de plaat af
// float netVanDePlaatKracht = 0.2;
// float netInHokKracht = 0.15;

// float armKracht500mg = 0.33;  //vierde proto
// float armKracht4000mg = 0.64;
// //op de plaat
// float netUitHokKracht = 0.25;
// float netOpDePlaatKracht = 0.3;
// //van de plaat af
// float netVanDePlaatKracht = 0.3;
// float netInHokKracht = 0.20;

float armKracht500mg = 0.33;  //vierde proto
float armKracht4000mg = 0.68;
//op de plaat
float netUitHokKracht = 0.25;
float netOpDePlaatKracht = 0.35;
//van de plaat af
float netVanDePlaatKracht = 0.30;
float netInHokKracht = 0.25;




bool armMotorAan = false;


void armGewichtUpdate(){
  armGewicht = limieteerF(armGewicht,  MIN_ARMGEWICHT,   MAX_ARMGEWICHT);  
  armTargetKracht = mapF(armGewicht,   MIN_ARMGEWICHT,   MAX_ARMGEWICHT,    armKracht500mg, armKracht4000mg);
  Serial.print("armTargetKracht: ");
  Serial.println(armTargetKracht);
  
}

void armInit(){
  setPwm(armMotor);
  armGewichtUpdate();
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

    pwmWriteF(armMotor, armKracht);
  }

}


bool isNaaldErop(){
  return armKracht == armTargetKracht;
}

bool naaldErop(){
  armMotorAan = true;
  return isNaaldErop();
}




bool isNaaldEraf(){
  return armKracht == 0;
}

bool naaldEraf(){
  armMotorAan = false;
  return isNaaldEraf();
}





