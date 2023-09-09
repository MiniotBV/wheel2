#include <EEPROM.h>

int eepromShit = 0;
float eepromVersie;



#define EEPROM_VERSIE             0

#define EEPROM_ARMKRACHT_500MG    100
#define EEPROM_ARMKRACHT_4000MG   110
#define EEPROM_ARMGEWICHT         120

#define EEPROM_WATERPAS_OFFSET    200

#define EEPROM_TRACK_OFFSET       300

#define EEPROM_ARMHOEK_MIN        400
#define EEPROM_ARMHOEK_MAX        410




void eepCommit(){
  EEPROM.commit();
  eepromShit = 0;
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
  
  eepromLeesFloatWaarde( EEPROM_ARMKRACHT_500MG,  armKracht500mg);
  eepromLeesFloatWaarde( EEPROM_ARMKRACHT_4000MG,  armKracht4000mg);

  eepromLeesFloatWaarde( EEPROM_ARMGEWICHT,  armTargetGewicht);

  eepromLeesFloatWaarde( EEPROM_WATERPAS_OFFSET,  orientatie.gefilterdOffset);

  eepromLeesFloatWaarde( EEPROM_TRACK_OFFSET,  trackOffset);

  eepromLeesFloatWaarde( EEPROM_ARMHOEK_MIN,  armHoekMin);
  eepromLeesFloatWaarde( EEPROM_ARMHOEK_MAX,  armHoekMax);

}



void eepromOpslaan(){
  EEPROM.put( EEPROM_VERSIE,              eepromVersie);
  EEPROM.put( EEPROM_ARMKRACHT_500MG,     armKracht500mg);
  EEPROM.put( EEPROM_ARMKRACHT_4000MG,    armKracht4000mg);
  EEPROM.put( EEPROM_ARMGEWICHT,          armTargetGewicht);
  EEPROM.put( EEPROM_WATERPAS_OFFSET,     orientatie.gefilterdOffset);
  EEPROM.put( EEPROM_TRACK_OFFSET,        trackOffset);

  EEPROM.put( EEPROM_ARMHOEK_MIN,         armHoekMin);
  EEPROM.put( EEPROM_ARMHOEK_MAX,         armHoekMax);
}



void eepromPrint(){
  // Serial.println("EEPROM_VERSIE:            " + String(eepromVersie,                  5));
  Serial.println("EEPROM_ARMKRACHT_500MG:   " + String(armKracht500mg,                5));
  Serial.println("EEPROM_ARMKRACHT_4000MG:  " + String(armKracht4000mg,               5));
  Serial.println("EEPROM_ARMGEWICHT:        " + String(armTargetGewicht,              5));
  Serial.println("EEPROM_WATERPAS_OFFSET:   " + String(orientatie.gefilterdOffset,    5));
  // Serial.println("EEPROM_TRACK_OFFSET:      " + String(trackOffset,                   5));
  Serial.println("EEPROM_ARMHOEK_MIN        " + String(armHoekMin,                    5));
  Serial.println("EEPROM_ARMHOEK_MAX        " + String(armHoekMax,                    5));
}





void opslagInit(){
  EEPROM.begin(4096);
}






void eepromCalibreerOrientatie(){
  orientatie.gefilterdOffset = orientatie.gefilterd;
  Serial.println("orientatie.gefilterdOffset: " + String(orientatie.gefilterdOffset, 5));  
}