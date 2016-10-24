/**
 * Settings.h
 * 
 * 
 */

#ifndef __KNITTY_KITTY_SETTINGS__
#define __KNITTY_KITTY_SETTINGS__

// Versioning 
#define KNITTY_KITTY_VERSION_MAJ 0
#define KNITTY_KITTY_VERSION_MIN 1

// Machine prefs
#define MACHINE       "930"
#define MACHINE_NAME  "Brother 930e"

#define NUM_SOLENOIDS   16
#define NEEDLEBED_COUNT 200

// Pin mappings
#define ENCODER_A 2  // This needs to map to an interruptable pin!
#define ENCODER_B 3
#define ENCODER_C 4

#define ENDLINE_L A1
#define ENDLINE_R A0

#define SOL00_PIN 22
#define SOL01_PIN 24
#define SOL02_PIN 26
#define SOL03_PIN 28
#define SOL04_PIN 30
#define SOL05_PIN 32
#define SOL06_PIN 34
#define SOL07_PIN 36
#define SOL08_PIN 37
#define SOL09_PIN 35
#define SOL10_PIN 33
#define SOL11_PIN 31
#define SOL12_PIN 29
#define SOL13_PIN 27
#define SOL14_PIN 25
#define SOL15_PIN 23

// Default filter values
#define FILTER_VALUE_LEFT_MIN     32
#define FILTER_VALUE_RIGHT_MIN    32
#define FILTER_VALUE_LEFT_MAX     700
#define FILTER_VALUE_RIGHT_MAX    700



// Tuning parameters
#define LOOKAHEAD 4

#endif //__KNITTY_KITTY_SETTINGS__
