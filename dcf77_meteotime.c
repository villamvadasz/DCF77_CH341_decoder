#include <stdio.h>
#include <string.h>
#include "dcf77_meteotime.h"
#include "dcf77_meteotime_text.h"
#include "c_dcf77.h"

#include "dcf77.h"
#include "k_stdtype.h"
#ifdef DCF77_USE_USB_TO_PRINT
	#include "serial_usb.h"
#endif

volatile uint32 do_dcf77_meteotime_1ms = 0;

uint32 dcf77_meteotime_mUintClear = 0;
uint8 dcf77_meteotime_utcOffset = 0;
uint8 dcf77_meteotime_hour = 0;
uint8 dcf77_meteotime_min = 0;
uint16 dcf77_meteotime_item = 0;

DCF77_WeatherData dcf77_WeatherData[480];

uint32 dcf77_reverse(uint32 value, uint32 bitNr);
uint32 getValueFromClear(uint32 clearValue, uint32 bitPosition, uint32 length);

void init_dcf77_meteotime(void) {
	init_dcf77_meteotime_packet();
}

void do_dcf77_meteotime(void) {
	do_dcf77_meteotime_packet();
	if (do_dcf77_meteotime_1ms) {
		do_dcf77_meteotime_1ms = 0;
		{
			static uint16 do_dcf77_meteotime_60000ms_cnt = 0;
			do_dcf77_meteotime_60000ms_cnt++;
			//if (do_dcf77_meteotime_60000ms_cnt >= 60000) 
			{
				if (dcf77_meteotime_packet_get_received_weather(&dcf77_meteotime_mUintClear, &dcf77_meteotime_hour, &dcf77_meteotime_min, &dcf77_meteotime_utcOffset)) {
					dcf77_meteotime_item = dcf77_meteotime_CalculateSetNumber(dcf77_meteotime_hour, dcf77_meteotime_min, dcf77_meteotime_utcOffset);
					dcf77_WeatherData[dcf77_meteotime_item].clearValue = dcf77_meteotime_mUintClear;
					dcf77_WeatherData[dcf77_meteotime_item].hour = dcf77_meteotime_hour;
					dcf77_WeatherData[dcf77_meteotime_item].min = dcf77_meteotime_min;
					dcf77_WeatherData[dcf77_meteotime_item].utcOffset = dcf77_meteotime_utcOffset;

					dcf77_meteotime_print(dcf77_meteotime_mUintClear, dcf77_meteotime_hour, dcf77_meteotime_min, dcf77_meteotime_utcOffset);
				}
			}
		}
	}
}

void isr_dcf77_meteotime_1ms(void) {
	do_dcf77_meteotime_1ms = 1;
	isr_dcf77_meteotime_packet_1ms();
}

uint8 dcf77_meteotime_getMeteoData(uint32 item, DCF77_WeatherData *weatherData) {
	uint8 result = 0;
	if (weatherData != NULL) {
		if (item < 480) {
			*weatherData = dcf77_WeatherData[item];
			result = 1;
		}
	}
	return result;
}

