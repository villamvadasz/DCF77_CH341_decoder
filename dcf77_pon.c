#include <stdio.h>
#include <string.h>

#include "dcf77.h"
#include "mal.h"
#include "c_dcf77.h"

uint8 dcf77_reset_done = 0;

void dcf77_pon(void) {
	static unsigned int dcf77_port_change = 5000;
	static uint32 dcf77_pon_delay = 0;
	static unsigned char dcf77_pon_sm = 0;
	switch (dcf77_pon_sm) {
		case 0 : {
			dcf77_set_PON(1);
			dcf77_pon_delay = 3000;
			dcf77_pon_sm = 1;
		}
		case 1 : {
			if (dcf77_pon_delay) {
				dcf77_pon_delay--;
			} else {
				dcf77_set_PON(0);
				dcf77_reset_done = 1;
				#if defined(DCF77_TRIGGER_PON_EVERY_X_HOUR)
					dcf77_pon_delay = 1000 * 60 * 60 * DCF77_TRIGGER_PON_EVERY_X_HOUR;
					dcf77_pon_sm = 2;
				#else
					#if defined(DCF77_TRIGGER_PON_WHEN_NO_CHANGE_ON_PORT)
						dcf77_port_change = 5000;
						dcf77_pon_sm = 5;
					#else
						dcf77_pon_sm = 99;
					#endif
				#endif
			}
			break;
		}
		case 2 : {
			if (dcf77_pon_delay) {
				dcf77_pon_delay--;
			} else {
				dcf77_set_PON(1);
				dcf77_pon_delay = 1500;
				dcf77_pon_sm = 1;
			}
			break;
		}
		case 5 : {
			static uint8 dcf77_port_change_prev = 0;
			if (dcf77_port_change_prev != dcf77_port_value_isr) {
				dcf77_port_change = 5000;
			}
			if (dcf77_port_change) {
				dcf77_port_change--;
			} else {
				dcf77_set_PON(1);
				dcf77_pon_delay = 1500;
				dcf77_pon_sm = 1;
			}
		
			break;
		}	
		case 99 : {
			break;
		}
		default : {
			dcf77_pon_sm = 0;
			break;
		}
	}
}
