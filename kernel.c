#include <stdbool.h> /* C doesn't have booleans by default. */
#include <stddef.h>
#include <stdint.h>

/* Hardware text mode color constants. */
enum vga_color
{
    COLOR_BLACK = 0,
    COLOR_BLUE = 1,
    COLOR_GREEN = 2,
    COLOR_CYAN = 3,
    COLOR_RED = 4,
    COLOR_MAGENTA = 5,
    COLOR_BROWN = 6,
    COLOR_LIGHT_GREY = 7,
    COLOR_DARK_GREY = 8,
    COLOR_LIGHT_BLUE = 9,
    COLOR_LIGHT_GREEN = 10,
    COLOR_LIGHT_CYAN = 11,
    COLOR_LIGHT_RED = 12,
    COLOR_LIGHT_MAGENTA = 13,
    COLOR_LIGHT_BROWN = 14,
    COLOR_WHITE = 15,
};
 
uint8_t make_color(enum vga_color fg, enum vga_color bg)
{
    return fg | bg << 4;
}
 
uint16_t make_vgaentry(char c, uint8_t color)
{
    uint16_t c16 = c;
    uint16_t color16 = color;
    return c16 | color16 << 8;
}
 
size_t strlen(const char* str)
{
    size_t ret = 0;
    while ( str[ret] != 0 )
        ret++;
    return ret;
}
 
#define _VGA_WIDTH 80
#define _VGA_HEIGHT 24
#define _VGA_LASTROW 23
static const size_t VGA_WIDTH = _VGA_WIDTH;
static const size_t VGA_HEIGHT = _VGA_HEIGHT;
static const size_t VGA_H_OVERFLOW = _VGA_HEIGHT;
static const size_t VGA_W_OVERFLOW = _VGA_WIDTH;
static const size_t VGA_LASTROW = _VGA_LASTROW;
 
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;
 
void terminal_initialize()
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    terminal_buffer = (uint16_t*) 0xB8000;
    for ( size_t y = 0; y < VGA_HEIGHT; y++ )
    {
        for ( size_t x = 0; x < VGA_WIDTH; x++ )
        {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = make_vgaentry(' ', terminal_color);
        }
    }
}

// Moves all VGA text up a row, replacing the bottom row with spaces
void terminal_scroll() {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            const size_t lookahead_index = index + VGA_WIDTH;

            if (y == VGA_LASTROW)
                terminal_buffer[index] = make_vgaentry(' ', terminal_color);
            else
                terminal_buffer[index] = terminal_buffer[lookahead_index];
        }
    }
    terminal_row = VGA_LASTROW;
    terminal_column = 0;
}
 
void terminal_setcolor(uint8_t color)
{
    terminal_color = color;
}
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = make_vgaentry(c, color);
}
 
// Put a character in the VGA terminal
// Supports linewrapping, newlines, and terminal scrolling
void terminal_putchar(char c) {
    // Print character with line wrapping
    if (c != '\n') {
        terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
        if (++terminal_column == VGA_W_OVERFLOW) {
            terminal_column = 0;
            if (++terminal_row == VGA_H_OVERFLOW)
                terminal_scroll();
        }
    } else {
        // Newline special case
        terminal_column = 0;
        if (++terminal_row == VGA_H_OVERFLOW)
            terminal_scroll();
    }
}

// Write a string to the VGA terminal
void terminal_writestring(const char* data) {
    size_t datalen = strlen(data);
    for (size_t i = 0; i < datalen; i++)
        terminal_putchar(data[i]);
}

// Print the Panes-OS logo in full RGB color!
void terminal_writelogo() {
    uint8_t old_color = terminal_color;

    terminal_color = make_color(COLOR_LIGHT_RED, COLOR_BLACK);
    terminal_putchar('P');
    terminal_putchar('a');
    terminal_color = make_color(COLOR_LIGHT_GREEN, COLOR_BLACK);
    terminal_putchar('n');
    terminal_putchar('e');
    terminal_color = make_color(COLOR_LIGHT_BLUE, COLOR_BLACK);
    terminal_putchar('s');
    terminal_writestring("-OS");

    terminal_color = old_color;
}
 
void kernel_main() {
    terminal_initialize();
    terminal_writestring("Welcome to ");
    terminal_writelogo();
    terminal_writestring("!\n");
}
