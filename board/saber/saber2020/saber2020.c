
#ifndef __CONFIG_H
#define __CONFIG_H

#include <common.h>
#include <bootstage.h>
#include <log.h>
#include <init.h>

#include "tv.h"


int board_init(void) {
	
    debug("Board init\n");
    TV_println("Board init (using TV)");

	return 0;
}

void print_stack_trace(void) {
    uint32_t stackPointer;
    asm volatile("mv %[value], sp" : [value] "=r" (stackPointer));

    while(stackPointer <= 0xFFFFFFF0) {
        uint32_t stackValue = *((uint32_t*)stackPointer);
        if(stackValue != 0 && stackValue >= 0x80400000 && stackValue < 0x80450000) {
            TV_print(" @");
            TV_print_hex(stackPointer);
            TV_print(":");
            TV_print_hex(stackValue);
            TV_print(" ");
        }
        stackPointer += 4;
    }

}

void show_boot_progress(int progress) {
    if(progress < 0){
        TV_print("Boot Error:    ");
        TV_println_hex(-progress);
        print_stack_trace();
    }else{
        TV_print("Boot Progress: ");
        TV_println_hex(progress);
    }
}

#endif