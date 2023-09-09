#include <EEPROM.h>

int eepromShit = 0;
bool eepromNogOpslaan = false;
float eepromVersie;



#define EEPROM_VERSIE             0

#define EEPROM_ARMKRACHT_500MG    100
#define EEPROM_ARMKRACHT_4000MG   110
#define EEPROM_ARMKRACHT_HOK      130
#define EEPROM_ARMGEWICHT         120

#define EEPROM_WATERPAS_OFFSETX   200
#define EEPROM_WATERPAS_OFFSETY   210
#define EEPROM_WATERPAS_OFFSETZ   220

#define EEPROM_TRACK_OFFSET       300

#define EEPROM_ARMHOEK_MIN        400
#define EEPROM_ARMHOEK_MAX        410




void eepCommit(){
	EEPROM.commit();
	eepromShit = 0;
  eepromNogOpslaan = false;
}





void eepromLeesFloatWaarde(int adress, float& waarde){
	float buffer = 0;
	buffer = EEPROM.get(adress, buffer);

	if(isfinite(buffer)){
		waarde = buffer;
		Serial.println("gelezen adress: " + String(adress) + "  waarde: " + String(buffer, 5));
	}else{
		EEPROM.put(adress, waarde);    
		Serial.println("geschreven adress: " + String(adress) + "  waarde: " + String(buffer, 5));
	}
}




void eepromUitlezen(){
	
	eepromLeesFloatWaarde( EEPROM_VERSIE,  eepromVersie);
	
	eepromLeesFloatWaarde( EEPROM_ARMKRACHT_500MG,    arm.krachtLaag);
	eepromLeesFloatWaarde( EEPROM_ARMKRACHT_4000MG,   arm.krachtHoog);
  eepromLeesFloatWaarde( EEPROM_ARMKRACHT_HOK,      arm.netInHokGewicht);

	eepromLeesFloatWaarde( EEPROM_ARMGEWICHT,         arm.targetGewicht);

	eepromLeesFloatWaarde( EEPROM_WATERPAS_OFFSETX,   orientatie.xOffset);
  eepromLeesFloatWaarde( EEPROM_WATERPAS_OFFSETY,   orientatie.yOffset);
  eepromLeesFloatWaarde( EEPROM_WATERPAS_OFFSETZ,   orientatie.zOffset);

	eepromLeesFloatWaarde( EEPROM_TRACK_OFFSET,       trackOffset);

	eepromLeesFloatWaarde( EEPROM_ARMHOEK_MIN,        armHoekMin);
	eepromLeesFloatWaarde( EEPROM_ARMHOEK_MAX,        armHoekMax);

}



void eepromOpslaan(){
  eepromVersie = versie;
	EEPROM.put( EEPROM_VERSIE,              eepromVersie);//eepromVersie);
	EEPROM.put( EEPROM_ARMKRACHT_500MG,     arm.krachtLaag);
	EEPROM.put( EEPROM_ARMKRACHT_4000MG,    arm.krachtHoog);
  EEPROM.put( EEPROM_ARMKRACHT_HOK,       arm.netInHokGewicht);
	EEPROM.put( EEPROM_ARMGEWICHT,          arm.targetGewicht);
	EEPROM.put( EEPROM_WATERPAS_OFFSETX,    orientatie.xOffset);
  EEPROM.put( EEPROM_WATERPAS_OFFSETY,    orientatie.yOffset);
  EEPROM.put( EEPROM_WATERPAS_OFFSETZ,    orientatie.zOffset);
	EEPROM.put( EEPROM_TRACK_OFFSET,        trackOffset);

	EEPROM.put( EEPROM_ARMHOEK_MIN,         armHoekMin);
	EEPROM.put( EEPROM_ARMHOEK_MAX,         armHoekMax);

  eepromShit = 1;
  eepromNogOpslaan = false;
}



void eepromPrint(){
	Serial.println("EEPROM_VERSIE:            " + String(eepromVersie,                  5));
	Serial.println("EEPROM_ARMKRACHT_500MG:   " + String(arm.krachtLaag,                5));
	Serial.println("EEPROM_ARMKRACHT_4000MG:  " + String(arm.krachtHoog,                5));
  Serial.println("EEPROM_ARMKRACHT_HOK:     " + String(arm.netInHokGewicht,           5));
	Serial.println("EEPROM_ARMGEWICHT:        " + String(arm.targetGewicht,             5));
  Serial.println("EEPROM_WATERPAS_OFFSETX:  " + String(orientatie.xOffset,            5));
  Serial.println("EEPROM_WATERPAS_OFFSETY:  " + String(orientatie.yOffset,            5));
	Serial.println("EEPROM_WATERPAS_OFFSETZ:  " + String(orientatie.zOffset,            5));
	Serial.println("EEPROM_TRACK_OFFSET:      " + String(trackOffset,                   5));
	Serial.println("EEPROM_ARMHOEK_MIN        " + String(armHoekMin,                    5));
	Serial.println("EEPROM_ARMHOEK_MAX        " + String(armHoekMax,                    5));
}





void opslagInit(){
	EEPROM.begin(4096);
}




















