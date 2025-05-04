#ifndef _DCF77_METEOTIME_DECRYPT_H_
#define _DCF77_METEOTIME_DECRYPT_H_

	#include "k_stdtype.h"
	
	extern uint8 dcf77_meteotime_decrypt(unsigned char *encrypted, unsigned char *decrypted);
	extern void dcf77_meteotime_decrypt_dcf77_to_encrypted_packet(unsigned char *dcf77_data_0, unsigned char *dcf77_data_1, unsigned char *dcf77_data_2, unsigned char *dcf77_meteotime_ecrypted);

#endif
