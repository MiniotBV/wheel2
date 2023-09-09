

bool plateauLogica = true;

// float targetRpm = 0;//staat nu in compVaartSensor.h


float uitBuff;





float plateau33P = 0.1;    //pid
float plateau33I = 0.005;

float plateau45P = 0.02;    //pid
float plateau45I = 0.005;

float plateauRustP = 0.01;
float plateauRustI = 0.04;


float plateauP = 0.005;//plateau33P;    //pid
float plateauI = 0.03;//plateau33I;

// float plateauP = 0.01;//plateau33P;    //pid
// float plateauI = 0.02;//plateau33I;

// float plateauP = plateauRustP;    //pid
// float plateauI = plateauRustI;
float plateauD = 0;



void plateauInit(){
  setPwm(motorP);
  setPwm(motorN);
}





void updatePlateauPID(){
  // if(staat == S_NAALD_EROP){
  //   if(isPlaatOngeveer7Inch()){
  //     plateauP = plateau45P;
  //     plateauI = plateau45I;
  //     Serial.println("hallo 45");
  //     return;
  //   }

  //   plateauP = plateau33P;
  //   plateauI = plateau33I;
  //    Serial.println("hallo 33");
  //   return;
  // }

  // Serial.println("hallo Rust");
  // plateauP = plateauRustP;
  // plateauI = plateauRustI;
}














float pid(float rpmIn){
  // updatePIDwaarde();  

  double uit = (targetRpm - rpmIn) * plateauP;
  uit = limieteerF(uit, -1, 1);
  basis += uit * plateauI;            //breng basis voltage naar gemiddelde voor de juiste snelheid
  basis = limieteerF(basis, -1, 1);
  
  return  limieteerF(uit + basis,  -1,  1);
}










void plateauStaatDingen(){

  if(!plateauLogica){return;}  
  
  float glad = strobo.glad;//gladglad;

  if(plateauAan){                   //staat de motor aan?

    if( opsnelheid == true){ //         tegen gehouden
      
      if(glad > targetRpm * 4 ){ //te snel 200%
        Serial.println("^");
        stoppen();
      }
      else if(glad  <  targetRpm * 0.75   &&   draaienInterval.sinds() > 1000){ //te langzaam 70%
        Serial.println("T");
        stoppen();
      }
    }

    
    
    if(  opsnelheid == false    ){ //                                                 tegen gehouden
      
      if(glad  >  targetRpm * 0.9){ //                       op snelheid (95% van de snelheid)
        Serial.println("O");
        opsnelheid = true;
      }

      if(glad  <  targetRpm * 0.1   &&     draaienInterval.sinds() > 1500){//   <5% target snelheid na een kort tijd
        
        Serial.println("kon niet opgang komen");
        stoppen();//--------------------------------------------
      }
    }
  
  
  
  }else{
    if(draaienInterval.sinds() > 1000   &&   uitdraaien == false){//                              aangeslingerd
      if(glad  >  rpm33 * 0.50   &&   staat == S_HOK){                                                       //50% van de 33.3 snelheid
        Serial.println("A");
        spelen();
        return;
      }
    }

    if(uitdraaien == true && glad < rpm33 * 0.05){ //     <5% van de 33.3 snelheid                                                    uitgedraaid
      uitdraaien = false;
      draaienInterval.reset();
      Serial.println("U");
    }
  }
  
}











Interval plateauInt(10000, MICROS);

void plateauFunc(){

  if(plateauInt.loop()){

    strobo.update();
    // float vaart = strobo.getVaart();
    // float vaart = strobo.vaart;
    float vaart = strobo.glad;
    
    // if(strobo.compMeten){
    vaart += (1/plateauP) * (strobo.plateauComp/100);
    // }
    

    if(plateauAan){             //staat de motor aan?


      uitBuff = pid(vaart);//                  bereken motor kracht
      
      pwmFase(uitBuff, motorP, motorN, false);
      
    }else{
      
      pwmFase(0, motorP, motorN, false);
      basis = 0; // reset I
    }


 
    plateauStaatDingen();

  }
}








