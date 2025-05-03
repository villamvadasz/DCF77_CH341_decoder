#include <stdio.h>
#include <string.h>

#include "dcf77.h"
#include "dcf77_meteotime.h"
#include "c_dcf77.h"
#include "rtc.h"
#include "k_stdtype.h"
#include "mal.h"

#ifdef DCF77_USE_USB_TO_PRINT
	#include "serial_usb.h"
#endif

#define DCF77_VALIDITY_TIMEPERIOD 120000

unsigned char dcf77_bitstream[DCF77_BITSTREAM_BYTE_CNT] = {0};

uint8 dummyPORT_DCF77 = 0;
uint8 dummyTRIS_DCF77 = 0;
volatile unsigned int do_dcf77_1ms = 0;
uint8 dcf77_port_value_isr = 0;

TimeDate dcf77_timedate;
uint32 dcf77_timedate_valid = 0;

void dcf77_filter_port_isr(void);
void dcf77_simple_process(unsigned char value);

__attribute__(( weak )) void init_dcf77_meteotime(void) {}
__attribute__(( weak )) void do_dcf77_meteotime(void) {}
__attribute__(( weak )) void isr_dcf77_meteotime_1ms(void) {}
__attribute__(( weak )) void dcf77_meteotime_packet_add_received_message(unsigned char *dcf77_data) {}
__attribute__(( weak )) uint8 dcf77_meteotime_decrypt(unsigned char *encrypted, unsigned char *decrypted) {return 0;}

		
void init_dcf77(void) {
	memset(dcf77_bitstream, 0xFF, sizeof(dcf77_bitstream));

	#ifndef DCF77_TCO
		DCF77_TCO_TRIS = 1;
	#endif
	
	dcf77_set_PON(1);
	
	#ifndef DCF77_USE_SIMPLE_PROCESSING
		init_dcf77_phase();
	#endif
	init_dcf77_meteotime();
	
}

void do_dcf77(void) {
	do_dcf77_meteotime();
	if (do_dcf77_1ms) {
		do_dcf77_1ms = 0;

		/*{
			//Debug LED
			if (DCF77_TCO_PORT) {
				TRISGbits.TRISG6 = 0;
				LATGbits.LATG6 = 1;
			} else {
				TRISGbits.TRISG6 = 0;
				LATGbits.LATG6 = 0;
			}
		}*/

		dcf77_pon();
		
		if (dcf77_timedate_valid) {
			dcf77_timedate_valid--;
		}
		
		#ifndef DCF77_USE_SIMPLE_PROCESSING
			do_dcf77_phase_1ms();
		#endif
	}
}

void isr_dcf77_1ms(void) {
	dcf77_filter_port_isr();

	isr_dcf77_meteotime_1ms();

	#ifndef DCF77_USE_SIMPLE_PROCESSING
		isr_dcf77_phase_1ms();
	#endif
	
	do_dcf77_1ms = 1;
}

unsigned int dcf77_getTime(TimeDate *ptr) {
	unsigned int result = 0;
	if (ptr != NULL) {
		*ptr = dcf77_timedate;
		if (dcf77_timedate_valid) {
			result = 1;
		}
	}
	return result;
}

void dcf77_filter_port_isr(void) {
	static uint8 dcf77_collect_bits = 0;
	static uint32 dcf77_undersample_cnt = 0;

	//Filter PORT signal
	if (dcf77_get_TCO()) {
		#ifdef DCF77_POLLIN_INVERTED_SIGNAL
			dcf77_port_value_isr = 1;
		#else
			dcf77_port_value_isr = 0;
		#endif
	} else {
		#ifdef DCF77_POLLIN_INVERTED_SIGNAL
			dcf77_port_value_isr = 0;
		#else
			dcf77_port_value_isr = 1;
		#endif
	}

	if (dcf77_port_value_isr) {
		dcf77_collect_bits++;
	}	

	dcf77_undersample_cnt++;
	if (dcf77_undersample_cnt >= DCF77_PHASE_UNDERSAMPLE) {
		uint8 dcf77_port_value_isr_temp = 0;

		dcf77_undersample_cnt = 0;

		if (dcf77_collect_bits >= (DCF77_PHASE_UNDERSAMPLE / 2)) {
			dcf77_port_value_isr_temp = 1;
		} else {
			dcf77_port_value_isr_temp = 0;
		}

		#ifdef DCF77_USE_SIMPLE_PROCESSING
			dcf77_simple_process(dcf77_port_value_isr_temp);
		#else
			dcf77_add_phase(dcf77_port_value_isr_temp);
		#endif

		dcf77_collect_bits = 0;
	}

}

