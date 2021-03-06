/**
 * Settings.h
 * 
 * 
 */

#ifndef __KNITTY_KITTY_SETTINGS__
#define __KNITTY_KITTY_SETTINGS__

// Versioning 
#define KNITTY_KITTY_VERSION_MAJ 0
#define KNITTY_KITTY_VERSION_MIN 2

// Machine prefs
#define MACHINE       "930"
#define MACHINE_NAME  "Brother 930e"

#define NUM_SOLENOIDS   16
#define NEEDLEBED_COUNT 200

#define N_BUFFERS 2

// Pin mappings
#define ENCODER_A 2  // This needs to map to an interruptable pin!
#define ENCODER_B 3
#define ENCODER_C 4

#define ENDLINE_L A1
#define ENDLINE_R A0

#define SOL00_PIN  52
#define SOL01_PIN  50
#define SOL02_PIN  48
#define SOL03_PIN  46
#define SOL04_PIN  44
#define SOL05_PIN  42
#define SOL06_PIN  40
#define SOL07_PIN  38
#define SOL08_PIN  22
#define SOL09_PIN  24
#define SOL10_PIN  26
#define SOL11_PIN  28
#define SOL12_PIN  30
#define SOL13_PIN  32
#define SOL14_PIN  34
#define SOL15_PIN  36

// Default filter values
#define FILTER_VALUE_LEFT_MIN     32
#define FILTER_VALUE_RIGHT_MIN    32
#define FILTER_VALUE_LEFT_MAX     730
#define FILTER_VALUE_RIGHT_MAX    730


#endif //__KNITTY_KITTY_SETTINGS__
