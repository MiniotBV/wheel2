const int versie = 32;

const char help[] PROGMEM = "";



#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 dac;

unsigned long draaienStart = 0;       //timers
unsigned long pidInterval = 0;
unsigned long hallInterval = 0;
unsigned long hallSamples = 0;
unsigned long interval = 0;
unsigned long serialInterval = 0;

bool golven = false;                   //status
bool leesbareGolven = false;
bool uitdraaien = false;
bool opgestart = false;
bool opsnelheid = false;
bool rpmIs33 = true;

#define inGladSamples 10
float in;
float inOud;
double inGladPre;
float inGlad;
//float uit;

#define sampleMax 65500               //samples
#define sampleLengte 2
unsigned int samples[sampleLengte];
unsigned long sampleTeller = 0;



int hallPin = 3;                       //io
int knopRpm = 2;

int led =     13;


#define EEPROM_uit false

#define ROMminrpm 10
#define ROMmaxrpm 2000
#define ROMrpm33 10

#define naar45 1.35

double rpm33 = 33.333;
double targetRpm = 0;

double basis = 0;      


#define ROMpidP 100                    //pid
#define ROMpidI 110
#define ROMpidD 120
float P = 500;
float I = 0.01;
float D = 0;




//---------------------------------------------------------------TEST
#define ROMtest 200     
bool test = false;
unsigned long glitchTestInterval = 0;










void setup() {

  dac.begin(0x61);
  dac.setVoltage(0, false); //set motor naar 0V
 
  
  Serial.begin(115200);

  pinMode(knopRpm, INPUT_PULLUP);
  


  setRpm(sampleMax);





  double i = 0;   
  EEPROM.get(ROMrpm33, i);  //EEPROM rpm33
  if(!(i > ROMminrpm && i < ROMmaxrpm) || EEPROM_uit){   //!(i > n) omdat elke vergelijking met i false is als er nog niks in eeprom staat
    EEPROM.put(ROMrpm33, rpm33);
  }else{ rpm33 = i;}




  float j = 0;
  EEPROM.get(ROMpidP, j);  //EEPROM pidP
  if(!(j >= 0) || EEPROM_uit){  //!(j >= 0) omdat elke vergelijking met j false is als er nog niks in eeprom staat
    EEPROM.put(ROMpidP, P);
  }else{ P = j;}

  EEPROM.get(ROMpidI, j);  //EEPROM pidI
  if(!(j >= 0) || EEPROM_uit){
    EEPROM.put(ROMpidI, I);
  }else{ I = j;}
  
  EEPROM.get(ROMpidD, j);  //EEPROM pidD
  if(!(j >= 0) || EEPROM_uit){
    EEPROM.put(ROMpidD, D);
  }else{ D = j;}



  bool dd = false;
  EEPROM.get(ROMtest, dd);  //EEPROM test of de motor aan moet in het begin of dat hij alleen op commando aan gaat
  if(!(dd >= 0) || EEPROM_uit){
    EEPROM.put(ROMtest, test);
  }else{ test = dd ? 1 : 0;}





  
  if(test == true){//-------------------------------------------------test dingen:
    golven = true;
    //draaien();  //starten zonder commando
  }
  


//  attachInterrupt(1, hall, RISING);  //sensor aansluiten
//  attachInterrupt(0, hall, CHANGE);  //sensor aansluiten
//  attachInterrupt(0, hall, FALLING);  //sensor aansluiten
    attachInterrupt(1, hall, CHANGE);  //sensor aansluiten
}


















