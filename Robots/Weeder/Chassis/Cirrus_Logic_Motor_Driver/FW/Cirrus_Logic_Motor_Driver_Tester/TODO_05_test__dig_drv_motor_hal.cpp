
#include <avr/io.h>
#include <util/delay.h>

#include "avr_io_bitfields.h"

#define PHASE_HZ 1000
#define PWM_HZ 3000
//#define DUTY 55 // For this PWM, need 14us to pass phase.
//#define DUTY 60 // For this PWM, need 10us to pass phase.
#define DUTY 70
#define DEAD_TIME_US 1

#define wait_for(expr) do{}while(!(expr))

typedef enum {
	CH_MIN = -1,
	CH_FLT =  0,
	CH_MAX =  1
} ch_state_t;


#define T_pwm_us      (1000000L/(PWM_HZ))
#define T_active_us   (T_pwm_us*(DUTY)/100)
#define T_inactive_us (T_pwm_us*(100-(DUTY))/100)
void one_pwm_pulse(
	ch_state_t u,
	ch_state_t v,
	ch_state_t w
) {
	
	// 1 - active, 0 - inactive.
	for(int8_t pwm = 1; pwm >= 0; pwm--){
		
		// Turn off all.
		portd.b0 = 0; // UH
		portd.b1 = 0; // UL
		portd.b2 = 0; // VH
		portd.b3 = 0; // VL
		portd.b4 = 0; // WH
		portd.b5 = 0; // WL
		
		// Dead time.
		_delay_us(DEAD_TIME_US); // 1 us.
		
		switch(u){
			case CH_MIN:
				portd.b0 = !pwm; // UH
				portd.b1 = pwm; // UL
				break;
			case CH_FLT:
				// Both stay on 0.
				break;
			case CH_MAX:
				portd.b0 = pwm; // UH
				portd.b1 = !pwm; // UL
				break;
		}
		switch(v){
			case CH_MIN:
				portd.b2 = !pwm; // VH
				portd.b3 = pwm; // VL
				break;
			case CH_FLT:
				// Both stay on 0.
				break;
			case CH_MAX:
				portd.b2 = pwm; // VH
				portd.b3 = !pwm; // VL
				break;
		}
		switch(w){
			case CH_MIN:
				portd.b4 = !pwm; // WH
				portd.b5 = pwm; // WL
				break;
			case CH_FLT:
				// Both stay on 0.
				break;
			case CH_MAX:
				portd.b4 = pwm; // WH
				portd.b5 = !pwm; // WL
				break;
		}
		
		// Delay of specific active/inactive state.
		if(pwm){
			_delay_us(T_active_us-DEAD_TIME_US); 
		}else{
			_delay_us(T_inactive_us-DEAD_TIME_US); 
		}
	}
	
	
}

void one_phase_pulse(
	ch_state_t u,
	ch_state_t v,
	ch_state_t w
) {
	for(int i = 0; i < (PWM_HZ)/(PHASE_HZ); i++){
		one_pwm_pulse(u, v, w);
	}
}


struct bf_a {
	unsigned dir : 1;
	unsigned pulse : 1;
	unsigned zero : 1;
	unsigned cba : 3; // TODO Some union.
	unsigned _nc0 :1;
	unsigned _nc1 :1;
};
#define o_cnc_hal (*((volatile bf_a*)(&PORTA)))
#define i_cnc_hal (*((const volatile bf_a*)(&PINA)))

struct bf_c {
	unsigned leftmost_green : 1;
	unsigned right_green : 1;
	unsigned jtag_pins : 4;
	unsigned left_red :1;
	unsigned rightmost_red :1;
};
#define o_led (*((volatile bf_c*)(&PORTC)))

int main(void) {
	// Make all pins outputs.
	DDRA = bf_to_u8<bf_a>({
		.dir = 0, // in
		.pulse = 0, // in
		.zero = 1, // out
		.cba = 0b000,  // in
		// in NC
		._nc0 = 0,
		._nc1 = 0,
	});
	DDRB = 0xff; // All outputs.
	DDRC = bf_to_u8<bf_c>({
		.leftmost_green = 1,
		.right_green = 1,
		.jtag_pins = 0b0000, // JTAG pins to be inputs.
		.left_red = 1,
		.rightmost_red = 1,
	});
	DDRD = bf_to_u8<bf_8b>({
		.b0 = 1, // UH
		.b1 = 1, // UL
		.b2 = 1, // VH
		.b3 = 1, // VL
		.b4 = 1, // WH
		.b5 = 1, // WL
		.b6 = 1, // #EN
		.b7 = 0, // FAULT
	});
	
	PORTD = bf_to_u8<bf_8b>({
		.b0 = 0, // UH
		.b1 = 0, // UL
		.b2 = 0, // VH
		.b3 = 0, // VL
		.b4 = 0, // WH
		.b5 = 0, // WL
		.b6 = 1, // #EN
		.b7 = 0, // FAULT
	});
	
	o_led.leftmost_green = 0;
	o_led.rightmost_red = 0;
	
	
	portd.b6 = 0; // #EN active.
	
	while(1){ // Infinite loop.
		//o_led.leftmost_green = (i_cnc_hal.cba>>0)&1;
		//o_led.rightmost_red  = (i_cnc_hal.cba>>1)&1;
		
		while(1){
			one_phase_pulse(CH_MAX, CH_MIN, CH_FLT); // ABC=001
			if(i_cnc_hal.cba == 0b100){
				o_led.leftmost_green = 1;
				break;
			}
		}
		while(1){
			one_phase_pulse(CH_MAX, CH_FLT, CH_MIN); // ABC=101
			if(i_cnc_hal.cba == 0b101){
				o_led.rightmost_red = 1;
				break;
			}
		}
		while(1){
			one_phase_pulse(CH_FLT, CH_MAX, CH_MIN); // ABC=100
			if(i_cnc_hal.cba == 0b001){
				break;
			}
		}
		while(1){
			one_phase_pulse(CH_MIN, CH_MAX, CH_FLT); // ABC=110
			if(i_cnc_hal.cba == 0b011){
				break;
			}
		}
		while(1){
			one_phase_pulse(CH_MIN, CH_FLT, CH_MAX); // ABC=010
			if(i_cnc_hal.cba == 0b010){
				break;
			}
		}
		while(1){
			one_phase_pulse(CH_FLT, CH_MIN, CH_MAX); // ABC=011
			if(i_cnc_hal.cba == 0b110){
				break;
			}
		}
		
		o_led.leftmost_green = 0;
		o_led.rightmost_red = 0;
	}
}
