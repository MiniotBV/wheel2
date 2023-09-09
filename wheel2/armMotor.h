
class ArmMotor
{
  public:
  #define MIN_GEWICHT 0.5  // in gram
  #define MAX_GEWICHT 4    // in gram
  #define HOK_GEWICHT -10

  float targetGewicht = 2.1;
  float gewicht = HOK_GEWICHT;
  float kracht = 0;

  // float snelheidOp = 0.004;
  // float snelheidAf = 0.01;

  float snelheidOp = 1000;//ms;
  float snelheidAf = 500;//ms;

  // float krachtHok = 0.04;
  float krachtLaag = 0.13;//0.08;
  float krachtHoog = 0.30;//0.22;


  float netOpDePlaatGewicht = 0.5;//0.25;
  float netInHokGewicht = -1.5;


  bool armMotorAan = false;

  Interval armInt {10, MILLIS};
  Interval naaldEropInterval {0, MILLIS};
  Interval aanInterval {0, MILLIS};
  Interval uitInterval {0, MILLIS};


  void armInit() { setPwm(armMotor); }

  float armGewicht2pwm(float gewicht){
    float pwm = mapF(gewicht,   MIN_GEWICHT,   MAX_GEWICHT,    krachtLaag, krachtHoog);
    return limieteerF(pwm, 0, 1);  
  }

  float pwm2armGewicht(float pwm){
    float gewicht = mapF(pwm,  krachtLaag, krachtHoog,  MIN_GEWICHT,   MAX_GEWICHT);
    return gewicht;
  }

  bool isNaaldErop(){
    return gewicht == targetGewicht;
  }

  bool isNaaldEraf(){
    return gewicht == HOK_GEWICHT;
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
    gewicht = HOK_GEWICHT; // zet de arm dan meteen uit
    armMotorAan = false;
    return true;  
  }

  void armFunc()
  {
    if(armInt.loop()){

      if(staat == S_CALIBREER){
        gewicht = pwm2armGewicht(kracht);
        pwmWriteF(armMotor, kracht);
        return;
      }

      if(staat == S_HOMEN_VOOR_SPELEN  ||  staat == S_HOMEN_VOOR_SCHOONMAAK  ||staat == S_NAAR_HOK){
        if(armMotorAan == true){
          debug("NAALD HAD NOOIT AAN MOGEN STAAN!!!");
          naaldNoodStop();
        }
      }

      

      if(armMotorAan == true){//moet de arm motor aan?

        uitInterval.reset();
        
        if(gewicht > netOpDePlaatGewicht - 0.01){//is de arm al op de plaat? (-0.01gr omdat hij dan niet glitcht)
          gewicht = targetGewicht;//zet dan de arm meteen op target gewicht
        }else{
          gewicht = mapF(aanInterval.sinds(), 0, snelheidOp, netInHokGewicht, netOpDePlaatGewicht);
          // gewicht = mapF(aanInterval.sinds(), 0, snelheidOp, pwm2armGewicht(0), netOpDePlaatGewicht);
        }
      
      }else{// moet de arm motor uit?

        aanInterval.reset();

        if(gewicht < netInHokGewicht){ //is de arm al van de plaat?
          gewicht = HOK_GEWICHT; // zet de arm dan meteen uit
        }else{
          gewicht = mapF(uitInterval.sinds(), 0, snelheidAf, netOpDePlaatGewicht, netInHokGewicht);
        }
      }



      kracht = armGewicht2pwm(gewicht);

      pwmWriteF(armMotor, kracht);





      if(gewicht != targetGewicht){
        naaldEropInterval.reset();
      }
    }

  }
};



