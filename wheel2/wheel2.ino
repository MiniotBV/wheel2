//  Wheel2 
//  rp2040

//https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json

#define versie 146
  

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
	// pinMode(plateauIndex, INPUT_PULLUP);

	gpio_set_irq_enabled_with_callback(plateauA,   GPIO_IRQ_EDGE_RISE + GPIO_IRQ_EDGE_FALL,  aan,   &gpio_callback);
	gpio_set_irq_enabled_with_callback(plateauB,   GPIO_IRQ_EDGE_RISE + GPIO_IRQ_EDGE_FALL,  aan,   &gpio_callback);
}




#include "compVaartSensor.h"
COMPVAART strobo(16, 720);





#include "plateau.h"

Interval ledInt(200, MILLIS);




#include "versterker.h"

#include "kar.h"

#include "plaatLees.h"




#include "knoppen.h"

#include "display.h"




#include "opslag.h"

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


	
	multicore_launch_core1(loop2);
	
	pinMode(slaapStand, OUTPUT);
	digitalWrite(slaapStand, 1); // hou de batterij aan


	enableInterupts(true);//zet interrupts aan


	pinMode(ledWit, OUTPUT);
	// digitalWrite(ledWit, 1);//zet led aan



}








void core1Dingen(){
	displayUpdate();
	
	serieelFunc();

	knoppenUpdate();

	arm.armFunc();  
}




void loop2(){

  displayInit();

	while(1){
		core1Dingen();

		if(eepromShit){//sckakel core2 tijdelijk uit om te zorge dat er vijlig flash beschreven kan worde
			sleep_ms(100);
		}
	}
}






void loop() {

	if(eepromShit){//dit moet omdat je core2 moet uitschakelen om in flash te schrijven, zodat je niet leest en schrijft tegelijkertijd
		delay(20);
		eepCommit();
		Serial.println("opgeslagen!");
	}


	plaatLeesFunc();

	karMotorFunc();

	volumeFunc();

  orientatie.update();

	plateauFunc();

  bluetoothFunc();


	// digitalWrite(ledWit, isNaaldEropVoorZoLang(1000));
	// digitalWrite(ledWit, isPlaatAanwezig());
}







void gpio_callback(uint gpio, uint32_t events) {
	strobo.interrupt();
}

