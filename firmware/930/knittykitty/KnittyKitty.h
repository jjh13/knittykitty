
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
  uint32_t current_line;
  uint32_t free_buffer;
};

#define KNITTY_KITTY_SIG 0x4D454F57
#endif // __KNITTY_KITTY_DEF__
