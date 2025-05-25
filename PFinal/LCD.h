/*
 * LCD.h
 *
 * Created: 3/12/2025 4:54:45 PM
 *  Author: Gaelp
 */ 


#ifndef LCD_H_
#define LCD_H_

#define F_CPU 16000000UL // Frecuencia de 16MHz
#include <avr/io.h>
#include <util/delay.h>

#define DATA_BUS	PORTC
#define CTL_BUS		PORTC
#define DATA_DDR	DDRC
#define CTL_DDR		DDRC
#define LCD_RS		PINC0
#define LCD_EN		PINC1
#define LCD_D4		PINC2
#define LCD_D5		PINC3
#define LCD_D6		PINC4
#define LCD_D7		PINC5

#define LCD_CMD_CLEAR_DISPLAY			0x01
#define LCD_CMD_CURSOR_HOME				0x02

// Display control
#define LCD_CMD_DISPLAY_OFF				0x08
#define LCD_CMD_DISPLAY_NO_CURSOR		0x0c
#define LCD_CMD_DISPLAY_CURSOR_NO_BLINK	0x0E
#define LCD_CMD_DISPLAY_CURSOR_BLINK	0x0F

// Function set
#define LCD_CMD_4BIT_2ROW_5X7			0x28
#define LCD_CMD_8BIT_2ROW_5X7			0x38

// Functions prototypes
void lcd_init(void);
void lcd_send_command(uint8_t);
void lcd_write_character(uint8_t);
void lcd_write_word(uint8_t []);
void lcd_clear(void);
void lcd_goto_xy(uint8_t, uint8_t);


#endif /* LCD_H_ */