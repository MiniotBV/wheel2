#include <Wire.h>





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

















Interval orientatieInt(10, MILLIS);
Interval staatGoedInterval( 0, MILLIS);

// ========================
//    ORIENTATIE
// ========================
class Orientatie //          QMA7981
{
	public:
  bool golven = false;
	
	bool isStaand    = false;
	bool isStaandOud = false;

	bool isFout = false;
	bool isFoutOud = false;
	
	byte adress = 0b0010010;

	float x, y, z;
  float xRuw, yRuw, zRuw;
	float xOffset = 0,yOffset = 0,zOffset = 0;//0.05;
	// int id;
	unsigned long loop;
	bool eersteKeer = true;


  // int calwaardeofzo;


  void print() {
    String output = "xRuw: " + String(xRuw, 3) + " yRuw: " + String(yRuw, 3) + " zRuw: " + String(zRuw, 3) + "\n";
    output += "x: " + String(x, 3) + " y: " + String(y, 3) + " z: " + String(z, 3) + "\n";
    output += (isStaand ? "staand\n" : "liggend\n");
    
    Serial.println(output);
  }

	void update()
	{
		if(orientatieInt.loop() && millis() > 200){//staat hij ook al 200ms aan?
			
			if(eersteKeer){
				eersteKeer = false;
				reset();
        return;
			}
			
			// loop = millis();

			// id = i2cRead(adress, 0x00);
      float xSensorRuw = read_accel_axis(1);
      // float ySensorRuw = read_accel_axis(3);
      float zSensorRuw = read_accel_axis(5);

			xRuw += (xSensorRuw - xRuw)/10;
			// yRuw += (ySensorRuw - yRuw)/10;
			zRuw += (zSensorRuw - zRuw)/10;

      x += ( (xRuw - xOffset) - x) / 10;
      // y += ( (yRuw - yOffset) - y) / 10;
      z += ( (zRuw - zOffset) - z) / 10;





			// if(isFout){
			// 	isFout = ! isOngeveer(y, 0, 0.1);
			// }else{
			// 	isFout = ! isOngeveer(y, 0, 0.15);        
			// }
			

			// if( isFout && !isFoutOud ){
			// 	isFoutOud = isFout;
			// 	setStaat(S_FOUTE_ORIENTATIE);
			// }

			// if( !isFout ){
			// 	if(isFout != isFoutOud)  {
			// 		isFoutOud = isFout;
			// 		staatGoedInterval.reset();
			// 	}  

			// 	if(staatGoedInterval.sinds() > 1000  &&  staat == S_FOUTE_ORIENTATIE){
			// 		setStaat(S_HOK);
			// 	}
			// }


      y += (armHoekCall - y)/100;

      if(isFout){
				isFout = ! isOngeveer(armHoekCall, 0, 0.6);
			}else{
        if(staat == S_HOK  && staatVeranderd.sinds() > 1000){
          isFout = ! isOngeveer(armHoekCall, 0, 0.8);   
        }  
			}
			

			if( isFout && !isFoutOud ){
				isFoutOud = isFout;
				setStaat(S_FOUTE_ORIENTATIE);
			}

			if( !isFout ){
				if(isFout != isFoutOud)  {
					isFoutOud = isFout;
					staatGoedInterval.reset();
				}  

				if(staatGoedInterval.sinds() > 3000  &&  staat == S_FOUTE_ORIENTATIE){
					setStaat(S_HOK);
				}
			}




			


			isStaand = !(  isOngeveer(x, 0, 0.4)   &&   isOngeveer(z, -1, 0.4)  );


			if(isStaand != isStaandOud)
			{
				isStaandOud = isStaand;
				print();
			}




      if(golven){
        debug(String(x, 5) + "," + String(y, 5) + "," + String(z, 5));// + "," + String(calwaardeofzo));
      }
		}

	}

