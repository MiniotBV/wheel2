

bool plateauLogica = false;

// float targetRpm = 0;//staat nu in compVaartSensor.h

float basis = 0;
float uitBuff;




float plateauP = 0.1;    //pid
float plateauI = 0.005;
float plateauD = 0;

float plateau33P = 0.1;    //pid
float plateau33I = 0.005;

float plateau45P = 0.02;    //pid
float plateau45I = 0.005;

float plateauRustP = 0.01;
float plateauRustI = 0.005;



void plateauInit(){
  setPwm(motorP);
  setPwm(motorN);
}














void updatePIDwaarde(){
}   



float pid(float rpmIn){
  // updatePIDwaarde();  

  double uit = (targetRpm - rpmIn) * plateauP;
  
  uit = limieteerF(uit, -1, 1);
  
  basis += uit * plateauI;            //breng basis voltage naar gemiddelde voor de juiste snelheid
  
  basis = limieteerF(basis, -1, 1);
  
  return  limieteerF(uit + basis,  -1,  1);
}







Interval plateauInt(10000, MICROS);

void plateauFunc(){

  if(plateauInt.loop()){

    strobo.update();
    // float vaart = strobo.getVaart();
    // float vaart = strobo.vaart;
    float vaart = strobo.glad;
    // vaart += strobo.plateauComp;

    if(plateauAan){             //staat de motor aan?


      uitBuff = pid(vaart);//                  bereken motor kracht
      
      pwmFase(uitBuff, motorP, motorN, false);
      
    }else{
      
      pwmFase(0, motorP, motorN, false);
      basis = 0; // reset I
    }














    // float glad = strobo.getGlad();
    float glad = strobo.gladglad;

    if(plateauLogica){
      if(plateauAan){                   //staat de motor aan?



        if( opsnelheid == true){ //         tegen gehouden
          
          if(glad > targetRpm * 2 ){ //te snel 200%
            // Serial.println("^");
            // if(test){
            //   plateauStoppen();
            // }
            // stoppen();
          }
          else if(glad  <  targetRpm * 0.80   &&   draaienInterval.sinds() > 500){ //te langzaam 70%
            Serial.println("T");//----------------
            // if(test){
            //   plateauStoppen();
            // }
            stoppen();//--------------------
          }
        }

        
        
        if(  opsnelheid == false    ){ //                                                 tegen gehouden
          
          if(glad  <  targetRpm * 0.01   &&     draaienInterval.sinds() > 1000){//   <5% target snelheid na een kort tijd
            
            Serial.println("kon niet opgang komen");
            stoppen();//--------------------------------------------
          }
        }



        if(glad  >  targetRpm * 0.95   &&   opsnelheid == false ){ //                       op snelheid (95% van de snelheid)
          Serial.println("O");
          opsnelheid = true;
        }
      
      }




      if( !plateauAan   &&   draaienInterval.sinds() > 1000   &&   uitdraaien == false){//                              aangeslingerd
        if(vaart  >  rpm33 * 0.50){                                                       //50% van de 33.3 snelheid
          Serial.println("A");
          if(staat == S_HOK){
            spelen();
          }
        }
      }


      
      if(uitdraaien == true && glad < rpm33 * 0.05){ //     <5% van de 33.3 snelheid                                                    uitgedraaid
        uitdraaien = false;
        draaienInterval.reset();
        Serial.println("U");
      }
    }

  }
}








