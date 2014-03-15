/**
 * Convert a PWM signal to analog
 *
 * Expecting a single channel value > 1000ns and < 2000ns. The result shall be
 * min: 5V, max 0V. RSSI is expected that way by APM on an analog 5V pin.
 *
 * This code is written for a 5V Arduino Pro Mini.
 *
 * (c) 2014, Simon Wunderlin, <swunderlin()gmai1:com>
 */

//#define DEBUG
#define PPM_MAX 2000
#define PPM_MIN 1000
#define ANALOG_OUT_PIN 3
#define PPM_INT 0 // pin 2, ppm in

//int Chan1Interrupt = 0; // pin 2, ppm in

volatile unsigned long startPulse = 0;
volatile double val, val_last = 0;
uint8_t a_out = 0;

void setup() {
	// out analog output, 5-0 V
	pinMode(ANALOG_OUT_PIN, OUTPUT);
	// attaching interrupt to int 0 (pin 2 on arduino pro mini)
	attachInterrupt(PPM_INT, begin, RISING);
#ifdef DEBUG
	Serial.begin(115200);
#endif
}

void loop() {
	// use last good ppm frame and convert it to an analog value, 0-255
	// 255 == 5V, 5V = 0%, 0V = 100% on APM:Plane
	a_out = map(val, PPM_MAX, PPM_MIN, 0, 255);
	// write it out to analog pin
	analogWrite(ANALOG_OUT_PIN, a_out);
#ifdef DEBUG
	Serial.print(a_out);
	Serial.print("\t");
	Serial.println(val);
#endif
}

void begin() {
	startPulse = micros();
	detachInterrupt(PPM_INT);
	attachInterrupt(PPM_INT, end, FALLING);
}

void end() {
	val = micros() - startPulse;
	detachInterrupt(PPM_INT);
	attachInterrupt(PPM_INT, begin, RISING);
	if (val < PPM_MIN || val > PPM_MAX)
		val = val_last;
	else
		val_last = val;
}
