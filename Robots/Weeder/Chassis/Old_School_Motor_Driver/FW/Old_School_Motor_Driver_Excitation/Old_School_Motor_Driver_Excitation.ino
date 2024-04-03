


#define T_MS (500)
#define DELAY() do{ delay(T_MS); }while(0)

// Pin positions.
#define N_SIG 2



void setup() {

	Serial.begin(115200);
	Serial.println("setup() running...");
	delay(1); // [ms]

	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, 0);
	pinMode(N_SIG, OUTPUT);
	digitalWrite(N_SIG, 0);

}


void loop() {
	DELAY();
	digitalWrite(LED_BUILTIN, 0);
	digitalWrite(N_SIG, 0);
	
	DELAY();
	digitalWrite(LED_BUILTIN, 1);
	digitalWrite(N_SIG, 1);
}
