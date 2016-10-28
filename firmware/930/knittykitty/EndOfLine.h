/**
 * EndOfLine.h
 * 
 * Provides a class that senses the values for the line sensors.
 * Also provides an interface to calibrate and store filter values.
 */
#include <EEPROM.h>

#ifndef __KNITTY_KITTY_EOL__
#define __KNITTY_KITTY_EOL__

#include "Settings.h"
#include "KnittyKitty.h"

class EndOfLine {

public:
    EndOfLine() : m_bCalibrate(false), 
                  m_iLeftMin(FILTER_VALUE_LEFT_MIN), 
                  m_iLeftMax(FILTER_VALUE_LEFT_MAX), 
                  m_iRightMin(FILTER_VALUE_RIGHT_MIN), 
                  m_iRightMax(FILTER_VALUE_RIGHT_MAX) {
    }
    ~EndOfLine() { }

    uint16_t readLeft() {
        return analogRead(ENDLINE_L);
    }

    uint16_t readRight() {
        return analogRead(ENDLINE_R);
    }

    uint16_t getLeftMin() {
        return m_iLeftMin + 1;
    }
    uint16_t getLeftMax() {
        return m_iLeftMax - 1;
    }
    uint16_t getRightMin() {
        return m_iRightMin + 1;
    }
    uint16_t getRightMax() {
        return m_iRightMax - 1;
    }

    void calibrateUpdate() {
        uint16_t left = readLeft();
        uint16_t right = readRight();

        if(left < m_iLeftMin) m_iLeftMin = left;
        if(left > m_iLeftMax) m_iLeftMax= left;
        if(right < m_iRightMin) m_iRightMin = right;
        if(right > m_iRightMax) m_iRightMax = right;
    }

private:
    uint16_t m_iLeftMin, m_iLeftMax, m_iRightMin, m_iRightMax;
    bool m_bCalibrate;
};

#endif //__KNITTY_KITTY_EOL__

