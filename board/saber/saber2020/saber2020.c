
#ifndef __CONFIG_H
#define __CONFIG_H

#include <common.h>
#include <bootstage.h>
#include <log.h>
#include <init.h>


int board_init(void) {
	
    printf("Board init\n");

	return 0;
}

void print_stack_trace(void) {
    uint32_t stackPointer;
    int i;

    asm volatile("mv %[value], sp" : [value] "=r" (stackPointer));

    printf("Stack trace:\n");

    for(i = 0; i < 32; i++) {
        uint32_t stackValue = *((uint32_t*)stackPointer);
        if(stackValue != 0) {
            printf(" @%8x:%8x ", stackPointer, stackValue);
            // TV_print(" @");
            // TV_print_hex(stackPointer);
            // TV_print(":");
            // TV_print_hex(stackValue);
            // TV_print(" ");
        }
        stackPointer += 4;
    }

}

void show_boot_progress(int progress) {
    if(progress < 0){
        printf("Boot Error: %d\n", -progress);
        // TV_print("Boot Error:    ");
        // TV_println_hex(-progress);
        print_stack_trace();
    }else{
        printf("Boot Progress: %d\n", progress);
        // TV_print("Boot Progress: ");
        // TV_println_hex(progress);
    }
}

#endif