/*
 * mfrc522.c
 *
 * Created: 5/23/2025 8:38:31 AM
 *  Author: Gaelp
 */ 

#include "mfrc522.h"
#include <util/delay.h>

void SPI_init() {
	SPI_DDR |= (1 << SPI_CS) | (1 << SPI_MOSI) | (1 << SPI_SCK);
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
	CS_DISABLE();
}

char SPI_transfer(char data) {
	SPDR = data;
	while (!(SPSR & (1 << SPIF)));
	return SPDR;
}

void MFRC522_write(uint8_t addr, uint8_t val) {
	CS_ENABLE();
	SPI_transfer((addr << 1) & 0x7E);
	SPI_transfer(val);
	CS_DISABLE();
}

uint8_t MFRC522_read(uint8_t addr) {
	CS_ENABLE();
	SPI_transfer(((addr << 1) & 0x7E) | 0x80);
	uint8_t val = SPI_transfer(0x00);
	CS_DISABLE();
	return val;
}

void MFRC522_setBitMask(uint8_t reg, uint8_t mask) {
	uint8_t tmp = MFRC522_read(reg);
	MFRC522_write(reg, tmp | mask);
}

void MFRC522_clearBitMask(uint8_t reg, uint8_t mask) {
	uint8_t tmp = MFRC522_read(reg);
	MFRC522_write(reg, tmp & (~mask));
}

void MFRC522_reset() {
	MFRC522_write(CommandReg, PCD_SOFTRESET);
	_delay_ms(50);
}

void MFRC522_antenna_on() {
	uint8_t temp = MFRC522_read(TxControlReg);
	if (!(temp & 0x03)) {
		MFRC522_setBitMask(TxControlReg, 0x03);
	}
}

void MFRC522_init() {
	SPI_init();
	MFRC522_reset();
	MFRC522_write(TxModeReg, 0x00);
	MFRC522_write(RxModeReg, 0x00);
	MFRC522_write(ModWidthReg, 0x26);
	MFRC522_write(TModeReg, 0x80);
	MFRC522_write(TPrescalerReg, 0xA9);
	MFRC522_write(TReloadRegL, 0xE8);
	MFRC522_write(TReloadRegH, 0x03);
	MFRC522_write(TxASKReg, 0x40);
	MFRC522_write(ModeReg, 0x3D);
	MFRC522_antenna_on();
}

char MFRC522_request(uint8_t reqMode, uint8_t *tagType) {
	char status;
	uint8_t backBits;

	MFRC522_write(BitFramingReg, 0x07);
	tagType[0] = reqMode;
	status = MFRC522_toCard(PCD_TRANSCEIVE, tagType, 1, tagType, &backBits);
	return (status && (backBits == 0x10));
}

char MFRC522_anticoll(uint8_t *serNum) {
	char status;
	uint8_t i;
	uint8_t serNumCheck = 0;
	uint8_t unLen;

	MFRC522_write(BitFramingReg, 0x00);
	serNum[0] = 0x93;
	serNum[1] = 0x20;

	status = MFRC522_toCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);
	if (!status) return 0;

	for (i = 0; i < 4; i++)
	serNumCheck ^= serNum[i];
	return (serNumCheck == serNum[4]);
}

char MFRC522_toCard(uint8_t command, uint8_t *sendData, uint8_t sendLen,
uint8_t *backData, uint8_t *backLen) {
	uint8_t status = 0;
	uint8_t irqEn = 0x77;
	uint8_t waitIRq = 0x30;
	uint8_t lastBits;
	uint8_t n;
	uint16_t i;

	MFRC522_write(CommandReg, PCD_IDLE);
	MFRC522_write(CommIEnReg, irqEn | 0x80);
	MFRC522_clearBitMask(CommIrqReg, 0x80);
	MFRC522_setBitMask(FIFOLevelReg, 0x80);

	for (i = 0; i < sendLen; i++) {
		MFRC522_write(FIFODataReg, sendData[i]);
	}

	MFRC522_write(CommandReg, command);
	if (command == PCD_TRANSCEIVE)
	MFRC522_setBitMask(BitFramingReg, 0x80);

	i = 2000;
	do {
		n = MFRC522_read(CommIrqReg);
		i--;
	} while ((i != 0) && !(n & 0x01) && !(n & waitIRq));

	MFRC522_clearBitMask(BitFramingReg, 0x80);

	if (i != 0) {
		if (!(MFRC522_read(ErrorReg) & 0x1B)) {
			status = 1;
			if (n & irqEn & 0x01) status = 0;

			if (command == PCD_TRANSCEIVE) {
				n = MFRC522_read(FIFOLevelReg);
				lastBits = MFRC522_read(ControlReg) & 0x07;
				*backLen = (lastBits) ? (n - 1) * 8 + lastBits : n * 8;
				for (i = 0; i < n; i++) {
					backData[i] = MFRC522_read(FIFODataReg);
				}
			}
		}
	}

	return status;
}