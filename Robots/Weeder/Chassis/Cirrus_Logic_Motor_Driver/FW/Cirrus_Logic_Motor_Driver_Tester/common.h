
#pragma once


#include <Arduino.h>

#include <avr/io.h>
#include <util/delay.h>

#include "avr_io_bitfields.h"



typedef union {
	struct {
		//u8 pinb;
		u8 : 8;
		//u8 ddrb;
		u8 ddr_out0 : 6;
		u8          : 2;
		//u8 portb;
		u8 uh     : 1; // PB0
		u8 ul     : 1; // PB1
		u8 vh     : 1; // PB2
		u8 vl     : 1; // PB3
		u8 wh     : 1; // PB4
		u8 wl     : 1; // PB5
		// GND
		u8        : 2;

		//u8 pinc;
		// GND
		// Not used
		// Not used
		u8 dir     : 1; // PC0
		u8 pls     : 1; // PC1
		u8 : 6;
		//u8 ddrc;
		u8 ddr_in0  : 2; // PC0-1
		u8 : 6;
		//u8 portc;
		u8 : 8;

		//u8 pind;
		u8 : 6;
		u8 fault    : 1; // PD6
		u8 : 1;
		//u8 ddrd;
		u8 : 6;
		u8 ddr_in1  : 1;  // PD6
		u8 ddr_out1 : 1; // PD7
		//u8 portd;
		u8 : 7;
		u8 n_en     : 1; // PD7
	};
} bf_dig_drv;
#define dig_drv (*((volatile bf_dig_drv*)(&PINB)))

