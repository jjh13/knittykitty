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
ClipColumn *g_cols[7];

// State Variables
kk_state              ret_state      = KK_IDLE;
carriage_direction_t  carriage_dir   = UNKNOWN_DIRECTION;
byte                  lbuffer[NEEDLEBED_COUNT],
                      rbuffer[NEEDLEBED_COUNT];
int                   test_sol_1_pos = 0;
int                   test_sol_2_pos = 0;
uint8_t               lbed_pos;
uint8_t               rbed_pos;



void setup() {
    g_Eol = new EndOfLine();
    g_Encoders = new Encoders(g_Eol);
    g_Solenoids = new SolenoidArray();
    g_needleBed = new NeedleBed(g_Solenoids, g_Encoders);
    g_Protocol = new KKProtocol(g_Eol, g_Encoders, g_needleBed);
    
    for(int i = 0; i < 7; i++) {
        g_cols[i] = new ClipColumn(i);
    }

    attachInterrupt(digitalPinToInterrupt(ENCODER_A), encoderChangeIsr, CHANGE);
}

void encoderChangeIsr() {
    static int pin = 0; 
    g_Encoders->updateState();
    g_Solenoids->clearSolenoids();
    g_needleBed->updateBed();

    pin++;
    
    g_cols[pin % 7]->update();
}

/**
 * This handles the state transitions of the FSM.
 */
void loop() {

    static int pin = 0;
    //g_cols[++pin % 7]->update();
    
    for(int i = 0; i < 7; i++) {
       g_cols[i]->tick();
    }
    switch(g_knitState) {
        case KK_IDLE:
            ret_state = KK_IDLE;

//            ret_state = KK_TEST_SOL2;
            g_knitState = KK_RECV_SYNC;
            carriage_dir = g_Encoders->getCarriageDirection();

        break;

        case KK_SEND_SYNC:
                
            g_Protocol->sendSync(ret_state, g_cols);
            g_knitState = ret_state;
        break;

        case KK_RECV_SYNC:
            if(!g_Protocol->recvSync((byte*)(&g_knitState), rbuffer, lbuffer, rbed_pos, lbed_pos)) {
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
            g_Solenoids->setState(1, true);

            if(carriage_dir == UNKNOWN_DIRECTION && g_Encoders->getCarriageDirection() != UNKNOWN_DIRECTION) {
                carriage_dir = g_Encoders->getCarriageDirection();

            } else if(carriage_dir !=  g_Encoders->getCarriageDirection()) {
                carriage_dir = g_Encoders->getCarriageDirection();
                test_sol_1_pos = (test_sol_1_pos + 1) % NUM_SOLENOIDS;
                Serial.println(test_sol_1_pos);
                g_Solenoids->writeSolenoids();
            }
            g_knitState = KK_RECV_SYNC;
            break;

        case KK_TEST_SOL2:
            ret_state = KK_TEST_SOL2;

            for(int i = 0; i < NEEDLEBED_COUNT; i++) {
                lbuffer[i] = false;
            }
            lbuffer[95] = true;
            lbuffer[100] = true;
            lbuffer[108] = true;
            g_needleBed->updateRowData(lbuffer,
                                           carriage_dir == CARRIAGE_LEFT ? 0 : NEEDLEBED_COUNT,
                                           carriage_dir == CARRIAGE_LEFT ? NEEDLEBED_COUNT : 0);

            g_Solenoids->clearSolenoids();
            g_needleBed->updateBed();
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
            g_knitState = KK_RECV_SYNC;
            break;

        case KK_KNIT:
            g_knitState = KK_RECV_SYNC;
            ret_state = KK_KNIT_L;
            g_needleBed->updateRowData(lbuffer, 0, 0);
            break;

        case KK_KNIT_L:
            g_knitState = KK_RECV_SYNC;
            ret_state = KK_KNIT_L;

            if(g_Encoders->getCarriageDirection() == CARRIAGE_RIGHT && g_Encoders->getCarriagePosition() < lbed_pos)
            {
              ret_state = KK_KNIT_R; 
              g_needleBed->updateRowData(rbuffer, 0, 0);
            }
            break;
            
        case KK_KNIT_R:
            g_knitState = KK_RECV_SYNC;
            ret_state = KK_KNIT_R;
           
            if((g_Encoders->getCarriageDirection() == CARRIAGE_LEFT && g_Encoders->getCarriagePosition() > rbed_pos)
               || (g_Encoders->getCarriageType() != KNIT_CARRIAGE))
            {
              ret_state = KK_IDLE; 
              g_needleBed->clearBed();
            }
            break;


        case KK_KNITLACE:
            g_knitState = KK_RECV_SYNC;
            ret_state = KK_KNITLACE_R;
            g_needleBed->updateRowData(rbuffer, 0, 0);
            break;
            
        case KK_KNITLACE_R:
            g_knitState = KK_RECV_SYNC;
            ret_state = KK_KNITLACE_R;
           
            if((g_Encoders->getCarriageDirection() == CARRIAGE_LEFT && g_Encoders->getCarriagePosition() > rbed_pos))
            {
              ret_state = KK_KNITLACE_L; 
              g_needleBed->updateRowData(lbuffer, 0, 0);
            }
            break;
            
        case KK_KNITLACE_L:
            g_knitState = KK_RECV_SYNC;
            ret_state = KK_KNITLACE_L;
            
            if(g_Encoders->getCarriageDirection() == CARRIAGE_LEFT && g_Encoders->getCarriagePosition() < lbed_pos)
            {
              ret_state = KK_IDLE; 
              g_needleBed->clearBed();
            }
            
            break;

        case KK_INIT:
            g_knitState = KK_IDLE;
            g_needleBed->clearBed();
            test_sol_1_pos = 0;
            test_sol_2_pos = 99;
            break;

        default:
            g_knitState = KK_INIT;
            break;

    }
}
