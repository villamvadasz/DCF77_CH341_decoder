#ifndef _DCF77_H_
#define _DCF77_H_

	#include "rtc.h"
	
	#define DCF77_NUMBER_OF_BITS 60
	#define DCF77_MAX_LENGTH_OF_LOW_VALUE 200
	#define DCF77_MAX_LENGTH_OF_HIGH_VALUE 1900
	#define DCF77_LENGTH_NONE 1900
	#define DCF77_LENGTH_ONE 200
	#define DCF77_LENGTH_ZERO 100
	#define DCF77_NUMBER_OF_BITS 60
	#define DCF77_PHASE_UNDERSAMPLE 10
	#define DCF77_BITSTREAM_BYTE_CNT ((DCF77_NUMBER_OF_BITS + 7) / 8)

	extern uint8 dcf77_port_value_isr;

	extern unsigned int dcf77_getTime(TimeDate *ptr);

	//Internal interface
	extern uint8 dcf77_check_buffer_validity(unsigned char *dcf77_data);
	extern unsigned char dcf77_get_bit(unsigned char *dcf77_data, unsigned int bitNr);
	extern uint8 dcf77_get_buffer_summer_winter_time(unsigned char *dcf77_data);
	extern uint16 dcf77_get_buffer_year(unsigned char *dcf77_data);
	extern uint8 dcf77_get_buffer_month(unsigned char *dcf77_data);
	extern uint8 dcf77_get_buffer_day(unsigned char *dcf77_data);
	extern uint8 dcf77_get_buffer_hour(unsigned char *dcf77_data);
	extern uint8 dcf77_get_buffer_min(unsigned char *dcf77_data);
	extern uint32 dcf77_get_buffer_packet_cnt(unsigned char *dcf77_data);
	
	extern void dcf77_add_bitstream(uint8 value, uint8 sec);
	extern unsigned char dcf77_read_bitstream(uint32 sec);

	extern void dcf77_add_phase(unsigned char value);
	extern void dcf77_freeze_phase(void);
	extern unsigned char dcf77_read_phase(uint32 x);
	extern unsigned char dcf77_read_phase_advance_one_second(uint32 x);

	extern void dcf77_process_buffer(unsigned char *dcf77_data);

	extern void dcf77_pon(void);

	extern void init_dcf77_phase(void);
	extern void do_dcf77_phase_1ms(void);
	extern void isr_dcf77_phase_1ms(void);
	
	extern void dcf77_set_PON(unsigned char value);
	extern unsigned char dcf77_get_TCO(void);
	
	extern void init_dcf77(void);
	extern void do_dcf77(void);
	extern void isr_dcf77_1ms(void);

#endif