	void calibreer(){
		xOffset += x;
		debug("orientatie.xOffset: " + String(xOffset, 5)); 

    yOffset += y;
		debug("orientatie.yOffset: " + String(yOffset, 5)); 

    zOffset += (z + 1); // hij moet op -1 uitkomen
		debug("orientatie.zOffset: " + String(zOffset, 5)); 
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




	void reset(){
		// reset
		i2cWrite(adress, 0x36, 0xB6);//soft reset
		i2cWrite(adress, 0x36, 0x00);

		delay(10);

		//set_mode
		uint8_t data = i2cRead(adress, 0x11);
		set_bit(&data, 7, 1);//1 = active, 0 = deactive;
		i2cWrite(adress, 0x11, data);


    // calwaardeofzo = i2cRead(adress, 0x0F);
		//set_full_scale_range
		// data = 0b11110000;
		// data |= (RANGE_2G & 0b1111);
		// i2cWrite(adress, 0x0F, data);
    // i2cWrite(adress, 0x0F, 0b00000001);
    // calwaardeofzo = i2cRead(adress, 0x0F);


	}

};



Orientatie orientatie;
























bool jackIn = false;
int volume = 22;
int volumeOud;
bool isNaaldEropOud = false;
bool volumeOverRide = false;





bool isNaaldLangGenoegOpDePlaatVoorGeluid(){
	return arm.isNaaldEropVoorZoLang(2000)  &&  staat == S_SPELEN  &&  !puristenMode; // als de puristenMode aanstaat mag er geen geluid komen
}








void versterkerInit(){
	pinMode(koptelefoonEn, OUTPUT);
	digitalWrite(koptelefoonEn, 1);
}


Interval versterkerInt(20, MILLIS);

void volumeFunc(){
  if (!versterkerInt.loop()) return;

  if(!isNaaldLangGenoegOpDePlaatVoorGeluid()   &&    !volumeOverRide){
    digitalWrite(koptelefoonEn, 0);
    volumeOud = -88;// om een herzend te triggeren
    // Serial.println("geluid uit");
    return;
  }

  
  if( volume != volumeOud   ||   isNaaldEropOud !=    isNaaldLangGenoegOpDePlaatVoorGeluid()    ||   volumeOverRide){//  ||   jackIn != digitalRead(koptelefoonAangesloten)){
    
    digitalWrite(koptelefoonEn, 1);      
    
    volumeOud = volume;
    isNaaldEropOud = isNaaldLangGenoegOpDePlaatVoorGeluid();
    
    //0b11000000);//stereo
    //0b11010000);//links in mono koptelefoon
    //0b11100000);//links in bridge-tied speaker
    int err = 0;
    err = i2cWrite(0x60, 1, 0b11000000);

    int waarde = volume;
    err = i2cWrite(0x60, 2, byte(waarde));

    if(err){
      debug("geen koptelefoon versterker");
    }else{
      // Serial.println("volume: " + String(volume) + " geschreven");
    }
    
  }
}













//--------------------------het commando begind met dit karrakter
#define BT_KNOP_IN "4"
#define BT_KNOP_UIT "c"

//--------------------------het commando eindigd met dit karakter
#define BT_VOLGEND_NUMMER "b"
#define BT_VORRIG_NUMMER  "c"
#define BT_DOORSPOELEN    "9"
#define BT_TERUGSPOELEN   "8"

#define BT_PLAY           "4" //koptelefoon op
#define BT_PAUZE          "6" //koptelefoon af?

// #define BT_PAUZE_LANG

//AT+
//AT+SCAN
//AT+REST
//AT+DELVMLINK

Interval bluetoothInt(20, MILLIS);
bool bluetoothInitNogDoen = true;
bool bluetoothDebug = false;
bool draadlozeVersie = false;


String bluetoothBuffer = "";






void bluetoothScrijf(String commando){
  Serial2.print(commando);

  if(bluetoothDebug){
    Serial.println("BT UIT:" + commando);
  }
}



void bluetoothInit(){
  Serial2.setRX(BT_RXD);
  Serial2.setTX(BT_TXD);
  Serial2.setPollingMode(true);
  Serial2.setFIFOSize(128);
  Serial2.begin(9600);

  bluetoothScrijf("AT+");
}











void bluetoothOntcijfering(){
  if(bluetoothDebug){
    debug("BT IN:" + bluetoothBuffer);
  }


  if(bluetoothBuffer.startsWith("income_opid:")){
    bluetoothBuffer.replace("income_opid:", "");
    bluetoothBuffer.trim();

    if(bluetoothBuffer.startsWith(BT_KNOP_IN)){
      bluetoothBuffer.remove(0, 1); // gooi de in uit getal weg
      debug("BT KNOP_IN: " + bluetoothBuffer);
      
      // if(bluetoothBuffer == BT_PLAY){
      //   if(staat == S_PAUZE || staat == S_SPELEN) { pauze(); }//miscnien s_spelen dr uithalen
      //   else if(staat == S_HOK) { spelen(); }
      // }

      if(bluetoothBuffer == BT_PLAY){
        if(staat == S_PAUZE) { pauze(); }//miscnien s_spelen dr uithalen
        else if(staat == S_HOK) { spelen(); }
      }

      else if(bluetoothBuffer == BT_PAUZE){
        if(staat == S_PAUZE || staat == S_SPELEN) { pauze(); }//miscnien s_spelen dr uithalen
      }

      else if(bluetoothBuffer == BT_VOLGEND_NUMMER){
        if((staat == S_SPELEN || staat == S_PAUZE || staat == S_NAAR_NUMMER)){
          naarVolgendNummer();
        }
      }

      else if(bluetoothBuffer == BT_VORRIG_NUMMER){
        if((staat == S_SPELEN || staat == S_PAUZE || staat == S_NAAR_NUMMER)){
          naarVorrigNummer();
        }
      }


    }
    else if(bluetoothBuffer.startsWith(BT_KNOP_UIT)){
      bluetoothBuffer.remove(0, 1); // gooi de in uit getal weg
      debug("BT KNOP_UIT: " + bluetoothBuffer);
    }
    else {
      debug("BT KNOP_ONBEKENT: " + bluetoothBuffer);
    }
  
  }
  
  if(bluetoothBuffer.startsWith("OK+")){
    if(millis() < 2000){
      draadlozeVersie = true; // ff checken of er wel een bluetooth module is aangesloten
    }
    
    
  }
  
  
}





Interval bluetoothCheckVoorOpstart(2000, MILLIS);




void bluetoothFunc(){
  if(millis() < 1000)return;
  
  if(bluetoothInitNogDoen){
    bluetoothInitNogDoen = false;
    bluetoothInit();
    return;    
  }

  if(bluetoothCheckVoorOpstart.eenKeer()){
    bluetoothScrijf("AT+");
  }
  
  if(bluetoothInt.loop()){

    while(Serial2.available() > 0){
      char c = Serial2.read();
      if( c == '\n' || c == '\r' ){
        // if(c == '\n')Serial.print("<nl>");
        // if(c == '\r')Serial.print("<cr>");
        if(bluetoothBuffer != ""){
          bluetoothOntcijfering();
        }

        bluetoothBuffer = "";
      }else{
        bluetoothBuffer += c;
      }
    }

  }
}
