void dcf77_meteotime_print(uint32 clearValue, uint32 hour, uint32 min, uint32 utcOffset) {
	unsigned char str[128];

	memset(str, 0, 128);
	
	putString_usb("<------------------------------\r\n");

	putString_usb("     - Minute...........................: ");
	memset(str, 0, 128);
	sprint_binary(str, dcf77_reverse(min % 10, 4), 4);
	putString_usb(str);
	memset(str, 0, 128);
	sprint_binary(str, dcf77_reverse(min / 10, 4), 4);
	putString_usb(str);
	putString_usb("       ");


	memset(str, 0, 128);
	sprintf(str, "%2.2u", min);
	putString_usb(str);
	putString_usb("\r\n");

	putString_usb("     - Stunde...........................: ");
	memset(str, 0, 128);
	sprint_binary(str, dcf77_reverse(hour % 10, 4), 4);
	putString_usb(str);
	memset(str, 0, 128);
	sprint_binary(str, dcf77_reverse(hour / 10, 4), 4);
	putString_usb(str);
	putString_usb("       ");


	memset(str, 0, 128);
	sprintf(str, "%2.2u", hour);
	putString_usb(str);
	putString_usb("\r\n");
	
	putString_usb("     - Tag..............................: 00101000       14\r\n");
	putString_usb("     - Monat............................: 00100          04\r\n");
	putString_usb("     - Wochentag........................: 100             1\r\n");
	putString_usb("     - Jahr.............................: 10100100       25\r\n");
	putString_usb("\r\n");
	dcf77_meteotime_getDay_string(str, clearValue, hour, min, 0); 
	putString_usb(str);putString_usb("\r\n");

	memset(str, 0, 128);
	dcf77_meteotime_getNight_string(str, clearValue, hour, min, 0); 
	putString_usb(str);putString_usb("\r\n");

	if (dcf77_meteotime_getAnomaly(clearValue, hour, min, 0) ) {
		memset(str, 0, 128);
		dcf77_meteotime_getAnomalyType_string(str, NULL, clearValue, hour, min, 0);
		if (str[0] != 0) {
			putString_usb(str);putString_usb("\r\n");
		}
		dcf77_meteotime_getAnomalyType_string(NULL, str, clearValue, hour, min, 0);
		if (str[0] != 0) {
			putString_usb(str);putString_usb("\r\n");
		}
	} else {
		memset(str, 0, 128);
		dcf77_meteotime_getExtreme_string(str, clearValue, hour, min, 0);
		if (str[0] != 0) {
			putString_usb(str);putString_usb("\r\n");
		}
	}


	memset(str, 0, 128);
	dcf77_meteotime_getRain_string(str, clearValue, hour, min, 0);
	if (str[0] != 0) {
		putString_usb(str);putString_usb("\r\n");
	}
	

	memset(str, 0, 128);
	if (dcf77_meteotime_getAnomaly(clearValue, hour, min, 0) ) {
		dcf77_meteotime_getWind_string(NULL, str, clearValue, hour, min, 0);
		if (str[0] != 0) {
			putString_usb(str);putString_usb("\r\n");
		}
	} else {
		dcf77_meteotime_getWind_string(str, NULL, clearValue, hour, min, 0);
		if (str[0] != 0) {
			putString_usb(str);putString_usb("\r\n");
			dcf77_meteotime_getWind_string(NULL, str, clearValue, hour, min, 0);
			putString_usb(str);putString_usb("\r\n");
		}
	}

	memset(str, 0, 128);
	dcf77_meteotime_getAnomaly_string(str, clearValue, hour, min, 0);
	if (str[0] != 0) {
		putString_usb(str);putString_usb("\r\n");
	}


	memset(str, 0, 128);
	dcf77_meteotime_getTemperature_string(str, clearValue, hour); 
	putString_usb(str);putString_usb("\r\n");
	putString_usb("     - Dekoderstatus....................: 10\r\n");
	putString_usb("-------------------------------\r\n");

	putString_usb("Dekodierte Daten.......: ");
	memset(str, 0, 128);
	sprint_binary(str, clearValue, 24);
	putString_usb(str);
	putString_usb("\r\n");
	putString_usb("Datum & Uhrzeit........: 14.04.2025 ");
	memset(str, 0, 128);
	sprintf(str, "%2.2u:%2.2u", hour, min);
	putString_usb(str);
	putString_usb("\r\n");



	memset(str, 0, 128);
	dcf77_meteotime_getRegion_string(str, NULL, NULL, NULL, clearValue, hour, min, 0); 
	if (str[0] == 0) {
		putString_usb("\r\n");
	} else {
		putString_usb(str); putString_usb("\r\n");
	}
	dcf77_meteotime_getRegion_string(NULL, str, NULL, NULL, clearValue, hour, min, 0); 
	if (str[0] == 0) {
		putString_usb("\r\n");
	} else {
		putString_usb(str); putString_usb("\r\n");
	}
	dcf77_meteotime_getRegion_string(NULL, NULL, str, NULL, clearValue, hour, min, 0); 
	if (str[0] == 0) {
		putString_usb("\r\n");
	} else {
		putString_usb(str); putString_usb("\r\n");
	}
	dcf77_meteotime_getRegion_string(NULL, NULL, NULL, str, clearValue, hour, min, 0); 
	if (str[0] == 0) {
		putString_usb("\r\n");
	} else {
		putString_usb(str); putString_usb("\r\n");
	}


	putString_usb("------------------------------>\r\n");
	putString_usb("\r\n");
}

