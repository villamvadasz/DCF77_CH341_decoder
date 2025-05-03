#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <stdbool.h>
#include <conio.h>

#include "dcf77.h"

#pragma comment(lib, "winmm.lib")

typedef HANDLE WINAPI (*TCH341OpenDevice)(ULONG DevI);
typedef ULONG WINAPI (*TCH341GetVerIC)(ULONG iIndex);
typedef BOOL WINAPI(*TCH341GetInput)(ULONG iIndex, PULONG iStatus);
typedef BOOL WINAPI(*TCH341SetOutput)(ULONG iIndex, ULONG iEnable, ULONG iSetDirOut, ULONG iSetDataOut);
TCH341OpenDevice CH341OpenDevice = NULL;
TCH341GetVerIC CH341GetVerIC = NULL;
TCH341GetInput CH341GetInput = NULL;
TCH341SetOutput CH341SetOutput = NULL;

//TCO is D0
//PON is D3

ULONG ch341_iIndex = 0;
ULONG ch341_iEnable = 0xC;
ULONG ch341_iSetDirOut = 0;//0b10 PIN0 is Input, PIN1 is Output (not 1 is out, 0 is in, strange)
ULONG ch341_iSetDataOut = 0;
volatile unsigned int running = 1;

void DCF77_PON(unsigned int value);
unsigned int DCF77_TCO(void);
void sleep_microseconds(unsigned int microsecs);

void CALLBACK TimerProc(UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2) {
	do_dcf77();
	isr_dcf77_1ms();
	do_dcf77();
}

int main() {
    HINSTANCE hDll = LoadLibrary("CH341DLL.DLL");
    if (hDll == NULL) {
        printf("Could not load the DLL\n");
        return 1;
    }

    CH341OpenDevice = (TCH341OpenDevice)GetProcAddress(hDll, "CH341OpenDevice");
    CH341GetVerIC = (TCH341GetVerIC)GetProcAddress(hDll, "CH341GetVerIC");
    CH341GetInput = (TCH341GetInput)GetProcAddress(hDll, "CH341GetInput");
    CH341SetOutput = (TCH341SetOutput)GetProcAddress(hDll, "CH341SetOutput");
    if (
		(CH341OpenDevice == NULL) ||
		(CH341GetInput == NULL) ||
		(CH341SetOutput == NULL) ||
		0
	) {
        printf("Could not locate the function\n");
        FreeLibrary(hDll);
        return 1;
    }

	ch341_iIndex = 0;
    HANDLE handle = CH341OpenDevice(ch341_iIndex);  // Call the function
	if (handle == INVALID_HANDLE_VALUE) {
        printf("Could not open device\n");
        FreeLibrary(hDll);
        return 1;
	} else {
		ULONG version = CH341GetVerIC(ch341_iIndex);
		//printf("Version: 0x%X\r\n", version);
		
		ch341_iEnable = 0xC;

		init_dcf77();
		
		extern void dcf77_meteotime_full_day(void);
		//dcf77_meteotime_full_day();
		running = 1;
		
		timeBeginPeriod(1); // Request 1ms resolution
		UINT timerId = timeSetEvent(1, 0, TimerProc, 0, TIME_PERIODIC);
		
		while (running) {
			if (_kbhit()) {
				running = 0;
			}
			Sleep(10);
		}
		timeKillEvent(timerId);
		timeEndPeriod(1);
	}

    FreeLibrary(hDll);
    return 0;
}

void putChar_usb(char ch) {
	printf("%c", ch);
}

void putString_usb(char *str) {
	printf("%s", str);
}

void DCF77_usb_PON(unsigned int value) {
	ch341_iSetDirOut |= 0x8;//0b10 PIN0 is Input, PIN1 is Output (not 1 is out, 0 is in, strange)
	if (value) {
		//printf("PON 1\n");
		ch341_iSetDataOut |= 0x8;
	} else {
		//printf("PON 0\n");
		ch341_iSetDataOut &= ~0x8;
	}

	BOOL retVal = CH341SetOutput(ch341_iIndex, ch341_iEnable, ch341_iSetDirOut, ch341_iSetDataOut);
	if (retVal == false) {
		printf("Could not set output\n");
	}

}

unsigned int DCF77_usb_TCO(void) {
	unsigned int retVal = 0;
	ULONG iStatus = 0;
	BOOL retVal_CH341GetInput = CH341GetInput(ch341_iIndex, &iStatus);
	if (retVal_CH341GetInput == false) {
		printf("Could not get input\n");
	}
	if (iStatus & 0x1) {
		retVal = 1;
	} else {
		retVal = 0;
	}
	return retVal;
}

void print_at(int row, int col, const char *text) {
    // ANSI escape sequence: \033[row;colH
    printf("\033[%d;%dH%s", row, col, text);
}

int sprint_binary(char *str, unsigned int value, unsigned int bits) {
	unsigned int x = 0;
	for (x = 0; x < bits; x++) {
		unsigned int v = value & (1 << ((bits - 1)-x));
		if (v) {
			sprintf(str, "%s1", str);
		} else {
			sprintf(str, "%s0", str);
		}
	}
	return bits;
}

void print_binary(unsigned int value, unsigned int bits) {
	unsigned int x = 0;
	for (x = 0; x < bits; x++) {
		unsigned int v = value & (1 << x);
		if (v) {
			printf("1");
		} else {
			printf("0");
		}
	}
}

void sleep_microseconds(unsigned int microsecs) {
    LARGE_INTEGER freq, start, now;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);

    LONGLONG wait_ticks = (freq.QuadPart * microsecs) / 1000000;

    do {
        QueryPerformanceCounter(&now);
    } while ((now.QuadPart - start.QuadPart) < wait_ticks);
}
