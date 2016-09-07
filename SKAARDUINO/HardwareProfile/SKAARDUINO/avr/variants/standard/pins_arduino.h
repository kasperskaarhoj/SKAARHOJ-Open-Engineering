#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <avr/pgmspace.h>

// ATMEL ATMEGA1284P
//
//                       +---\/---+
//           (D 0) PB0  1|        |40  PA0 (AI 0 / D24)
//           (D 1) PB1  2|        |39  PA1 (AI 1 / D25)
//      INT2 (D 2) PB2  3|        |38  PA2 (AI 2 / D26)
//       PWM (D 3) PB3  4|        |37  PA3 (AI 3 / D27)
//    PWM/SS (D 4) PB4  5|        |36  PA4 (AI 4 / D28)
//      MOSI (D 5) PB5  6|        |35  PA5 (AI 5 / D29)
//  PWM/MISO (D 6) PB6  7|        |34  PA6 (AI 6 / D30)
//   PWM/SCK (D 7) PB7  8|        |33  PA7 (AI 7 / D31)
//                 RST  9|        |32  AREF
//                 VCC 10|        |31  GND 
//                 GND 11|        |30  AVCC
//               XTAL2 12|        |29  PC7 (D 23)
//               XTAL1 13|        |28  PC6 (D 22)
//      RX0 (D 8)  PD0 14|        |27  PC5 (D 21) TDI
//      TX0 (D 9)  PD1 15|        |26  PC4 (D 20) TDO
// RX1/INT0 (D 10) PD2 16|        |25  PC3 (D 19) TMS
// TX1/INT1 (D 11) PD3 17|        |24  PC2 (D 18) TCK
//      PWM (D 12) PD4 18|        |23  PC1 (D 17) SDA
//      PWM (D 13) PD5 19|        |22  PC0 (D 16) SCL
//      PWM (D 14) PD6 20|        |21  PD7 (D 15) PWM
//                       +--------+
//

/*
   PCINT15-8: D7-0  : bit 1
   PCINT31-24: D15-8  : bit 3
   PCINT23-16: D23-16 : bit 2
   PCINT7-0: D31-24   : bit 0
   */

#define SKAARDUINO_VARIANT "V1"

#define NUM_DIGITAL_PINS            32
#define NUM_ANALOG_INPUTS           8
#define analogInputToDigitalPin(p)  ( (p) < NUM_ANALOG_INPUTS ? (p) + 24 : -1 )
#define digitalPinToAnalogPin(p)    ( (p) >= 24 && (p) <= 31 ? (p) - 24 : -1 )
//#define analogPinToChannel(p)       ( (p) < NUM_ANALOG_INPUTS ? (p) : (p) >= 24 ? (p) - 24 : -1 )    //required macro for mighty-1284p core
#define digitalPinHasPWM(p)         ( (p) == 3 || (p) == 4 || (p) == 6 || (p) == 7 || (p) == 12 || (p) == 13 || (p) == 14 || (p) == 15 )

static const uint8_t SS   = 4;
static const uint8_t MOSI = 5;
static const uint8_t MISO = 6;
static const uint8_t SCK  = 7;

static const uint8_t SDA = 17;
static const uint8_t SCL = 16;
static const uint8_t LED_BUILTIN = 7;

static const uint8_t A0 = 24;
static const uint8_t A1 = 25;
static const uint8_t A2 = 26;
static const uint8_t A3 = 27;
static const uint8_t A4 = 28;
static const uint8_t A5 = 29;
static const uint8_t A6 = 30;
static const uint8_t A7 = 31;

#define digitalPinToPCICR(p)    ( (p) >= 0 && (p) < NUM_DIGITAL_PINS ? &PCICR : (uint8_t *)0 )
#define digitalPinToPCICRbit(p) ( (p) <= 7 ? 1 : (p) <= 15 ? 3 : (p) <= 23 ? 2 : 0 )
#define digitalPinToPCMSK(p)    ( (p) <= 7 ? &PCMSK1 : (p) <= 15 ? &PCMSK3 : (p) <= 23 ? &PCMSK2 : &PCMSK0 )
#define digitalPinToPCMSKbit(p) ( (p) % 8 )
// return associated INTx number for associated/valid pins,
// otherwise NOT_AN_INTERRUPT
#define digitalPinToInterrupt(p) \
	(\
	(p) == 10 ? 0 : \
	(p) == 11 ? 1 : \
	(p) == 2 ? 2 : \
	NOT_AN_INTERRUPT)


