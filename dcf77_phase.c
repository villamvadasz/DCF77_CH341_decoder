#include <stdio.h>
#include <string.h>

#include "dcf77.h"
#include "dcf77_meteotime.h"
#include "c_dcf77.h"

#include "mal.h"

#ifndef DCF77_USE_SIMPLE_PROCESSING

unsigned char dcf77_data_current[8] = {0};

#define DCF77_PHASE_NUMBER_OF_BITS (1000 * 2 / DCF77_PHASE_UNDERSAMPLE) //need to store atleas two full seconds.
#define DCF77_PHASE_BYTE_CNT ((DCF77_PHASE_NUMBER_OF_BITS + 7) / 8)
unsigned char dcf77_phase_isr[DCF77_PHASE_BYTE_CNT] = {0};
uint32 dcf77_phase_cnt_isr = 0;
unsigned char dcf77_phase_freeze[DCF77_PHASE_BYTE_CNT] = {0};
uint32 dcf77_phase_cnt_freeze = 0;

uint32 DCF77_BITSTREAM_BYTE_CNT_var = DCF77_BITSTREAM_BYTE_CNT;
uint32 DCF77_PHASE_NUMBER_OF_BITS_var = DCF77_PHASE_NUMBER_OF_BITS;
uint32 DCF77_PHASE_BYTE_CNT_var = DCF77_PHASE_BYTE_CNT;

volatile uint32 do_dcf77_extract_bits = 0;

uint8 dcf77_local_clock_second = 0;
sint32 dcf77_local_phase_isr = 0; //0...999 will lock to 500
uint8 dcf77_trigger_asyncron_calculation = 0;
uint32 dcf77_delay = 0;
uint32 dcf77_act_sum_100 = 0;
uint32 dcf77_min_sum_100 = 0xFFFFFFFF;
uint32 dcf77_at_delay = 0;
uint8 dcf77_trigger_fast_sync = 0;
uint8 dcf77_lock_status = 0;
#ifdef DCF77_USE_USB_TO_PRINT
	char dcf77_debug_text[128];
#endif
uint32 dcf77_reception_error_0 = 0;

extern uint8 dcf77_reset_done;

void dcf77_state_machine(void);
void dcf77_extract_bits(void);

void init_dcf77_phase(void) {
	memset(dcf77_phase_isr, 0xFF, sizeof(dcf77_phase_isr));
	dcf77_phase_cnt_isr = 0;
}

void do_dcf77_phase_1ms(void) {
	dcf77_state_machine();

	if (do_dcf77_extract_bits) {
		do_dcf77_extract_bits = 0;

		dcf77_extract_bits();

		extern TimeDate dcf77_timedate;
		dcf77_timedate.sec = dcf77_local_clock_second % 60;

	}
}

void isr_dcf77_phase_1ms(void) {
	dcf77_local_phase_isr++;

	if (dcf77_local_phase_isr >= 1000) {
		dcf77_local_phase_isr -= 1000;
		
		dcf77_freeze_phase();
		{//it takes 2 seconds till the change in the local time is noticable in the bit stream
			static unsigned char dcf77_dead_time = 0;
			dcf77_dead_time++;
			if (dcf77_dead_time >= 2) {
				dcf77_dead_time = 0;
				dcf77_trigger_asyncron_calculation = 1;
			}
		}
		do_dcf77_extract_bits = 1;
	}
}

void dcf77_add_phase(unsigned char value) {
	unsigned int byte_val = dcf77_phase_cnt_isr / 8;
	unsigned int bit_val = dcf77_phase_cnt_isr % 8;
	if (value) {
		dcf77_phase_isr[byte_val] |= (1 << bit_val);
	} else {
		dcf77_phase_isr[byte_val] &= ~(1 << bit_val);
	}
	dcf77_phase_cnt_isr++;
	if (dcf77_phase_cnt_isr >= DCF77_PHASE_NUMBER_OF_BITS) {
		dcf77_phase_cnt_isr = 0;
	}
}

