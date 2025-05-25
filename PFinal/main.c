/*
 * PFinal.c
 *
 * Created: 5/18/2025 5:50:57 PM
 * Author : Gaelp
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <time.h>
#include <stdbool.h>
#include "EEPROM.h"
#include "GPIODriver.h"
#include "LCD.h"
#include "mfrc522.h"
#include "UART.h"
#include "servo.h"

#define F_CPU 16000000UL
#define LCD_WIDTH 16  // Adjust for your LCD size (16x2 or 20x4)
#define RST_DDR  DDRB
#define RST_PORT PORTB
#define RST_PIN  PB1
#define MAX_HISTORIAL 20
#define HISTORIAL_START 10
#define HISTORIAL_INDICE 9

char PressedKey(void);
void timer1_init(void);
void MFRC522_resetPinInit(void);
uint8_t read_rfid_tag(void);
uint8_t compare_uids(uint8_t *uid1, uint8_t *uid2);
void validarPasswd(uint8_t passwd[]);
void cambiarPasswd(uint8_t newPasswd[]);
void mensajeHoraPasswd(void);
void abrirPuerta();
void cerrarPuerta();
void actualizarIndiceHistorial(uint8_t idx);
void actualizarHistorial();
void leerHistorial();

volatile uint8_t hora = 0;
volatile uint8_t minuto = 0;
volatile uint8_t segundo = 0;
volatile uint8_t contadorSegundosReal = 0;  // Contador de segundos reales
volatile uint8_t contadorPuertaAbierta = 0;
char hora_str[12];
volatile uint8_t actualizarHora = 0;
bool puertaAbierta = false;
bool leyendoHistorial = false;
uint8_t saved_uid[4];
uint8_t known_uid[4] = {0x3E, 0x1B, 0xED, 0x00};
uint8_t known_uid2[4] = {0x08, 0x13, 0x24, 0x91};



int main(void){
	// Inicializacion de puerto D para Teclado Matricial 4x4
	DDRD |=0b00001111; //D0-D3 outputs
	DDRD &=0b00001111; //D4-D7 inputs
	PORTD |=0b11110000; //D4-D7 Pull ups
	PORTD |=0b00001111; //D0-D3 outputs high
	GPIO_ConfigPin(PORT_D, 0, OUTPUT);
	PORTD &= ~(1 << PORTD0); 
	char pressedKey = 'A'; // Inicializa variable de lectura de tecla presionada
	
	lcd_init(); // Inicializacion de LCD 16x2
	timer1_init(); // Inicializacion de Timer para la hora
	MFRC522_resetPinInit();
	MFRC522_init();
	servo_init();
	//UART_init(9600, UART_8BITS, UART_PARITY_DISABLE, UART_STOPBIT_1);
	sei();
	
	uint8_t passwd[4];
	uint8_t newPasswd[4];
	uint8_t teclasPresionadas = 0;
	uint8_t cambioPasswd = 0;
	
	mensajeHoraPasswd();
	
	while(1){
		pressedKey = PressedKey();
		
		//if (read_rfid_tag()) {
			//if (compare_uids(saved_uid, known_uid)) {
				//UART_string("Authorized tag detected!\r\n");
			//} else {
				//UART_string("Unknown tag.\r\n");
			//}
			//_delay_ms(1000); // Debounce
		//}
		
		// Se actualiza el reloj si paso un minuto
		if (actualizarHora && !leyendoHistorial) {
			actualizarHora = 0;

			snprintf(hora_str, sizeof(hora_str), "%02u:%02u", hora, minuto);
			lcd_goto_xy(1, 7);
			lcd_write_word(hora_str);
		}
		
		// Cambio de pin - Usuario presiona dos veces seguidas '*'
		if (pressedKey == '*' && !puertaAbierta) {
			cambioPasswd++;
			} else if (pressedKey != 'A') {
			cambioPasswd = 0;
		}
		if (cambioPasswd == 2){
			cambioPasswd++;
			teclasPresionadas = 0;
			
			if (cambioPasswd == 3){
				lcd_clear();
				lcd_goto_xy(0,0);
				lcd_write_word("- Changing Code");
				lcd_goto_xy(1, 0);
				lcd_write_word("New Code: ");
				
				while (teclasPresionadas < 4){
					pressedKey = PressedKey();
					if ((pressedKey >= '0' && pressedKey <= '9')){
						lcd_goto_xy(1, 10+teclasPresionadas);
						lcd_write_character('*');
						newPasswd[teclasPresionadas] = pressedKey - '0';
						teclasPresionadas++;
					}
				}
				
				cambiarPasswd(newPasswd);
				mensajeHoraPasswd();
				teclasPresionadas = 0;
				pressedKey = 'A';
				cambioPasswd = 0;
			}
		}
		
		
		// Usuario ingresa código
		if (((pressedKey >= '0' && pressedKey <= '9') && teclasPresionadas < 4) && !puertaAbierta){
			lcd_goto_xy(0, 10+teclasPresionadas);
			lcd_write_character('*');
			passwd[teclasPresionadas] = pressedKey - '0';
			teclasPresionadas++;
			
			// Codigo valido
			if (teclasPresionadas >= 4){
				validarPasswd(passwd);
				teclasPresionadas = 0;
			}
		}
		
		// Usuario aproxima Tarjeta RFID
		if (read_rfid_tag() && !puertaAbierta){
			// Tag RFID valido
			if (compare_uids(saved_uid, known_uid) || compare_uids(saved_uid, known_uid2)) {
				//UART_string("Authorized tag detected!\r\n");
				abrirPuerta();
			} else{
				lcd_clear();
				lcd_goto_xy(0,0);
				lcd_write_word("Incorrect RFID!");
				lcd_goto_xy(1,0);
				lcd_write_word("Try again...");
				_delay_ms(2000);
				mensajeHoraPasswd();
			}
			_delay_ms(1000);// Debounce
		}
		
		// Usuario abre historial con '#'
		if (pressedKey == '#'){
			leerHistorial();
		}
		
		// Usuario cierra puerta con '*'
		if (puertaAbierta && pressedKey == '*')
			cerrarPuerta();
	}
}

void timer1_init() {
	// Prescaler de 1024
	// Frecuencia del timer: 16MHz / 1024 = 15625 Hz
	// Para 1s = 15625 ciclos
	TCCR1A = 0;
	TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); // CTC + prescaler 1024
	OCR1A = 15625; // 1Hz/1s
	TIMSK1 = (1 << OCIE1A); // Habilita interrupción por comparación
}


ISR(TIMER1_COMPA_vect) {
	contadorSegundosReal++;

	if (contadorSegundosReal >= 10) {
		contadorSegundosReal = 0;

		minuto++;
		segundo = 0;  // Reiniciamos segundos simulados (si se muestran)

		if (minuto >= 60) {
			minuto = 0;
			hora++;

			if (hora >= 24) {
				hora = 0;
			}
		}
	}
	
	actualizarHora = 1;
}

void MFRC522_resetPinInit() {
	RST_DDR |= (1 << RST_PIN);     // RST as output
	RST_PORT |= (1 << RST_PIN);    // RST high
}

uint8_t read_rfid_tag(void) {
	uint8_t uid[4];
	uint8_t tagType[2];

	if (MFRC522_request(0x26, tagType)) {
		if (MFRC522_anticoll(uid)) {
			memcpy(saved_uid, uid, 4); // Save UID globally
			return 1; // Success
		}
	}
	return 0; // Failure
}

uint8_t compare_uids(uint8_t *uid1, uint8_t *uid2) {
	for (uint8_t i = 0; i < 4; i++) {
		if (uid1[i] != uid2[i])
		return 0; // Not equal
	}
	return 1; // Equal
}

char PressedKey(void) {
	static uint8_t keyReleased = 1; // Bandera que indica si se soltó la tecla anterior
	char key = 0;

	// Escanea cada fila del teclado
	PORTD = 0b11110111; _delay_ms(1);
	if (!(PIND & (1 << PIND7))) key = '1';
	else if (!(PIND & (1 << PIND6))) key = '4';
	else if (!(PIND & (1 << PIND5))) key = '7';
	else if (!(PIND & (1 << PIND4))) key = '*';

	if (!key) {
		PORTD = 0b11111011; _delay_ms(1);
		if (!(PIND & (1 << PIND7))) key = '2';
		else if (!(PIND & (1 << PIND6))) key = '5';
		else if (!(PIND & (1 << PIND5))) key = '8';
		else if (!(PIND & (1 << PIND4))) key = '0';
	}

	if (!key) {
		PORTD = 0b11111101; _delay_ms(1);
		if (!(PIND & (1 << PIND7))) key = '3';
		else if (!(PIND & (1 << PIND6))) key = '6';
		else if (!(PIND & (1 << PIND5))) key = '9';
		else if (!(PIND & (1 << PIND4))) key = '#';
	}

	if (!key) {
		PORTD = 0b11111110; _delay_ms(1);
		if (!(PIND & (1 << PIND7))) key = 'A';
		else if (!(PIND & (1 << PIND6))) key = 'B';
		else if (!(PIND & (1 << PIND5))) key = 'C';
		else if (!(PIND & (1 << PIND4))) key = 'D';
	}

	// Lógica de detección de nueva pulsación
	if (key && keyReleased) {
		keyReleased = 0;
		return key;
	}

	// Si ya no se detecta ninguna tecla, marcamos como liberada
	if (!key) {
		keyReleased = 1;
	}

	return 'A';
}

void validarPasswd(uint8_t passwd[]){
	uint8_t contadorCorrecto = 0;
	
	for (uint8_t i=0; i<4; i++){
		if (EEPROM_read(i) == passwd[i])
			contadorCorrecto++;
	}
	
	if (contadorCorrecto == 4){
		lcd_clear();
		lcd_goto_xy(0,0);
		lcd_write_word("Succesful Code!");
		lcd_goto_xy(1,0);
		lcd_write_word("Opening Door...");
		abrirPuerta();
	} else{
		lcd_clear();
		lcd_goto_xy(0,0);
		lcd_write_word("Incorrect Code!");
		lcd_goto_xy(1,0);
		lcd_write_word("Try again...");
		_delay_ms(2000);
		mensajeHoraPasswd();
	}
}

void cambiarPasswd(uint8_t newPasswd[]){
	uint8_t contadorCorrecto = 0;
	
	for (uint8_t i=0; i<4; i++){
		EEPROM_update(i, newPasswd[i]);
	}
}

void mensajeHoraPasswd(void){
	lcd_clear();
	lcd_goto_xy(1, 0);
	lcd_write_word("Hour: ");
	lcd_goto_xy(0,0);
	lcd_write_word("Password: ");
}

void abrirPuerta(void){
	puertaAbierta = true;
	
	//Logica abrir puerta
	
	lcd_clear();
	lcd_goto_xy(0,0);
	lcd_write_word("Status Door:");
	lcd_goto_xy(1,0);
	lcd_write_word("OPEN");
	GPIO_EscribirPin(PORTD, 0, HIGH);
	servoAngulo(37);
	
	actualizarHistorial();
}

void cerrarPuerta(void){
	puertaAbierta = false;
	
	//Logica cerrar puerta
	servoAngulo(160);
	
	mensajeHoraPasswd();
	
}


void actualizarIndiceHistorial(uint8_t idx){
	if (idx < MAX_HISTORIAL){
		idx++;
	} else{
		idx = 0;
	}
	EEPROM_update(HISTORIAL_INDICE, idx);
}

void actualizarHistorial(){
	uint8_t indice = EEPROM_read(HISTORIAL_INDICE);
	uint16_t direccion = HISTORIAL_START + (indice * 2);
	
	EEPROM_update(direccion, hora);
	EEPROM_update(direccion+1, minuto);
	
	actualizarIndiceHistorial(indice);	
}

void leerHistorial(){
	leyendoHistorial = true;
	uint8_t indiceLectura = 0;
	uint8_t c = 1;
	uint8_t hr, min;
	char key = 'A';
	char horaHistorial[12];
	
	lcd_clear();
	lcd_goto_xy(0,0);
	lcd_write_word("Entry History");
	lcd_goto_xy(1,0);
	lcd_write_word("Press '#': Next");
	
	while (1){
		key = PressedKey();
		
		if (key == '#'){
			hr = EEPROM_read(HISTORIAL_START + (indiceLectura * 2));
			min = EEPROM_read((HISTORIAL_START + (indiceLectura * 2))+1);
			
			if (hr <= 24 && min <= 60){
				lcd_goto_xy(1,0);
				snprintf(horaHistorial, sizeof(horaHistorial), "%u. %02u:%02u      ", c, hr, min);
				lcd_write_word(horaHistorial);
				indiceLectura++;
				c++;
			} else{
				lcd_goto_xy(1,0);
				lcd_write_word("END             ");
				_delay_ms(2000);
				break;
			}
			key = 'A';
		}
	}
	mensajeHoraPasswd();
	leyendoHistorial = false;
}

void UART_print_dec(uint8_t num) {
	char buf[4]; // máximo 3 dígitos + '\0'
	snprintf(buf, sizeof(buf), "%u", num);
	UART_string(buf);
}