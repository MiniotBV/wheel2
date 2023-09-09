//  ====================================
//    Arm motor class
//  ====================================
class ArmMotor
{
  public:
  #define MIN_ARMGEWICHT 0.5  // in gram
  #define MAX_ARMGEWICHT 4    // in gram
  #define HOK_ARMGEWICHT -10

  float targetGewicht = 2.3;
  float armGewicht = HOK_ARMGEWICHT;
  float armKracht = 0;

  float snelheidOp = 0.004;
  float snelheidAf = 0.01;

  // float armKracht500mg = 0.33;  //7de proto
  // float armKracht4000mg = 0.66;

  float krachtMin = 0.28;   //  500 mg   7de proto
  float krachtMax = 0.58;  // 4000 mg

  //op de plaat
  float netUitHokGewicht = -0.8;
  float netOpDePlaatGewicht = 0.5;
  //van de plaat af
  float netVanDePlaatGewicht = 0;
  float netInHokGewicht = -1;


  bool isMotorAan = false;

  Interval armInt {10, MILLIS};
  Interval naaldEropInterval {0, MILLIS};

  void armInit() { setPwm(armMotor); }

  float gewicht2pwm(float w)
  {
    float pwm = mapF(w, MIN_ARMGEWICHT, MAX_ARMGEWICHT, krachtMin, krachtMax);
    return limitF(pwm, 0, 1);  
  }

  float pwm2gewicht(float pwm){
    float gewicht = mapF(pwm, krachtMin, krachtMax, MIN_ARMGEWICHT, MAX_ARMGEWICHT);
    return gewicht;
  }

  bool isNaaldErop(){
    // return armGewicht == armTargetGewicht  &&  naaldEropInterval.sinds() > 250;
    return armGewicht == targetGewicht;
  }

  bool isNaaldEraf() { return armGewicht == HOK_ARMGEWICHT; }

  int isNaaldEropSinds() { return naaldEropInterval.sinds(); }

  bool isNaaldEropVoorZoLang(int ms)
  {
    return isNaaldErop() && naaldEropInterval.sinds() > ms;
  }

  bool naaldErop() { isMotorAan = true; return isNaaldErop(); }
  bool naaldEraf() { isMotorAan = false; return isNaaldEraf(); }



  bool naaldNoodStop(){
    armGewicht = HOK_ARMGEWICHT; // zet de arm dan meteen uit
    isMotorAan = false;
    return true;  
  }

  void armFunc()
  {
    if(armInt.loop()){

      if(staat == S_CALIBREER){
        armGewicht = pwm2gewicht(armKracht);
        pwmWriteF(armMotor, armKracht);
        return;
      }

      if(staat == S_HOMEN_VOOR_SPELEN || staat == S_HOMEN_VOOR_SCHOONMAAK || staat == S_NAAR_HOK){
        if(isMotorAan)
        {
          Serial.println("NAALD HAD NOOIT AAN MOGEN STAAN!!!");
          naaldNoodStop();
        }
      }

      if(isMotorAan)  // motor moet aan
      {

        if(armGewicht < netUitHokGewicht) // arm net aan => nognetInHokGewicht
          armGewicht = netUitHokGewicht;

        if(armGewicht < targetGewicht) // target gewicht
          armGewicht += snelheidOp;

        if(armGewicht > netOpDePlaatGewicht)  // arm op plaat =>
          armGewicht = targetGewicht;         // target gewicht
      }
      else // motor moet uit
      {
        if(armGewicht > netVanDePlaatGewicht) // arm net uit =>
          armGewicht = netVanDePlaatGewicht;  // meeste gewicht eraf

        if(armGewicht > HOK_ARMGEWICHT) // niet helemaal uit =>
          armGewicht -= snelheidAf;     // minder hard
        
        if(armGewicht < netInHokGewicht) // arm van plaat =>
          armGewicht = HOK_ARMGEWICHT;   // meteen uit
      }

      armKracht = gewicht2pwm(armGewicht);
      pwmWriteF(armMotor, armKracht);

      if(armGewicht != targetGewicht){
        naaldEropInterval.reset();
      }
    }

  }
};



