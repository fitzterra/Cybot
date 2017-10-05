/**
 * Application compile time config settings.
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>
#include "sharedSpace.h"

// Define this to enable debugging
#define _DEBUG

// The speed for the serial port for debug output and also for the serial input
// handler if enabled.
#define SERIAL_SPEED 115200
// If using a Bluetooth input or output, these are the TX and RX pins for the
// Software Serial connection to the BT module and also the speed.
#define SOFTSERIAL_RX 12
#define SOFTSERIAL_TX 11
#define SOFTSERIAL_SPEED 9600
// If SOFTSERIAL_EN is defined, the config mechanism will make a globally
// accessible software serial port available called SSerial which will behave
// the same as the standard Serial port.
// If enabling Bluetooth input below (INP_BT_EN), SOFTSERIAL_EN would
// automatically be defined since the BT input mechanism depends on SSerial.
// Define this to create the SSerial port - see also OpenSoftSerial in utils.
//#define SOFTSERIAL_EN

// When in learn mode, how long to wait for input before timing out and exiting
// learn mode without saving the learned input. The value is in milliseconds
#define LEARNMODE_TIMEOUT 10*1000;

// Define this if you want to get feedback on current speed and direction over
// the Bluetooh software serial port.
#define BT_FEEDBACK


// @@@@@@@@@@@@ Feature selection definitions @@@@@@@@@@@@@@@
//
// Comment the #defines out to disable the feature, leave it uncommented to
// enable the feature. Each feature will have it's own config section below, so
// remember to also configure the feature there if it has been enabled.
//
// ~~~~~~ Control input options ~~~~~~~~
// These can all be available as long as the hardware does not conflict
// NOTE! Any changes here will mean the EEPROM signature needs to be changed -
//       see commandMaps.eepromSig in commands.cpp
#define INP_SERIAL_EN   // Enables serial input control
#define INP_IR_EN       // Enables Infrared input control
#define INP_BT_EN       // Enables Bluetooth Serial input Control
//
// ~~~~~~ Wheel driver options ~~~~~~~
// Continues rotation servos or HBridge (Left/Right Forward/Reverse) wheel
// control are supported. Only enable one of the two options, then configure the
// selection option below.
//
//#define SERVO_DRV_EN  // Needs one digital pin for each wheel
#define HBRIDGE_DRV_EN  // Needs 2 digital pins per wheel
// ~~~~~~ Sensor options ~~~~~~~
//#define LINEFOL_EN      // Enables the line follower feature
//#define BUMPERS_EN      // Enables bumper switches
// ~~~~~~ Display options ~~~~~~~
//#define LCD_EN           // Enable Nokia mono LCD (PDC8544)
#define OLED_EN          // Enable OLED display

// Auto define DISPLAY_EN if eithe LCD or OLED is enable
#if defined(LCD_EN) || defined(OLED_EN)
    #define DISPLAY_EN
#endif
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


// Sanity check at compile time to ensure only one of servo or hbridge control
// is enabled
#if defined(SERVO_DRV_EN) && defined(HBRIDGE_DRV_EN)
#error "You should only define SERVO_DRV_EN or HBRIDGE_DRV_EN, not both!"
#endif
// And that at least on input method has been defined
#if !(defined(INP_SERIAL_EN) || defined(INP_IR_EN) || defined(INP_BT_EN))
#error "You have not defined any input method. At least one of INP_???_EN defines are needed."
#endif

// We do not support LCD yet, so error out if enabled
#ifdef LCD_EN
#error "The LCD display is not supported yet. Only OLED for now."
#endif

// ############### General utility definitions #################
// NOTE!!! DO NOT change the values of these defs - LEFT and RIGHT are used as
// array indexes and should always be values 0 and 1
enum { LEFT=0, RIGHT=1, FORWARD=2, BACKWARD=3, STOP=4 };

// ############### Serial Input config #################
#ifdef INP_SERIAL_EN
// This is the minimum delay required between successive serial input characters.
// If successive input is received within this delay period, the input will be
// read from the serial line, but it will be discarded. Note that keystrokes that
// deliver multiple characters like CRLF for Enter will only ever show the first
// character.
// The reason for this delay is to prevent overenthusiastic pressing of keys to
// send the same command to a robot. Many robot commands takes a short while
// before the results can be seen.
#define SI_MIN_DELAY 100
// Some commands will behave differently depending on whether the command is being
// repeated or just sent in quick succession. This value determines the max time
// interval between successive receipt of the same character which would count as
// a repeat of this input.
#define SI_REPEAT_MAX 250
#endif // INP_SERIAL_EN

// ############### IR Input config #################
// ??? Uses the IRremote library ????
#ifdef INP_IR_EN
// IR Receiver pin
#define IR_PIN  2
// As for SI_MIN_DELAY, but only for IR
#define IR_MIN_DELAY 100
// As for SI_REPEAT_MAX, but only for IR
#define IR_REPEAT_MAX 250
#endif // INP_IR_EN

// ############### BT Input config #################
// Uses the SSerial port (see SOFTSERIAL_EN above) with an HC-06 BT module for
// simple serial comms.
// Note that the software serial RX and TX pins are defined above with the
// SOFTSERIAL_RX and SOFTSERIAL_TX defines.
#ifdef INP_BT_EN
// Make sure we set up the SSerial port.
#define SOFTSERIAL_EN
// As for SI_MIN_DELAY, but only for BT
#define BT_MIN_DELAY 100
// As for SI_REPEAT_MAX, but only for BT
#define BT_REPEAT_MAX 250
#endif // INP_BT_EN

// ########## Servo Wheel Drivers Config ###########
#ifdef SERVO_DRV_EN 
#define SERVO_LEFT      5   // Digital Pin for left servo
#define SERVO_RIGHT     6   // Digital Pin for right servo
#endif  // SERVO_DRV_EN 

// ######### HBridge Wheel Drivers Config ##########
#ifdef HBRIDGE_DRV_EN 
// To use software PWM for the HBridge speed control, define this below. Using
// software PWM allows any digital pin to be used for the drive control. If not
// using software PWN, then only pins that have hardware PWM support can be
// used.
//#define HBRIDGE_SOFT_PWM
#define HBRIDGE_LEFT_FWD  5  // PWM pin for left wheel forward
#define HBRIDGE_LEFT_REV  3  // PWM pin for left wheel reverse
#define HBRIDGE_RIGHT_FWD 9  // PWM pin for right wheel forward
#define HBRIDGE_RIGHT_REV 6  // PWM pin for right wheel reverse
#define POWER_START_TIME  800 // Do a power start over this many millis
#define POWER_START_TAPER 100 // For power start, taper to target speed at this rate.
#endif  // HBRIDGE_DRV_EN 

// ############# Line Follower Config ##############
#ifdef LINEFOL_EN
#define LINEFOL_LEFT    4   // Line Follower Left Sensor Analog pin
#define LINEFOL_RIGHT   5   // Line Follower Right Sensor Analog pin
#define LINEFOL_MIN     400  // Min 'black' reading. Lower indicates off line
#define LINEFOL_MAX     1000  // Max 'black' reading. Higher means error.
#endif  //LINEFOL_EN

// ################ Bumpers Config #################
#ifdef BUMPERS_EN
#define BUMP_FL_PIN     2   // Front left bumper digital pin
#define BUMP_FR_PIN     4   // Front right bumper digital pin
#define BUMP_RL_PIN     2   // Rear left bumper digital pin
#define BUMP_RR_PIN     4   // Rear right bumper digital pin
#define BUMPED          0   // The pin state when the sensor IS bumped
// Bit positions for bumper bits in a bumper bit storage int
enum bumberBits_t {BUMP_FL, BUMP_FR, BUMP_RL, BUMP_RR}
// A digital output pin that will be set high if any bumper is active.
// Can be used to light an LED or something.
#define BUMP_LED_PIN    13
#endif  // BUMPERS_EN

// ########## PDC8544 (Nokia) LCD Config ###########
#ifdef LCD_EN
// These are hard-coded in the LCD lib, but we define them here as a reminder
// for comparison with other pins in use.
#define LCD_DC          8 
#define LCD_RESET       9 
#define LCD_SCE         10 
#define LCD_SDIN        11 
#define LCD_SCLK        13 
#define LCD_RATE        100  // LCD update rate in milliseconds
#endif  //LCD_EN

// ################# OLED Config ##################
#ifdef OLED_EN
// The OLED display supported here is a 128x64 display using the SSD1306 driver
// chip over I2C. The smallest library currently known that supports this
// display is the one from ACROBORIC:
// https://github.com/acrobotic/Ai_Ardulib_SSD1306
// It can be installed via the Arduino Library Manager.
#include <ACROBOTIC_SSD1306.h>

// Display update frequency in milliseconds
#define DISP_UPD_FREQ 500

#endif  // OLED_EN

// #########################################

// Make SSerial available if required. The actual instantiation of SSerial is
// done in config.cpp, but the opening of the port should still be done via
// SSerial.begin() or the OpenSoftSerial() helper in utils.
#ifdef SOFTSERIAL_EN
#include <SoftwareSerial.h>
extern SoftwareSerial SSerial;
#endif //SOFTSERIAL_EN

#endif  //_CONFIG_H_
