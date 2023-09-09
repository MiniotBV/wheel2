bool plateauLogica = true;




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


float basis = 0;
float uitBuff;






enum rpmStaats{
  AUTO,
  R33,
  R45,
  R78
};


enum rpmStaats rpmStaat = AUTO;




float autoRpm;

void updatePlateauRpm(){
  if(!plateauAan){
    return;    
  }

  if(rpmStaat == AUTO){
    targetRpm = autoRpm;
    return;
  }
  if(rpmStaat == R33){
    targetRpm = rpm33;
    return;
  }
  if(rpmStaat == R45){
    targetRpm = rpm45;
    return;
  }
}





void setPlateauRpm(float rpm){
  if(rpm == targetRpm){return;}//als er nisk veranderd is, hoeft er niks gerestet te worden
  autoRpm = rpm;

  updatePlateauRpm();
  
  Serial.println("setPlateauRpm() reset");
  strobo.clearCompSamples();
  draaienInterval.reset();
  // opsnelheid = false;
}






void plateauDraaien(){
  plateauAan = true;
  setPlateauRpm(rpm33);

  basis = 0.75;
  
  // strobo.clearCompSamples();
  // draaienInterval.reset();
  
  Serial.println("plateauStart()");
}






void plateauStoppen(){
  plateauAan = false;
  targetRpm = 0;
  draaienInterval.reset();
  uitdraaien = true;
  opsnelheid = false;
}








void plateauInit(){
  setPwm(motorP);
  setPwm(motorN);
}
















float pid(float rpmIn){
  // updatePIDwaarde();  

  double uit = (targetRpm - rpmIn) * plateauP;
  uit = limieteerF(uit, -1, 1);
  basis += uit * plateauI;            //breng basis voltage naar gemiddelde voor de juiste snelheid
  basis = limieteerF(basis, 0, 1);

  basis = limieteerF(basis, 0, 0.6 + (rpmIn / 30) );
  
  return  uit + basis;
}










void plateauStaatDingen(){

  if(!plateauLogica){return;}  
  
  float glad = strobo.glad;//gladglad;



  if(plateauAan){                   //staat de motor aan?

    if(staat == S_FOUTE_ORIENTATIE  ||
    staat == S_ERROR  ||
    staat == S_HOK
    ){
      Serial.println("plateau was perongeluk nog aant draaien");
      plateauStoppen();
    }
    

    if( opsnelheid == true){ //         tegen gehouden
      
      if(glad > targetRpm * 4 ){ //te snel 200%
        Serial.println("^");
        stoppen();
      }
      else if(glad  <  targetRpm * 0.65   &&   draaienInterval.sinds() > 1500){ //te langzaam 70%
        Serial.println("T");
        stoppen();
      }
    }

    
    
    if(  opsnelheid == false    ){ //                                                 tegen gehouden
      
      if(glad  >  targetRpm * 0.95){ //                       op snelheid (95% van de snelheid)
        Serial.println("O");
        opsnelheid = true;
      }

      if(glad  <  targetRpm * 0.1   &&     draaienInterval.sinds() > 1000){//   <5% target snelheid na een kort tijd
        
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
    // vaart += (1/plateauP) * (strobo.plateauComp/100);
    // }
    

    if(plateauAan){             //staat de motor aan?


      uitBuff = pid(vaart);//                  bereken motor kracht
      uitBuff += strobo.plateauComp / 100;

      uitBuff = limieteerF(uitBuff, -1, 1);
      
      pwmFase(uitBuff, motorP, motorN, false);
      
    }else{
      
      pwmFase(0, motorP, motorN, false);
      basis = 0; // reset I
    }


 
    plateauStaatDingen();

  }
}








