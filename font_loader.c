#include <font_loader.h>

// Loads font bitmap from VGA RAM. No file system to load external bitmaps.
// Source = http://wiki.osdev.org/VGA_Fonts
// Could not get this code to work in the end. Just changes a portion of the drawing canvas to blue.
void LoadFontFromRam(uint8_t *buffer)
{
    asm volatile("movl  %0, %%edi\n\t"
                "movw	$0x03CE, %%dx\n\t"
                "movb	$5, %%al\n\t"
                "outb	%%al, %%dx\n\t"
                "movw	$0x0406, %%ax\n\t"
                "outw	%%ax, %%dx\n\t"
                "movw	$0x03C4, %%dx\n\t"
                "movw	$0x0402, %%ax\n\t"
                "outw	%%ax, %%dx\n\t"
                "movw	$0x0604, %%ax\n\t"
                "outw	%%ax, %%dx\n\t"
                "movl	$0x0A0000, %%esi\n\t"
                "movl	$256, %%ecx\n\t"
                "_loop:\n\t"
                "movsd\n\t"
                "movsd\n\t"
                "movsd\n\t"
                "movsd\n\t"
                "addl	$16, %%esi\n\t"
                "subl   $1, %%ecx\n\t"
                "cmpl   $0, %%ecx\n\t"
                "jne     _loop\n\t"
                "movw	$0x0302, %%ax\n\t"
                "outw	%%ax, %%dx\n\t"
                "movw	$0x0204, %%ax\n\t"
                "outw	%%ax, %%dx\n\t"
                "movw	$0x03CE, %%dx\n\t"
                "movw	$0x1005, %%ax\n\t"
                "outw	%%ax, %%dx\n\t"
                "movw	$0x0E06, %%ax\n\t"
                "outw	%%ax, %%dx\n\t"
                : : "D"(buffer)
                );
}