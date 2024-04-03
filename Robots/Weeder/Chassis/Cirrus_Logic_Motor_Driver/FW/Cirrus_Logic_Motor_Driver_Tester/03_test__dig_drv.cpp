
#include "common.h"


int main(void) {
	// TODO Test this.

	dig_drv.ddr_out0 = ~0;
	dig_drv.ddr_out1 = ~0;
	dig_drv.ddr_in0 = 0;
	dig_drv.ddr_in1 = 0;

	dig_drv.n_en = 0; // #EN active.
	
	uint8_t u = 0;
	uint8_t v = 0;
	uint8_t w = 0;
	while(1){ //infinite loop
		_delay_ms(1); // 1kHz.
		u = ~u;
		v = ~v;
		w = ~w;
		
		if(u){
			dig_drv.ul = 0;
		}else{
			dig_drv.uh = 0;
		}
		if(v){
			dig_drv.vl = 0;
		}else{
			dig_drv.vh = 0;
		}
		if(w){
			dig_drv.wl = 0;
		}else{
			dig_drv.wh = 0;
		}
		// Dead time.
		_delay_us(1); // 1 us.
		if(u){
			dig_drv.ul = 1;
		}else{
			dig_drv.uh = 1;
		}
		if(v){
			dig_drv.vl = 1;
		}else{
			dig_drv.vh = 1;
		}
		if(w){
			dig_drv.wl = 1;
		}else{
			dig_drv.wh = 1;
		}
		
	}

}
