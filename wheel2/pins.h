#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

#define BOARD_WHEEL // Select wich board to use: BOARD_PICO, BOARD_WHEEL


////////// Pico Board inside Wheel 2 /////////////
#if defined(BOARD_WHEEL)
#define BOARD_DESCRIPTION "Wheel 2 rp2040"

//------------------- Power
#define SLEEPMODE_PIN               0

//------------------- Display
#define DISPLAY_IN_PIN              9
#define DISPLAY_OUT_PIN             10
#define DISPLAY_CLOCK_PIN           11
#define DISPLAY_LATCH_PIN           12
#define DISPLAY_EN_PIN              13 //~ PWM6 B
#define DISPLAY_POTMETER_PIN        28 // ADC2

//------------------- Carriage steppers
#define CARRIAGE_STEPPER_AN_PIN     2 //~ PWM1 A
#define CARRIAGE_STEPPER_AP_PIN     3 //~ PWM1 B
#define CARRIAGE_STEPPER_BN_PIN     4 //~ PWM2 A
#define CARRIAGE_STEPPER_BP_PIN     5 //~ PWM2 B

//------------------- Plateau encoder
#define PLATEAU_A_PIN               8
#define PLATEAU_B_PIN               7
#define PLATEAU_EN_PIN              6

//------------------- Plateau motor
#define PLATEAU_MOTOR_P_PIN         15 //~ PWM7 B
#define PLATEAU_MOTOR_N_PIN         14 //~ PWM7 A

//------------------- Arm
#define ARM_MOTOR_PIN               16 //~ PWM0 A
#define SCANNER_LED_PIN             17
#define ARM_ANGLE_SENSOR_PIN        26 // ADC0
#define SCANNER_PIN                 27 // ADC1

//------------------- Leds
#define LED_PIN                     19 //~ PWM1 B -> same as 'stepperAP'

//------------------- I2C
#define I2C_SDA_PIN                 22
#define I2C_SCL_PIN                 23

//------------------- Bluetooth
#define BT_TXD_PIN                  20 // UART1 TX
#define BT_RXD_PIN                  21 // UART1 RX

#define AMP_HEADSET_CONNECTED_PIN   25
#define AMP_HEADSET_EN_PIN          24

#endif // BOARD_WHEEL
//////////////////////////////////////////////////









////////// Real Raspberry Pi Pico Board //////////
#if defined(BOARD_PICO)
#define BOARD_DESCRIPTION "Raspberry Pi Pico"

//------------------- Power
#define SLEEPMODE_PIN               0

//------------------- Display
#define DISPLAY_IN_PIN              9
#define DISPLAY_OUT_PIN             10
#define DISPLAY_CLOCK_PIN           11
#define DISPLAY_LATCH_PIN           12
#define DISPLAY_EN_PIN              13 //~ PWM6 B
#define DISPLAY_POTMETER_PIN        28 // ADC2

//------------------- Carriage steppers
#define CARRIAGE_STEPPER_AN_PIN     2 //~ PWM1 A
#define CARRIAGE_STEPPER_AP_PIN     3 //~ PWM1 B
#define CARRIAGE_STEPPER_BN_PIN     4 //~ PWM2 A
#define CARRIAGE_STEPPER_BP_PIN     5 //~ PWM2 B

//------------------- Plateau encoder
#define PLATEAU_A_PIN               8
#define PLATEAU_B_PIN               7
#define PLATEAU_EN_PIN              6

//------------------- Plateau motor
#define PLATEAU_MOTOR_P_PIN         15 //~ PWM7 B
#define PLATEAU_MOTOR_N_PIN         14 //~ PWM7 A

//------------------- Arm
#define ARM_MOTOR_PIN               16 //~ PWM0 A
#define SCANNER_LED_PIN             17
#define ARM_ANGLE_SENSOR_PIN        26 // ADC0
#define SCANNER_PIN                 27 // ADC1

//------------------- Leds
#define LED_PIN                     25

//------------------- I2C
#define I2C_SDA_PIN                 2
#define I2C_SCL_PIN                 3

//------------------- Bluetooth
#define BT_TXD_PIN                  20 // UART1 TX
#define BT_RXD_PIN                  21 // UART1 RX

#define AMP_HEADSET_CONNECTED_PIN   25
#define AMP_HEADSET_EN_PIN          24

#endif // BOARD_PICO
//////////////////////////////////////////////////




#endif // PINS_H
