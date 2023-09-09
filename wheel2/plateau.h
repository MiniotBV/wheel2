



// float targetRpm = 0;//staat nu in compVaartSensor.h

float basis = 0;
float uitBuff;


float plateauP = 0.1;    //pid
float plateauI = 0.05;
float plateauD = 0;




void plateauInit(){
  setPwm(motorP);
  setPwm(motorN);
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


    float vaart = TLE5012.getVaart();

    uitBuff = pid(vaart);//                  bereken motor kracht
    // uitBuff = pid(strobo.getVaart());//                  bereken motor kracht

    if(targetRpm != 0){             //staat de motor aan?
    
      pwmFase(uitBuff, motorP, motorN, false);
      
    }else{
      
      pwmFase(0, motorP, motorN, false);
      basis = 0; // reset I
    }














    float glad = TLE5012.getGlad();
    float gladDiv = TLE5012.gladDiv;

    if(targetRpm != 0){             //staat de motor aan?
    
      if(glad > 200){ //    4 * rpm33 om te snel te gaan                       snaar dr af / tesnel
        Serial.println("te snel");
        if(test){
          plateauStoppen();
        }
      }



      if( opsnelheid == true    ||    draaienInterval.sinds() > 3000){ //         tegen gehouden
        
        if(glad > targetRpm * 2 ){ //te snel 200%
          Serial.println("te snel");
          if(test){
            plateauStoppen();
          }
          stoppen();
        }
        else if(glad  <  targetRpm * 0.70){ //te langzaam 70%
          Serial.println("tegen gehouden");
          if(test){
            plateauStoppen();
          }
          stoppen();
        }
      }

      
      
      if(  opsnelheid == false    ){ //                                                 tegen gehouden
        
        // if(gladDiv < -0.2     &&     draaienInterval.sinds() > 200){ //   gladDiv < -0.05             decelereren
          
        //   Serial.println("gedecelereed");
        //   // stoppen();
        // }
        // else 
        if(glad  <  targetRpm * 0.01   &&     draaienInterval.sinds() > 1000){//   <5% target snelheid na een kort tijd
          
          Serial.println("kon niet opgang komen");
          stoppen();
        }
      }



      if( targetRpm > 1   &&   glad  >  targetRpm * 0.95   &&   opsnelheid == false ){ //                       op snelheid (95% van de snelheid)
        Serial.println("opsnelheid");
        opsnelheid = true;
      }
    
    }




    if(targetRpm == 0 && draaienInterval.sinds() > 1000 && uitdraaien == false){//                              aangeslingerd
      if(vaart  >  rpm33 * 0.50){                                                       //50% van de 33.3 snelheid
        Serial.println("aangeslingerd");
        if(test){
          plateauDraaien();
        }
        if(staat == S_HOK){
          spelen();
        }
      }
    }


    
    if(uitdraaien == true && glad < rpm33 * 0.05){ //     <5% van de 33.3 snelheid                                                    uitgedraaid
      uitdraaien = false;
      draaienInterval.reset();
      Serial.println("uitgedraaid");
    }

  }
}








