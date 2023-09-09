#include <EEPROM.h>


bool eepromShit = false;
float eepromVersie = 1;

#define EEPROM_VERSIE             0

#define EEPROM_ARMKRACHT_500MG    100
#define EEPROM_ARMKRACHT_4000MG   110
#define EEPROM_ARMGEWICHT         120

#define EEPROM_WATERPAS_OFFSET    200

#define EEPROM_TRACK_OFFSET       300






void eepCommit(){
  eepromShit = true;
  delay(100);
  EEPROM.commit();
  delay(100);
  eepromShit = false;
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

}



void eepromOpslaan(){
  EEPROM.put( EEPROM_VERSIE,              eepromVersie);
  EEPROM.put( EEPROM_ARMKRACHT_500MG,     armKracht500mg);
  EEPROM.put( EEPROM_ARMKRACHT_4000MG,    armKracht4000mg);
  EEPROM.put( EEPROM_ARMGEWICHT,          armTargetGewicht);
  EEPROM.put( EEPROM_WATERPAS_OFFSET,     orientatie.gefilterdOffset);
  EEPROM.put( EEPROM_TRACK_OFFSET,        trackOffset);
}



void eepromPrint(){
  Serial.println("EEPROM_VERSIE: "            +String(eepromVersie));
  Serial.println("EEPROM_ARMKRACHT_500MG: "   +String(armKracht500mg));
  Serial.println("EEPROM_ARMKRACHT_4000MG: "  +String(armKracht4000mg));
  Serial.println("EEPROM_ARMGEWICHT: "        +String(armTargetGewicht));
  Serial.println("EEPROM_WATERPAS_OFFSET: "   +String(orientatie.gefilterdOffset));
  Serial.println("EEPROM_TRACK_OFFSET: "      +String(trackOffset));
}





void opslagInit(){
  EEPROM.begin(4096);
}