#include <console.h>
#include <keyboard.h>
#include <console.h>

void User_ConsoleWriteCharacter(unsigned char c)
{
	asm volatile("movl $1, %%eax\n\t"
				 "movzx %0, %%ebx\n\t"
				 "int $0x80\n"
				 : : "b"(c)
				);
}

void User_ConsoleWriteString(char* str)
{
	asm volatile("xor %%eax, %%eax\n\t"
				 "leal (%0), %%ebx\n\t"
				 "int $0x80\n"
				 : : "b"(str)
				);
}

void User_ConsoleWriteInt(unsigned int i, unsigned int base)
{
	asm volatile("movl $2, %%eax\n\t"
				 "movl %0, %%ecx\n\t"
				 "movl %1, %%ebx\n\t"
				 "int $0x80\n"
				 : : "c"(base), "b"(i)
				);
}

void User_ConsoleClearScreen(const uint8_t c)
{
	asm volatile("movl $3, %%eax\n\t"
				 "movzx %0, %%ebx\n\t"
				 "int $0x80\n"
				 : : "b"(c)
				);
}

void User_ConsoleGotoXY(unsigned int x, unsigned int y) 
{
	asm volatile("movl $4, %%eax\n\t"
				 "movl %0, %%ecx\n\t"
				 "movl %1, %%ebx\n\t"
				 "int $0x80\n"
				 : : "c"(y), "b"(x)
				);
}

void User_SetResolution(int width, int height, uint8_t chainMode)
{
	asm volatile("xor %%eax, %%eax\n\t"
				"movzx %0, %%edx\n\t"
				"movl %1, %%ecx\n\t"
				"movl %2, %%ebx\n\t"
				"int $0x81\n"
				: : "d"(chainMode), "c"(height), "b"(width)
				);
}	

void User_SetColourPalette(int red, int green, int blue, int index)
{
	// Here we use 4 parameters which requires using index
	asm volatile("movl $1, %%eax\n\t"
				"movl %0, %%esi\n\t"
				"movl %1, %%edx\n\t"
				"movl %2, %%ecx\n\t"
				"movl %3, %%ebx\n\t"
				"int $0x81\n"
				: : "S"(index), "d"(blue), "c"(green), "b"(red)
				);
}

void User_SetLineStyle(int dashLength)
{
	asm volatile("movl $2, %%eax\n\t"
				"movl %0, %%ebx\n\t"
				"int $0x81\n"
				: : "b"(dashLength)
				);
}

void User_ClearScreen()
{
	asm volatile("movl $3, %%eax\n\t"
				 "int $0x81\n"
				 : :);
}
			
void User_SetPixel(unsigned int x, unsigned int y, uint8_t colour)
{
	asm volatile("movl $4, %%eax\n\t"
				 "movzx %0, %%edx\n\t"
				 "movl %1, %%ecx\n\t"
				 "movl %2, %%ebx\n\t"
				 "int $0x81\n"
				 : : "d"(colour), "c"(y), "b"(x)
				);
}
	
void User_MoveTo(unsigned int x, unsigned int y)
{
	asm volatile("movl $5, %%eax\n\t"
				 "movl %0, %%ecx\n\t"
				 "movl %1, %%ebx\n\t"
				 "int $0x81\n"
				 : : "c"(y), "b"(x)
				);
}

void User_DrawChar(char c, uint8_t colour)
{
	asm volatile("movl $6, %%eax\n\t"
				"movzx %0, %%ecx\n\t"
				"movzx %1, %%ebx\n\t"
				"int $0x81\n"
				: : "c"(colour), "b"(c)
				);
}

void User_DrawString(const char* str, uint8_t colour)
{
	asm volatile("movl $7, %%eax\n\t"
				"movzx %0, %%ecx\n\t"
				"movl %1, %%ebx\n\t"
				"int $0x81\n"
				: : "c"(colour), "b"(str)
				);
}

void User_LineTo(unsigned int x, unsigned int y, uint8_t colour)
{
	asm volatile("movl $8, %%eax\n\t"
				 "movzx %0, %%edx\n\t"
				 "movl %1, %%ecx\n\t"
				 "movl %2, %%ebx\n\t"
				 "int $0x81\n"
				 : : "d"(colour), "c"(y), "b"(x)
				);
}
			
void User_DrawRectangle(unsigned int x, unsigned int y, unsigned int width, unsigned int height, uint8_t colour)
{
	User_MoveTo(x, y);
	asm volatile("movl $9, %%eax\n\t"
				"movzx %0, %%edx\n\t"
				"movl %1, %%ecx\n\t"
				"movl %2, %%ebx\n\t"
				"int $0x81\n"
				: : "d"(colour), "c"(height), "b"(width)
				);
}

void User_FillRectangle(unsigned int x, unsigned int y, unsigned int width, unsigned int height, uint8_t colour)
{
	User_MoveTo(x, y);
	asm volatile("movl $10, %%eax\n\t"
				"movzx %0, %%edx\n\t"
				"movl %1, %%ecx\n\t"
				"movl %2, %%ebx\n\t"
				"int $0x81\n"
				: : "d"(colour), "c"(height), "b"(width)
				);
}

void User_DrawCircle(int radius, uint8_t colour)
{
	asm volatile("movl $11, %%eax\n\t"
				"movzx %0, %%ecx\n\t"
				"movl %1, %%ebx\n\t"
				"int $0x81\n"
				: : "c"(colour), "b"(radius)
				);
}

void User_FillCircle(int radius, uint8_t colour)
{
	asm volatile("movl $12, %%eax\n\t"
				"movzx %0, %%ecx\n\t"
				"movl %1, %%ebx\n\t"
				"int $0x81\n"
				: : "c"(colour), "b"(radius)
				);
}

void User_DrawPolygon(int *coordinates, int numberOfVertices, uint8_t colour)
{
	asm volatile("movl $13, %%eax\n\t"
				"movzx %0, %%edx\n\t"
				"movl %1, %%ecx\n\t"
				"movl %2, %%ebx\n\t"
				"int $0x81\n"
				: : "d"(colour), "c"(numberOfVertices), "b"(coordinates)
				);
}

void User_FillPolygon(int *coordinates, int numberOfVertices, uint8_t colour)
{
	asm volatile("movl $14, %%eax\n\t"
				"movzx %0, %%edx\n\t"
				"movl %1, %%ecx\n\t"
				"movl %2, %%ebx\n\t"
				"int $0x81\n"
				: : "d"(colour), "c"(numberOfVertices), "b"(coordinates)
				);
}

void User_FloodFill(int startX, int startY, uint8_t replacementColour)
{
	asm volatile("movl $15, %%eax\n\t"
				"movzx %0, %%edx\n\t"
				"movl %1, %%ecx\n\t"
				"movl %2, %%ebx\n\t"
				"int $0x81\n"
				: : "d"(replacementColour), "c"(startY), "b"(startX)
				);
}