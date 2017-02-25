
#ifndef __KNITTY_KITTY_DEF__
#define __KNITTY_KITTY_DEF__

enum carriage_t: byte{
    UNKNOWN_CARRIAGE = 0,
    KNIT_CARRIAGE,
    LACE_CARRIAGE
};

enum carriage_direction_t : byte {
    UNKNOWN_DIRECTION = 0,
    CARRIAGE_LEFT, // RIGHT TO LEFT
    CARRIAGE_RIGHT // LEFT TO RIGHT
};

enum beltshift_t : byte{
    UNKNOWN_SHIFT = 0,
    REGULAR,
    SHIFTED
};

enum kk_state : byte {
    KK_INIT = 0,
    KK_IDLE,
    KK_SEND_SYNC,
    KK_RECV_SYNC,
    KK_TEST_SENSORS,
    KK_TEST_SOL1,
    KK_TEST_SOL2,
    KK_TEST_LC,
    KK_CALIBRATE,
    KK_KNIT,
    KK_KNIT_L,
    KK_KNIT_R,
    KK_KNITLACE,
    KK_KNITLACE_L,
    KK_KNITLACE_R
};

enum kk_opcode : byte {
    KK_OP_NOP = 0,
    KK_OP_RESET,
    KK_OP_TEST_SENSOR,
    KK_OP_SOL_TEST1,
    KK_OP_SOL_TEST2,
    KK_OP_LC_TEST,
    KK_OP_KNIT,
    KK_OP_LACE,
    KK_OP_POKE_PIXEL_L,
    KK_OP_POKE_PIXEL_R,
    KK_OP_SYNC,
    KK_OP_INC_OFFSET,
    KK_OP_DEC_OFFSET,
};

struct kk_state_pack {
  uint32_t header_sig;
  int16_t carriage_position;
  uint8_t carriage_type;
  uint8_t carriage_direction;
  uint8_t belt_shift;
  uint8_t state;
  uint16_t left_eol_value;
  uint16_t right_eol_value;
  uint8_t ver_maj;
  uint8_t ver_min;

  uint8_t rows[8];
};

#define TIMER_VALUE 10000

#define CLIP_L0 A10
#define CLIP_L1 A13
#define CLIP_L2 A9
#define CLIP_L3 A11
#define CLIP_L4 A8
#define CLIP_L5 A12
#define CLIP_L6 A14

int clip_index[] = {
  CLIP_L0,
  CLIP_L1,
  CLIP_L2,
  CLIP_L3,
  CLIP_L4,
  CLIP_L5,
  CLIP_L6,
};



class ClipColumn {
private:
  char lastVal;

  int aport;
  
  int timers[20];
public:
  ClipColumn(int port) : lastVal(-1){
    timers[0] = 0;
    timers[1] = 0;
    timers[2] = 0;
    timers[3] = 0;
    aport = port;
  }
  ~ClipColumn() { }

  void update_(int v) {
    if(v & 1) timers[0] = TIMER_VALUE;
    if(v & 2) timers[1] = TIMER_VALUE;
    if(v & 4) timers[2] = TIMER_VALUE;
    if(v & 8) timers[3] = TIMER_VALUE;
  }

  void tick() {
    if(timers[0] > 0) timers[0]--;
    if(timers[1] > 0) timers[1]--;
    if(timers[2] > 0) timers[2]--;
    if(timers[3] > 0) timers[3]--;
  }

  char getBinValue() {
    char r = 0;
    if(timers[0] > 0) r |= 1;
    if(timers[1] > 0) r |= 2;
    if(timers[2] > 0) r |= 4;
    if(timers[3] > 0) r |= 8;
    return r;
  }
  void hack_print() {
    char v = getBinValue();
    if(v & 1) Serial.println("A");
    if(v & 2) Serial.println("B");
    if(v & 4) Serial.println("C");
    if(v & 8) Serial.println("D");
  }
   void update() {
    int v = analogRead(clip_index[aport]);
  byte r = 0;
  if(v <= 5){ r = 0; }
  else if(v > 10 && v <= 60) { r = 8; }
  else if(v > 80 && v <= 113) { r = 4; }
  else if(v > 140 && v <= 161) { r = 12; }
  else if(v > 171 && v <= 204) { r = 2; }
  else if(v > 230 && v <= 243) { r = 10; }
  else if(v > 253 && v <= 279) { r = 6; }
  else if(v > 289 && v <= 311) { r = 14; }
  else if(v > 321 && v <= 341) { r = 1; }
  else if(v > 351 && v <= 368) { r = 9; }
  else if(v > 378 && v <= 393) { r = 5; }
  else if(v > 410 && v <= 416) { r = 13; }
  else if(v > 426 && v <= 438) { r = 3; }
  else if(v > 448 && v <= 458) { r = 11; }
  else if(v > 468 && v <= 477) { r = 7; }
  else if(v > 490 && v <= 495) { r = 15; }
  this->update_(r);
}


};


#define KNITTY_KITTY_SIG 0x4D454F57
#endif // __KNITTY_KITTY_DEF__
