//	=================
//	versterker.h
//	=================

bool isjackIn = false;
int volume = 32;
int volumeOud;
bool isNaaldEropOud = false;
bool volumeOverRide = false;



bool isNaaldLangGenoegOpDePlaat(){
	return arm.isNaaldEropSinds() > 500 && staat == S_SPELEN;
}

void versterkerInit()
{
	Wire1.setSCL(SCL);
	Wire1.setSDA(SDA);
	pinMode(koptelefoonEn, OUTPUT);
	digitalWrite(koptelefoonEn, 1);
}

Interval versterkerInt(20, MILLIS);

void volumeFunc()
{
	orientatie.update();

	if (versterkerInt.loop())
	{

		if (volume != volumeOud || isNaaldEropOud != isNaaldLangGenoegOpDePlaat() || volumeOverRide)
		{ //  ||   isjackIn != digitalRead(koptelefoonAangesloten)){

			if (!(arm.isNaaldErop() && staat == S_SPELEN) && !volumeOverRide)
			{
				// waarde = 0;
				digitalWrite(koptelefoonEn, 0);
				return;
			}

			volumeOverRide = false;
			digitalWrite(koptelefoonEn, 1);

			int waarde = volume;

			volumeOud = volume;
			isNaaldEropOud = isNaaldLangGenoegOpDePlaat();

			int err = 0;
			//	stereo			|	L (mono koptel.)	|	L bridge-tied speaker
			//	0b11000000	|	0b11010000				|	0b11100000

			err = i2cWrite(0x60, 1, 0b11000000);
			err = i2cWrite(0x60, 2, byte(waarde));

			if (err)
			{
				Serial.println("geen koptelefoon versterker");
			}
		}
	}
}
