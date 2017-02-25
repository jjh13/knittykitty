/**
 *
 */


#ifndef __KNITTY_KITTY_PROTOCOL__
#define __KNITTY_KITTY_PROTOCOL__

#include "KnittyKitty.h"


class KKProtocol {
public:
    KKProtocol(EndOfLine *e, Encoders *enc, NeedleBed *bed) : m_eol(e), m_enc(enc), m_bed(bed) {
        Serial.begin(115200);
    }
    ~KKProtocol() {

    }

    /**
     * Syncronizes the host with the state of the knitting
     * machine firmware.
     */
    void sendSync(byte state, ClipColumn **cols) {
        kk_state_pack info;

        info.header_sig         = KNITTY_KITTY_SIG;
        info.carriage_position  = m_enc->getCarriagePosition();
        info.carriage_type      = m_enc->getCarriageType();
        info.carriage_direction = m_enc->getCarriageDirection();
        info.belt_shift         = m_enc->getBeltShift();
        info.state              = state;
        info.left_eol_value     = m_eol->readLeft();
        info.right_eol_value    = m_eol->readRight();
        info.ver_maj            = m_bed->offset; //KNITTY_KITTY_VERSION_MAJ;
        info.ver_min            = KNITTY_KITTY_VERSION_MIN;

        for(int i = 0; i < 7; i++) {
            info.rows[i] = cols[i]->getBinValue();
        }
        Serial.write((byte*)&info, sizeof(kk_state_pack));

    }

    bool recvSync(byte *state, byte *rbuffer, byte *lbuffer, uint8_t &right, uint8_t &left) {
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

            case KK_OP_SYNC:
                *state = KK_SEND_SYNC;
                break;

            case KK_OP_KNIT:
            {
                *state = KK_KNIT;
                uint8_t bytes[NEEDLEBED_COUNT/8];
                
                while(Serial.available() < 2);
                left = Serial.read();
                right = Serial.read();

                Serial.readBytes(bytes, NEEDLEBED_COUNT/8);
                for(uint8_t i = 0; i < NEEDLEBED_COUNT; i++) {
                    uint8_t byte = i / 8;
                    uint8_t bit = i % 8;

                    rbuffer[i] = (bytes[byte] & (1 << bit)) > 0;
                }

                Serial.readBytes(bytes, NEEDLEBED_COUNT/8);
                for(uint8_t i = 0; i < NEEDLEBED_COUNT; i++) {
                    uint8_t byte = i / 8;
                    uint8_t bit = i % 8;

                    lbuffer[i] = (bytes[byte] & (1 << bit)) > 0;
                }

                break;
            }

            case KK_OP_LACE:
            {
                *state = KK_KNITLACE;
                uint8_t bytes[NEEDLEBED_COUNT/8];
                while(Serial.available() < 2);
                left = Serial.read();
                right = Serial.read();

                Serial.readBytes(bytes, NEEDLEBED_COUNT/8);
                for(uint8_t i = 0; i < NEEDLEBED_COUNT; i++) {
                    uint8_t byte = i / 8;
                    uint8_t bit = i % 8;

                    lbuffer[i] = (bytes[byte] & (1 << bit)) > 0;
                }

                Serial.readBytes(bytes, NEEDLEBED_COUNT/8);
                for(uint8_t i = 0; i < NEEDLEBED_COUNT; i++) {
                    uint8_t byte = i / 8;
                    uint8_t bit = i % 8;

                    rbuffer[i] = (bytes[byte] & (1 << bit)) > 0;
                }
                break;
            }

            case KK_OP_POKE_PIXEL_L:
                return false;

            case KK_OP_POKE_PIXEL_R:
                return false;

            case KK_OP_NOP:
            default:
                return false;

            case KK_OP_INC_OFFSET:
              m_bed->offset += 1;
              return false;
            case KK_OP_DEC_OFFSET:
              m_bed->offset -= 1;
              return false;
              break;

        }
        return true;

    }

private:
    EndOfLine *m_eol;
    Encoders *m_enc;
    NeedleBed *m_bed;
};


#endif