void dcf77_freeze_phase(void) {

	//memcpy(dcf77_phase_freeze_dbg[3], dcf77_phase_freeze_dbg[2], sizeof(dcf77_phase_isr));
	//memcpy(dcf77_phase_freeze_dbg[2], dcf77_phase_freeze_dbg[1], sizeof(dcf77_phase_isr));
	//memcpy(dcf77_phase_freeze_dbg[1], dcf77_phase_freeze_dbg[0], sizeof(dcf77_phase_isr));
	//memcpy(dcf77_phase_freeze_dbg[0], dcf77_phase_isr, sizeof(dcf77_phase_isr));

	memcpy(dcf77_phase_freeze, dcf77_phase_isr, sizeof(dcf77_phase_isr));
	dcf77_phase_cnt_freeze = dcf77_phase_cnt_isr;
}

unsigned char dcf77_read_phase(uint32 x) {
	unsigned char result = 0xFF;
	if (x < DCF77_PHASE_NUMBER_OF_BITS) {
		uint32 calculated_bit = 0;
		unsigned int byte_val = 0;
		unsigned int bit_val = 0;
		calculated_bit = dcf77_phase_cnt_freeze;
		//calculated_bit -= DCF77_PHASE_NUMBER_OF_BITS;
		calculated_bit += x;
		calculated_bit %= DCF77_PHASE_NUMBER_OF_BITS;
		byte_val = calculated_bit / 8;
		bit_val = calculated_bit % 8;
		
		if (dcf77_phase_freeze[byte_val] & (1 << bit_val)) {
			result = 1;
		} else {
			result = 0;
		}
	} else {
		result = 0;
	}
	return result;
}

unsigned char dcf77_read_phase_advance_one_second(uint32 x) {
	unsigned char result = 0xFF;
	if (x < DCF77_PHASE_NUMBER_OF_BITS) {
		uint32 calculated_bit = 0;
		unsigned int byte_val = 0;
		unsigned int bit_val = 0;
		calculated_bit = dcf77_phase_cnt_freeze;
		calculated_bit += (DCF77_PHASE_NUMBER_OF_BITS / 2);//
		calculated_bit += x;
		calculated_bit %= DCF77_PHASE_NUMBER_OF_BITS;
		byte_val = calculated_bit / 8;
		bit_val = calculated_bit % 8;
		
		if (dcf77_phase_freeze[byte_val] & (1 << bit_val)) {
			result = 1;
		} else {
			result = 0;
		}
	} else {
		result = 0;
	}
	return result;
}