void dcf77_process_buffer(unsigned char *dcf77_data) {
	if (dcf77_data != NULL) {
		if (dcf77_check_buffer_validity(dcf77_data)) {
			//Parity is ok, and some of the bits too
			unsigned char z1_17 = dcf77_get_bit(dcf77_data, 17);
			unsigned char z2_18 = dcf77_get_bit(dcf77_data, 18);

			//unsigned char dofw_42 = dcf77_get_bit(dcf77_data, 42);
			//unsigned char dofw_43 = dcf77_get_bit(dcf77_data, 43);
			//unsigned char dofw_44 = dcf77_get_bit(dcf77_data, 44);
			#ifdef DCF77_USE_USB_TO_PRINT
				putChar_usb('O');
				putChar_usb('K');
				putChar_usb('\r');
				putChar_usb('\n');
			#endif
			
			//DCF77 is sending UTC+1 or UTC+2
			extern uint8 dcf77_local_clock_second;

			dcf77_timedate.sec = dcf77_local_clock_second;	
			dcf77_timedate.min = dcf77_get_buffer_min(dcf77_data);
			dcf77_timedate.hour = dcf77_get_buffer_hour(dcf77_data);
			dcf77_timedate.day = dcf77_get_buffer_day(dcf77_data);
			dcf77_timedate.month = dcf77_get_buffer_month(dcf77_data);
			dcf77_timedate.year = dcf77_get_buffer_year(dcf77_data);
			
			//But we need to provide UTC so we have to remove this
			if (z1_17) {//Summertime +2
				rtc_LocalTimeToUTC(&dcf77_timedate, 1, 1);
			}
			if (z2_18) {//Wintertime +1
				rtc_LocalTimeToUTC(&dcf77_timedate, 1, 0);
			}
			//Now we have UTC
			rtc_update_dofw_and_week(&dcf77_timedate);
			dcf77_timedate_valid = DCF77_VALIDITY_TIMEPERIOD;
			
			//Process weather data
			dcf77_meteotime_packet_add_received_message(dcf77_data);

			#ifdef DCF77_USE_USB_TO_PRINT
				putChar_usb('\r');
				putChar_usb('\n');
				rtc_print_time(&dcf77_timedate);
				putChar_usb('\r');
				putChar_usb('\n');
			#endif
		}
	}
}

unsigned char dcf77_get_bit(unsigned char *dcf77_data, unsigned int bitNr) {
	unsigned char result = 0;
	if (dcf77_data != NULL) {
		unsigned char nrByte = bitNr / 8;
		unsigned char nrBit = bitNr % 8;
		unsigned char byteRead = dcf77_data[nrByte];
		byteRead >>= nrBit;
		byteRead &= 0x01;
		if (byteRead) {
			result = 1;
		}
	}
	return result;
}

void dcf77_add_bitstream(uint8 value, uint8 sec) {
	if (sec <= DCF77_NUMBER_OF_BITS) {
		unsigned int byte_val = sec / 8;
		unsigned int bit_val = sec % 8;
		if (value) {
			dcf77_bitstream[byte_val] |= (1 << bit_val);
		} else {
			dcf77_bitstream[byte_val] &= ~(1 << bit_val);
		}
	}
}

unsigned char dcf77_read_bitstream(uint32 sec) {
	unsigned char result = 0xFF;
	if (sec <= DCF77_NUMBER_OF_BITS) {
		unsigned int byte_val = sec / 8;
		unsigned int bit_val = sec % 8;

		if (dcf77_bitstream[byte_val] & (1 << bit_val)) {
			result = 1;
		} else {
			result = 0;
		}
	}
	return result;
}

void dcf77_set_PON(unsigned char value) {
	#ifdef DCF77_PON
		DCF77_PON(value);//used for PC debugging
	#else
		DCF77_PON_TRIS = 0;
		if (value) {
			DCF77_PON_LAT = 1;//Off
		} else {
			DCF77_PON_LAT = 0;//ON
		}
	#endif
}

unsigned char dcf77_get_TCO(void) {
	unsigned char retVal = 0;
	#ifdef DCF77_TCO
		retVal = DCF77_TCO();
	#else
		retVal = DCF77_TCO_PORT;
	#endif
	return retVal;
}

