int armTargetKracht = 500;
int armKracht = 0;
int armSnelheid = 1;
unsigned int armLoop = 0;


//op de plaat
int netUitHokKracht = 250;
int netOpDePlaatKracht = 400;


//van de plaat af
int netVanDePlaatKracht = 300;
int netInHokKracht = 100;


bool armMotorAan = false;



void armFunc(){
  if(millis() - armLoop > 10){
    armLoop = millis();

    if(armMotorAan == true){//moet de arm motor aan?

      if(armKracht < netUitHokKracht){//als de arm net aan staat jump meteen naar nogNetInHokKracht
        armKracht = netUitHokKracht;
      }

      if(armKracht < armTargetKracht){//is de arm al op het target gewicht?
        armKracht += armSnelheid;
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
        armKracht -= armSnelheid; // zet hem dan wat minder hard
      }
      
      if(armKracht < netInHokKracht){ //is de arm al van de plaat?
        armKracht = 0; // zet de arm dan meteen uit
      }
    }

    pwmWrite(armMotor, armKracht);
  }

}