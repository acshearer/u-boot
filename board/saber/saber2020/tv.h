
// TV stands for Text Video

#include <common.h>

#ifndef SABER_TV_H
#define SABER_TV_H

#define TV_println(x) {TV_print(x);*TV_cmdReg = TV_COMMAND_NEW_LINE;}
#define TV_mvprint(row, col, x) {TV_move(row, col);TV_print(x);}
#define TV_mvprintln(row, col, x) {TV_move(row, col);TV_println(x);}

#define TV_println_hex(x) {TV_print_hex(x);*TV_cmdReg = TV_COMMAND_NEW_LINE;}
#define TV_mvprint_hex(row, col, x) {TV_move(row, col);TV_print_hex(x);}
#define TV_mvprintln_hex(row, col, x) {TV_move(row, col);TV_println_hex(x);}

#define TV_println_num(x, r) {TV_print_num(x, r);*TV_cmdReg = TV_COMMAND_NEW_LINE;}
#define TV_mvprint_num(row, col, x, r) {TV_move(row, col);TV_print_num(x, r);}
#define TV_mvprintln_num(row, col, x, r) {TV_move(row, col);TV_println_num(x, r);}

#define TV_ADDR_VCR 0x10003000
#define TV_ADDR_VDR 0x10003004

#define TV_COMMAND_SET_CHAR 0b00000001
#define TV_COMMAND_ADVANCE  0b00000010
#define TV_COMMAND_PUT_CHAR 0b00000011
#define TV_COMMAND_SET_X    0b00000100
#define TV_COMMAND_SET_Y    0b00000101
#define TV_COMMAND_NEW_LINE 0b00001000

#define TV_COMMAND_GET_X    0b00010100
#define TV_COMMAND_GET_Y    0b00010101

#define TV_cmdReg  ((uint8_t*)TV_ADDR_VCR)
#define TV_dataReg ((uint8_t*)TV_ADDR_VDR)

void TV_move(uint8_t row, uint8_t col);

void TV_getCursor(uint8_t* row, uint8_t* col);

void TV_print(const char* str);

void TV_print_num(const int x, const int radix);

char bits2Hex(const uint8_t bits);

void TV_print_hex(const uint32_t x);

void TV_clear(void);

#endif