
#include <avr/io.h>
#include <util/delay.h>

#include "avr_io_bitfields.h"

#if 1
#define CYCLES 1
#else
#define CYCLES -1 // Infinite.
#endif
#define ROUNDS 4

#if 0
#define PHASE_HZ 1
#define PULSE_HZ 1
#elif 1
#define PHASE_HZ 10
#define PULSE_HZ 10
#else
#define PHASE_HZ 100
#define PULSE_HZ 100
#endif

#if 0
#define PWM_HZ 1000
#define DUTY 55
#else
#define PWM_HZ 20000
#define DUTY 80
#endif

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
	for(int i = 0; i < (PWM_HZ)/(PULSE_HZ); i++){
		one_pwm_pulse(u, v, w);
	}
	for(int i = 0; i < (PWM_HZ)/(PHASE_HZ)-(PWM_HZ)/(PULSE_HZ); i++){
		_delay_us(T_pwm_us); 
	}
}


typedef struct {
	unsigned dir : 1;
	unsigned pulse : 1;
	unsigned zero : 1;
	unsigned hal : 3;
	unsigned _nc0 :1;
	unsigned _nc1 :1;
} bf_a;
#define pa (*((volatile bf_a*)(&PORTA)))

typedef struct {
	unsigned leftmost_green_led : 1;
	unsigned right_green_led : 1;
	unsigned jtag_pins : 4;
	unsigned left_red_led :1;
	unsigned rightmost_red_led :1;
} bf_c;
#define pc (*((volatile bf_c*)(&PORTC)))

int main(void) {
	// Make all pins outputs.
	DDRA = bf_to_u8<bf_a>({
		.dir = 0, // in
		.pulse = 0, // in
		.zero = 1, // out
		.hal = 0b000, // in
		// in NC
		._nc0 = 0,
		._nc1 = 0,
	});
	DDRB = 0xff; // All outputs.
	DDRC = bf_to_u8<bf_c>({
		.leftmost_green_led = 1, // Leftmost green LED.
		.right_green_led = 1, // Right green LED.
		.jtag_pins = 0b0000, // JTAG pins to be inputs.
		.left_red_led = 1, // Left red LED.
		.rightmost_red_led = 1, // Rightmost red LED.
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
	
	portc.b0 = 1; // Leftmost green LED.
	portc.b7 = 0; // Rightmost red LED.
	
	
	portd.b6 = 0; // #EN active.
	
	for(uint8_t cycles = 0; cycles < CYCLES; cycles++){
		for(uint8_t rounds = 0; rounds < ROUNDS; rounds++){
			one_phase_pulse(CH_MAX, CH_MIN, CH_FLT); // ABC=001
			one_phase_pulse(CH_MAX, CH_FLT, CH_MIN); // ABC=101
			one_phase_pulse(CH_FLT, CH_MAX, CH_MIN); // ABC=100
			one_phase_pulse(CH_MIN, CH_MAX, CH_FLT); // ABC=110
			one_phase_pulse(CH_MIN, CH_FLT, CH_MAX); // ABC=010
			one_phase_pulse(CH_FLT, CH_MIN, CH_MAX); // ABC=011
		}
	}
}
