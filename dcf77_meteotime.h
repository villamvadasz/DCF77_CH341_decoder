#ifndef _DCF77_METEOTIME_H_
#define _DCF77_METEOTIME_H_

	#include "k_stdtype.h"

	typedef struct _DCF77_WeatherData {
		uint32 clearValue;
		uint8 hour;
		uint8 min;
		uint8 utcOffset;
	} DCF77_WeatherData;

	extern uint8 dcf77_meteotime_getMeteoData(uint32 item, DCF77_WeatherData *weatherData);

	extern void dcf77_meteotime_print(uint32 clearValue, uint32 hour, uint32 min, uint32 utcOffset);
	extern void dcf77_meteotime_getTemperature_string(char *str, uint32 clearValue, uint32 hour);
	extern void dcf77_meteotime_getRegion_string(char *str1, char *str2, char *str3, char *str4, uint32 clearValue, uint32 hour, uint32 min, uint32 utcOffset);
	extern void dcf77_meteotime_getDay_string(char *str, uint32 clearValue, uint32 hour, uint32 min, uint32 utcOffset);
	extern void dcf77_meteotime_getNight_string(char *str, uint32 clearValue, uint32 hour, uint32 min, uint32 utcOffset);
	extern void dcf77_meteotime_getExtreme_string(char *str, uint32 clearValue, uint32 hour, uint32 min, uint32 utcOffset);
	extern void dcf77_meteotime_getRain_string(char *str, uint32 clearValue, uint32 hour, uint32 min, uint32 utcOffset);
	extern void dcf77_meteotime_getWind_string(char *str1, char *str2, uint32 clearValue, uint32 hour, uint32 min, uint32 utcOffset);
	extern void dcf77_meteotime_getAnomaly_string(char *str, uint32 clearValue, uint32 hour, uint32 min, uint32 utcOffset);
	extern void dcf77_meteotime_getAnomalyType_string(char *str1, char *str2, uint32 clearValue, uint32 hour, uint32 min, uint32 utcOffset);
	
	extern unsigned int dcf77_meteotime_getAnomaly(uint32 clearValue, uint32 hour, uint32 min, uint32 utcOffset);

	extern uint8 dcf77_meteotime_decrypt_dataset(unsigned char *encrypted, unsigned char *decrypted, uint32 *clearValue);

	//Internal interface
	extern uint16 dcf77_meteotime_CalculateSetNumber(uint32 hour, uint32 min, uint32 utcOffset);
	extern void dcf77_meteotime_packet_add_received_message(unsigned char *dcf77_data);
	extern uint8 dcf77_meteotime_packet_get_received_weather(uint32 *clearValue, uint8 *hour, uint8 *min, uint8 *utcOffset);

	extern void init_dcf77_meteotime(void);
	extern void do_dcf77_meteotime(void);
	extern void isr_dcf77_meteotime_1ms(void);
	
	extern void init_dcf77_meteotime_packet(void);
	extern void do_dcf77_meteotime_packet(void);
	extern void isr_dcf77_meteotime_packet_1ms(void);
	

#endif
