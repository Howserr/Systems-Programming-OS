#include <string.h>
#include <console.h>
#include <hal.h>
#include <exception.h>
#include "physicalmemorymanager.h"
#include "virtualmemorymanager.h"
#include "bootinfo.h"
#include <keyboard.h>
#include <sysapi.h>
#include <vgaapi.h>
#include <user.h>
#include <vgamodes.h>

// This is a dummy __main.  For some reason, gcc puts in a call to 
// __main from main, so we just include a dummy.
 
BootInfo *	_bootInfo;
 
void __main() {}

void InitialisePhysicalMemory()
{
	// Initialise the physical memory manager
	// We place the memory bit map in the next available block of memory after the kernel.
	// Note that all memory addresses here are physical memory addresses since we are dealing
	// with management of physical memory

	uint32_t memoryMapAddress = 0x100000 + _bootInfo->KernelSize;
	if (memoryMapAddress % PMM_GetBlockSize() != 0)
	{
		// Make sure that the memory map is going to be aligned on a block boundary
		memoryMapAddress = (memoryMapAddress / PMM_GetBlockSize() + 1) * PMM_GetBlockSize();
	}
	uint32_t sizeOfMemoryMap = PMM_Initialise(_bootInfo, memoryMapAddress);

	// We now need to mark various regions as unavailable
	
	// Mark first page as unavailable (so that a null pointer is invalid)
	PMM_MarkRegionAsUnavailable(0x00, PMM_GetBlockSize());

	// Mark memory used by kernel as unavailable
	PMM_MarkRegionAsUnavailable(0x100000, _bootInfo->KernelSize);

	// Mark memory used by memory map as unavailable
	PMM_MarkRegionAsUnavailable(PMM_GetMemoryMap(), sizeOfMemoryMap);

	// Reserve two blocks for the stack and make unavailable (the stack is set at 0x90000 in boot loader)
	uint32_t stackSize = PMM_GetBlockSize() * 2;
	PMM_MarkRegionAsUnavailable(_bootInfo->StackTop - stackSize, stackSize);
	
	// Reserve two block for the kernel stack and make unavailable
	PMM_MarkRegionAsUnavailable(0x80000 - stackSize, stackSize);
}
void Initialise()
{
	ConsoleClearScreen(0x1F);
	ConsoleWriteString("UODOS 32-bit Kernel. Kernel size is ");
	ConsoleWriteInt(_bootInfo->KernelSize, 10);
	ConsoleWriteString(" bytes\n");
	HAL_Initialise();
	InitialisePhysicalMemory();
	VMM_Initialise();
	KeyboardInstall(33); 
	InitialiseSysCalls();
	InitialiseVgaCalls();
}

void main(BootInfo * bootInfo) 
{
	_bootInfo = bootInfo;
	Initialise();
	// Uncomment out the following line when you have code that is capable of being running
	// in ring 3
	HAL_EnterUserMode();
	
	// Uncomment out the following line to switch into VGA mode 13h
	// Note that the screen will be full of rubbish until you clear the screen
	// since it will just display what is in memory starting at 0xA0000.
	VGA_SetGraphicsMode(360, 300, 0);
	User_SetResolution(360, 300, 0);
	User_ClearScreen();

	// New colour (white-ish) and title text
	User_SetColourPalette(60, 60, 60, 16);
	User_MoveTo(130, 10);
	User_DrawString("Demonstration", 16);

	//Set of lines, top left
	User_MoveTo(10, 10);
	User_LineTo(115, 10, 1);

	User_SetLineStyle(1);
	User_MoveTo(10, 20);
	User_LineTo(115, 20, 2);

	User_SetLineStyle(2);
	User_MoveTo(10, 30);
	User_LineTo(115, 30, 3);

	User_SetLineStyle(3);
	User_MoveTo(10, 40);
	User_LineTo(115, 40, 4);

	// Set of squares middle left
	User_SetLineStyle(1);
	User_DrawRectangle(10, 50, 50, 40, 5);

	User_SetLineStyle(3);
	User_DrawRectangle(65, 50, 50, 40, 6);

	User_SetLineStyle(0);
	User_DrawRectangle(10, 95, 50, 40, 7);

	User_FillRectangle(65, 95, 50, 40, 8);

	// Pair of circles top right
	User_SetLineStyle(1);
	User_MoveTo(160, 50);
	User_DrawCircle(25, 9);

	User_MoveTo(220, 50);
	User_FillCircle(25, 10);

	// Triplet of diamonds middle left
	int poly1[] = {150, 80, 170, 100, 150, 120, 130, 100};
	User_DrawPolygon(poly1, 4, 11);

	int poly2[] = {200, 80, 220, 100, 200, 120, 180, 100};
	User_SetLineStyle(0);
	User_DrawPolygon(poly2, 4, 11);

	// Flood fill does not work with chain mode so disabling this section of the demo
	// The use of GetPixel is causing the OS to repeatedly boot cycle however I can not determine the cause
	// As I have scanline for filling polygons I decided it is not worth the effort to debug this
	// especially seeing as unchained support was a last miniute addition
	//User_MoveTo(230, 72);
	//User_DrawString("Flood fill", 12);

	//int poly3[] = {250, 80, 270, 100, 250, 120, 230, 100};
	//User_DrawPolygon(poly3, 4, 12);
	//User_FloodFill(235, 100, 12);
	
	// Two concave polygons bottom
	User_SetColourPalette(43, 6, 55, 17);
	User_MoveTo(10, 140);
	User_DrawString("Scanline Fill", 17);

	int poly4[] = { 10, 150, 100, 190, 100, 150, 10, 190 };
	User_FillPolygon(poly4, 4, 17);

	User_SetColourPalette(55, 30, 5, 18);
	int poly5[] = { 110, 170, 140, 150, 160, 165, 190, 150, 190, 190, 160, 175, 140, 190 };
	User_FillPolygon(poly5, 7, 18);
}