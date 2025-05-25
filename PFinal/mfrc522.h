/*
 * mfrc522.h
 *
 * Created: 5/23/2025 8:37:55 AM
 *  Author: Gaelp
 */ 


#ifndef MFRC522_H_
#define MFRC522_H_

#include <avr/io.h>

// SPI Pins
#define SPI_DDR   DDRB
#define SPI_PORT  PORTB
#define SPI_CS    PB2
#define SPI_MOSI  PB3
#define SPI_MISO  PB4
#define SPI_SCK   PB5

// MFRC522 Commands
#define PCD_IDLE              0x00
#define PCD_TRANSCEIVE        0x0C
#define PCD_AUTHENT           0x0E
#define PCD_SOFTRESET         0x0F

// MFRC522 Registers
#define CommandReg            0x01
#define CommIEnReg            0x02
#define DivIEnReg             0x03
#define CommIrqReg            0x04
#define FIFOLevelReg          0x0A
#define FIFODataReg           0x09
#define BitFramingReg         0x0D
#define ControlReg            0x0C
#define ErrorReg              0x06
#define Status2Reg            0x08
#define ModeReg               0x11
#define TxControlReg          0x14
#define TxASKReg              0x15
#define CRCResultRegH         0x21
#define CRCResultRegL         0x22
#define VersionReg            0x37
#define TxModeReg           0x12
#define RxModeReg           0x13
#define ModWidthReg         0x24
#define TModeReg            0x2A
#define TPrescalerReg       0x2B
#define TReloadRegL         0x2D
#define TReloadRegH         0x2C


// Utility macros
#define CS_ENABLE()   (SPI_PORT &= ~(1 << SPI_CS))
#define CS_DISABLE()  (SPI_PORT |= (1 << SPI_CS))
#define RST_HIGH()    (PORTB |= (1 << PB1))
#define RST_LOW()     (PORTB &= ~(1 << PB1))

void SPI_init();
char SPI_transfer(char data);
void MFRC522_init();
void MFRC522_reset();
void MFRC522_write(uint8_t addr, uint8_t val);
uint8_t MFRC522_read(uint8_t addr);
void MFRC522_antenna_on();
void MFRC522_setBitMask(uint8_t reg, uint8_t mask);
void MFRC522_clearBitMask(uint8_t reg, uint8_t mask);
char MFRC522_request(uint8_t reqMode, uint8_t *tagType);
char MFRC522_anticoll(uint8_t *serNum);
char MFRC522_toCard(uint8_t command, uint8_t *sendData, uint8_t sendLen,
uint8_t *backData, uint8_t *backLen);


#endif
