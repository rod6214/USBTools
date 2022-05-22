
; PIC16F877A Configuration Bit Settings

; Assembly source line config statements

; CONFIG
  CONFIG  FOSC = XT             ; Oscillator Selection bits (XT oscillator)
  CONFIG  WDTE = OFF            ; Watchdog Timer Enable bit (WDT disabled)
  CONFIG  PWRTE = OFF           ; Power-up Timer Enable bit (PWRT disabled)
  CONFIG  BOREN = ON            ; Brown-out Reset Enable bit (BOR enabled)
  CONFIG  LVP = OFF             ; Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
  CONFIG  CPD = OFF             ; Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
  CONFIG  WRT = OFF             ; Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
  CONFIG  CP = OFF              ; Flash Program Memory Code Protection bit (Code protection off)

// config statements should precede project file includes.
#include <xc.inc>

RESET_STEP    equ 0
MASK_INT_STEP equ 1
SYNC_F	       equ 1
READY	       equ 2
RESET_F       equ 3
NMI	       equ 4
CYCLE	       equ 0xA0
RESET_CYCLE  equ 4
READY_CYCLE  equ 2
NMI_CYCLE    equ 7
ACUM	       equ 0x21
CSTATUS	       equ 0x22
ACUMB          equ 0x23
ACUMC          equ 0x24
STACK_LENGTH   equ 2
RESET_READY    equ 1
Gie	       equ 7
Inte	       equ 7
Rp0	       equ 5
Rp1	       equ 6
Intedg	       equ 6	
Z	       equ 2	       
	       
PSECT resetVec, class=CODE, delta=2
resetVec:
    PAGESEL main
    goto main
    nop
    nop
    nop
    
    org 0x04
perifVec:
    bcf INTCON, Gie   ; Disable global interrupts
    btfsc CSTATUS, RESET_READY
    goto app_interrupts
startup_int:
    bcf INTCON, 1   ; Clear external interrupt flag
    rlf ACUMC, f
    btfsc ACUMC, 2 ; Skip if bit 2 is clear
    bsf PORTB, 2
    btfsc ACUMC, 3 ; Skip if bit 3 is clear
    bsf PORTB, 3
    movf PORTB, w
    andlw 12
    sublw 12
    btfss STATUS, Z ; Check if bit2 and bit3 are loaded
    goto exit_int
    btfss CSTATUS, 0 ; Check if sync is ready
    goto wait_sync ; If not then go to sync subroutine
    movf ACUMC, w
    btfss ACUMC, 7 ; Skip if bit 3 is clear
    goto exit_int
    bsf PORTB, 4
    goto complete_exit
wait_sync:
    btfss PORTB, 1
    goto wait_sync
    bsf CSTATUS, 0
exit_int:
    bsf INTCON, Gie   ; Enable global interrupts
    retfie
complete_exit:
;    bcf CSTATUS, 0
    bsf CSTATUS, RESET_READY ; Finish reset/startup sequence
    clrf ACUMC
;    incf ACUMC, f
    bcf INTCON, Gie   ; Disable global interrupts
    retfie
app_interrupts:
    retfie

PSECT code, delta=2
main:
    bcf STATUS, Rp1
    bcf STATUS, Rp0 ; Used bank 0
    clrf PORTA	    ; Reset PORTA
    clrf PORTB	    ; Reset PORTB
    clrf PORTC
    bsf STATUS, Rp0 ; Used bank 1
    movlw 255	    ; Set all pin as input
    movwf PORTA	    ; Save working register into PORTA
    movlw 3
    movwf PORTB	    ; Set up PORTB for I/O
    clrf PORTC
    
    bcf OPTION_REG, Intedg ; Configure INT/PB0 as falling edge
    bcf STATUS, Rp0   ; Used bank 0
    bcf INTCON, 1   ; Clear external interrupt flag
    bsf INTCON, Inte   ; Enable external interrupt on port b
    bsf INTCON, Gie   ; Enable global interrupts
    movlw CYCLE
    movwf FSR	    ; Point to data memory
    movlw STACK_LENGTH
    movwf ACUM
clear_mem:
    clrf INDF
    incf FSR, f
    decfsz ACUM, f
    goto clear_mem
    movlw CYCLE
    movwf FSR
    movlw 1
    movwf ACUMC
reset_loop:
    btfss CSTATUS, RESET_READY
    goto reset_loop
app_loop:
    goto app_loop
goto $
    end resetVec