void loop() {
  
  //---------------------------------------------------------------------------------------PID
  if(micros()-pidInterval > 10000){
    pidInterval = micros();



    if(micros() - hallInterval > sampleMax){    //te lang geen puls
      samples[sampleTeller % sampleLengte] = sampleMax;
      sampleTeller++;
    }


    in = getRpm();            //bereken rpm
    int uitBuff = bereken(in) + basis;          //bereken motor kracht
//    int uitBuff = uit + basis;
    if(uitBuff < 0){uitBuff = 0;}
    if(uitBuff > 3500){uitBuff = 3500;}

    if(targetRpm != 0){             //staat de motor aan?
      dac.setVoltage(uint16_t(uitBuff), false);
    }else{
      dac.setVoltage(0, false);
    }

    


    inGladPre += in; //                                                                        filter ruis uit 'in' 
    inGladPre /= 1.0 + (1.0 / inGladSamples);
    inGlad = inGladPre / inGladSamples;

    
    if(targetRpm > 1  &&  inGlad > rpm33*4){ //    4 * rpm33 om te snel te gaan                       snaar dr af / tesnel
      Serial.println(">");
      if(test){
        stoppen();
      }
    }


    
    if(targetRpm > 1  &&  millis() - draaienStart > 3000){ //                                      tegen gehouden
      if(inGlad > targetRpm * 2 ){ //te snel 200%
        Serial.println("E");
        if(test){
          stoppen();
        }
      }else if(inGlad  <  targetRpm * 0.70){ //te langzaam 70%
        Serial.println("T");
        if(test){
          stoppen();
        }
      }
    }



    if( targetRpm > 1   &&   inGlad  >  targetRpm * 0.98   &&   opsnelheid == false ){ //                       op snelheid (98% van de snelheid)
      Serial.println("O");
      opsnelheid = true;
    }


    
    if(targetRpm == 0 && millis() - draaienStart > 1000 && uitdraaien == false){//                              aangeslingerd
      if(in  >  rpm33 * 0.50){                                                       //50% van de 33.3 snelheid
        Serial.println("A");
        if(test){
          draaien();
        }
      }
    }


    
    if(uitdraaien == true && inGlad < rpm33 * 0.05){ //     <5% van de 33.3 snelheid                                                    uitgedraaid
      uitdraaien = false;
      draaienStart = millis();
      Serial.println("U");
    }



    








    
    if(golven){//                                                                           golven
      if(leesbareGolven){
        Serial.print(in, 3);
        Serial.print(", ");
        Serial.print(inGlad, 3);
        Serial.print(", ");
        Serial.print(uitBuff, DEC);
        Serial.print(", ");
        Serial.print(basis, 2);
        Serial.print(", ");
        Serial.print(targetRpm, 3);
        
        Serial.println();
      }else{
        Serial.print(in, 3);
        Serial.print(", ");
        Serial.print(targetRpm, 3);
        Serial.print(", ");
        Serial.print(inGlad, 3);
        Serial.println();
      }
    }
    
  }







  //----------------------------------------------------------------------------------------COMMUNICATIE
  if(micros()-serialInterval > 20000){ 
    serialInterval = micros();

    while(Serial.available() > 0){
      
      char buff = Serial.read();
      if      (buff == 'D'){    //draaien
        draaien();
      
      
      }else if(buff == 'S'){    //stoppen
        stoppen();
      
      
      }else if(buff == 'G'){    //golven aan
        golven = true;
      
      
      }else if(buff == 'g'){    //golven uit
        golven = false;


            
      }else if(buff == 'T'){    //testmodus aan
        test = true;
        EEPROM.put(ROMtest, test);
        Serial.println("testmodus aan");


      }else if(buff == 't'){    //testmodus uit
        test = false;
        EEPROM.put(ROMtest, test);
        Serial.println("testmodus uit");
      
      
      }else if(buff == 'm'){    //meer golven
        golven = true;
        leesbareGolven = !leesbareGolven;
      
      
      }else if(buff == 'p'){    //set P
        P = Serial.parseFloat();
        EEPROM.put(ROMpidP, P);
        Serial.print("p: ");
        Serial.println(P);
      
      
      }else if(buff == 'i'){    //set I
        I = Serial.parseFloat();
        EEPROM.put(ROMpidI, I);
        Serial.print("i: ");
        Serial.println(I);
      
      
      }else if(buff == 'd'){    //set D
        D = Serial.parseFloat();
        EEPROM.put(ROMpidD, D);
        Serial.print("d: ");
        Serial.println(D);
        
      
      
      }else if(buff == '+'){    //set sneller
        double i = Serial.parseFloat();
        if(i == 0){i = 1;}      // als er geen nummer is toegevoegd is het 1
        rpm33 += i;
        EEPROM.put(ROMrpm33, rpm33);
        targetRpm = rpmIs33 ? rpm33 : rpm33 * naar45;
        
      
      }else if(buff == '-'){    //set langzamer
        double i = Serial.parseFloat();
        if(i == 0){i = 1;}      // als er geen nummer is toegevoegd is het 1
        rpm33 -= i;
        EEPROM.put(ROMrpm33, rpm33);
        targetRpm = rpmIs33 ? rpm33 : rpm33 * naar45;
      
      
      }else if(buff == '='){    //set snelheid
        double i = Serial.parseFloat();
        if(i > 10){
          rpm33 = i;
          EEPROM.put(ROMrpm33, rpm33);
          targetRpm = rpmIs33 ? rpm33 : rpm33 * naar45;
        }
      


      
      }else if(buff == '?'){    //set snelheid
        Serial.print(test ? "testmodus - " : "");
        Serial.print(rpmIs33 ? "rpmKnop:33 " : "rpmKnop:45 ");
        Serial.println(digitalRead(knopRpm) ? "(5V)" : "(0V)");
        Serial.print("rpm33: ");Serial.print(rpm33);Serial.print("    rpm45: ");Serial.println(rpm33 * naar45);
        
        Serial.print("targetRpm: ");
        Serial.println(targetRpm);
        Serial.print("p: ");
        Serial.println(P);
        Serial.print("i: ");
        Serial.println(I);
        Serial.print("d: ");
        Serial.println(D);

        Serial.print("versie: ");
        Serial.println(versie);
        
        Serial.println();
      
      
      }else if(buff == 'h'){    //help
        for (int k = 0; k < strlen_P(help); k++) {
          Serial.write((char)pgm_read_byte_near(help + k));
        }
      
      
      
      }
    }
  }



  
  
  









  //----------------------------------------------------------------------------------------LED
  digitalWrite(led, micros() % 20000 >   1000 + ((in - targetRpm) * 4000) );
  
  
}
















