#include <Wire.h>


bool jackIn = false;
int volume = 60;
int volumeOud;



void versterkerInit(){
  Wire1.setSCL(SCL);
  Wire1.setSDA(SDA);

}

Interval versterkerInt(20, MILLIS);

void volumeFunc(){
  if(versterkerInt.loop()){
    

    if( volume != volumeOud ){//  ||   jackIn != digitalRead(koptelefoonAangesloten)){
      int err = 0;

      Wire1.begin();
      Wire1.beginTransmission(0x60); //verbinden met tpa60
      Wire1.write(1);

      // Wire1.write(0b11010000);//links in,  mono koptelefoon

              
      Wire1.write(0b11000000);//stereo
      // Wire1.write(0b11010000);//links in mono koptelefoon
      // Wire1.write(0b11100000);//links in bridge-tied speaker
      err = Wire1.endTransmission();
    
    
      Wire1.beginTransmission(0x60);
      Wire1.write(2);
      Wire1.write(byte(volume));
      err = Wire1.endTransmission();

      volumeOud = volume;

      if(err){
        Serial.println("geen koptelefoon versterker");
      }else{
        Serial.print("volume: ");
        Serial.println(volume);
      }
      
    }
  }

}