#define PA 1
#define PB 2
#define PC 3
#define PD 4

// specify port for each pin D0-D31
#define PORT_D0 PB
#define PORT_D1 PB
#define PORT_D2 PB
#define PORT_D3 PB
#define PORT_D4 PB
#define PORT_D5 PB
#define PORT_D6 PB
#define PORT_D7 PB
#define PORT_D8 PD
#define PORT_D9 PD
#define PORT_D10 PD
#define PORT_D11 PD
#define PORT_D12 PD
#define PORT_D13 PD
#define PORT_D14 PD
#define PORT_D15 PD
#define PORT_D16 PC
#define PORT_D17 PC
#define PORT_D18 PC
#define PORT_D19 PC
#define PORT_D20 PC
#define PORT_D21 PC
#define PORT_D22 PC
#define PORT_D23 PC
#define PORT_D24 PA
#define PORT_D25 PA
#define PORT_D26 PA
#define PORT_D27 PA
#define PORT_D28 PA
#define PORT_D29 PA
#define PORT_D30 PA
#define PORT_D31 PA

// specify port bit for each pin D0-D31
#define BIT_D0 0
#define BIT_D1 1
#define BIT_D2 2
#define BIT_D3 3
#define BIT_D4 4
#define BIT_D5 5
#define BIT_D6 6
#define BIT_D7 7
#define BIT_D8 0
#define BIT_D9 1
#define BIT_D10 2
#define BIT_D11 3
#define BIT_D12 4
#define BIT_D13 5
#define BIT_D14 6
#define BIT_D15 7
#define BIT_D16 0
#define BIT_D17 1
#define BIT_D18 2
#define BIT_D19 3
#define BIT_D20 4
#define BIT_D21 5
#define BIT_D22 6
#define BIT_D23 7
#define BIT_D24 0
#define BIT_D25 1
#define BIT_D26 2
#define BIT_D27 3
#define BIT_D28 4
#define BIT_D29 5
#define BIT_D30 6
#define BIT_D31 7

// macro equivalents of PROGMEM arrays port_to_mode_PGM[] etc. below
#define PORT_TO_MODE(x) (x == 1 ? &DDRA : (x == 2 ? &DDRB : (x == 3 ? &DDRC : (x == 4 ? &DDRD : NOT_A_PORT)))) 
#define PORT_TO_OUTPUT(x) (x == 1 ? &PORTA : (x == 2 ? &PORTB : (x == 3 ? &PORTC : (x == 4 ? &PORTD : NOT_A_PORT))))
#define PORT_TO_INPUT(x) (x == 1 ? &PINA : (x == 2 ? &PINB : (x == 3 ? &PINC : (x == 4 ? &PIND : NOT_A_PORT)))) 
 
#ifdef ARDUINO_MAIN
// these arrays map port names (e.g. port B) to the
// appropriate addresses for various functions (e.g. reading
// and writing)
const uint16_t PROGMEM port_to_mode_PGM[] =
{
  NOT_A_PORT,
  (uint16_t) &DDRA,
  (uint16_t) &DDRB,
  (uint16_t) &DDRC,
  (uint16_t) &DDRD,
};

const uint16_t PROGMEM port_to_output_PGM[] =
{
  NOT_A_PORT,
  (uint16_t) &PORTA,
  (uint16_t) &PORTB,
  (uint16_t) &PORTC,
  (uint16_t) &PORTD,
};

const uint16_t PROGMEM port_to_input_PGM[] =
{
  NOT_A_PORT,
  (uint16_t) &PINA,
  (uint16_t) &PINB,
  (uint16_t) &PINC,
  (uint16_t) &PIND,
};

