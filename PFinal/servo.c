#include "servo.h"

void servo_init(void) {
	DDRB |= (1 << PB1); // OC1A = Pin 9 en Arduino Uno

	// Modo 14: Fast PWM con TOP en ICR1
	TCCR1A = (1 << COM1A1) | (1 << WGM11);
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11); // Prescaler 8

	ICR1 = 39999; // TOP = 20 ms -> 16MHz / (8 * 50 Hz) - 1

	OCR1A = 3000; // Pulso inicial (1.5 ms = posición neutral)
}

void servoAngulo(uint8_t angulo) {
	if (angulo > 180) {
		angulo = 180;
	}

	// 0–180 a 1100–4700
	uint16_t valorOCR = ((uint32_t)angulo * (4700 - 1100)) / 180 + 1100;
	OCR1A = valorOCR;
}