void dcf77_meteotime_getRegion_string(char *str1, char *str2, char *str3, char *str4, uint32 clearValue, uint32 hour, uint32 min, uint32 utcOffset) {
	//int num = 2;
	int num2 = dcf77_meteotime_CalculateSetNumber(hour, min, utcOffset);
	int num3 = num2 % 60;
	int num4 = num2 % 30 + 60;
	int num5 = num2 / 60;
	int num6 = (num2 - 420) / 30;
	if (num2 >= 420) {
		//num = 4;
	}
//Region.................: 01:59 Region: 59 (Höchstwerte, 1. Tag (Heute))
//                                       59 - D - Stuttgart, Baden-Württemberg (Nördl. Baden Württemberg)
	if (str1 != NULL) {
		str1 += sprintf(str1, "Region.................: ");
		str1 += sprintf(str1, "%2.2u", hour);
		str1 += sprintf(str1, ":");
		str1 += sprintf(str1, "%2.2u", min);
		str1 += sprintf(str1, " Region: ");
		str1 += sprintf(str1, "%2u", num3);
		str1 += sprintf(str1, " ");
		str1 += sprintf(str1, "%s", mStringArrForecastType60[num5]);
	}
	if (str2 != NULL) {
		str2 += sprintf(str2, "                                       ");
		str2 += sprintf(str2, "%2.1u", num3);
		str2 += sprintf(str2, " - ");
		str2 += sprintf(str2, "%s", mStringArrRegions[num3]);
	}
	if (str3 != NULL) {
		if (num2 >= 420) {
			str3 += sprintf(str3, "                         ");
			str3 += sprintf(str3, "%2.2u", hour);
			str3 += sprintf(str3, ":");
			str3 += sprintf(str3, "%2.2u", min);
			str3 += sprintf(str3, " Region: ");
			str3 += sprintf(str3, "%2.u", num4);
			str3 += sprintf(str3, " ");
			str3 += sprintf(str3, "%s", mStringArrForecastType90[num6]);
		} else {
			str3 += sprintf(str3, "");
		}
	}
	if (str4 != NULL) {
		if (num2 >= 420) {
			str4 += sprintf(str4, "                                       ");
			str4 += sprintf(str4, "%2.u", num4);
			str4 += sprintf(str4, " - ");
			str4 += sprintf(str4, "%s", mStringArrRegions[num4]);
		} else {
			str4 += sprintf(str4, "");
		}
	}
}

void dcf77_meteotime_getTemperature_string(char *str, uint32 clearValue, uint32 hour) {
	if (str != NULL) {
//	     - Temperatur.......................: 010110         26     4Â°C
		str += sprintf(str, "     - Temperatur.......................: ");
		sint32 num = getValueFromClear(clearValue, 16, 6);
		str += sprint_binary(str, num, 6);
		str += sprintf(str, "         ");
		num = dcf77_reverse(num, 6);
		str += sprintf(str, "%u", num);
		str += sprintf(str, "    ");
		
		
		switch (num) {
			case 0: {
				str += sprintf(str, "<-21 °C");
				break;
			}
			case 63: {
				str += sprintf(str, ">40 °C");
				break;
			}
			default: {
				str += sprintf(str, "%2.1d°C", num - 22);
		
				if (( (hour%6) > 2) && (hour < 21)) {
					//str += sprintf(str, " minimum");
				} else {
					//str += sprintf(str, " maximum");
				}
				break;
			}
		}
	}
}

void dcf77_meteotime_getDay_string(char *str, uint32 clearValue, uint32 hour, uint32 min, uint32 utcOffset) {
	if (str != NULL) {
		//- Tag..............................: 0110            6   starker Regen
		str += sprintf(str, "OUT: - Tag..............................: ");
		
        if (hour < 21) {//Between 21:00-23:59 significant weather & temperature is for cities 60-89 wind and wind direction for cities 0-59.
			sint32 num = getValueFromClear(clearValue, 0, 4);
			str += sprint_binary(str, num, 4);
			str += sprintf(str, "           ");
			num = dcf77_reverse(num, 4);
			str += sprintf(str, "%2u   ", num);
			if (num == 1) {
				num = 16;
			}
			str += sprintf(str, "%s", mStringArrForecast[mByteArrForecastLookup[num]]);
		} else {
			sint32 num = getValueFromClear(clearValue, 0, 4);
			str += sprint_binary(str, num, 4);
			str += sprintf(str, "           ");
			num = dcf77_reverse(num, 4);
			str += sprintf(str, "%2u   ", num);
			if (num == 1) {
				num = 16;
			}
			str += sprintf(str, "%s", mStringArrForecast[mByteArrForecastLookup[num]]);
		}
	}
}

