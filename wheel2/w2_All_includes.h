//  =====================================================
//  All the include files and needed classes for wheel 2
//  =====================================================
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "pins.h"
#include "helper.h"
#include "rpm.h"
WheelRpm rpm;
#include "pwm.h"
//#include "rpm.h"  // rpm class + ??
#include "interval.h"
#include "staat.h"
#include "errors.h"
#include "armMotor.h"
ArmMotor arm;
#include "compVaartSensor.h"
COMPVAART strobo(16, 720);
#include "plateau.h"
Interval ledInt(200, MILLIS);
#include "w2_i2c.h"
#include "orientatie.h"
Orientatie orientatie;
#include "versterker.h"
#include "plaatLees.h"
PlaatLees lees;
#include "kar.h"
#include "knoppen.h"
#include "display.h"
#include "opslag.h"
#include "serieel.h"