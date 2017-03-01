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
    void sendSync(byte state, int currentLine, int buffersAvail) {
        kk_state_pack info;

        info.header_sig         = KNITTY_KITTY_SIG;
        info.carriage_position  = m_enc->getCarriagePosition();
        info.carriage_type      = m_enc->getCarriageType();
        info.carriage_direction = m_enc->getCarriageDirection();
        info.belt_shift         = m_enc->getBeltShift();
        info.state              = state;
        info.left_eol_value     = m_eol->readLeft();
        info.right_eol_value    = m_eol->readRight();
        info.ver_maj            = m_bed->getAtPosition(info.carriage_position); //KNITTY_KITTY_VERSION_MAJ;
        info.ver_min            = m_bed->offset; //KNITTY_KITTY_VERSION_MIN;

        info.current_line       = currentLine;
        info.free_buffer        = buffersAvail;
        Serial.write((byte*)&info, sizeof(kk_state_pack));
    }

    bool recvSync(
          byte *state, 
          kk_state ret_state, 
          kk_state *bufferdStates, 
          int &currentPatt, 
          int &currentTail, 
          int &availBuffers, 
          byte rbuffer[N_BUFFERS][200], 
          byte lbuffer[N_BUFFERS][200], 
          uint8_t *right, 
          uint8_t *left,
          uint32_t *lineNoBuffer) {
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
                currentPatt = 0;
                currentTail = 0;
                availBuffers = N_BUFFERS;
                *state = KK_INIT;
                break;

            case 'U':
            case KK_OP_SYNC:
                *state = KK_SEND_SYNC;
                break;

            case KK_OP_KNIT:
            {
                *state = KK_IDLE;
                
                uint8_t bytes[NEEDLEBED_COUNT/8];

                uint32_t line = 0;

                while(Serial.available() < 2);
                left[currentTail] = Serial.read();
                right[currentTail] = Serial.read();
                Serial.readBytes((byte*)&line, 4);
                Serial.readBytes(bytes, NEEDLEBED_COUNT/8);
                
                // Always write to the tail buffer, then increment it
                

                // ------------------------
                
                for(uint8_t i = 0; i < NEEDLEBED_COUNT; i++) {
                    uint8_t byte = i / 8;
                    uint8_t bit = i % 8;

                    rbuffer[currentTail][i] = (bytes[byte] & (1 << bit)) > 0;
                }

                Serial.readBytes(bytes, NEEDLEBED_COUNT/8);
                for(uint8_t i = 0; i < NEEDLEBED_COUNT; i++) {
                    uint8_t byte = i / 8;
                    uint8_t bit = i % 8;

                    lbuffer[currentTail][i] = (bytes[byte] & (1 << bit)) > 0;
                }
                bufferdStates[currentTail] = KK_KNIT;
                
                if(availBuffers <= 0) {
                    *state = KK_INIT;
                    currentPatt = 0;
                    currentTail = 0;
                    availBuffers = N_BUFFERS;
                    return false;
                }
                
                currentTail = (currentTail + 1)  % N_BUFFERS;
                lineNoBuffer[currentTail] = line;
                availBuffers--;

                break;
            }

            case KK_OP_LACE:
            {
                *state = KK_IDLE;
                uint32_t line = 0;

                uint8_t bytes[NEEDLEBED_COUNT/8];
                while(Serial.available() < 2);
                left[currentTail] = Serial.read();
                right[currentTail] = Serial.read();
                                
                Serial.readBytes((byte*)&line, 4);
                Serial.readBytes(bytes, NEEDLEBED_COUNT/8);

                for(uint8_t i = 0; i < NEEDLEBED_COUNT; i++) {
                    uint8_t byte = i / 8;
                    uint8_t bit = i % 8;

                    lbuffer[currentTail][i] = (bytes[byte] & (1 << bit)) > 0;
                }

                Serial.readBytes(bytes, NEEDLEBED_COUNT/8);
                for(uint8_t i = 0; i < NEEDLEBED_COUNT; i++) {
                    uint8_t byte = i / 8;
                    uint8_t bit = i % 8;

                    rbuffer[currentTail][i] = (bytes[byte] & (1 << bit)) > 0;
                }
                bufferdStates[currentTail] = KK_KNITLACE;
                lineNoBuffer[currentTail] = line;

                if(availBuffers <= 0) {
                    *state = KK_INIT;
                    currentPatt = 0;
                    currentTail = 0;
                    availBuffers = N_BUFFERS;
                    return false;
                }
                
                currentTail = (currentTail + 1)  % N_BUFFERS;
                availBuffers--;
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