const uint8_t PROGMEM digital_pin_to_port_PGM[NUM_DIGITAL_PINS] =
{
  PORT_D0,
  PORT_D1,
  PORT_D2,
  PORT_D3,
  PORT_D4,
  PORT_D5,
  PORT_D6,
  PORT_D7,
  PORT_D8,
  PORT_D9,
  PORT_D10,
  PORT_D11,
  PORT_D12,
  PORT_D13,
  PORT_D14,
  PORT_D15,
  PORT_D16,
  PORT_D17,
  PORT_D18,
  PORT_D19,
  PORT_D20,
  PORT_D21,
  PORT_D22,
  PORT_D23,
  PORT_D24,
  PORT_D25,
  PORT_D26,
  PORT_D27,
  PORT_D28,
  PORT_D29,
  PORT_D30,
  PORT_D31
};

const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[NUM_DIGITAL_PINS] =
{
  _BV(BIT_D0),
  _BV(BIT_D1),
  _BV(BIT_D2),
  _BV(BIT_D3),
  _BV(BIT_D4),
  _BV(BIT_D5),
  _BV(BIT_D6),
  _BV(BIT_D7),
  _BV(BIT_D8),
  _BV(BIT_D9),
  _BV(BIT_D10),
  _BV(BIT_D11),
  _BV(BIT_D12),
  _BV(BIT_D13),
  _BV(BIT_D14),
  _BV(BIT_D15),
  _BV(BIT_D16),
  _BV(BIT_D17),
  _BV(BIT_D18),
  _BV(BIT_D19),
  _BV(BIT_D20),
  _BV(BIT_D21),
  _BV(BIT_D22),
  _BV(BIT_D23),
  _BV(BIT_D24),
  _BV(BIT_D25),
  _BV(BIT_D26),
  _BV(BIT_D27),
  _BV(BIT_D28),
  _BV(BIT_D29),
  _BV(BIT_D30),
  _BV(BIT_D31)
};

const uint8_t PROGMEM digital_pin_to_timer_PGM[] =
{
  NOT_ON_TIMER, /* 0  - PB0 */
  NOT_ON_TIMER, /* 1  - PB1 */
  NOT_ON_TIMER, /* 2  - PB2 */
  TIMER0A,     	/* 3  - PB3 */
  TIMER0B, 	/* 4  - PB4 */
  NOT_ON_TIMER, /* 5  - PB5 */
  TIMER3A, 	/* 6  - PB6 */
  TIMER3B,	/* 7  - PB7 */
  NOT_ON_TIMER, /* 8  - PD0 */
  NOT_ON_TIMER, /* 9  - PD1 */
  NOT_ON_TIMER, /* 10 - PD2 */
  NOT_ON_TIMER, /* 11 - PD3 */
  TIMER1B,     	/* 12 - PD4 */
  TIMER1A,     	/* 13 - PD5 */
  TIMER2B,     	/* 14 - PD6 */
  TIMER2A,     	/* 15 - PD7 */
  NOT_ON_TIMER, /* 16 - PC0 */
  NOT_ON_TIMER, /* 17 - PC1 */
  NOT_ON_TIMER, /* 18 - PC2 */
  NOT_ON_TIMER, /* 19 - PC3 */
  NOT_ON_TIMER, /* 20 - PC4 */
  NOT_ON_TIMER, /* 21 - PC5 */
  NOT_ON_TIMER, /* 22 - PC6 */
  NOT_ON_TIMER, /* 23 - PC7 */
  NOT_ON_TIMER, /* 24 - PA0 */
  NOT_ON_TIMER, /* 25 - PA1 */
  NOT_ON_TIMER, /* 26 - PA2 */
  NOT_ON_TIMER, /* 27 - PA3 */
  NOT_ON_TIMER, /* 28 - PA4 */
  NOT_ON_TIMER, /* 29 - PA5 */
  NOT_ON_TIMER, /* 30 - PA6 */
  NOT_ON_TIMER  /* 31 - PA7 */
};

#endif // ARDUINO_MAIN

#endif // Pins_Arduino_h
// vim:ai:cin:sts=2 sw=2 ft=cpp
