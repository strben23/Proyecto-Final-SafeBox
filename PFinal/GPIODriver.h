/*
 * GPIODriver.h
 * Librería para controlar puertos GPIO del ATmega328P
 *
 * Created: 2/26/2025 4:36:45 PM
 *  Author: Gaelp
 */ 


#ifndef GPIODRIVER_H_
#define GPIODRIVER_H_

#include <avr/io.h>

// Definiciones para configuración de pines
#define INPUT 0
#define OUTPUT 1

// Definiciones para estados
#define LOW 0
#define HIGH 1

// Definiciones de puertos
#define PORT_B 0
#define PORT_C 1
#define PORT_D 2

/**
 * Configura un pin específico como entrada o salida
 * puerto Número de puerto (PORT_B, PORT_C, PORT_D)
 * pin Número de pin (0-7)
 * modo (INPUT o OUTPUT)
 */
void GPIO_ConfigPin(uint8_t puerto, uint8_t pin, uint8_t modo);

/**
 * Configura todos los pines de un puerto como entrada o salida
 * puerto Número de puerto (PORT_B, PORT_C, PORT_D)
 * modo (INPUT o OUTPUT)
 */
void GPIO_ConfigPuerto(uint8_t puerto, uint8_t modo);

/**
 * Lee el estado de un pin específico
 * puerto Número de puerto (PORT_B, PORT_C, PORT_D)
 * pin Número de pin (0-7)
 * @return Estado del pin (LOW o HIGH)
 */
uint8_t GPIO_LeerPin(uint8_t puerto, uint8_t pin);

/**
 * Escribe un valor en un pin específico
 * puerto Número de puerto (PORT_B, PORT_C, PORT_D)
 * pin Número de pin (0-7)
 * valor Valor a escribir (LOW o HIGH)
 */
void GPIO_EscribirPin(uint8_t puerto, uint8_t pin, uint8_t valor);

/**
 * Lee el estado de todo el puerto
 * puerto Número de puerto (PORT_B, PORT_C, PORT_D)
 * @return Estado del puerto (8 bits, cada bit corresponde a un pin)
 */
uint8_t GPIO_LeerPuerto(uint8_t puerto);

/**
 * Escribe valores en todo el puerto
 * puerto Número de puerto (PORT_B, PORT_C, PORT_D)
 * valor a escribir (8 bits, cada bit corresponde a un pin)
 */
void GPIO_EscribirPuerto(uint8_t puerto, uint8_t valor);


#endif /* GPIODRIVER_H_ */