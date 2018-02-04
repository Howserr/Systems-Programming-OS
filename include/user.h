#ifndef _USER_H
#define _USER_H
#include <keyboard.h>

void User_ConsoleWriteCharacter(unsigned char c); 
void User_ConsoleWriteString(char* str); 
void User_ConsoleWriteInt(unsigned int i, unsigned int base); 
void User_ConsoleClearScreen(const uint8_t c); 
void User_ConsoleGotoXY(unsigned int x, unsigned int y);

void User_SetResolution(int width, int height, uint8_t chainMode);
void User_SetColourPalette(int red, int green, int blue, int index);
void User_SetLineStyle(int dashLength);
void User_ClearScreen();
void User_SetPixel(unsigned int x, unsigned int y, uint8_t colour);
void User_MoveTo(unsigned int x, unsigned int y);
void User_DrawChar(char c, uint8_t colour);
void User_DrawString(const char* str, uint8_t colour);
void User_LineTo(unsigned int x, unsigned int y, uint8_t colour);
void User_DrawRectangle(int x, int y, int width, int height, uint8_t colour);
void User_FillRectangle(int x, int y, int width, int height, uint8_t colour);
void User_DrawCircle(int radius, uint8_t colour);
void User_FillCircle(int radius, uint8_t colour);
void User_DrawPolygon(int *coordinates, int numberOfVertices, uint8_t colour);
void User_FillPolygon(int *coordinates, int numberOfVertices, uint8_t colour);
void User_FloodFill(int startX, int startY, uint8_t replacementColour);

#endif