class WheelRpm
{
  public:
  // WheelRpm() { myRpm = AUTO; }

  const float r33 = 100.0f/3.0f;    //  #define rpm33 33.333
  const float r45 = 45.0f;          //  #define rpm45 45
  const float r78 = 78.0f;          //  #define rpm78 78
  float autoRpm, targetRpm {0.0f};
  
  bool isRpm33()   { return myRpm == R33;}
  bool isRpm45()   { return myRpm == R45;}
  bool isRpmAuto() { return myRpm == AUTO;}
  //bool isRpm78()   { return myRpm == R78;}

  // toggle through AUTO-R33-R45...AUTO
  void switchUserRpm()
  {
    myRpm = (rpmSetting)(myRpm + 1);  // can't add enums directly
    if(myRpm >= R00) myRpm = AUTO;
  }
  
  void setNewTargetRpm()
  {
    if(myRpm == AUTO) { targetRpm = autoRpm; return; }
	  if(myRpm == R33) { targetRpm = r33; return; }
	  if(myRpm == R45) { targetRpm = r45; return; }
	  //if(myRpm == R78) { targetRpm = r78; return; }
  }
  float rpm2blokken()
  {
    if(myRpm >= R00) return 0.0f;
    float blocks[3] {1.0f, 3.0f, 4.0f}; // AUTO=1, R33=3, R45=4 (add 7 for R78)
    return blocks[myRpm];
    // if(rpmStaat == R33){
    //   blokken = 3;//3.33;
    // }  
    // else if(rpmStaat == R45){
    //   blokken = 4;//4.5;
    // }
    // else if(rpmStaat == AUTO){
    //   blokken = 1;//4.5;
  } 
  
  private:
  enum rpmSetting : unsigned char { AUTO, R33, R45, R00, R78 };  // R00 used as END
  rpmSetting myRpm = AUTO;
};