#include <Wire.h>


bool jackIn = false;
int volume = 32;
int volumeOud;
bool isNaaldEropOud = false;
bool volumeOverRide = false;




char i2cRead(byte adress, byte reg)
{
  int err = 0;
  
  Wire1.begin();
  Wire1.beginTransmission(adress);
  Wire1.write(reg);
  err = Wire1.endTransmission(false);

  if(err) return -111;
  
  if(Wire1.requestFrom(adress, 1) <= 0) return -111;
  return Wire1.read();
}



int i2cWrite(byte adress, byte reg, byte data)
{
  Wire1.begin();
  Wire1.beginTransmission(adress);
  Wire1.write(reg);
  Wire1.write(data);

  // if(Wire1.endTransmission()){
    // Serial.print("err@: ");
    // Serial.print(adress);
    // Serial.print(" er:");
    // Serial.println(Wire1.endTransmission());
  // }
  
  return Wire1.endTransmission();
}



void set_bit(uint8_t *byte, uint8_t n, bool value)
{
    *byte = (*byte & ~(1UL << n)) | (value << n);
}

bool get_bit(uint8_t byte, uint8_t n)
{
    return (byte >> n) & 1U;
}










// ========================
//    ORIENTATIE
// ========================
class Orientatie //          QMA7981
{
  public:
  // Orientatie()//(byte a)
  // {
  //   // i2cWrite(adress, 0x1B, 0b10000000);
  // }
  
  bool isStaand    = false;
  bool isStaandOud = false;

  bool isFout = false;
  bool isFoutOud = false;
  
  byte adress = 0b0010010;

  float x, y, z;
  float gefilterd, gefilterdPrev;
  float gefilterdOffset = 0.05;
  int id;
  unsigned long loop;
  bool eersteKeer = true;


  void print()
  {
    // update();
    // Serial.print("id: ");
    // Serial.print(id);

    Serial.print("x:");
    Serial.print(x,3);
    Serial.print(" y:");
    Serial.print(y,3);
    Serial.print(" z:");
    Serial.print(z,3);
    
    Serial.println( isStaand ? " staand" : " liggend");
  }

  void update()
  {
    if(millis() - loop <= 10 || millis() < 200) return;
    /*if(state.isNotSPELEN() && millis() - loop > 1000) { */
    // if(millis() - loop <= 1000)return;

    if(eersteKeer){
      eersteKeer = false;
      reset();
    }
    
    loop = millis();

    // id = i2cRead(adress, 0x00);

    x += (read_accel_axis(1) - x)/10;
    y += (read_accel_axis(3) - y)/10;
    z += (read_accel_axis(5) - z)/10;

    gefilterdPrev += ((y - gefilterdOffset) - gefilterdPrev) / 10;
    gefilterd += (gefilterdPrev - gefilterd) / 10;

    isFout = ! isOngeveer(gefilterd, 0, 0.1);

    if( isFout  != isFoutOud ){
      isFoutOud = isFout;

      if(isFout){
        setStaat(S_FOUTE_ORIENTATIE);
      }
    }
    
    isStaand = !(  isOngeveer(x, 0, 0.4)   &&   isOngeveer(z, -1, 0.4)  );



    if(isStaand != isStaandOud)
    {
      isStaandOud = isStaand;
      // Serial.print("orientatie veranderd: ");
      print();
    }

    // stick.isStaand = isStaand;

    // print();
  }

  float read_accel_axis(uint8_t reg)
  {
      Wire1.begin();
      Wire1.beginTransmission(adress);
      Wire1.write(reg);
      Wire1.endTransmission(false);
      Wire1.requestFrom(adress, 2);
      int16_t data = (Wire1.read() & 0b11111100) | (Wire1.read() << 8); // dump into a 16 bit signed int, so the sign is correct
      data = data / 4;// divide the result by 4 to maintain the sign, since the data is 14 bits

      float buf = data / 4096.0;
      return buf;
  }



  enum qma7981_full_scale_range_t
  {
      RANGE_2G = 0b0001,
      RANGE_4G = 0b0010,
      RANGE_8G = 0b0100,
      RANGE_16G = 0b1000,
      RANGE_32G = 0b1111
  };

  enum qma7981_bandwidth_t
  {
      MCLK_DIV_BY_7695 = 0b000,
      MCLK_DIV_BY_3855 = 0b001,
      MCLK_DIV_BY_1935 = 0b010,
      MCLK_DIV_BY_975 = 0b011,
      MCLK_DIV_BY_15375 = 0b101,
      MCLK_DIV_BY_30735 = 0b110,
      MCLK_DIV_BY_61455 = 0b111
  };

  enum qma7981_clock_freq_t
  {
      CLK_500_KHZ = 0b0000,
      CLK_333_KHZ = 0b0001,
      CLK_200_KHZ = 0b0010,
      CLK_100_KHZ = 0b0011,
      CLK_50_KHZ = 0b0100,
      CLK_25_KHZ = 0b0101,
      CLK_12_KHZ_5 = 0b0110,
      CLK_5_KHZ = 0b0111
  };




  void reset(){
    //reset
    i2cWrite(adress, 0x36, 0xB6);
    i2cWrite(adress, 0x36, 0x00);

    delay(10);

    //set_mode
    uint8_t data = i2cRead(adress, 0x11);
    set_bit(&data, 7, 1);//1 = active, 0 = deactive;

    //set_clock_freq
    data &= 0b11110000;      // clear bits 0-3
    data |= (CLK_500_KHZ & 0b1111); // set freq on bits 0-3
    i2cWrite(adress, 0x11, data);


    //set_bandwidth
    data = 0b11100000;
    data |= (MCLK_DIV_BY_7695 & 0b111);
    i2cWrite(adress, 0x10, data);

    //set_full_scale_range
    data = 0b11110000;
    data |= (RANGE_2G & 0b1111);
    i2cWrite(adress, 0x0F, data);

  }

};



Orientatie orientatie;















void versterkerInit(){
  Wire1.setSCL(SCL);
  Wire1.setSDA(SDA);

}

Interval versterkerInt(20, MILLIS);
Interval orientatieInt(100, MILLIS);

void volumeFunc(){
  if(versterkerInt.loop()){
    

    if( volume != volumeOud   ||   isNaaldEropOud !=    (isNaaldErop() && staat == S_SPELEN)    || volumeOverRide){//  ||   jackIn != digitalRead(koptelefoonAangesloten)){
      
      
      int waarde = volume;
      
      if(!(isNaaldErop() && staat == S_SPELEN)   &&    !volumeOverRide){
        waarde = 0;
      }

      volumeOverRide = false;
      
      volumeOud = volume;
      isNaaldEropOud = (isNaaldErop() && staat == S_SPELEN);
      
      
      int err = 0;


      //0b11000000);//stereo
      //0b11010000);//links in mono koptelefoon
      //0b11100000);//links in bridge-tied speaker
    
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



  orientatie.update();


}


























