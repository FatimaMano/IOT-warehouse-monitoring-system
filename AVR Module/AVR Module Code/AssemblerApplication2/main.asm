.include "m328pdef.inc"
.include "delay_Macro.inc"
.include "uartMacros.inc"
.equ temperaturepin = 0
.equ Flamepin = 2
.equ buzpin = 3
.equ ledpin = 4  ; Assuming pin 12
.def serial_input = r25
.def Buzzerstatus = r15

.equ thresholdThermistor = 250
.equ thresholdFlame = 150

.org 0x0000
  rjmp RESET

RESET:
  ldi r21, 1 << temperaturepin  ; Define temperature sensor pin as ADC0 (PC0)
  ldi r22, 1 << Flamepin  ; Define flame sensor pin as ADC2  (PC2)
  ldi r23, 1 << buzpin  ; Define buzzer pin as PB3
  ldi r24, 1 << ledpin  ; Define light pin as PB4 (assuming pin 12)

  ; I/O Pins Configuration
  sbi DDRB, 3 ; Set PB3 pin for Output to Buzzer
  cbi PORTB, 3 ; BUZZER OFF
  sbi DDRB, 4 ; Set PB4 pin for Output to Light
  cbi PORTB, 4 ; LIGHT OFF

  ; ADC Configuration
  ldi r16,0b11000111   ; [ADEN ADSC ADATE ADIF ADIE ADIE ADPS2 ADPS1 ADPS0]
  sts ADCSRA, r16

  ldi r16, 0b01100010 ; [REFS1 REFS0 ADLAR – MUX3 MUX2 MUX1 MUX0]
  sts ADMUX, r16 ; Select ADC2 (PC2) pin

  sbi PORTC, PC0 ; Enable Pull-up Resistor for temperature sensor
  Serial_begin;

loop:
  ldi r16,0
  Serial_readStr
  mov serial_input,r16
  cpi serial_input,'O'
  breq BUZZER_ON
  cpi serial_input,'F'
  breq BUZZER_OFF
  ; Read temperature sensor
  lds r16,ADCSRA
  ori r16,(1<<ADSC)
  sts ADCSRA,r16; Start ADC conversion
  wait_temp:
    lds r16, ADCSRA ; wait for conversion to complete
    sbrs r16, ADSC ; skip if ADSC is clear
    rjmp wait_temp
  lds r16, ADCL ; Must Read ADCL before ADCH
  lds r17, ADCH
  delay 100 ; delay 100ms

  cpi r17, thresholdThermistor ; compare temperature sensor reading with the desired threshold
  brsh LED_ON		;jump if same or lower
  cbi PORTB,PB4		;LED OFF
  rjmp loop

FLAME_CHECK:
  ; Read flame sensor
  lds r16, PC1 ; Select Flamepin as ADC input
  sts ADMUX, r16
  lds r16,ADCSRA
  ori r16,(1<<ADSC)
  sts ADCSRA,r16; Start ADC conversion
  wait_flame:
    lds r16, ADCSRA ; wait for conversion to complete
    sbrs r16, ADSC ; skip if ADSC is clear
    rjmp wait_flame
  lds r16, ADCL ; Must Read ADCL before ADCH
  lds r17, ADCH

  cpi r17, thresholdFlame ; compare flame sensor reading with the desired threshold
  brlo BUZZER_ON ; jump if less than or equal to (r17 <= thresholdFlame)
  cbi PORTB, PB3 ; BUZZER OFF
  rjmp loop
LED_ON:
	SBI PORTB,PB4		;LED ON
BUZZER_ON:
  sbi PORTB, PB3 ; BUZZER ON
  rjmp loop
BUZZER_OFF:
  cbi PORTB, PB3 ; BUZZER ON
  rjmp loop
