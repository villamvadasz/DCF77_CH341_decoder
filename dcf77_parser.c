#include <stdio.h>
#include <string.h>

#include "dcf77.h"
#include "dcf77_meteotime.h"
#include "c_dcf77.h"

uint8 dcf77_check_buffer_validity(unsigned char *dcf77_data) {
	uint8 result = 0;
	if (dcf77_data != NULL) {
		if (
			(dcf77_get_bit(dcf77_data, 0) == 0) && //Always 0
			(dcf77_get_bit(dcf77_data, 20) == 1) && //Always 1
			//(dcf77_get_bit(dcf77_data, 59) == 0xFF) //Normaly 0xFF except leap second
			1
		) {
			unsigned char z1_17 = dcf77_get_bit(dcf77_data, 17);
			unsigned char z2_18 = dcf77_get_bit(dcf77_data, 18);

			unsigned char min_21 = dcf77_get_bit(dcf77_data, 21);
			unsigned char min_22 = dcf77_get_bit(dcf77_data, 22);
			unsigned char min_23 = dcf77_get_bit(dcf77_data, 23);
			unsigned char min_24 = dcf77_get_bit(dcf77_data, 24);
			unsigned char min_25 = dcf77_get_bit(dcf77_data, 25);
			unsigned char min_26 = dcf77_get_bit(dcf77_data, 26);
			unsigned char min_27 = dcf77_get_bit(dcf77_data, 27);
			unsigned char min_parity = dcf77_get_bit(dcf77_data, 28);
			unsigned char min_calc_parity = 0;
			
			unsigned char hour_29 = dcf77_get_bit(dcf77_data, 29);
			unsigned char hour_30 = dcf77_get_bit(dcf77_data, 30);
			unsigned char hour_31 = dcf77_get_bit(dcf77_data, 31);
			unsigned char hour_32 = dcf77_get_bit(dcf77_data, 32);
			unsigned char hour_33 = dcf77_get_bit(dcf77_data, 33);
			unsigned char hour_34 = dcf77_get_bit(dcf77_data, 34);
			unsigned char hour_parity = dcf77_get_bit(dcf77_data, 35);
			unsigned char hour_calc_parity = 0;
			
			unsigned char day_36 = dcf77_get_bit(dcf77_data, 36);
			unsigned char day_37 = dcf77_get_bit(dcf77_data, 37);
			unsigned char day_38 = dcf77_get_bit(dcf77_data, 38);
			unsigned char day_39 = dcf77_get_bit(dcf77_data, 39);
			unsigned char day_40 = dcf77_get_bit(dcf77_data, 40);
			unsigned char day_41 = dcf77_get_bit(dcf77_data, 41);
			
			unsigned char dofw_42 = dcf77_get_bit(dcf77_data, 42);
			unsigned char dofw_43 = dcf77_get_bit(dcf77_data, 43);
			unsigned char dofw_44 = dcf77_get_bit(dcf77_data, 44);
			
			unsigned char month_45 = dcf77_get_bit(dcf77_data, 45);
			unsigned char month_46 = dcf77_get_bit(dcf77_data, 46);
			unsigned char month_47 = dcf77_get_bit(dcf77_data, 47);
			unsigned char month_48 = dcf77_get_bit(dcf77_data, 48);
			unsigned char month_49 = dcf77_get_bit(dcf77_data, 49);

			unsigned char year_50 = dcf77_get_bit(dcf77_data, 50);
			unsigned char year_51 = dcf77_get_bit(dcf77_data, 51);
			unsigned char year_52 = dcf77_get_bit(dcf77_data, 52);
			unsigned char year_53 = dcf77_get_bit(dcf77_data, 53);
			unsigned char year_54 = dcf77_get_bit(dcf77_data, 54);
			unsigned char year_55 = dcf77_get_bit(dcf77_data, 55);
			unsigned char year_56 = dcf77_get_bit(dcf77_data, 56);
			unsigned char year_57 = dcf77_get_bit(dcf77_data, 57);
			unsigned char year_parity = dcf77_get_bit(dcf77_data, 58);
			unsigned char year_calc_parity = 0;
			
			min_calc_parity = min_21;
			min_calc_parity += min_22;
			min_calc_parity += min_23;
			min_calc_parity += min_24;
			min_calc_parity += min_25;
			min_calc_parity += min_26;
			min_calc_parity += min_27;
			min_calc_parity += min_parity;
			min_calc_parity &= 1;
			
			hour_calc_parity = hour_29;
			hour_calc_parity += hour_30;
			hour_calc_parity += hour_31;
			hour_calc_parity += hour_32;
			hour_calc_parity += hour_33;
			hour_calc_parity += hour_34;
			hour_calc_parity += hour_parity;
			hour_calc_parity &= 1;
			
			year_calc_parity = day_36;
			year_calc_parity += day_37;
			year_calc_parity += day_38;
			year_calc_parity += day_39;
			year_calc_parity += day_40;
			year_calc_parity += day_41;

			year_calc_parity += dofw_42;
			year_calc_parity += dofw_43;
			year_calc_parity += dofw_44;

			year_calc_parity += month_45;
			year_calc_parity += month_46;
			year_calc_parity += month_47;
			year_calc_parity += month_48;
			year_calc_parity += month_49;

			year_calc_parity += year_50;
			year_calc_parity += year_51;
			year_calc_parity += year_52;
			year_calc_parity += year_53;
			year_calc_parity += year_54;
			year_calc_parity += year_55;
			year_calc_parity += year_56;
			year_calc_parity += year_57;
			year_calc_parity += year_parity; //36-58
			year_calc_parity &= 1;
			
			if (
				(min_calc_parity == 0) && 
				(hour_calc_parity == 0) && 
				(year_calc_parity == 0) &&
				(z1_17 != z2_18)
			) {
				result = 1;
			}
		}
	}
	return result;
}

