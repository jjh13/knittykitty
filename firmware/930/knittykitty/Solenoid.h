/**
 * Solenoid.h
 * 
 * Abstracts the interface to the solenoid
 * array.
 */

#ifndef __KNITTY_KITTY_SOLENOID__
#define __KNITTY_KITTY_SOLENOID__

#include "Settings.h"

#include "KnittyKitty.h"

class SolenoidArray {

public:
    SolenoidArray() {
        for(unsigned int i = 0; i < NUM_SOLENOIDS; i++) {
            pinArrayState[i] = false;
            pinMode(solenoidPinMapping[i], OUTPUT);
            digitalWrite(solenoidPinMapping[i], LOW);
            
        }
    }
    ~SolenoidArray() { }

    /**
     * 
     */
    void setState(uint8_t solenoid, bool state) {
        if(solenoid < NUM_SOLENOIDS) {
            pinArrayState[solenoid] = state;
        }
    }

    /**
     * 
     */
    bool getState(uint8_t solenoid) {
        if(solenoid > NUM_SOLENOIDS) return false;
        return pinArrayState[solenoid];
    }

    /**
     * 
     */
    void clearSolenoids() {
        for(int i = 0; i < NUM_SOLENOIDS; i++) {
            pinArrayState[i] = false;
        }
    }
    
    /**
     * 
     */
    void writeSolenoids() {
      Serial.print("writing solenoids\n");
        for(int i = 0; i < NUM_SOLENOIDS; i++) {
            
            digitalWrite(solenoidPinMapping[i], (uint8_t)!pinArrayState[i]);
            
        }
    }

private:
    // True is active (so a 0), false is inactive (1).
    boolean pinArrayState[NUM_SOLENOIDS];

    // This is the same pin mapping as the knitic (move this into settings)
    const int solenoidPinMapping[NUM_SOLENOIDS] = {
 // 22,24,26,28,30,32,34,36,37,35,33,31,29,27,25,23};
      SOL00_PIN, SOL01_PIN, SOL02_PIN, SOL03_PIN, SOL04_PIN, SOL05_PIN, SOL06_PIN, SOL07_PIN,
      SOL08_PIN, SOL09_PIN, SOL10_PIN, SOL11_PIN, SOL12_PIN, SOL13_PIN, SOL14_PIN, SOL15_PIN
    };
};

#endif //__KNITTY_KITTY_SOLENOID__

