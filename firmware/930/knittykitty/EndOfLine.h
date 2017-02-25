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
                  m_iRightMax(FILTER_VALUE_RIGHT_MAX),
                  m_iLeftCache(0), m_iRightCache(0) {
    }
    ~EndOfLine() { }

    void update() {
      m_iLeftCache = analogRead(ENDLINE_L);
      m_iRightCache = analogRead(ENDLINE_R);
    }
    uint16_t readLeft() {
        return m_iLeftCache();
    }

    uint16_t readRight() {
        return m_iRightCache();
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
        this->update();
        uint16_t left = readLeft();
        uint16_t right = readRight();

        if(left < m_iLeftMin) m_iLeftMin = left;
        if(left > m_iLeftMax) m_iLeftMax= left;
        if(right < m_iRightMin) m_iRightMin = right;
        if(right > m_iRightMax) m_iRightMax = right;
    }

private:
    uint16_t m_iLeftMin, m_iLeftMax, m_iRightMin, m_iRightMax;
    uint16_t m_iLeftCache, m_iRightCache;
    bool m_bCalibrate;
};

#endif //__KNITTY_KITTY_EOL__

