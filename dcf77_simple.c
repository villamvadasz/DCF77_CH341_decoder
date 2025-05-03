#include <stdio.h>
#include <string.h>

#include "dcf77.h"
#include "dcf77_meteotime.h"
#include "c_dcf77.h"

#ifdef DCF77_USE_SIMPLE_PROCESSING

uint8 dcf77_local_clock_second = 0;

//Called every 10ms with a prefiltered value
//Can be moved to main if a decoupling buffer is added
void dcf77_simple_process(unsigned char value) {
	uint8 do_dcf77_process_message = 0;
	static unsigned int dcf77_sp_sm = 0;
	static unsigned int dcf77_sp_missing_len = 0;
	static unsigned int dcf77_sp_bit_len = 0;
	static unsigned int dcf77_wait_for_some_signal = 10;
	static unsigned char value_prev = 0;

	switch (dcf77_sp_sm) {
		case 0 : {
			if (value) {
				if (dcf77_wait_for_some_signal > 0) {
					dcf77_wait_for_some_signal--;
				} else {
					dcf77_sp_sm = 1;
				}
			} else {
			}
			break;
		}
		case 1 : {
			if (value) {
				dcf77_sp_missing_len = 0;
				dcf77_sp_bit_len++;
				if (dcf77_sp_bit_len >= (1000 / (2 * DCF77_PHASE_UNDERSAMPLE) ) ) {
					dcf77_sp_bit_len = 0;
					dcf77_sp_sm = 0;
				}
			} else {
				dcf77_sp_missing_len++;
				if (dcf77_sp_missing_len >= ((1200) / (DCF77_PHASE_UNDERSAMPLE) ) ) {
					dcf77_sp_missing_len = 0;
					//detected missing second
					dcf77_local_clock_second = 0;
					#ifdef DCF77_USE_USB_TO_PRINT
						printf("X\r\n");
					#endif
					do_dcf77_process_message = 1;
				}

				if (value_prev) {
					//printf(" %u\r\n", dcf77_sp_bit_len);
					if (dcf77_sp_bit_len >= (1200 / (2 * DCF77_PHASE_UNDERSAMPLE) ) ) {
						//missing bit, this is bad.
						//printf("?");
						dcf77_sp_sm = 0;
					} else if (dcf77_sp_bit_len >= ((150) / (DCF77_PHASE_UNDERSAMPLE) ) ) {
						//Long pulse bit 1
						#ifdef DCF77_USE_USB_TO_PRINT
							printf("1");
						#endif
						dcf77_add_bitstream(1, dcf77_local_clock_second);
						dcf77_local_clock_second++;
					} else {
					//} else if (dcf77_sp_bit_len >= ((50) / (DCF77_PHASE_UNDERSAMPLE) ) ) {
						//Short pulse bit 0
						#ifdef DCF77_USE_USB_TO_PRINT
							printf("0");
						#endif
						dcf77_add_bitstream(0, dcf77_local_clock_second);
						dcf77_local_clock_second++;
					}
					
					if (dcf77_local_clock_second >= 59) {
					}
					dcf77_sp_bit_len = 0;
				}
			}
			break;
		}
		default : {
			dcf77_sp_sm = 0;
			break;
		}
	}
	value_prev = value;

	if (do_dcf77_process_message) {
		extern unsigned char dcf77_bitstream[DCF77_BITSTREAM_BYTE_CNT];
		do_dcf77_process_message = 0;
		dcf77_process_buffer(dcf77_bitstream);
	}
}

#endif