//char stimulation[] = "0 11010101001010 000101 01000001 0100010 100000 110 01001 100001001X0 00111011111010 000101 11000000 0100010 100000 110 01001 100001001X0 00101110000010 000101 00100001 0100010 100000 110 01001 100001001X0 10110111001101 000101 10100000 0100010 100000 110 01001 100001001X0 01110010101001 000101 01100000 0100010 100000 110 01001 100001001X0 01110010101110 000101 11100001 0100010 100000 110 01001 100001001X0 11011111111101 000101 00010001 0100010 100000 110 01001 100001001X";
//
//void dcf77_test_function(void) {
//	unsigned int x = 0;
//	unsigned int y = 0;
//	unsigned int stimulationCnt = 16;
//	unsigned int delayerror = 100;
//	volatile int readBit = 0;
//	init_dcf77();
//
//	for (x = 0; x < 60; x++) {
//		dcf77_add_bitstream(0);
//	}
//	dcf77_add_bitstream(1);
//	for (x = 0; x < 58; x++) {
//		dcf77_add_bitstream(0);
//	}
//	readBit = dcf77_read_bitstream(0 + 1);
//	readBit = dcf77_read_bitstream(1 + 1);
//	readBit = dcf77_read_bitstream(59 + 1);
//	readBit = dcf77_read_bitstream(60 + 1);
//
//	dcf77_add_bitstream(1);
//	for (x = 0; x < 59; x++) {
//		dcf77_add_bitstream(0);
//	}
//	readBit = dcf77_read_bitstream(0);
//	readBit = dcf77_read_bitstream(1);
//	readBit = dcf77_read_bitstream(59);
//	readBit = dcf77_read_bitstream(60);
//
//	//0 11010101001010 000101 01000001 0100010 100000 110 01001 100001001  Mi, 01.12.21 22:02:00, WZ   
//	//char stimulation[] = "0-11010101001010-000101-01000001-0100010-100000-110-01001-100001001X";
//
//
//	for (y = 0; y < 1200; y++) {
//		//stimulation[stimulationCnt] = '0';
//		if (stimulation[stimulationCnt] == '0') {
//			dcf77_stimulate = 0;
//			for (x = 0; x < delayerror; x++) {
//				do_dcf77();
//				isr_dcf77_1ms();
//			}
//			dcf77_stimulate = 1;
//			for (x = 0; x < 100; x++) {
//				do_dcf77();
//				isr_dcf77_1ms();
//			}
//			dcf77_stimulate = 0;
//			for (x = 0; x < 900-delayerror; x++) {
//				do_dcf77();
//				isr_dcf77_1ms();
//			}
//			dcf77_stimulate = -1;
//		} else if (stimulation[stimulationCnt] == '1') {
//			dcf77_stimulate = 0;
//			for (x = 0; x < delayerror; x++) {
//				do_dcf77();
//				isr_dcf77_1ms();
//			}
//			dcf77_stimulate = 1;
//			for (x = 0; x < 200; x++) {
//				do_dcf77();
//				isr_dcf77_1ms();
//			}
//			dcf77_stimulate = 0;
//			for (x = 0; x < 800 - delayerror; x++) {
//				do_dcf77();
//				isr_dcf77_1ms();
//			}
//			dcf77_stimulate = -1;
//		} else if (stimulation[stimulationCnt] == 'X') {
//			dcf77_stimulate = 0;
//			for (x = 0; x < delayerror; x++) {
//				do_dcf77();
//				isr_dcf77_1ms();
//			}
//			dcf77_stimulate = 0;
//			for (x = 0; x < 200; x++) {
//				do_dcf77();
//				isr_dcf77_1ms();
//			}
//			dcf77_stimulate = 0;
//			for (x = 0; x < 800 - delayerror; x++) {
//				do_dcf77();
//				isr_dcf77_1ms();
//			}
//			dcf77_stimulate = -1;
//		}
//
//		stimulationCnt++;
//		if (stimulationCnt >= (sizeof(stimulation) / sizeof(*stimulation))) {
//			stimulationCnt = 0;
//		}
//		if ((stimulation[stimulationCnt] == '-') || (stimulation[stimulationCnt] == ' ')) {
//			stimulationCnt++;
//			if (stimulationCnt >= (sizeof(stimulation) / sizeof(*stimulation))) {
//				stimulationCnt = 0;
//			}
//		}
//	}
//}
