
// TV stands for Text Video

#include "tv.h"

void TV_move(uint8_t row, uint8_t col) {
    *TV_dataReg = row;
    *TV_cmdReg = TV_COMMAND_SET_Y;
    *TV_dataReg = col;
    *TV_cmdReg = TV_COMMAND_SET_X;
}

void TV_getCursor(uint8_t* row, uint8_t* col) {
    *TV_cmdReg = TV_COMMAND_GET_Y;
    *row = *TV_dataReg;
    *TV_cmdReg = TV_COMMAND_GET_X;
    *col = *TV_dataReg;
}

void TV_print(const char* str) {
    char* c = (char*)str;
    while(*c != '\0'){
        if(*c == '\n'){
            *TV_cmdReg = TV_COMMAND_NEW_LINE;
        }else{
            *TV_dataReg = (uint8_t)(*c);
            *TV_cmdReg = TV_COMMAND_PUT_CHAR;
        }
        c++;
    }
}

void TV_print_num(const int x, const int radix) {
    // char buffer[40];
    // itoa(x, buffer, radix);
    // TV_print(buffer);
    TV_print("(num not supported, HEX: ");
    TV_print_hex(x);
    TV_print(")");
}

char bits2Hex(const uint8_t bits){
    if(bits >= 0xA) {
        return 'A'+bits-0xA;
    } else {
        return '0'+bits;
    }
}

void TV_print_hex(const uint32_t x) {
    *TV_dataReg = bits2Hex((x >> 28) & 0xF);
    *TV_cmdReg = TV_COMMAND_PUT_CHAR;
    *TV_dataReg = bits2Hex((x >> 24) & 0xF);
    *TV_cmdReg = TV_COMMAND_PUT_CHAR;
    *TV_dataReg = bits2Hex((x >> 20) & 0xF);
    *TV_cmdReg = TV_COMMAND_PUT_CHAR;
    *TV_dataReg = bits2Hex((x >> 16) & 0xF);
    *TV_cmdReg = TV_COMMAND_PUT_CHAR;
    *TV_dataReg = bits2Hex((x >> 12) & 0xF);
    *TV_cmdReg = TV_COMMAND_PUT_CHAR;
    *TV_dataReg = bits2Hex((x >> 8) & 0xF);
    *TV_cmdReg = TV_COMMAND_PUT_CHAR;
    *TV_dataReg = bits2Hex((x >> 4) & 0xF);
    *TV_cmdReg = TV_COMMAND_PUT_CHAR;
    *TV_dataReg = bits2Hex(x & 0xF);
    *TV_cmdReg = TV_COMMAND_PUT_CHAR;
}

void TV_clear(void) {
    *TV_dataReg = 0;
    *TV_cmdReg = TV_COMMAND_SET_Y;
    *TV_cmdReg = TV_COMMAND_SET_X;
    for(int i = 0; i < 80*60; i++){
        *TV_cmdReg = TV_COMMAND_PUT_CHAR;
    }
}