#ifndef SERVO_H_
#define SERVO_H_

#include <avr/io.h>

void servo_init(void);
void servoAngulo(uint8_t angulo);

#endif /* SERVO_H_ */
