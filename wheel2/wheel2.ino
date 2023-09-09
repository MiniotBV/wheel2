//  =============================================
//  Wheel2 RP2040 moederbord. year: 2022
//  - use board: Raspberry Pi Pico/RP2040 by 
//    Earle F. Philhower. Version 2.2 or higher
//  - use USB stack: Pico SDK
//  =============================================
#define versie 84
float trackSensorPos;
#include "w2_All_includes.h"  //  All the include files and needed classes

void enableInterupts(bool aan)
{
	gpio_set_irq_enabled_with_callback(plateauA, GPIO_IRQ_EDGE_RISE + GPIO_IRQ_EDGE_FALL, aan, &gpio_callback);
	gpio_set_irq_enabled_with_callback(plateauB, GPIO_IRQ_EDGE_RISE + GPIO_IRQ_EDGE_FALL, aan, &gpio_callback);
}

void setup()
{
	analogReadResolution(12); // force to 12-bit
	Serial.begin(115200);
	opslagInit();
	eepromUitlezen();
	versterkerInit();
	displayInit();  
	arm.armInit();
	karInit();
	lees.init();
	plateauInit();

	multicore_launch_core1(loop2);
	
	pinMode(slaapStand, OUTPUT);
	digitalWrite(slaapStand, 1); // hou de batterij aan

	enableInterupts(true);//zet interrupts aan

	pinMode(ledWit, OUTPUT);
	digitalWrite(ledWit, 1);//zet led aan

	berekenAntiCogging();
}


void core1Dingen()
{
	displayUpdate();
	serieelFunc();
	knoppenUpdate();
	arm.armFunc();  
}


void loop2()
{
	while(1)
  {
		core1Dingen();
    // core2 must sleep to write to flash
		if(eepromShit) { sleep_ms(100); }
	}
}


void loop() 
{
  

	if(eepromShit)  // avoid flash read/write conflicts
  {
		delay(20);
		eepCommit();
		Serial.println("opgeslagen!");
	}

	lees.loopFunc();
	karMotorFunc();
	volumeFunc();
	plateauFunc();

	// digitalWrite(ledWit, isNaaldEropVoorZoLang(1000));
	// digitalWrite(ledWit, isPlaatAanwezig());
}

//  =============================================
//      STROBO INTERRUPT
//  =============================================
void gpio_callback(uint gpio, uint32_t events) { strobo.interrupt(); }

