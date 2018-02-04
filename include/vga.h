#ifndef _VGA_H
#define _VGA_H

#include <stdint.h>

void SetResolution(int width, int height, uint8_t chainMode);
void SetColourPalette(uint8_t red, uint8_t green, uint8_t blue, int index);
void SetLineStyle(int dashLength);
void ClearScreen();
void SetPixel(unsigned int x, unsigned int y, unsigned int colour);
bool MoveTo(int x, int y);
void DrawChar(char c, uint8_t colour);
void DrawString(const char* str, uint8_t colour);
bool LineTo(int x, int y, uint8_t colour);
void DrawRectangle(int width, int height, uint8_t colour);
void FillRectangle(int width, int height, uint8_t colour);
void DrawCircle(int radius, uint8_t colour);
void FillCircle(int radius, uint8_t colour);
void DrawPolygon(int *coordinates, int numberOfVertices, uint8_t colour);
void FillPolygon(int *coordinates, int numberOfVertices, uint8_t colour);
void FloodFill(int startX, int startY, uint8_t replacementColour);

#endif