void dcf77_state_machine(void) {
	static unsigned int dcf77_ac_sm = 0;
	switch (dcf77_ac_sm) {
		case 0 : {
			if (dcf77_trigger_asyncron_calculation) {
				dcf77_trigger_asyncron_calculation = 0;
				if (dcf77_reset_done) {
					dcf77_delay = 0;
					dcf77_act_sum_100 = 0;
					dcf77_min_sum_100 = 0xFFFFFFFF;
					dcf77_at_delay = 0;
					if (dcf77_trigger_fast_sync) {
						dcf77_trigger_fast_sync--;
						if (dcf77_trigger_fast_sync == 0) {
							dcf77_ac_sm = 1;
						}
					} else {
						dcf77_ac_sm = 2;
					}
				} else {
					dcf77_trigger_fast_sync = 5;
				}
			}
			break;
		}
		case 1 : {
			sint32 dcf77_error = 0;
			#ifdef DCF77_USE_USB_TO_PRINT
				sint32 dcf77_error_print = 0;
			#endif
			uint16 dcf77_count_one_bits = 0;
			uint16 dcf77_count_one_bits_max = 0;
			uint32 x = 0;
			for (x = 0; x < (100 / DCF77_PHASE_UNDERSAMPLE); x++) {
				if (dcf77_read_phase(x)) {
					dcf77_count_one_bits ++;
				}
			}
			for (x =  (100 / DCF77_PHASE_UNDERSAMPLE); x < (1000 / DCF77_PHASE_UNDERSAMPLE); x++) {
				if (dcf77_read_phase(x)) {
					dcf77_count_one_bits ++;
				}
				if (dcf77_read_phase(x - (100 / DCF77_PHASE_UNDERSAMPLE) )) {
					dcf77_count_one_bits --;
				}
				if (dcf77_count_one_bits_max < dcf77_count_one_bits) {
					dcf77_count_one_bits_max = dcf77_count_one_bits;
					dcf77_at_delay = x - (100 / DCF77_PHASE_UNDERSAMPLE);
				}
			}

			dcf77_error = 500 - (dcf77_at_delay * DCF77_PHASE_UNDERSAMPLE);

			#ifdef DCF77_USE_USB_TO_PRINT
				dcf77_error_print = dcf77_error;
				sprintf(dcf77_debug_text, "\r\nFast:%i\r\n", dcf77_error_print);
				putString_usb(dcf77_debug_text);
			#endif

			lock_isr();
			dcf77_local_phase_isr += dcf77_error;
			unlock_isr();

			dcf77_ac_sm = 99;
			break;
		}
		case 2 : {
			uint32 dcf77_expectedSignalValue = 0;
			uint32 x = 0;
			if (dcf77_delay < (1000 / DCF77_PHASE_UNDERSAMPLE)) {//100us @80MHz
				dcf77_act_sum_100 = 0;

				//100   111 000
				dcf77_expectedSignalValue = 1;
				for (x = 0; x < (100 / DCF77_PHASE_UNDERSAMPLE); x++) {
					if (dcf77_read_phase(x + dcf77_delay) != dcf77_expectedSignalValue) {
						dcf77_act_sum_100 += 1;
					}
				}
				/* Not used with intend
				//dcf77_expectedSignalValue = 0;
				//for (x = (100 / DCF77_PHASE_UNDERSAMPLE); x < (200 / DCF77_PHASE_UNDERSAMPLE); x++) {
				//	if (dcf77_read_phase(x + dcf77_delay) != dcf77_expectedSignalValue) {
				//		dcf77_act_sum_100 += 1;
				//	}
				//}
				*/
				//common xxx xxx 0000000
				dcf77_expectedSignalValue = 0;
				for (x = (200 / DCF77_PHASE_UNDERSAMPLE); x < (1000 / DCF77_PHASE_UNDERSAMPLE); x++) {
					if (dcf77_read_phase(x + dcf77_delay) != dcf77_expectedSignalValue) {
						dcf77_act_sum_100 += 1;
						//dcf77_act_sum_200 += 1;
					}
				}

				if (dcf77_min_sum_100 > dcf77_act_sum_100) {
					dcf77_min_sum_100 = dcf77_act_sum_100;
					dcf77_at_delay = dcf77_delay;
				}

				dcf77_delay++;
			} else {
				if ((dcf77_local_clock_second > 58) || (dcf77_local_clock_second < 2)) {
					dcf77_at_delay = 50;
				} else {
					sint32 dcf77_error = 0;
					#ifdef DCF77_USE_USB_TO_PRINT
						sint32 dcf77_error_print = 0;
					#endif

					dcf77_error = 500 - (dcf77_at_delay * DCF77_PHASE_UNDERSAMPLE);
					#ifdef DCF77_USE_USB_TO_PRINT
						dcf77_error_print = dcf77_error;
					#endif
					if (dcf77_error > 20) {//limit change to +- 20ms
						if (dcf77_error > 50) {
							dcf77_lock_status = 0;
						}
						dcf77_error = 20;
					} else if (dcf77_error < -20) {
						if (dcf77_error < -50) {
							dcf77_lock_status = 0;
						}
						dcf77_error = -20;
					} else {
						dcf77_error	/= 2;
						dcf77_lock_status = 1;
					}
					lock_isr();
					dcf77_local_phase_isr += dcf77_error;
					unlock_isr();


					//Debug LED
					if (dcf77_lock_status) {
						//TRISGbits.TRISG6 = 0;
						//LATGbits.LATG6 = 1;
					} else {
						#ifdef DCF77_USE_USB_TO_PRINT
							sprintf(dcf77_debug_text, "\r\nSlow:%i\r\n", dcf77_error_print);
							putString_usb(dcf77_debug_text);
						#endif

						//TRISGbits.TRISG6 = 0;
						//LATGbits.LATG6 = 0;
					}


					//dcf77_phase_lock_debug[dcf77_phase_lock_debug_cnt] = dcf77_at_delay;

					//dcf77_phase_lock_debug_cnt++;
					//if (dcf77_phase_lock_debug_cnt >= (sizeof(dcf77_phase_lock_debug) / sizeof(*dcf77_phase_lock_debug)) ) {
					//	dcf77_phase_lock_debug_cnt = 0;
					//}

					{
						static int dbg_holder = 0;
						dbg_holder++;
						if (dbg_holder >= 32) {
							dbg_holder = 0;
						}
					}

				}

				dcf77_ac_sm = 99;
			}
			break;
		}
		case 99 : {
			dcf77_trigger_asyncron_calculation = 0;
			dcf77_ac_sm = 0;
			break;
		}
		default : {
			dcf77_ac_sm = 0;
			dcf77_trigger_asyncron_calculation = 0;
			break;
		}
	}			
}