void dcf77_meteotime_getNight_string(char *str, uint32 clearValue, uint32 hour, uint32 min, uint32 utcOffset) {
	if (str != NULL) {
		//- Nacht............................: 0010            4   bedeckt
		str += sprintf(str, "     - Nacht............................: ");

        if (hour < 21) {//Between 21:00-23:59 significant weather & temperature is for cities 60-89 wind and wind direction for cities 0-59.
			sint32 num = getValueFromClear(clearValue, 4, 4);
			str += sprint_binary(str, num, 4);
			str += sprintf(str, "           ");
			num = dcf77_reverse(num, 4);
			str += sprintf(str, "%2u   ", num);
			str += sprintf(str, "%s", mStringArrForecast[mByteArrForecastLookup[num]]);
		} else {
			sint32 num = getValueFromClear(clearValue, 4, 4);
			str += sprint_binary(str, num, 4);
			str += sprintf(str, "           ");
			num = dcf77_reverse(num, 4);
			str += sprintf(str, "%2u   ", num);
			str += sprintf(str, "%s", mStringArrForecast[mByteArrForecastLookup[num]]);
		}
	}
}

void dcf77_meteotime_getExtreme_string(char *str, uint32 clearValue, uint32 hour, uint32 min, uint32 utcOffset) {
	if (str != NULL) {
		if ((hour)%6>2) {
		} else {
			//     - Wetterextreme....................: 0000            0   Keine
			str += sprintf(str, "     - Wetterextreme....................: ");
			sint32 num = getValueFromClear(clearValue, 8, 4);
			str += sprint_binary(str, num, 4);
			str += sprintf(str, "           ");
			num = dcf77_reverse(num, 4);
			str += sprintf(str, "%2u   ", num);
			str += sprintf(str, "%s", mStringArrExtremeWeather[num]);
		}
	}
}

void dcf77_meteotime_getRain_string(char *str, uint32 clearValue, uint32 hour, uint32 min, uint32 utcOffset) {
	if (str != NULL) {
		if ((hour)%6>2) {
		} else {
			//     - Niederschlagswahrscheinlichkeit..: 011             6   90%
			str += sprintf(str, "     - Niederschlagswahrscheinlichkeit..: ");
			sint32 num = getValueFromClear(clearValue, 12, 3);
			str += sprint_binary(str, num, 3);
			str += sprintf(str, "            ");
			num = dcf77_reverse(num, 3);
			str += sprintf(str, "%2u   ", num);
			str += sprintf(str, "%s", mStringArrPrecipitation[num]);
		}
	}
}

void dcf77_meteotime_getWind_string(char *str1, char *str2, uint32 clearValue, uint32 hour, uint32 min, uint32 utcOffset) {
	if (str1 != NULL) {
		if ((hour)%6>2) {
			str1 += sprintf(str1, "     - Windrichtung.....................: ");
			sint32 num = getValueFromClear(clearValue, 8, 4);
			str1 += sprint_binary(str1, num, 4);
			str1 += sprintf(str1, "           ");
			num = dcf77_reverse(num, 4);
			str1 += sprintf(str1, "%2.1u   ", num);
			str1 += sprintf(str1, "%s", mStringArrWind[num]);
		}
	}
	if (str2 != NULL) {
		if ((hour)%6>2){
			str2 += sprintf(str2, "     - Windstärke.......................: ");
			sint32 num = getValueFromClear(clearValue, 12, 3);
			str2 += sprint_binary(str2, num, 3);
			str2 += sprintf(str2, "            ");
			num = dcf77_reverse(num, 3);
			str2 += sprintf(str2, "%2.u   ", num);
			str2 += sprintf(str2, "%s", mStringArrWindStrength[num]);
			str2 += sprintf(str2, " Bft");
		}
	}
}

unsigned int dcf77_meteotime_getAnomaly(uint32 clearValue, uint32 hour, uint32 min, uint32 utcOffset) {
	unsigned int result = 0;
	sint32 num = getValueFromClear(clearValue, 15, 1);
	num = dcf77_reverse(num, 1);
	if (num == 0) {
		result = 0;
	} else {
		result = 1;
	}
	return result;
}

void dcf77_meteotime_getAnomaly_string(char *str, uint32 clearValue, uint32 hour, uint32 min, uint32 utcOffset) {
	if (str != NULL) {
		//     - Wetteranomalie...................: 0                   Nein
		str += sprintf(str, "     - Wetteranomalie...................: ");

		sint32 num = getValueFromClear(clearValue, 15, 1);
		str += sprint_binary(str, num, 1);
		str += sprintf(str, "                   ");
		num = dcf77_reverse(num, 1);
		if (num == 0) {
			str += sprintf(str, "Nein");
		} else {
			str += sprintf(str, "Ja");
		}
	}
}