uint16 dcf77_get_buffer_year(unsigned char *dcf77_data) {
	uint16 result = 0;
	if (dcf77_data != NULL) {
		uint16 year = 0;
		unsigned char year_50 = dcf77_get_bit(dcf77_data, 50);
		unsigned char year_51 = dcf77_get_bit(dcf77_data, 51);
		unsigned char year_52 = dcf77_get_bit(dcf77_data, 52);
		unsigned char year_53 = dcf77_get_bit(dcf77_data, 53);
		unsigned char year_54 = dcf77_get_bit(dcf77_data, 54);
		unsigned char year_55 = dcf77_get_bit(dcf77_data, 55);
		unsigned char year_56 = dcf77_get_bit(dcf77_data, 56);
		unsigned char year_57 = dcf77_get_bit(dcf77_data, 57);

		year = year_50 * 1;
		year += year_51 * 2;
		year += year_52 * 4;
		year += year_53 * 8;
		year += year_54 * 10;
		year += year_55 * 20;
		year += year_56 * 40;
		year += year_57 * 80;
			
		year += 2000;
		result = year;
	}
	return result;
}

uint8 dcf77_get_buffer_summer_winter_time(unsigned char *dcf77_data) {
	uint8 result = 0;
	if (dcf77_data != NULL) {
		uint8 offset = 0;
		unsigned char z1_17 = dcf77_get_bit(dcf77_data, 17);
		unsigned char z2_18 = dcf77_get_bit(dcf77_data, 18);

		if (z1_17) {//Summertime
			offset = 2;
		}
		if (z2_18) {//Wintertime
			offset = 1;
		}
		
		result = offset;
	}
	return result;
}

uint8 dcf77_get_buffer_month(unsigned char *dcf77_data) {
	uint8 result = 0;
	if (dcf77_data != NULL) {
		uint8 month = 0;
		unsigned char month_45 = dcf77_get_bit(dcf77_data, 45);
		unsigned char month_46 = dcf77_get_bit(dcf77_data, 46);
		unsigned char month_47 = dcf77_get_bit(dcf77_data, 47);
		unsigned char month_48 = dcf77_get_bit(dcf77_data, 48);
		unsigned char month_49 = dcf77_get_bit(dcf77_data, 49);

		month = month_45 * 1;
		month += month_46 * 2;
		month += month_47 * 4;
		month += month_48 * 8;
		month += month_49 * 10;
		result = month;
	}
	return result;
}
	
uint8 dcf77_get_buffer_day(unsigned char *dcf77_data) {
	uint8 result = 0;
	if (dcf77_data != NULL) {
		uint8 day = 0;
		unsigned char day_36 = dcf77_get_bit(dcf77_data, 36);
		unsigned char day_37 = dcf77_get_bit(dcf77_data, 37);
		unsigned char day_38 = dcf77_get_bit(dcf77_data, 38);
		unsigned char day_39 = dcf77_get_bit(dcf77_data, 39);
		unsigned char day_40 = dcf77_get_bit(dcf77_data, 40);
		unsigned char day_41 = dcf77_get_bit(dcf77_data, 41);

		day = day_36 * 1;
		day += day_37 * 2;
		day += day_38 * 4;
		day += day_39 * 8;
		day += day_40 * 10;
		day += day_41 * 20;

		result = day;
	}
	return result;
}

uint8 dcf77_get_buffer_hour(unsigned char *dcf77_data) {
	uint8 result = 0;
	if (dcf77_data != NULL) {
		uint8 hour = 0;
		unsigned char hour_29 = dcf77_get_bit(dcf77_data, 29);
		unsigned char hour_30 = dcf77_get_bit(dcf77_data, 30);
		unsigned char hour_31 = dcf77_get_bit(dcf77_data, 31);
		unsigned char hour_32 = dcf77_get_bit(dcf77_data, 32);
		unsigned char hour_33 = dcf77_get_bit(dcf77_data, 33);
		unsigned char hour_34 = dcf77_get_bit(dcf77_data, 34);

		hour = hour_29 * 1;
		hour += hour_30 * 2;
		hour += hour_31 * 4;
		hour += hour_32 * 8;
		hour += hour_33 * 10;
		hour += hour_34 * 20;

		result = hour;
	}

	return result;
}

uint8 dcf77_get_buffer_min(unsigned char *dcf77_data) {
	uint8 result = 0;
	if (dcf77_data != NULL) {
		uint8 min = 0;
		unsigned char min_21 = dcf77_get_bit(dcf77_data, 21);
		unsigned char min_22 = dcf77_get_bit(dcf77_data, 22);
		unsigned char min_23 = dcf77_get_bit(dcf77_data, 23);
		unsigned char min_24 = dcf77_get_bit(dcf77_data, 24);
		unsigned char min_25 = dcf77_get_bit(dcf77_data, 25);
		unsigned char min_26 = dcf77_get_bit(dcf77_data, 26);
		unsigned char min_27 = dcf77_get_bit(dcf77_data, 27);

		min = min_21 * 1;
		min += min_22 * 2;
		min += min_23 * 4;
		min += min_24 * 8;
		min += min_25 * 10;
		min += min_26 * 20;
		min += min_27 * 40;

		result = min;
	}
			
	return result;
}

uint32 dcf77_get_buffer_packet_cnt(unsigned char *dcf77_data) {
	uint32 result = 0;
	if (dcf77_data != NULL) {
		uint8 min = dcf77_get_buffer_min(dcf77_data);
		uint8 hour = dcf77_get_buffer_hour(dcf77_data);

		result = (hour * 60) + min;
		
	}
	return result;
}