void dcf77_extract_bits(void) {
	uint8 do_dcf77_process_message = 0;
	//Now dcf77_local_clock_ms matches the DCF77 1 s pulse signal.
	uint32 x = 0;
	static uint8 dcf77_second_clear = 0;
	static uint32 highBitCount_100 = 0;
	static uint32 highBitCount_200 = 0;
	uint8 dcf77_read_bit_value = 0;

	dcf77_local_clock_second++;

	if (dcf77_local_clock_second >= 65) {
		dcf77_local_clock_second = 0;
	}

	if (dcf77_second_clear) {
		dcf77_second_clear--;
	}

	if (dcf77_lock_status) {
		for (x = (500 / DCF77_PHASE_UNDERSAMPLE); x < (600 / DCF77_PHASE_UNDERSAMPLE); x++) {
			dcf77_read_bit_value = dcf77_read_phase_advance_one_second(x);
			if (dcf77_read_bit_value) {
				highBitCount_100++;
			}
		}
		for (x = (600 / DCF77_PHASE_UNDERSAMPLE); x < (700 / DCF77_PHASE_UNDERSAMPLE); x++) {
			dcf77_read_bit_value = dcf77_read_phase_advance_one_second(x);
			if (dcf77_read_bit_value) {
				highBitCount_200++;
			}
		}
	
		if ((highBitCount_100 >= ((DCF77_LENGTH_ZERO / DCF77_PHASE_UNDERSAMPLE) / 2)) && (highBitCount_200 < ((DCF77_LENGTH_ZERO / DCF77_PHASE_UNDERSAMPLE) / 2))) {
			dcf77_add_bitstream(0, dcf77_local_clock_second - 1);
			#ifdef DCF77_USE_USB_TO_PRINT
				putChar_usb('0');
			#endif
		} else if ((highBitCount_100 >= ((DCF77_LENGTH_ZERO / DCF77_PHASE_UNDERSAMPLE) / 2)) && (highBitCount_200 >= ((DCF77_LENGTH_ZERO/ DCF77_PHASE_UNDERSAMPLE) / 2))) {
			dcf77_add_bitstream(1, dcf77_local_clock_second - 1);
			#ifdef DCF77_USE_USB_TO_PRINT
				putChar_usb('1');
			#endif
		} else if ((highBitCount_100 < ((DCF77_LENGTH_ZERO / DCF77_PHASE_UNDERSAMPLE) / 2)) && (highBitCount_200 < ((DCF77_LENGTH_ZERO/ DCF77_PHASE_UNDERSAMPLE) / 2))) {
			if (dcf77_second_clear == 0) {
				#ifdef DCF77_USE_USB_TO_PRINT
					putChar_usb('X');
					putChar_usb('\r');
					putChar_usb('\n');
				#endif
				dcf77_second_clear = 50;
				dcf77_local_clock_second = 0;
				do_dcf77_process_message = 1;
			}
			//missing bit
		} else {
			dcf77_reception_error_0++;
		}
		highBitCount_100 = 0;
		highBitCount_200 = 0;
	
		if (do_dcf77_process_message) {
			unsigned int x = 0;
	
			do_dcf77_process_message = 0;
	
			memset(dcf77_data_current, 0x00, sizeof(dcf77_data_current)/sizeof(*dcf77_data_current));
	
			for (x = 0; x < 60; x++) {
				unsigned int maskForSet = 1;
	
				if (dcf77_read_bitstream(x) == 1) {
					maskForSet <<= (x % 8);
					dcf77_data_current[x / 8] |= maskForSet;
				}
			}
	
			dcf77_process_buffer(dcf77_data_current);
		}
	}
}

#endif
