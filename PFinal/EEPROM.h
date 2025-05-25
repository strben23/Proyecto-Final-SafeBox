/*
 * EEPROM.h
 *
 * Created: 4/9/2025 10:34:02 PM
 *  Author: Gaelp
 */ 


#ifndef EEPROM_H
#define EEPROM_H

#include <avr/io.h>

unsigned char EEPROM_read(unsigned int);
void EEPROM_write(unsigned int, unsigned char);
void EEPROM_update(unsigned int, unsigned char);

unsigned char EEPROM_read(unsigned int direccion)
{
	while(EECR & (1<<EEPE)); // Esperar a la escritura previa
	EEAR = direccion; // Configurar registro de la dirección
	EECR |= (1<<EERE); // Iniciar lectura del EEPROM
	return EEDR; // Devolver dato del registro de datos
}

void EEPROM_write(unsigned int direccion, unsigned char dato)
{
	while(EECR & (1<<EEPE));
	EEAR = direccion; // Configurar registro de dirección
	EEDR = dato; // Configurar registro de datos
	EECR |= (1<<EEMPE); // Escribir uno en el registro
	EECR |= (1<<EEPE); // Iniciar escritura en EEPROM
}


void EEPROM_update(unsigned int direccion, unsigned char dato)
{
	unsigned char current_value = EEPROM_read(direccion); // Guardar nuevo dato en variable
	if(current_value != dato) // Si el dato actual es diferente, guarda nuevo dato
	{
		EEPROM_write(direccion, dato); // Escritura del nuevo dato
	}
}

#endif /* EEPROM_H_ */