//------------------------------------------------------------------------------------HALL SENSOR
void hall(){

  interval = micros() - hallInterval;
  if(interval < 20){return;} //waarschijnlijk een dubbelle puls, want 1000us pp = (1000/360)*60 = 270rpm ///////waarschijnlijk een dubbelle puls, want 1000us pp = (1000/3)*60 = 20000rpm
  
  hallInterval = micros();
  
    
  if(interval > sampleMax){interval = sampleMax;}

  for(int i = sampleLengte - 1;   i > 0;   i--){ //shift samples
    samples[i] = samples[i-1];
  }
  samples[0] = interval;
    
}












//-----------------------------------------------------------------------------------------------------RPM BEREKENEN
//#define pulsPerRotatiePerMinuut (1000000.0 / 3) * 60
//#define pulsPerRotatiePerMinuut (1000000.0 / 360) * 60
#define pulsPerRotatiePerMinuut (1000000.0 / 180) * 60


double getRpm(){
  
  double totaal = 0;
  
  for(byte i = 0;   i < sampleLengte;   i++){
    totaal += samples[i];
  }

  
  totaal /= sampleLengte;

  if(totaal >= sampleMax-1){return 0;} // staat stil

  return pulsPerRotatiePerMinuut / totaal;  //  return totaal
}








void setRpm(double inter){
  for(byte i = 0; i < sampleLengte; i++){
    samples[i] = inter;
  }
}










void draaien(){
  if( targetRpm != 0 ){return;} // is de motor al aan het draaien? negeer dan 
  
  rpmIs33 = digitalRead(knopRpm);
  
  targetRpm = rpmIs33 ? rpm33 : rpm33 * naar45;
  basis = 0;//basis33 * ((in / targetRpm));
  
  draaienStart = millis();
  Serial.println("D");
  if(test){Serial.print("target: "); Serial.println(targetRpm);}
}









void stoppen(){
  targetRpm = 0;
  draaienStart = millis();
  uitdraaien = true;
  opsnelheid = false;
  Serial.println('S');
  //dac.setVoltage(0, false);
}










float bereken(float rpm){
  
  float uit = (targetRpm - rpm) * P;
  
  
  if(uit > 800){uit = 800;}                     //motor voltage binnen bereik houden
  if(uit < -basis){uit = -basis;}
  
  basis += uit * I;            //breng basis voltage naar gemiddelde voor de juiste snelheid
  
  
  return uit;
}
