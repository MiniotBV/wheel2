#include <Wire.h>
class Amplifier
{
  public:

  bool jackIn = false;
  int volume = 60;
  int volumeOud;

  void init()
  {
    Wire1.setSCL(SCL);
    Wire1.setSDA(SDA);
  }

  Interval interval {20, inMILLIS};

  void update()
  {
    if(!interval.loop()) return; //if(interval.loop()){
    if(volume == volumeOud) return; //if( volume != volumeOud ){//  ||   jackIn != digitalRead(koptelefoonAangesloten)){

    int err = 0;
    Wire1.begin();
    Wire1.beginTransmission(0x60); // tpa60
    Wire1.write(1);

    // Wire1.write(0b11010000);//links in,  mono koptelefoon
            
    Wire1.write(0b11000000);  // stereo
    // Wire1.write(0b11010000);//links in mono koptelefoon
    // Wire1.write(0b11100000);//links in bridge-tied speaker
    err = Wire1.endTransmission();

    Wire1.beginTransmission(0x60);
    Wire1.write(2);
    Wire1.write(byte(volume));
    err = Wire1.endTransmission();

    volumeOud = volume;
    if(err) Serial.println("geen koptelefoon versterker");
    // }else{
    //   // Serial.print("volume: ");
    //   // Serial.println(volume);
    // }
  }
};
