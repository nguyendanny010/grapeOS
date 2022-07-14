#include "kernel.h"
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"

#include <stdint.h>
#include <stddef.h>

uint16_t* video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

/*
 * Returns the hexadecimal representation of a character and color.
 * Because of endianness, the color must be shifted 8 bits to the left
 * then OR'd with the character.
 */
uint16_t terminal_make_char(char c, char color){
    return (color << 8) | c;
}

/*
 * Prints a character at the specified x and y.
 */
void terminal_putchar(int x, int y, char c, char color){
    video_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c, color);
}

/*
 * Writes a character to the terminal.
 */
void terminal_writechar(char c, char color){

    if(c == '\n'){
        terminal_col = 0;
        terminal_row++;
        return;
    }

    terminal_putchar(terminal_col, terminal_row, c, color);

    terminal_col++;

    if(terminal_col >= VGA_WIDTH){
        terminal_col = 0;
        terminal_row++;
    }
}

/*
 * Clears the terminal.
 */
void terminal_initalize(){

    video_mem = (uint16_t*)(0xB8000);

    for(int y = 0; y < VGA_HEIGHT; y++){
        for(int x = 0; x < VGA_WIDTH; x++){
            terminal_putchar(x, y, ' ', 0);
        }
    }
}

/*
 * Returns the length of the string.
 */
size_t strlen(const char* str){
    size_t len = 0;
    while(str[len]) len++;
    return len;
}

/*
 * Prints a string.
 */
void print(const char* str){

    size_t len = strlen(str);
    for(int i = 0; i < len; i++){
        terminal_writechar(str[i], 15);
    }
}

void kernel_main(){

    terminal_initalize();
    print("hello world");

    // Initialize the heap
    kheap_init();

    // Initialize the interrupt descriptor table
    idt_init();

    // Enable the system interrupts
    enable_interrupts();
}