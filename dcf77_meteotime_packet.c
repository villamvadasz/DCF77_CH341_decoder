#include <stdio.h>
#include <string.h>
#include "dcf77_meteotime.h"

#include "dcf77.h"
#include "k_stdtype.h"

volatile uint32 do_dcf77_meteotime_packet_1ms = 0;

uint8 dcf77_meteotime_packets[3][8];
uint8 dcf77_meteotime_packet_packets_cnt = 0;
uint8 dcf77_meteotime_packet_process_packets = 0;
uint8 dcf77_meteotime_packet_first = 0;
uint8 dcf77_meteotime_packet_second = 0;
uint8 dcf77_meteotime_packet_third = 0;

uint8 dcf77_meteotime_packet_decoded_packet_hour = 0;
uint8 dcf77_meteotime_packet_decoded_packet_min = 0;
uint8 dcf77_meteotime_packet_decoded_packet_utcOffset = 0;


uint8 dcf77_meteotime_packet_decoded_packet_there = 0;
uint32 dcf77_meteotime_packet_decoded_packet_clearValue = 0;
				

void init_dcf77_meteotime_packet(void) {
	memset(dcf77_meteotime_packets, 0x00, sizeof(dcf77_meteotime_packets));
	dcf77_meteotime_packet_packets_cnt = 0;
}

void do_dcf77_meteotime_packet(void) {
	if (do_dcf77_meteotime_packet_1ms) {
		do_dcf77_meteotime_packet_1ms = 0;
		if (dcf77_meteotime_packet_process_packets) {
			dcf77_meteotime_packet_process_packets = 0;
			{
				unsigned char dcf77_meteotime_packet_ecrypted[10];
				unsigned char dcf77_meteotime_packet_decrypted[4];
				uint32 dcf77_meteotime_packet_decrypted_value = 0;

				dcf77_meteotime_decrypt_dcf77_to_encrypted_packet(
					dcf77_meteotime_packets[dcf77_meteotime_packet_first], 
					dcf77_meteotime_packets[dcf77_meteotime_packet_second], 
					dcf77_meteotime_packets[dcf77_meteotime_packet_third], 
					dcf77_meteotime_packet_ecrypted);
		
		
				if (dcf77_meteotime_decrypt_dataset(dcf77_meteotime_packet_ecrypted, dcf77_meteotime_packet_decrypted, &dcf77_meteotime_packet_decrypted_value)) {
					
					dcf77_meteotime_packet_decoded_packet_there = 1;
					dcf77_meteotime_packet_decoded_packet_clearValue = dcf77_meteotime_packet_decrypted_value;

					dcf77_meteotime_packet_decoded_packet_hour = dcf77_get_buffer_hour(dcf77_meteotime_packets[dcf77_meteotime_packet_second]);
					dcf77_meteotime_packet_decoded_packet_min = dcf77_get_buffer_min(dcf77_meteotime_packets[dcf77_meteotime_packet_second]);
					dcf77_meteotime_packet_decoded_packet_utcOffset = dcf77_get_buffer_summer_winter_time(dcf77_meteotime_packets[dcf77_meteotime_packet_second]);

					//dcf77_meteotime_mUintClear = dcf77_meteotime_packet_decrypted_value;
					//dcf77_meteotime_utcOffset = dcf77_get_buffer_summer_winter_time(dcf77_meteotime_packets[second]);

				}
			}
		}
	}
}

void isr_dcf77_meteotime_packet_1ms(void) {
	do_dcf77_meteotime_packet_1ms = 1;
}

void dcf77_meteotime_packet_add_received_message(unsigned char *dcf77_data) {//8 bytes
	static unsigned char atLeast3received = 0;
	//Here a complete minute is received to assembly a weather message
	//We need 3 DCF message to be able to assembly a single weather message
	if ( (dcf77_data != NULL) && (dcf77_check_buffer_validity(dcf77_data)) ) {

		uint32 packet_cnt = dcf77_get_buffer_packet_cnt(dcf77_data);

		memcpy(dcf77_meteotime_packets[dcf77_meteotime_packet_packets_cnt], dcf77_data, 8);
		dcf77_meteotime_packet_packets_cnt++;
		if (dcf77_meteotime_packet_packets_cnt >= 3) {
			dcf77_meteotime_packet_packets_cnt = 0;
		}

		if (atLeast3received < 2) {
			atLeast3received ++;
		} else {
			dcf77_meteotime_packet_first = (dcf77_meteotime_packet_packets_cnt + 3 - 3) % 3;
			dcf77_meteotime_packet_second = (dcf77_meteotime_packet_packets_cnt + 3 - 2) % 3;
			dcf77_meteotime_packet_third = (dcf77_meteotime_packet_packets_cnt + 3 - 1) % 3;

			if ((packet_cnt % 3) == 0) {
				dcf77_meteotime_packet_process_packets = 1;
			}
		}
	}
}

uint8 dcf77_meteotime_packet_get_received_weather(uint32 *clearValue, uint8 *hour, uint8 *min, uint8 *utcOffset) {
	uint8 result = 0;
	if ((clearValue != NULL) && (hour != NULL) && (min != NULL) && (utcOffset != NULL)) {
		if (dcf77_meteotime_packet_decoded_packet_there) {
			dcf77_meteotime_packet_decoded_packet_there = 0;
			*clearValue = dcf77_meteotime_packet_decoded_packet_clearValue;
			*hour = dcf77_meteotime_packet_decoded_packet_hour;
			*min = dcf77_meteotime_packet_decoded_packet_min;
			*utcOffset = dcf77_meteotime_packet_decoded_packet_utcOffset;
			result = 1;
		}
	}
	return result;
}


uint8 dcf77_meteotime_decrypt_dataset(unsigned char *encrypted, unsigned char *decrypted, uint32 *clearValue) {
	uint8 result = 0;
	if ((encrypted != NULL) && (decrypted != NULL)) {
		if (dcf77_meteotime_decrypt(encrypted, decrypted)) {
			
			*clearValue = decrypted[0];
			*clearValue <<= 8;
			*clearValue += decrypted[1];
			*clearValue <<= 8;
			*clearValue += decrypted[2];
			*clearValue <<= 8;
			*clearValue += decrypted[3];
			
			result = 1;
		}
	}
	return result;
}
