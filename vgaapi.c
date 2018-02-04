#include <vgaapi.h>
#include <vga.h>
#include <hal.h>
#include <console.h>

#define MAX_VGACALL 16

typedef struct _VgaCallInfo
{
	void * VgaCall;
	int    ParamCount;
} VgaCallInfo;

VgaCallInfo _VgaCalls[MAX_VGACALL]; 

void InitialiseVgaCall(int index, void * vgaCall, int paramCount)
{
	if (index >= 0 && index < MAX_VGACALL)
	{
		_VgaCalls[index].VgaCall = vgaCall;
		_VgaCalls[index].ParamCount = paramCount;
	}
}

// The new interrupt attribute has not been used here since
// the code is so specialised.

void VgacallDispatcher() 
{
	static int index = 0;
	// Get index into _SysCalls table from eax
	asm volatile("movl %%eax, %0" : "=r" (index));

	if (index < MAX_VGACALL)
	{
		// Temporarily save the registers that are used to pass in the parameters
		asm volatile ("push %esi\n\t"
					  "push %edx\n\t"
					  "push %ecx\n\t"
					  "push %ebx\n\t");
		void * vgaFunction = _VgaCalls[index].VgaCall;
		int paramCount = _VgaCalls[index].ParamCount;
		// Now generate the code for the user call.  There is different
		// code depending on how many parameters are passed to the function.
		// After the call to the kernel routine, we remove the parameters from teh
		// stack by adjusting the stack pointer.  This is the standard C calling convention.
		switch (paramCount)
		{
			case 4:
			// We can now pass 4 parameters by making use of ESI
				asm volatile ("pop %%ebx\n\t"
							  "pop %%ecx\n\t"
							  "pop %%edx\n\t"
							  "pop %%esi\n\t"
							  "push %%esi\n\t"
							  "push %%edx\n\t"
							  "push %%ecx\n\t"
							  "push %%ebx\n\t"
							  "call *%0\n\t"
							  "addl $16, %%esp"
							  : : "r"(vgaFunction)
							  );
							  break;
			case 3:
				asm volatile ("pop %%ebx\n\t"
							  "pop %%ecx\n\t"
							  "pop %%edx\n\t"
							  "push %%edx\n\t"
							  "push %%ecx\n\t"
							  "push %%ebx\n\t"
							  "call *%0\n\t"
							  "addl $12, %%esp"
							  : : "r"(vgaFunction)
							  );
							  break;
				
			case 2:
				asm volatile ("pop %%ebx\n\t"
							  "pop %%ecx\n\t"
							  "pop %%edx\n\t"
							  "push %%ecx\n\t"
							  "push %%ebx\n\t"
							  "call *%0\n\t"
							  "addl $8, %%esp"
							  : : "r"(vgaFunction)
							  );
							  break;
							  
			case 1:
				asm volatile ("pop %%ebx\n\t"
							  "pop %%ecx\n\t"
							  "pop %%edx\n\t"
							  "push %%ebx\n\t"
							  "call *%0\n\t"
							  "addl $4, %%esp"
							  : : "r"(vgaFunction)
							  );
							  break;
							  
			case 0:
				asm volatile ("pop %%ebx\n\t"
							  "pop %%ecx\n\t"
							  "pop %%edx\n\t"
							  "call *%0\n\t"
							  : : "r"(vgaFunction)
							  );
							  break;
				 
		}
	}
	asm("leave");
	asm("iret");
}

#define I86_IDT_DESC_RING3		0x60

void InitialiseVgaCalls() 
{
	// Initialise all methods used by User.c
	InitialiseVgaCall(0, SetResolution, 3);
	InitialiseVgaCall(1, SetColourPalette, 4);
	InitialiseVgaCall(2, SetLineStyle, 1);
	InitialiseVgaCall(3, ClearScreen, 0);
	InitialiseVgaCall(4, SetPixel, 3);
	InitialiseVgaCall(5, MoveTo, 2);
	InitialiseVgaCall(6, DrawChar, 2);
	InitialiseVgaCall(7, DrawString, 2);
	InitialiseVgaCall(8, LineTo, 3);
	InitialiseVgaCall(9, DrawRectangle, 3);
	InitialiseVgaCall(10, FillRectangle, 3);
	InitialiseVgaCall(11, DrawCircle, 2);
	InitialiseVgaCall(12, FillCircle, 2);
	InitialiseVgaCall(13, DrawPolygon, 3);
	InitialiseVgaCall(14, FillPolygon, 3);
	InitialiseVgaCall(15, FloodFill, 3);

	// Install interrupt handler
	HAL_SetInterruptVector(0x81, VgacallDispatcher, I86_IDT_DESC_RING3);
}