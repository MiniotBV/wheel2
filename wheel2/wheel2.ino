/**
  Wheel2 Firmware
  Name: wheel2

  Author: Piet Kolkman (MiniotBv), refactored by Eduard Kuijt (EddyK69
  Many thanks to Piet, Peter & Greet from Miniot!

  Based on offical Miniot Wheel2 Firmware:
    https://drive.google.com/drive/folders/1BKKGxrlx6HUjvCHgJyVHJfkWcCUIp_lk
    https://pietk.com/wheel2/

  Install board (Raspberry Pi Pico, rp2040):
    https://arduino-pico.readthedocs.io/en/latest/install.html
    https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json

  Arduine Code Guidelines:
    https://sites.google.com/a/cabrillo.edu/cs-11m/howtos/cppdoc

  DebugLog Library:
    https://github.com/hideakitai/DebugLog

  Multi Core Processing:
    https://arduino-pico.readthedocs.io/en/latest/multicore.html
      "setup() and setup1() will be called at the same time, and the loop() or loop1() will 
      be started as soon as the core’s setup() completes (i.e. not necessarily simultaneously!)."
    https://arduino-pico.readthedocs.io/en/latest/sdk.html#multicore-core1-processing
      "Warning: While you may spawn multicore applications on CORE1 using the SDK,
      the Arduino core may have issues running properly with them.
      In particular, anything involving flash writes (i.e. EEPROM, filesystems) will
      probably crash due to CORE1 attempting to read from flash while CORE0 is writing to it."
    https://github.com/earlephilhower/arduino-pico/discussions/1479#discussioncomment-6043482
      "As a user you can run your own setup1/loop1 and never ever call rp2040.idle/resumeOtherCore
      and write to flash via EEPROM or LittleFS. The Arduino core plumbing here does all that for you"

  millis() overflow rollover (50 days!!)
    https://www.best-microcontroller-projects.com/arduino-millis.html
    https://www.norwegiancreations.com/2018/10/arduino-tutorial-avoiding-the-overflow-issue-when-using-millis-and-micros/
    https://techexplorations.com/guides/arduino/programming/millis-rollover/
*/

#define APP_VERSION 203

#include "log.h"
#include "pico/time.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "pins.h"
#include "wheel.h"


Wheel wheel(APP_VERSION);


// The normal, core 0 setup
void setup() {
  // Sets the size (in bits) of the value returned by analogRead(). It defaults to 10 bits.
  analogReadResolution(12);

  // Initialize Wheel
  wheel.init();

  enableInterupts(true);
} // setup()


// core 0 loop
void loop() {
  wheel.scanner.func();
  wheel.carriage.func();
  wheel.amplifier.func();
  wheel.orientation.update();
  wheel.plateau.func();

  wheel.bluetooth.func();

  wheel.display.bootLED(); // turn LED on
} // loop()


// Running on core 1
void setup1() {
  wheel.display.init();
} // setup1()


// core 1 loop
void loop1() {
  wheel.display.update();
  wheel.serialcomm.func();
  wheel.buttons.update();
  wheel.arm.func();
} // loop1()


void enableInterupts(bool enabled) {
  LOG_DEBUG("wheel2.ino", "enableInterupts");
  gpio_set_irq_enabled_with_callback(PLATEAU_A_PIN, GPIO_IRQ_EDGE_RISE + GPIO_IRQ_EDGE_FALL, enabled, &gpioCallback);
  gpio_set_irq_enabled_with_callback(PLATEAU_B_PIN, GPIO_IRQ_EDGE_RISE + GPIO_IRQ_EDGE_FALL, enabled, &gpioCallback);
} // enableInterupts


void gpioCallback(uint gpio, uint32_t events) {
  wheel.speedcomp.stroboInterrupt();
} // gpioCallback
