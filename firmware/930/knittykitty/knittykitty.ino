/**
 * This file contains the main state-machine of the knitty-kitty 
 * hardware.
 */


#include <Arduino.h>

#include "Settings.h"
#include "KnittyKitty.h"

#include "EndOfLine.h"
#include "Encoder.h"
#include "Solenoid.h"
#include "NeedleBed.h"
#include "KnittyKittyProtocol.h"


// Forward Declarations
void encoderChangeIsr();

// Globals
EndOfLine     *g_Eol        = NULL;
SolenoidArray *g_Solenoids  = NULL;
Encoders      *g_Encoders   = NULL;
KKProtocol    *g_Protocol   = NULL;
NeedleBed     *g_needleBed  = NULL;
kk_state      g_knitState   = KK_INIT;

// State Variables
kk_state              ret_state      = KK_IDLE;
carriage_direction_t  carriage_dir   = UNKNOWN_DIRECTION;
byte                  lbuffer[NEEDLEBED_COUNT], 
                      rbuffer[NEEDLEBED_COUNT];
int                   test_sol_1_pos = 0; 
int                   test_sol_2_pos = 0;

void setup() {
    g_Eol = new EndOfLine();
    g_Encoders = new Encoders(g_Eol);
    g_Solenoids = new SolenoidArray();
    g_needleBed = new NeedleBed(g_Solenoids, g_Encoders);
    g_Protocol = new KKProtocol(g_Eol, g_Encoders);

    attachInterrupt(digitalPinToInterrupt(ENCODER_A), encoderChangeIsr, CHANGE);
}

void encoderChangeIsr() {
    if(g_Encoders != NULL) g_Encoders->updateState();
    if(g_needleBed != NULL) g_needleBed->updateBed();
    switch(g_knitState) {
        default: 
            break;
    }
}

/**
 * This handles the state transitions of the FSM.
 */
void loop() {
    switch(g_knitState) {
        case KK_IDLE:
            ret_state = KK_IDLE;
            g_knitState = KK_SEND_SYNC;
            carriage_dir = g_Encoders->getCarriageDirection();
            test_sol_1_pos = 0;
            test_sol_2_pos = 0;
        break;
        
        case KK_SEND_SYNC:
            g_Protocol->sendSync(ret_state);
            g_knitState = KK_RECV_SYNC;
        break;
        
        case KK_RECV_SYNC:
            if(!g_Protocol->recvSync((byte*)(&g_knitState), lbuffer, rbuffer)) {
                g_knitState = ret_state;
            }
        break;

        case KK_TEST_SENSORS:
            ret_state = KK_TEST_SENSORS;
            g_knitState = KK_SEND_SYNC;
            break;

        case KK_TEST_SOL1:
            ret_state = KK_TEST_SOL1;
            
            g_Solenoids->clearSolenoids();
            g_Solenoids->setState(test_sol_1_pos, true);
            g_Solenoids->writeSolenoids();
            
            if(carriage_dir == UNKNOWN_DIRECTION && g_Encoders->getCarriageDirection() != UNKNOWN_DIRECTION) {
                carriage_dir = g_Encoders->getCarriageDirection();
                
            } else if(carriage_dir !=  g_Encoders->getCarriageDirection()) {
                carriage_dir = g_Encoders->getCarriageDirection();
                test_sol_1_pos += 1;
            }
            g_knitState = KK_SEND_SYNC;
            break;

        case KK_TEST_SOL2:
            ret_state = KK_TEST_SOL2;

            for(int i = 0; i < NEEDLEBED_COUNT; i++) {
                lbuffer[i] = false;
            }
            lbuffer[test_sol_2_pos] = true;
            
            if(carriage_dir == UNKNOWN_DIRECTION && g_Encoders->getCarriageDirection() != UNKNOWN_DIRECTION) {
                carriage_dir = g_Encoders->getCarriageDirection();
                
            } else if(carriage_dir !=  g_Encoders->getCarriageDirection()) {
                carriage_dir = g_Encoders->getCarriageDirection();
                g_needleBed->updateRowData(lbuffer, 
                                           carriage_dir == CARRIAGE_LEFT ? 0 : NEEDLEBED_COUNT, 
                                           carriage_dir == CARRIAGE_LEFT ? NEEDLEBED_COUNT : 0);
                test_sol_2_pos += 1;
                if(test_sol_2_pos > NEEDLEBED_COUNT)
                    test_sol_2_pos = 0;
            }
            g_knitState = KK_SEND_SYNC;
            break;
                    
        default:
            g_knitState = KK_INIT;
            break;
        
    }
}
