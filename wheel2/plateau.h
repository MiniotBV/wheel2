
bool plateauLogica = true;
bool plateauComp = true;

// float plateauP = 0.005;//plateau33P;    //pid
// float plateauI = 0.03;//plateau33I;
// float plateauD = 0;

// float plateauP = 1;//plateau33P;    //pid
// float plateauI = 0.1;//plateau33I;
// float plateauD = -3;

float plateauP = 1;//0.5;//plateau33P;    //pid
float plateauI = 0.02;//plateau33I;
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

  basis = 20;//40;//60;//75;
  
  // strobo.clearCompSamples();
  // draaienInterval.reset();
  
  // Serial.println("plateauStart()");
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













float vorrigeVaart = 0;
float pp, pd;


float pid(float rpmIn){

  float divTijd = rpmIn - vorrigeVaart;
  vorrigeVaart = rpmIn;


  float divTarget;
  if(strobo.plaatUitMiddenComp){
    divTarget = centerCompTargetRpm - rpmIn;
  }else{
    divTarget = targetRpm - rpmIn;
  }



  if(plateauComp){

    pp = divTarget * plateauP;

    if(abs(divTarget) < 3){
      basis += divTarget * plateauI;            //breng basis voltage naar gemiddelde voor de juiste snelheid
    }else{
      basis += divTarget * plateauI * 0.25;       //om langzaam opte starten
    }
    


    pd = divTijd * plateauD;
  }
  

  return  pp + basis + pd;
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
      return;
    }
    

    if( opsnelheid == true){ //         tegen gehouden
      
      if(glad > targetRpm * 4 ){ //te snel 200%
        Serial.println("^");
        stoppen();
        return;
      }
      else if(glad  <  targetRpm * 0.65   &&   draaienInterval.sinds() > 1000){ //te langzaam 70%
        Serial.println("T");
        stoppen();
        return;
      }
    }

    
    
    if(  opsnelheid == false    ){ //                                                 tegen gehouden
      
      if(glad  >  targetRpm * 0.95){ //                       op snelheid (95% van de snelheid)
        Serial.println("O");
        opsnelheid = true;
        return;
      }

      if(glad  <  targetRpm * 0.1   &&     draaienInterval.sinds() > 1000){//   <5% target snelheid na een kort tijd
        
        Serial.println("kon niet opgang komen");
        stoppen();//--------------------------------------------
        return;
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
      return;
    }
  }
  
}











Interval plateauInt(5000, MICROS);

void plateauFunc(){

  if(plateauInt.loop()){

    strobo.update();
    float vaart = strobo.glad;

    if(plateauAan){             //staat de motor aan?


      uitBuff = pid(vaart);//                  bereken motor kracht
      uitBuff += strobo.plateauComp;

      uitBuff = limieteerF(uitBuff, -100, 100);
      
      pwmFase(uitBuff / 100.0, motorN, motorP, false);
      
    }else{
      
      pwmFase(0, motorP, motorN, false);
      basis = 0; // reset I
    }


 
    plateauStaatDingen();

  }
}








