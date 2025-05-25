/*
 * LCD.c
 *
 * Created: 3/14/2025 1:40:53 PM
 *  Author: Gaelp
 */ 

#include "LCD.h"

void lcd_init(void)
{
	DATA_DDR = (1<<LCD_D7) | (1<<LCD_D6) | (1<<LCD_D5) | (1<<LCD_D4);
	CTL_DDR |= (1<<LCD_EN) | (1<<LCD_RS);
	DATA_BUS = (0<<LCD_D7) | (0<<LCD_D6) | (1<<LCD_D5) | (0<<LCD_D4);
	CTL_BUS |= (1<<LCD_EN);
	CTL_BUS &= ~(1<<LCD_RS);
	_delay_ms(1);
	CTL_BUS &= ~(1<<LCD_EN);
	_delay_ms(1);
	lcd_send_command(LCD_CMD_4BIT_2ROW_5X7);
	_delay_ms(1);
	lcd_send_command(LCD_CMD_DISPLAY_CURSOR_BLINK);
	_delay_ms(1);
	lcd_send_command(0x80);
}

void lcd_send_command(uint8_t command)
{
	DATA_BUS |= (command>>2) & 0b00111100;
	DATA_BUS &= (command>>2) | 0b11000011;
	CTL_BUS &= ~(1<<LCD_RS);
	CTL_BUS |= (1<<LCD_EN);
	_delay_ms(1);
	CTL_BUS &= ~(1<<LCD_EN);
	_delay_ms(1);
	DATA_BUS |= (command<<2) & 0b00111100;
	DATA_BUS &= (command<<2) | 0b11000011;
	CTL_BUS |= (1<<LCD_EN);
	_delay_ms(1);
	CTL_BUS &= ~(1<<LCD_EN);
	_delay_ms(1);
}

void lcd_write_character(uint8_t character)
{
	DATA_BUS |= (character>>2) & 0b00111100;
	DATA_BUS &= (character>>2) | 0b11000011;
	CTL_BUS |= (1<<LCD_RS);
	CTL_BUS |= (1<<LCD_EN);
	_delay_ms(2);
	CTL_BUS &= ~(1<<LCD_EN);
	_delay_ms(2);
	DATA_BUS |= (character<<2) & 0b00111100;
	DATA_BUS &= (character<<2) | 0b11000011;
	CTL_BUS |= (1<<LCD_EN);
	_delay_ms(2);
	CTL_BUS &= ~(1<<LCD_EN);
	_delay_ms(2);
}

void lcd_write_word(uint8_t word[20])
{
	int i = 0;
	while(word[i] != '\0')
	{
		lcd_write_character(word[i]);
		i++;
	}
}

void lcd_clear(void)
{
	lcd_send_command(LCD_CMD_CLEAR_DISPLAY);
	_delay_ms(5);
}

void lcd_goto_xy(uint8_t line, uint8_t pos)
{
	lcd_send_command((0x80 | (line << 6)) + pos);
	_delay_us(50);
}
