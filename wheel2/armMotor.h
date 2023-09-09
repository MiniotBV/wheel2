#define MAX_ARMGEWICHT 4//gr
#define MIN_ARMGEWICHT 0.5//gr

class Arm
{
  public:
  /** arm variables */
  float weight = 2.5f;
  float w500mg = 0.22f;
  float w4000mg = 0.56f;
  float targetWeight = 0.4f;
  float armKracht = 0.0f;
  float velocityOn = 0.00025f;
  float velocityOff = 0.001f;

  //op de plaat
  float netUitHokKracht = 0.15;
  float netOpDePlaatKracht = 0.2;

  //van de plaat af
  float netVanDePlaatKracht = 0.2;
  float netInHokKracht = 0.15;

  bool isMotorOn = false;

  void armInit() { setPwm(armMotor); }

  void updateWeight()
  {
    weight = limitF(weight, MIN_ARMGEWICHT, MAX_ARMGEWICHT);  
    targetWeight = mapF(weight, MIN_ARMGEWICHT, MAX_ARMGEWICHT, w500mg, w4000mg);
  }

  Interval interval {10, inMILLIS};

  void update()
  {
    if(!interval.loop()) return;  //if(interval.loop()){

    if(isMotorOn)   // moet arm motor aan?
    {
      // arm net aan => jump naar nogNetInHokKracht
      if(armKracht < netUitHokKracht) armKracht = netUitHokKracht;
      
      // arm al op target gewicht?
      if(armKracht < targetWeight) armKracht += velocityOn; 
      
      // arm al op plaat => meteen op target gewicht
      if(armKracht > netOpDePlaatKracht) armKracht = targetWeight;
    }

    if(!isMotorOn)    // moet arm motor uit?
    {
      // als arm net is uitgezet => haal meeste gewicht van de arm
      if(armKracht > netVanDePlaatKracht) armKracht = netVanDePlaatKracht;
      // arm nog niet helemaal uit => minder hard
      if(armKracht > 0) armKracht -= velocityOff;
      //is de arm al van de plaat? // zet de arm dan meteen uit
      if(armKracht < netInHokKracht) armKracht = 0;
    }

    pwmWriteF(armMotor, armKracht);
  }

  bool isNaaldErop()
  {
    isMotorOn = true;
    return armKracht == targetWeight;
  }

  bool isNaaldEraf()
  {
    isMotorOn = false;
    return armKracht == 0;
  }
};