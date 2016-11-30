/**
 * 
 */


#ifndef __KNITTY_KITTY_PROTOCOL__
#define __KNITTY_KITTY_PROTOCOL__

#include "KnittyKitty.h"


class KKProtocol {
public:
    KKProtocol(EndOfLine *e, Encoders *enc) : m_eol(e), m_enc(enc) {
        Serial.begin(115200);
    }
    ~KKProtocol() {
      
    }

    /** 
     * Syncronizes the host with the state of the knitting 
     * machine firmware.
     */
    void sendSync(byte state) {
        kk_state_pack info;

        info.header_sig         = KNITTY_KITTY_SIG;
        info.carriage_position  = m_enc->getCarriagePosition();
        info.carriage_type      = m_enc->getCarriageType();
        info.carriage_direction = m_enc->getCarriageDirection(); 
        info.belt_shift         = m_enc->getBeltShift();
        info.state              = state;
        info.left_eol_value     = m_eol->readLeft();
        info.right_eol_value    = m_eol->readRight();
        info.ver_maj            = KNITTY_KITTY_VERSION_MAJ;
        info.ver_min            = KNITTY_KITTY_VERSION_MIN;
        
        Serial.write((byte*)&info, sizeof(kk_state_pack));
/*        Serial.write("pos: ");
        Serial.print(info.left_eol_value);
        Serial.write(",");
        Serial.print(info.right_eol_value);
        Serial.write(",");
        Serial.print(info.carriage_position);
        //Serial.write("left: ");
        //Serial.print(info.left_eol_value);
        Serial.write("\n");
        //Serial.write("pos: ");
        //Serial.print(info.right_eol_value);
        //Serial.write("\n");*/
        
    }

    bool recvSync(byte *state, byte *rbuffer, byte *lbuffer) {
        kk_opcode opcode;

        if(Serial.available() <= 0)
            return false;

        opcode = (kk_opcode)Serial.read();
        
        switch(opcode) {
            case KK_OP_SOL_TEST1:
                *state = KK_TEST_SOL1;
                break;
                
            case KK_OP_SOL_TEST2:
                *state = KK_TEST_SOL2;
                break;
                
            case KK_OP_LC_TEST:
                *state = KK_TEST_LC;
                break;

            case KK_OP_TEST_SENSOR:
                *state = KK_TEST_SENSORS;
                break;
           
            case KK_OP_RESET:
                *state = KK_INIT;
                break;
                
            case KK_OP_NOP:
            default:
                return false;
                
        }
        return true;
      
    }
    
private:
    EndOfLine *m_eol;
    Encoders *m_enc;
};


#endif

