#include "log.h"
#include "wheel.h"
#include "pins.h"

Wheel::Wheel(float version) :
    shared(version),
    arm(shared),
    amplifier(shared, arm),
    orientation(shared, arm),
    speedcomp(shared, arm), // cart & plateau are passed via init()
    plateau(shared, speedcomp),
    scanner(shared, plateau), // cart is passed via init()
    cart(shared, arm, plateau, scanner), // speedcomp is passed via init()
    buttons(shared, amplifier, arm, bluetooth, cart, orientation, plateau, scanner),
    storage(shared, arm, cart, orientation),
    display(shared, amplifier, arm, buttons, cart, orientation, plateau, scanner, speedcomp, storage),
    serialcomm(shared, amplifier, arm, bluetooth, buttons, cart, orientation, plateau, scanner, speedcomp, storage),
    bluetooth(shared, cart, plateau) {
} // Wheel()

void Wheel::init() {
  LOG_DEBUG("wheel.cpp", "[init]");
  serialcomm.init();

  storage.init();

  amplifier.init();
  orientation.init();
  arm.init();
  buttons.init();
  cart.init(&speedcomp); // to prevent circular reference
  scanner.init(&cart); // to prevent circular reference
  plateau.init();
  speedcomp.init(&cart, &plateau); // to prevent circular reference

  storage.read();

  // Set pinmodes
  pinMode(SLEEPMODE_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(SLEEPMODE_PIN, 1); // keep battery on
  // digitalWrite(LED_PIN, 1); // turn LED on
} // init()
