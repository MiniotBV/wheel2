//motor heeft 48 stappen
//tandwiel heeft 12 tanden
//tandheugel heeft 1.5mm tand pitch 
//48 stappen / 12 tanden = 4 stappen per tand
//1.5mm / 4 stappen per tand = 0.375mm per stap

//PI = 2 stappen

float mmPerStap = 1.5 / ( 48 / 12 );
float karPositieVerhouding = ( 2 / PI ) * mmPerStap;

float karP = 0.001;

int stapperFaseA = 0;
int stapperFaseB = 0;

bool karMotorEnable = true;
float karMotorPositie = 0;
float karPositie = 0;


unsigned long karMotorLoop = 0;
unsigned long armHoekLoop = 0;

float armHoek = analogRead(hoekSensor);
float armHoekFilter = 2;


void karMotorFunc(){
  if(micros() - armHoekLoop > 500){
    armHoekLoop = micros();

    // armHoek = analogRead(hoekSensor);
    armHoek += (analogRead(hoekSensor) - armHoek) / armHoekFilter;

  }



    
      
  if(micros() - karMotorLoop > 1000){
    karMotorLoop = micros();

    // karMotorPositie += 0.001;

    // karMotorPositie += 0.01 * sin(micros()/3000000.0);

    karMotorPositie += limieteerF( -karP * ( armHoek - 1696 ) , -0.3, 0.3);

    karPositie = karMotorPositie * karPositieVerhouding;

    stapperFaseA = sin(karMotorPositie) * (PMAX-1);
    stapperFaseB = cos(karMotorPositie) * (PMAX-1);

    if(karMotorEnable){
      
      if(stapperFaseA > 0){
        pwmWrite(stapperAP,  (PMAX-1) - stapperFaseA );
        pwmWrite(stapperAN,  (PMAX-1));
      }else{
        pwmWrite(stapperAP,  (PMAX-1));
        pwmWrite(stapperAN,  (PMAX-1) + stapperFaseA );
      }

      if(stapperFaseB > 0){
        pwmWrite(stapperBP,  (PMAX-1) - stapperFaseB );
        pwmWrite(stapperBN,  (PMAX-1));
      }else{
        pwmWrite(stapperBP,  (PMAX-1));
        pwmWrite(stapperBN,  (PMAX-1) + stapperFaseB );
      }
    
    }else{
      pwmWrite(stapperAP,  0 );
      pwmWrite(stapperAN,  0 );
      
      pwmWrite(stapperBP,  0 );
      pwmWrite(stapperBN,  0 );
    }

  }
}



