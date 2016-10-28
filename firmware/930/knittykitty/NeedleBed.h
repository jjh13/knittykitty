/**
 * NeedleBed.h
 * 
 * The purpose of this class is to provide a container for a pattern line. It translates
 * a pattern into solenoid activations. It also buffers the pattern on the bed, i.e. based 
 * on a cariagge's rough position, it'll activate  a nearby window solenoids so that the exact
 * position of the carriage isn't necessary.
 * 
 * @author Joshua Horacsek
 */

#ifndef __KNITTY_KITTY_NEEDLEBED__
#define __KNITTY_KITTY_NEEDLEBED__

#include "Settings.h"

#include "KnittyKitty.h"

#include "EndOfLine.h"
#include "Encoder.h"
#include "Solenoid.h"

class NeedleBed {
public:
    NeedleBed(SolenoidArray *s, Encoders *e) : m_sol(s), m_enc(e) {
        clearBed();
    }

    ~NeedleBed() { }

    /**
     * Updates the bed based on the position of the carriages
     * from the encoder. This basically actuates the 
     */
    void updateBed() {
        int cpos = m_enc->getCarriagePosition();
        carriage_t carriage = m_enc->getCarriageType();
        carriage_direction_t dit = m_enc->getCarriageDirection();

        int start_pos = cpos, end_pos = cpos;

        // Translate the carriage position into a needle position
        // Take a look at the service manual, different carriages
        // will actuate needles at different offsets.
        switch(carriage) {
            default:
                start_pos = cpos + 16, end_pos = cpos + 16;
                start_pos -= LOOKAHEAD;
                end_pos += LOOKAHEAD;

                break;
        }
        // Bound the window to a resonable area
        if(start_pos < 0) start_pos = 0;
        if(start_pos > NEEDLEBED_COUNT) start_pos = NEEDLEBED_COUNT;
        if(end_pos < 0) end_pos = 0;
        if(end_pos > NEEDLEBED_COUNT) end_pos = NEEDLEBED_COUNT;

        // Activate solenoids
        m_sol->clearSolenoids();
        
        for(unsigned int i = start_pos; i < end_pos+1; i++){
            Serial.print(i); Serial.print(":"), Serial.print(m_currentrow[i]), Serial.print("\n");
            m_sol->setState(i%NUM_SOLENOIDS, m_currentrow[i]);   
        }
        
        m_sol->writeSolenoids();     
        Serial.print("Updaing");
    }

    /*
     * Writes new row data into the internal structure.
     */
    void updateRowData(byte *bed, int start, int end) {
        for(unsigned int i = 0; i < NEEDLEBED_COUNT; i++) {
            Serial.print(i); Serial.print(":"), Serial.print(m_currentrow[i]), Serial.print("\n");
            m_currentrow[i] = bed[i];
        }
    }

    /**
     * Clears all the needles on the bed (and resets the
     * solenoids)
     */
    void clearBed() {
        for(unsigned int i = 0; i < NEEDLEBED_COUNT; i++) {
            m_currentrow[i] = false;
            m_sol->setState(i%NUM_SOLENOIDS, false);   
        }
        m_sol->writeSolenoids();
    }
    
private:
    Encoders *m_enc;
    SolenoidArray *m_sol;
    boolean m_currentrow[NEEDLEBED_COUNT];
};

#endif //__KNITTY_KITTY_NEEDLEBED__
