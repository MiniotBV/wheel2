#include <Wire.h>


bool jackIn = false;
int volume = 60;
int volumeOud;
bool isNaaldEropOud = false;





char i2cRead(byte adress, byte reg, byte hoeveel)
{
  int err = 0;
  
  Wire1.begin();
  Wire1.beginTransmission(adress);
  Wire1.write(reg);
  err = Wire1.endTransmission();

  if(err) return -1;
  
  if(Wire1.requestFrom(adress, hoeveel) <= 0) return -1;
  return Wire1.read();
}


int i2cWrite(byte adress, byte reg, byte data)
{
  Wire1.begin();
  Wire1.beginTransmission(adress);
  Wire1.write(reg);
  Wire1.write(data);
  
  return Wire1.endTransmission();
}





void versterkerInit(){
  Wire1.setSCL(SCL);
  Wire1.setSDA(SDA);

}

Interval versterkerInt(20, MILLIS);
Interval orientatieInt(100, MILLIS);

void volumeFunc(){
  if(versterkerInt.loop()){
    

    if( volume != volumeOud   ||   isNaaldEropOud != isNaaldErop()){//  ||   jackIn != digitalRead(koptelefoonAangesloten)){
      
      int waarde = volume;
      
      if(!isNaaldErop()){
        waarde = 0;
      }
      
      volumeOud = volume;
      isNaaldEropOud = isNaaldErop();
      
      
      int err = 0;

      // Wire1.begin();
      // Wire1.beginTransmission(0x60); //verbinden met tpa60
      // Wire1.write(1);

      // // Wire1.write(0b11010000);//links in,  mono koptelefoon     
      // Wire1.write(0b11000000);//stereo
      // // Wire1.write(0b11010000);//links in mono koptelefoon
      // // Wire1.write(0b11100000);//links in bridge-tied speaker
      // err = Wire1.endTransmission();
    
    
      // Wire1.beginTransmission(0x60);
      // Wire1.write(2);
      // Wire1.write(byte(waarde));
      // err = Wire1.endTransmission();

      err = i2cWrite(0x60, 1, 0b11000000);
      err = i2cWrite(0x60, 2, byte(waarde));

      

      if(err){
        Serial.println("geen koptelefoon versterker");
      }else{
        // Serial.print("volume: ");
        // Serial.println(volume);
      }
      
    }
  }



  if(orientatieInt.loop()){
    
  }

}













// R1,R5 = 100Ohm
// R2,R6 = 1.30kOhm 1% 0.25W, Multicomp MF25 1K3
// R3,R7 = 39Ohm
// R4,R8 = 1.5kOhm 5% 1W, Multicomp MCF 1W 1K5
// R9,R11,R16,R20-R22 = 2.2kOhm
// R10,R12-R15,R19 = 47kOhm
// R17 = 150kOhm
// R18 = 470kOhm

// Capacitors
// C1,C3,C24 = 47µF 20% 25V, lead pitch 2.5mm, Rubycon type 25ZLG47M6.3X7
// C2,C4 = 100µF 20% 25V, lead pitch 2.5mm, 1.43A AC, Nichicon type UPM1E101MED
// C5,C8 = 100nF 10% 100V, lead pitch 7.5mm, Epcos type B32560J1104K
// C6,C7,C9,C10 = 4700µF 20% 35V, lead pitch 10mm, snap in, Panasonic type ECOS1VP472BA (25mm max. diameter)
// C11-C18 = 47nF 10% 50V ceramic, lead pitch 5mm
// C19,C20 = 47nF 20%, 630VDC X2, lead pitch 15mm, Vishay BCcomponents BFC233620473
// C21 = 4.7µF 20% 63V, lead pitch 2.5mm
// C22,C23 = 22µF 20% 35V, non polarised, lead pitch 2.5mm, Multicomp NP35V226M6.3X11

// Semiconductors
// D1-D4,D7 = 1N4002
// D5,D8-D16 = 1N4148
// D6,D17 = LED, green, 3mm
// T1,T3,T4,T6 = BC337
// T2,T5,T7 = BC327
// IC1,IC2 = LT1083 (Linear Technology)
// B1,B2 = GSIB1520 (15A/200V bridge rectifier) (Vishay General Semiconductor)
// K1,K2,K4 = 2-way PCB terminal block, lead pitch 5mm
// K3 = 3-way PCB terminal block, lead pitch 5mm
// D6,D17,S1 = 2-pin SIL pinheader, lead pitch 0.1 inch
// K5,K6 = 3-pin SIL pinheader, lead pitch 0.1 inch
// F1,F2 = fuse, 6.3A antisurge (time lag), with 20x5mm PCB mount fuseholder and cover
// Heatsink, Fischer Elektronik type SK92/75SA 1.6 K/W, size: 100x40mm, Farnell # 4621578, Reichelt # V7331G
// Miscellaneous
// M3 screws, nuts and washers for mounting IC1 and IC2 to heatsink (see text)
// TO-3P thermal pad, (Bergquist type K6-104)
// M4x10 screws for mounting heatsink to PCB
// PCB # 100124-2, Elektor Shop





















