#include <hal.h>
#include <console.h>
#include <keyboard.h>

#define MAX_SYSCALL 5

typedef struct _SysCallInfo
{
	void * SysCall;
	int    ParamCount;
} SysCallInfo;

SysCallInfo _SysCalls[MAX_SYSCALL]; 

void InitialiseSysCall(int index, void * sysCall, int paramCount)
{
	if (index >= 0 && index < MAX_SYSCALL)
	{
		_SysCalls[index].SysCall = sysCall;
		_SysCalls[index].ParamCount = paramCount;
	}
}

// The new interrupt attribute has not been used here since
// the code is so specialised.

void SyscallDispatcher() 
{
	static int index = 0;
	// Get index into _SysCalls table from eax
	asm volatile("movl %%eax, %0" : "=r" (index));

	if (index < MAX_SYSCALL)
	{
		// Temporarily save the registers that are used to pass in the parameters
		asm volatile ("push %edx\n\t"
					  "push %ecx\n\t"
					  "push %ebx\n\t");
		void * sysFunction = _SysCalls[index].SysCall;
		int paramCount = _SysCalls[index].ParamCount;
		// Now generate the code for the user call.  There is different
		// code depending on how many parameters are passed to the function.
		// After the call to the kernel routine, we remove the parameters from teh
		// stack by adjusting the stack pointer.  This is the standard C calling convention.
		switch (paramCount)
		{
			case 3:
				asm volatile ("pop %%ebx\n\t"
							  "pop %%ecx\n\t"
							  "pop %%edx\n\t"
							  "push %%edx\n\t"
							  "push %%ecx\n\t"
							  "push %%ebx\n\t"
							  "call *%0\n\t"
							  "addl $12, %%esp"
							  : : "r"(sysFunction)
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
							  : : "r"(sysFunction)
							  );
							  break;
							  
			case 1:
				asm volatile ("pop %%ebx\n\t"
							  "pop %%ecx\n\t"
							  "pop %%edx\n\t"
							  "push %%ebx\n\t"
							  "call *%0\n\t"
							  "addl $4, %%esp"
							  : : "r"(sysFunction)
							  );
							  break;
							  
			case 0:
				asm volatile ("pop %%ebx\n\t"
							  "pop %%ecx\n\t"
							  "pop %%edx\n\t"
							  "call *%0\n\t"
							  : : "r"(sysFunction)
							  );
							  break;
				 
		}
	}
	asm("leave");
	asm("iret");
}

#define I86_IDT_DESC_RING3		0x60

void InitialiseSysCalls() 
{
	InitialiseSysCall(0, ConsoleWriteString, 1);
	InitialiseSysCall(1, ConsoleWriteCharacter, 1);
	InitialiseSysCall(2, ConsoleWriteInt, 2);
	InitialiseSysCall(3, ConsoleClearScreen, 1);
	InitialiseSysCall(4, ConsoleGotoXY, 2);
	// Install interrupt handler!
	HAL_SetInterruptVector(0x80, SyscallDispatcher, I86_IDT_DESC_RING3);
}

