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
        carriage_direction_t dir = m_enc->getCarriageDirection();
        beltshift_t bs = m_enc->getBeltShift();

        if(carriage == KNIT_CARRIAGE) {
            int shift = (bs == SHIFTED) ? 8 : 0;
            int dir_shift = (dir == CARRIAGE_LEFT) ? 16 : -16;
            int pixel = cpos + dir_shift;            
            
            int start = pixel - 4;
            int end = pixel + 4;
            
            if(start < 0) start = 0;
            if(end > 199) end = 199; 
            for(; start < end; ++start)
                m_sol->setState((start+ shift)%NUM_SOLENOIDS, m_currentrow[start]);  


            //////////////////
        } else if(carriage == LACE_CARRIAGE) {
          
            int shift = (bs == SHIFTED) ? 8 : 0;
            int dir_shift = (dir == CARRIAGE_LEFT) ? 6 : -6;
            int pixel = cpos + dir_shift;       

            shift += (dir == CARRIAGE_LEFT)? 0 : 8;
            
            int start = pixel - 6;
            int end = pixel + 6;

            if(start < 0) start = 0;
            if(end > 199) end = 199; 
            for(; start < end; ++start)
                m_sol->setState((start+ shift)%NUM_SOLENOIDS, m_currentrow[start]);  

        }
        m_sol->writeSolenoids();  
           
    }

    /*
     * Writes new row data into the internal structure.
     */
    void updateRowData(byte *bed, int start, int end) {
        for(unsigned int i = 0; i < NEEDLEBED_COUNT; i++) {
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
