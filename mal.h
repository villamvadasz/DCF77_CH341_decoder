#ifndef _MAL_H_
#define _MAL_H_

	#define lock_isr()	;
	#define unlock_isr()	;

	extern unsigned int DCF77_usb_TCO(void);
	extern void DCF77_usb_PON(unsigned int value);
	extern void putChar_usb(char ch);
	extern void putString_usb(char *str);
	extern void print_at(int row, int col, const char *text);
	extern void print_binary(unsigned int value, unsigned int bits);
	extern int sprint_binary(char *str, unsigned int value, unsigned int bits);
	
#endif