void dcf77_meteotime_getAnomalyType_string(char *str1, char *str2, uint32 clearValue, uint32 hour, uint32 min, uint32 utcOffset) {
	if (str1 != NULL) {
		str1 += sprintf(str1, "     - Relatives Vormittagswetter.......: ");
		sint32 num = getValueFromClear(clearValue, 8, 2);
		str1 += sprint_binary(str1, num, 2);
		str1 += sprintf(str1, "             ");
		num = dcf77_reverse(num, 2);
		str1 += sprintf(str1, "%2.1u   ", num);
		str1 += sprintf(str1, "%s", mStringArrRelative[num]);
	}
	if (str2 != NULL) {
		str2 += sprintf(str2, "     - Sonnenscheindauer................: ");
		sint32 num = getValueFromClear(clearValue, 10, 2);
		str2 += sprint_binary(str2, num, 2);
		str2 += sprintf(str2, "             ");
		num = dcf77_reverse(num, 2);
		str2 += sprintf(str2, "%2.1u   ", num);
		str2 += sprintf(str2, "%s", mStringArrSunshine[num]);
	}
}

uint32 dcf77_reverse(uint32 value, uint32 bitNr) {
	uint32 result = 0;
	
	unsigned int x = 0;
	for (x = 0; x < bitNr; x++) {
		unsigned int bitValue = value & (1 << ((bitNr - 1) - x));
		result >>= 1;
		if (bitValue) {
			result |= (1 << (bitNr - 1));
		}
	}
	
	return result;
}

uint32 getValueFromClear(uint32 clearValue, uint32 bitPosition, uint32 length) {
	uint32 num = 0u;
	uint32 mask = 1;
	mask <<= length;
	mask -= 1;
	num = clearValue >> ((24 - bitPosition) - length);
	return (int)(num & mask);
}

uint16 dcf77_meteotime_CalculateSetNumber(uint32 hour, uint32 min, uint32 utcOffset) {
	uint16 result = 0;
	//Wintertime UTC + 1
	//Sommertime UTC + 2
	//Summertime akkor 0-as regioval kezdodik
	//0...480
	uint32 calculateSet = 0;

	calculateSet = hour * 20;
	calculateSet += (min - 2) / 3;

	if (utcOffset == 1) {
		calculateSet += 20;
		calculateSet %= 480;
 	}

	result = calculateSet;

	return result;
}

