//	Basic Console Output.

#include <stdint.h>
#include <string.h>
#include <console.h>

// Video memory
uint16_t *_videoMemory = (uint16_t *)0xB8000;

#define CONSOLE_HEIGHT		25
#define CONSOLE_WIDTH		80

// Current cursor position
uint8_t _cursorX = 0;
uint8_t _cursorY = 0;

// Current color
uint8_t	_colour = 15;

// Write byte to device through port mapped io
void  OutputByteToVideoController(unsigned short portid, unsigned char value) 
{
	asm volatile("movb	%1, %%al \n\t"
				 "movw	%0, %%dx \n\t"
				  "out	%%al, %%dx"
				 : : "m" (portid), "m" (value));
				 
}

// Update hardware cursor position
void UpdateCursorPosition(int x, int y) 
{
    uint16_t cursorLocation = y * 80 + x;

	// Send location to VGA controller to set cursor
	// Send the high byte
	OutputByteToVideoController(0x3D4, 14);
	OutputByteToVideoController(0x3D5, cursorLocation >> 8); 
	// Send the low byte
	OutputByteToVideoController(0x3D4, 15);
	OutputByteToVideoController(0x3D5, cursorLocation);      
}

// Displays a character
void ConsoleWriteCharacter(unsigned char c) 
{
    uint16_t attribute = _colour << 8;

	if(c == '\n')
	{
		if (_cursorY == CONSOLE_HEIGHT - 1)
		{
			for(int i = 0; i < _cursorY; i++)
			{	
				unsigned short *source = _videoMemory + ((i + 1) * CONSOLE_WIDTH);
				unsigned short *destination = _videoMemory + (i * CONSOLE_WIDTH);
				memcpy(destination, source, CONSOLE_WIDTH);
			}
			memsetw(_videoMemory + (_cursorY * CONSOLE_WIDTH), 0, CONSOLE_WIDTH);
		} 
		else 
		{
			_cursorY++;
		}

		_cursorX = 0;
		return;
	}
	// Assume printable characters

    int16_t* location = _videoMemory + (_cursorY * CONSOLE_WIDTH + _cursorX);
	*location = c | attribute;
	if (_cursorX == CONSOLE_WIDTH)
	{
		ConsoleWriteCharacter('\n');
	}
	_cursorX++;
	UpdateCursorPosition(_cursorX, _cursorY);
}

// Display specified string
void ConsoleWriteString(char* str) 
{
	if (!str)
	{
		return;
	}
	while (*str)
	{
		ConsoleWriteCharacter(*str++);
	}
}

void ConsoleWriteInt(unsigned int i, unsigned int base)
{
	char result[11] = {0};
	char* ptr = result, *ptr1 = result, tmp_char;

	if (base < 2 || base > 16)
	{
		return;
	}
	
	do
	{
		*ptr++ = "0123456789abcdef"[i % base];
		i /= base;
	} while(i);

	*ptr-- = '\0';

	while(ptr1 < ptr)
	{
		tmp_char = *ptr;
		*ptr-- = *ptr1;
		*ptr1++ = tmp_char;
	}
	ConsoleWriteString(result);
}

unsigned int ConsoleSetColour(const uint8_t c)
{
	_colour = c;
	return (unsigned int) c;
}

void ConsoleGotoXY(unsigned int x, unsigned int y)
{
	_cursorX = x;
	_cursorY = y;
	UpdateCursorPosition(x, y);
}

void ConsoleGetXY(unsigned* x, unsigned* y)
{
	*x = _cursorX;
	*y = _cursorY;
}

int ConsoleGetWidth()
{
	return CONSOLE_WIDTH;
}

int ConsoleGetHeight()
{
	return CONSOLE_HEIGHT;
}

void ConsoleClearScreen(const uint8_t c)
{
	ConsoleSetColour(c);
	uint16_t attribute = c << 8;
	for(int i = 0; i < CONSOLE_HEIGHT; i++)
	{
		for(int j = 0; j < CONSOLE_WIDTH; j++)
		{
			int16_t* location = _videoMemory + (i * CONSOLE_WIDTH + j);
			*location = attribute;
		}
	}
	_cursorX = 0;
	_cursorY = 0;
	UpdateCursorPosition(_cursorX, _cursorY);
}