/**
 * Solenoid.h
 *
 * Abstracts the interface to the solenoid
 * array.
 */

#ifndef __KNITTY_KITTY_ENCODER__
#define __KNITTY_KITTY_ENCODER__

#include "Settings.h"
#include "KnittyKitty.h"
#include "EndOfLine.h"


class Encoders {

public:
    Encoders(EndOfLine *eol_sensors) : m_direction(UNKNOWN_DIRECTION),
                                       m_carriage(UNKNOWN_CARRIAGE),
                                       m_eol(eol_sensors) {
        pinMode(ENCODER_A, INPUT);
        pinMode(ENCODER_B, INPUT);
        pinMode(ENCODER_C, INPUT);
    }
    ~Encoders() { }

    /*
     *
     */
    carriage_direction_t getCarriageDirection() {
        return m_direction;
    }

    /*
     *
     */
    carriage_t getCarriageType() {
        return m_carriage;
    }

    /*
     *
     */
    beltshift_t getBeltShift() {
        return m_beltshift;
    }

    /*
     *
     */
    int getCarriagePosition() {
        return m_position;
    }

    // This should be called on the transition of
    // encoder A
    /*
     *
     */
    void updateState() {
        static bool previousState = false;
        bool state = digitalRead(ENCODER_A);

        // This shouldn't happen, but on no
        // change for the encoder, bail.
        if(state == previousState)
            return;

        // High to low transition
        if(state == false && previousState == true) {
            encoderAFalling();
        } else { // Low to high
            encoderARising();
        }
        previousState = state;
    }

private:
    carriage_direction_t m_direction;
    carriage_t m_carriage;
    beltshift_t m_beltshift;

    EndOfLine *m_eol;
    int m_position;

    void encoderAFalling() { // handle moving left
        uint16_t eol_value = 0;

        if(m_direction == CARRIAGE_RIGHT) // Right is handled on rising edge
            return;

        // As long as we can decrement, do it.
        if(m_position  > 0 ) m_position--;

        //
        eol_value = m_eol->readRight();
        if(eol_value < m_eol->getRightMin() || eol_value > m_eol->getRightMax() ) {

            // Determine the carriage
            if(eol_value < m_eol->getRightMin()) {
                m_carriage = LACE_CARRIAGE;
            } else {
                m_carriage = KNIT_CARRIAGE;
            }

            // Belt shift.
            m_beltshift = digitalRead(ENCODER_C) ? SHIFTED : REGULAR;

            // We're restarting, so reset our position
            m_position = NEEDLEBED_COUNT-1;
        }
    }

    void encoderARising() { // Handle moving right
        uint16_t eol_value = 0;

        // Check the service manual for this.
        m_direction = digitalRead(ENCODER_B) ? CARRIAGE_LEFT : CARRIAGE_RIGHT;

        if(m_direction == CARRIAGE_LEFT) // Moving left should be handled on the falling edge
        {
            return;
        }

        if(m_position < NEEDLEBED_COUNT) m_position++;

        //
        eol_value = m_eol->readLeft();
        if(eol_value < m_eol->getLeftMin() || eol_value > m_eol->getLeftMax() ) {

            // Determine the carriage
            if(eol_value < m_eol->getLeftMin()) {
                m_carriage = LACE_CARRIAGE;
            } else {
                m_carriage = KNIT_CARRIAGE;
            }

            // Belt shift.
            m_beltshift = digitalRead(ENCODER_C) ? REGULAR : SHIFTED;

            // We're restarting, so reset our position
            m_position = 0;
        }
    }
};

#endif //__KNITTY_KITTY_SOLENOID__
