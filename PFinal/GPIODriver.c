/*
 * GPIODriver.c
 * Implementación de la librería GPIO para ATmega328P
 *
 * Created: 2/26/2025 4:40:20 PM
 *  Author: Gaelp
 */ 

#include "GPIODriver.h"

void GPIO_ConfigPin(uint8_t puerto, uint8_t pin, uint8_t modo) {
	volatile uint8_t *ddr_reg;
	
	// Seleccionar registro DDR según el puerto
	if (puerto == PORT_B) {
		ddr_reg = &DDRB;
		} else if (puerto == PORT_C) {
		ddr_reg = &DDRC;
		} else if (puerto == PORT_D) {
		ddr_reg = &DDRD;
	}
	
	// Configurar pin como entrada o salida
	if (modo == OUTPUT) {
		*ddr_reg |= (1 << pin);  // Configurar como salida
		} else {
		*ddr_reg &= ~(1 << pin); // Configurar como entrada
		
		// Si es entrada, habilitar resistencia pull-up interna
		volatile uint8_t *port_reg;
		if (puerto == PORT_B) {
			port_reg = &PORTB;
			} else if (puerto == PORT_C) {
			port_reg = &PORTC;
			} else if (puerto == PORT_D) {
			port_reg = &PORTD;
		}
		*port_reg |= (1 << pin);  // Habilitar pull-up
	}
}

void GPIO_ConfigPuerto(uint8_t puerto, uint8_t modo) {
	volatile uint8_t *ddr_reg;
	volatile uint8_t *port_reg;
	
	// Seleccionar registros según el puerto
	if (puerto == PORT_B) {
		ddr_reg = &DDRB;
		port_reg = &PORTB;
		} else if (puerto == PORT_C) {
		ddr_reg = &DDRC;
		port_reg = &PORTC;
		} else if (puerto == PORT_D) {
		ddr_reg = &DDRD;
		port_reg = &PORTD;
	}
	
	// Configurar todo el puerto
	if (modo == OUTPUT) {
		*ddr_reg = 0xFF;  // Todos los pines como salida
		} else {
		*ddr_reg = 0x00;  // Todos los pines como entrada
		*port_reg = 0xFF; // Habilitar resistencias pull-up para entradas
	}
}

uint8_t GPIO_LeerPin(uint8_t puerto, uint8_t pin) {
	volatile uint8_t *pin_reg;
	
	// Seleccionar registro PIN según el puerto
	if (puerto == PORT_B) {
		pin_reg = &PINB;
		} else if (puerto == PORT_C) {
		pin_reg = &PINC;
		} else if (puerto == PORT_D) {
		pin_reg = &PIND;
	}
	
	// Leer el estado del pin
	if (*pin_reg & (1 << pin)) {
		return HIGH;
		} else {
		return LOW;
	}
}

void GPIO_EscribirPin(uint8_t puerto, uint8_t pin, uint8_t valor) {
	volatile uint8_t *port_reg;
	
	// Seleccionar registro PORT según el puerto
	if (puerto == PORT_B) {
		port_reg = &PORTB;
		} else if (puerto == PORT_C) {
		port_reg = &PORTC;
		} else if (puerto == PORT_D) {
		port_reg = &PORTD;
	}
	
	// Escribir valor al pin
	if (valor == HIGH) {
		*port_reg |= (1 << pin);  // Establecer pin en alto
		} else {
		*port_reg &= ~(1 << pin); // Establecer pin en bajo
	}
}

uint8_t GPIO_LeerPuerto(uint8_t puerto) {
	// Devolver el estado de todo el puerto
	if (puerto == PORT_B) {
		return PINB;
		} else if (puerto == PORT_C) {
		return PINC;
		} else if (puerto == PORT_D) {
		return PIND;
	}
	return 0;
}

void GPIO_EscribirPuerto(uint8_t puerto, uint8_t valor) {
	// Escribir valor a todo el puerto
	if (puerto == PORT_B) {
		PORTB = valor;
		} else if (puerto == PORT_C) {
		PORTC = valor;
		} else if (puerto == PORT_D) {
		PORTD = valor;
	}
}