void dcf77_meteotime_full_day(void) {
	unsigned int x = 0;
	unsigned int full_day[] = {
		0b001001100000011001010110,
		0b001001100000011011100110,
		0b001001100000011001010110,
		0b011001100000011000100110,
		0b110001100000011011010110,
		0b011000100000011000010110,
		0b010010000000000000010110,
		0b011101100000011001010110,
		0b011001100000011011010110,
		0b001001100000011001010110,
		0b001001100000011001100110,
		0b001000100000010001100110,
		0b110011000000100000110110,
		0b110000100000100011010110,
		0b010011000000100011010110,
		0b011001100000011010000110,
		0b010001010000101000100110,
		0b001000100000100000010110,
		0b110001010000110001100110,
		0b010000100000100000100110,
		0b001001000000010011000110,
		0b001011000000010010100110,
		0b010010000000100010010110,
		0b001010000000100000100110,
		0b001011000000100000010110,
		0b011000100000011011010110,
		0b011000100000011001010110,
		0b001001100000011010010110,
		0b001000100000010011010110,
		0b110010000000000000110110,
		0b110011000000100011010110,
		0b001001010000001010010110,
		0b010100100000001001010110,
		0b001000100000010010100110,
		0b001000100000010011010110,
		0b001000100000010001010110,
		0b001000100000100011000110,
		0b010011000000000001010110,
		0b011001100000011010100110,
		0b011001100000011001000110,
		0b011001100000011010100110,
		0b011001100000011001010110,
		0b010011000000100011100110,
		0b011001100000011000010110,
		0b011001100000011001100110,
		0b001000100000100010110110,
		0b001001100000011000010110,
		0b001001000000100011010110,
		0b001001000000010001010110,
		0b001000100000010001010110,
		0b010000100000010001010110,
		0b001001000000010010010110,
		0b010010000000100011010110,
		0b001011000000100001000110,
		0b110010000000000011100110,
		0b010010000000000001100110,
		0b001010000000100000100110,
		0b011000100000011010010110,
		0b001010000000100010100110,
		0b001000100000010001010110,
		0b001001101100010000000110,
		0b001001100010010010000110,
		0b001001100010010001000110,
		0b011001100010110010111010,
		0b110001101100110010000110,
		0b011000100100110001000110,
		0b010010000010010000000110,
		0b011101101100010000000110,
		0b011001101100010010100110,
		0b001001100010010011000110,
		0b001001101000110000000110,
		0b001000100010010000111010,
		0b110011000010010010000110,
		0b110000101010010010000110,
		0b010011000010010000000110,
		0b011001100010110000111010,
		0b010001010010010010111010,
		0b001000101100010000000110,
		0b110001010010110011111010,
		0b010000100110010011111010,
		0b001001001010010011011010,
		0b001011001010010011011010,
		0b010010000110010011111010,
		0b001010000010010000111010,
		0b001011001010010010111010,
		0b011000100100010000000110,
		0b011000100100010010111010,
		0b001001100010010001000110,
		0b001000100010010000000110,
		0b110010001010010000000110,
		0b110011000110010011111010,
		0b001001010100010010000110,
		0b010100100101010011111010,
		0b001000100100010010111010,
		0b001000100100110000000110,
		0b001000101110010010000110,
		0b001000100010010001011010,
		0b010011000010010011111010,
		0b011001100110100010000110,
		0b011001100110010001011010,
		0b011001100000010010000110,
		0b011001101000010011000110,
		0b010011000010010010000110,
		0b011001101000110011000110,
		0b011001100100010011000110,
		0b001000101000010000000110,
		0b001001101010010000000110,
		0b001001000100110000000110,
		0b001001000000010000000110,
		0b001000101100010010000110,
		0b010000101010010011111010,
		0b001001000010010010000110,
		0b010010001010010010000110,
		0b001011001010010011011010,
		0b110010000110010000011010,
		0b010010001010010011101010,
		0b001010001010010001101010,
		0b011000101100010000111010,
		0b001010001010110010011010,
		0b001000100100010011111010,
		0b011000100000011010100110,
		0b001000100000010010100110,
		0b011000100000011010010110,
		0b011001100000011001000110,
		0b011001100000011011100110,
		0b001001100000011010010110,
		0b011001010000011001010110,
		0b011001100000011000010110,
		0b011001100000011011010110,
		0b011001100000011010010110,
		0b011001100000011011100110,
		0b011010110000011000100110,
		0b001000100000010000110110,
		0b001001100000011001010110,
		0b001001010000101010110110,
		0b011001100000011000000110,
		0b011001100000011011000110,
		0b011001100000011011100110,
		0b011001100000011010100110,
		0b001001100000011001010110,
		0b110001100000011011100110,
		0b110001100000011011100110,
		0b110000100000100010110110,
		0b100001100000011000010110,
		0b110001100000011011010110,
		0b001000100000010011010110,
		0b001001000000010001010110,
		0b011001100000011001100110,
		0b001001000000010011010110,
		0b010001100000011000110110,
		0b001001100000011001010110,
		0b011001010000011000010110,
		0b001000100000010000010110,
		0b001000100000010011000110,
		0b001000100000010001010110,
		0b001000100000010001010110,
		0b011000100000011001000110,
		0b010000100000100000110110,
		0b011010110000011000100110,
		0b011001100000011010100110,
		0b011010110100011101100110,
		0b011001100000011001010110,
		0b010101100000011001010110,
		0b011001100000011011100110,
		0b011001101000011101100110,
		0b001000100000010001010110,
		0b011001100000011000100110,
		0b001001000000010010010110,
		0b001001000000010011010110,
		0b011000100000011000010110,
		0b001000100000010001010110,
		0b001000100000010010010110,
		0b010001100000011000110110,
		0b110000100000100011010110,
		0b110010000000000011100110,
		0b100010000000000011100110,
		0b110001000000000000010110,
		0b001000100000010000010110,
		0b001000100000100010100110,
		0b001001100000011010010110,
		0b011000100110110010111010,
		0b001000100110110011111010,
		0b011000100010110001111010,
		0b011001101110110011011010,
		0b011001100010010000111010,
		0b001001101110110000000110,
		0b011001010010010011111010,
		0b011001100010010010111010,
		0b011001101100110010000110,
		0b011001100010110010111010,
		0b011001101110110010111010,
		0b011010111010010000111010,
		0b001000100010010000000110,
		0b001001101010110000000110,
		0b001001010010010001000110,
		0b011001100000110000111010,
		0b011001100000010000111010,
		0b011001101010010001111010,
		0b011001101110010001111010,
		0b001001100100010010000110,
		0b110001100100110010000110,
		0b110001100100110010000110,
		0b110000100100010001000110,
		0b100001100100110010000110,
		0b110001100100010001000110,
		0b001000101100010011111010,
		0b001001001100010000111010,
		0b011001100010100000000110,
		0b001001001100010011111010,
		0b010001100100010001000110,
		0b001001100100010010000110,
		0b011001011010010000000110,
		0b001000100110010011111010,
		0b001000100100010011011010,
		0b001000100010110000000110,
		0b001000101001110001000110,
		0b011000100010010001011010,
		0b010000100100010010000110,
		0b011010110110100010000110,
		0b011001100110010001011010,
		0b011010111000010010000110,
		0b011001101010010011000110,
		0b010101101100010000000110,
		0b011001100100010011000110,
		0b011001100100010111000110,
		0b001000100110010000000110,
		0b011001100010100011111010,
		0b001001001100110000000110,
		0b001001001100010011111010,
		0b011000101100110000000110,
		0b001000101100110011111010,
		0b001000101100110001000110,
		0b010001101100110011000110,
		0b110000100100010001000110,
		0b110010001100010000111010,
		0b100010001100010011111010,
		0b110001000100110001111010,
		0b001000101010010010111010,
		0b001000100010110011111010,
		0b001001100110010011111010,
		0b001011000000010011100110,
		0b010011000000010010100110,
		0b001000100000100010100110,
		0b011000100000011011000110,
		0b011000100000011011000110,
		0b011001000001011001100110,
		0b001000100000010010100110,
		0b011001100000011010000110,
		0b011001100000011010100110,
		0b011001101100011000000110,
		0b011001100000011010000110,
		0b011001100000011010111010,
		0b010001100000011010010110,
		0b011001100000011000100110,
		0b001001100000011011100110,
		0b011001100000011000000110,
		0b011001100000011010000110,
		0b001011000000010000100110,
		0b011011000000011000100110,
		0b001001100000011001100110,
		0b011000100000011011100110,
		0b011000100000011000010110,
		0b001001100000011000110110,
		0b011000100000011011100110,
		0b001001100000011011010110,
		0b110011000000100011110110,
		0b110000100000000010110110,
		0b011001100000011011100110,
		0b110000100000100001110110,
		0b110000100000100001110110,
		0b110001100000011000110110,
		0b011001100000011001000110,
		0b011001100000011010100110,
		0b011011100000011001111010,
		0b011001100000011000100110,
		0b001001100000011010010110,
		0b011001100000011011000110,
		0b001001100000011011010110,
		0b101101101000011111000110,
		0b011001100000011000000110,
		0b011001101100011010100110,
		0b011001100000011011010110,
		0b001000100000010010100110,
		0b101101100000011001100110,
		0b100000000000000000000100,
		0b011001100000011010100110,
		0b010000100000010001010110,
		0b010001100000011011010110,
		0b110011000000100000001110,
		0b110010000000000001110110,
		0b100001000000100001110110,
		0b110010000000100010110110,
		0b110011000000100011110110,
		0b011011010000011011100110,
		0b001001110000010011100110,
		0b110100100000011000010110,
		0b110100100000011001100110,
		0b011001100000011010100110,
		0b011001100000011000100110,
		0b001001100000011000010110,
		0b001011000110110010111010,
		0b010011001010110001111010,
		0b001000101010010000111010,
		0b011000101010110011011010,
		0b011000100000010001011010,
		0b011001001110001111111010,
		0b001000101010010010111010,
		0b011001101010010011011010,
		0b011001101110110000000110,
		0b011001100000010111011010,
		0b011001101000010000011010,
		0b011001100110010000011010,
		0b010001100110110011111010,
		0b011001101010010000111010,
		0b001001101010010010111010,
		0b011001100110110011011010,
		0b011001100110010001011010,
		0b001011001010010011011010,
		0b011011001010110011011010,
		0b001001101010010011111010,
		0b011000100010010011111010,
		0b011000101010010001111010,
		0b001001101010010000000110,
		0b011000100010010011111010,
		0b001001101010010000000110,
		0b110011001100010000000110,
		0b110000100100110000000110,
		0b011001100010100010111010,
		0b110000101100010011111010,
		0b110000101100010010000110,
		0b110001101000010000000110,
		0b011001101010010000111010,
		0b011001100110010010111010,
		0b011011101000010011101010,
		0b011001101010110010111010,
		0b001001101010010001111010,
		0b011001100010010000011010,
		0b001001100010010001111010,
		0b101101100100100101111010,
		0b011001101010010000011010,
		0b011001101000010111111010,
		0b011001101000010000100110,
		0b001000101010110010111010,
		0b101101100100010001000110,
		0b011001100100110000100110,
		0b011001101110110010111010,
		0b010000101010010001111010,
		0b010001101100110010000110,
		0b110011001100110010000110,
		0b110010001100110010000110,
		0b100001001100110000000110,
		0b110010001100010001000110,
		0b110011000010010011000110,
		0b011011010010010000000110,
		0b001001111100010010111010,
		0b110100101100010010111010,
		0b110100101100010010111010,
		0b011001101110010000111010,
		0b011001101000010011111010,
		0b001001101110110001111010,
		0b001001100000011010010110,
		0b001001100000011011100110,
		0b010000100000010000010110,
		0b011001100000011000100110,
		0b010000100000100010010110,
		0b110001000000000001010110,
		0b110001000000100011100110,
		0b001000100000000000010110,
		0b100011000000000001010110,
		0b010000100000100011100110,
		0b010001000000100011100110,
		0b011100100000010010000110,
		0b001000100000010010100110,
		0b001000100000010010100110,
		0b001000100000010001100110,
		0b011001100000011011000110,
		0b010101010000101000100110,
		0b001000100000010011100110,
		0b010101010000011001100110,
		0b001000100000010011000110,
		0b010000100000100010100110,
		0b001001100000011000100110,
		0b011000100000011001100110,
		0b010000100000010001100110,
		0b001000100000010010100110,
		0b110101100000011000010110,
		0b011001100000011000100110,
		0b011001100000011000010110,
		0b011001100000011001100110,
		0b110101100000011011100110,
		0b110101100000011000100110,
		0b010011000000100011100110,
		0b001001000000010000100110,
		0b001011000000010000000110,
		0b001011000000010000010110,
		0b001000100000010010100110,
		0b011100010000010001111010,
		0b011000100000011000100110,
		0b001001000000010000010110,
		0b010000100000010001100110,
		0b001010000000010000010110,
		0b011000100000011010010110,
		0b001000100000100010100110,
		0b010000100000010011100110,
		0b011001100000011011100110,
		0b001000100000010011100110,
		0b011001100000011010100110,
		0b011001100000011001100110,
		0b011001100000011010100110,
		0b011001100000011001010110,
		0b011001100000011010010110,
		0b011000100000011001010110,
		0b001001100000011010010110,
		0b001000100000010001100110,
		0b001000100000010011100110,
		0b001001000000100010010110,
		0b001000100000010010100110,
		0b011000100000011011000110,
		0b001000100000100001100110,
		0b011000100000011000100110,
		0b011011000010010011010110,
		0b011011000010010011010110,
		0b110011000010010010110110,
		0b011000100010110000010110,
		0b001001001100010011000110,
		0b011001100100010000100110,
		0b100010000010010010110110,
		0b110001001100010001010110,
		0b001001000110010000110110,
		0b110001100000010010010110,
		0b011001101010010011000110,
		0b011011001010010000010110,
		0b010101010110010011100110,
		0b011001101010010011010110,
		0b011001010110010010000110,
		0b010010000010110000100110,
		0b001000101100110001000110,
		0b011001101100010011000110,
		0b010001100010110011100110,
		0b110010001000010010100110,
		0b100010000000010000111010,
		0b010001100000010001010110,
		0b001001101110010000110110,
		0b011001100000010010010110,
		0b100010001110010000000110,
		0b010010000110010011000110,
		0b011000100110010011111010,
		0b011011000010010000010110,
		0b001001100110010011000110,
		0b011001001110010011010110,
		0b110010111110010001110110,
		0b010001100100010010010110,
		0b100011001010010010110110,
		0b100010000100010011110110,
		0b010011000110110000100110,
		0b010001101110100001100110,
		0b100001101000010000110110,
		0b010111001110010000010110,
		0b110010000110010011010110,
		0b010011000010010011100110,
		0b011001101010010001000110,
		0b001001000110010010010110,
		0b011000101100010000010110,
		0b011001100010010000010110,
		0b011001011100010011000110,
		0b110001010110010011000110,
		0b010101010010100001000110,
		0b011001010100010010000110,
		0b011000100110010011100110,
		0b110000101100010000010110,
		0b100001100100010001111010,
		0b001011000010010000110110,
		0b001011001110010001110110,
		0b110010000110010000001110,
		0b110000100000010000000110,
		0b001011011100010011000110,
		0b011001100000010011111010,
		0b100011000110010001110110,
		0b011011101000010000111010,
		0b110000101110010011110110
	};


	uint32 hour = 0;
	uint32 min = 2;
	for (x = 0; x < 480; x++) {
		uint32 clearValue = full_day[x];
		uint32 utcOffset = 0;
		dcf77_meteotime_print(clearValue, hour, min, utcOffset);

		min += 3;
		if (min >= 60) {
			min -= 60;
			hour++;
		}
	}
}