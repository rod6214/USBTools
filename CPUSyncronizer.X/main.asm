
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
 
 pdata equ 0x0C
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
 
PSECT resetVec, class=CODE, delta=2
resetVec:
    PAGESEL main
    goto main
    nop
    nop
    nop
    
    org 0x04
perifVec:
    bcf INTCON, 7   ; Disable global interrupts
    bcf INTCON, 1   ; Clear external interrupt flag
    rlf ACUMC, f
    btfsc ACUMC, 2 ; Skip if bit 2 is clear
    bsf PORTB, 2
    btfsc ACUMC, 3 ; Skip if bit 3 is clear
    bsf PORTB, 3
    movf PORTB, w
    andlw 12
    sublw 12
    btfss STATUS, 2 ; Check if bit2 and bit3 are loaded
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
    bsf INTCON, 7   ; Enable global interrupts
    retfie
complete_exit:
    bcf CSTATUS, 0
    clrf ACUMC
    incf ACUMC, f
    bcf INTCON, 7   ; Disable global interrupts
    retfie

PSECT code, delta=2
 
main:
    bcf STATUS, 6
    bcf STATUS, 5   ; Used bank 0
    clrf PORTA	    ; Reset PORTA
    clrf PORTB	    ; Reset PORTB
    clrf PORTC
    bsf STATUS, 5   ; Used bank 1
    movlw 255	    ; Set all pin as input
    movwf PORTA	    ; Save working register into PORTA
    movlw 3
    movwf PORTB	    ; Set up PORTB for I/O
    clrf PORTC
    
    bcf OPTION_REG, 6 ; Configure INT/PB0 as falling edge
    bcf STATUS, 5   ; Used bank 0
    bcf INTCON, 1   ; Clear external interrupt flag
    bsf INTCON, 4   ; Enable external interrupt on port b
    bsf INTCON, 7   ; Enable global interrupts
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
loop:
    goto loop
goto $
    end resetVec
