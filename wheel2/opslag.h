#include <EEPROM.h>


bool eepromShit = false;

#define EEPROM_VERSIE     0

#define EEPROM_ARMKRACHT_500MG 10
#define EEPROM_ARMKRACHT_4000MG 20

#define EEPROM_WATERPAS_OFFSET 30




void eepSchrijfInt(int addr, int val){
  EEPROM.write(addr,     (val >> 8) & 255);
  EEPROM.write(addr + 1,  val       & 255);
}

int eepLeesInt(int addr){
  return (EEPROM.read(addr) << 8) + EEPROM.read(addr + 1);
}



void eepSchrijfFloat(int addr, float val){
  eepSchrijfInt(addr, int( val * 10000.0 ) );
}

float eepLeesFloat(int addr){
  return eepLeesInt(addr) / 10000.0;
}


void eepCommit(){
  eepromShit = true;
  delay(100);
  EEPROM.commit();
  delay(100);
  eepromShit = false;
}









void eepromPrint(){
  Serial.println("EEPROM VERSIE: " + String(eepLeesInt(EEPROM_VERSIE)));
}





void opslagInit(){
  EEPROM.begin(4096);
}