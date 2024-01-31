//  Wheel2 
//  rp2040

//board instaleren:
//https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json

#define versie 170
  

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"

#include "pico/time.h"




#include "pins.h"
#include "helper.h"

#include "pwm.h"

#include "interval.h"


#include "staat.h"

#include "armMotor.h"
ArmMotor arm;





void enableInterupts(bool aan){
	pinMode(plateauA,     INPUT_PULLUP);
	pinMode(plateauB,     INPUT_PULLUP);
	pinMode(plateauEN,    OUTPUT);

	gpio_set_irq_enabled_with_callback(plateauA,   GPIO_IRQ_EDGE_RISE + GPIO_IRQ_EDGE_FALL,  aan,   &gpio_callback);
	gpio_set_irq_enabled_with_callback(plateauB,   GPIO_IRQ_EDGE_RISE + GPIO_IRQ_EDGE_FALL,  aan,   &gpio_callback);
}




#include "compVaartSensor.h"
COMPVAART strobo(16, 720);





#include "plateau.h"

Interval ledInt(200, MILLIS);



#include "kar.h"

#include "versterker.h"

#include "plaatLees.h"




#include "knoppen.h"

#include "opslag.h"

#include "display.h"

#include "serieel.h"
 



void setup() {
	analogReadResolution(12);// moet sinds nieuwe core versie, anders leest hij in 10bit

	Serial.begin();  

  Wire1.setSCL(SCL);
	Wire1.setSDA(SDA);

  

	opslagInit();
	eepromUitlezen();


	versterkerInit();

	

	arm.armInit();

	karInit();

	plaatLeesInit();

	plateauInit();


	
	pinMode(slaapStand, OUTPUT);
	digitalWrite(slaapStand, 1); // hou de batterij aan


	enableInterupts(true);//zet interrupts aan


	pinMode(ledWit, OUTPUT);
	// digitalWrite(ledWit, 1);//zet led aan



}



void setup1(){
  displayInit();
}

void loop1(){
	displayUpdate();
	serieelFunc();
	knoppenUpdate();
	arm.armFunc();  
}






void loop() {

	if(eepromShit){//dit moet omdat je core2 moet uitschakelen om in flash te schrijven, zodat je niet leest en schrijft tegelijkertijd
		// delay(20);
		eepCommit();
		Serial.println("opgeslagen!");
	}


	plaatLeesFunc();

	karMotorFunc();

	volumeFunc();

  orientatie.update();

	plateauFunc();

  bluetoothFunc();

  digitalWrite(ledWit, nieuweMillis() < 1000);//zet led aan

}







void gpio_callback(uint gpio, uint32_t events) {
	strobo.interrupt();
}

