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
