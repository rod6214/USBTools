
; PIC16F876A Configuration Bit Settings

; Assembly source line config statements

; CONFIG
  CONFIG  FOSC = HS             ; Oscillator Selection bits (HS oscillator)
  CONFIG  WDTE = OFF            ; Watchdog Timer Enable bit (WDT disabled)
  CONFIG  PWRTE = OFF           ; Power-up Timer Enable bit (PWRT disabled)
  CONFIG  BOREN = ON            ; Brown-out Reset Enable bit (BOR enabled)
  CONFIG  LVP = OFF             ; Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
  CONFIG  CPD = OFF             ; Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
  CONFIG  WRT = OFF             ; Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
  CONFIG  CP = OFF              ; Flash Program Memory Code Protection bit (Code protection off)

// config statements should precede project file includes.
#include <xc.inc>

RESET_STEP	equ 0
MASK_INT_STEP	equ 1
SYNC_F		equ 1
READY		equ 2
RESET_F		equ 3
NMI		equ 4
CYCLE		equ 0xA0
RESET_CYCLE	equ 4
READY_CYCLE	equ 2
NMI_CYCLE	equ 7
ACUM		equ 0x21
CSTATUS		equ 0x22
ACUMB		equ 0x23
ACUMC		equ 0x24
STACK_LENGTH	equ 2
RESET_READY	equ 1
SYNC_READY	equ 0
RQ_ONE_CYCLE	equ 0
Gie		equ 7
Inte		equ 4
Rp0		equ 5
Rp1		equ 6
Intedg		equ 6	
Z		equ 2	
Rbif		equ 0
Rbie		equ 3
Intf		equ 1	       
	       
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
    incf PORTC, f
    bcf INTCON, Intf   ; Clear external interrupt flag
    bsf INTCON, Gie   ; Disable global interrupts
    retfie
    
    bcf INTCON, Gie   ; Disable global interrupts
    btfsc CSTATUS, RESET_READY
    goto app_interrupts
startup_int:
    bcf INTCON, Intf   ; Clear external interrupt flag
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
    btfss CSTATUS, SYNC_READY ; Check if sync is ready
    goto wait_sync ; If not then go to sync subroutine
    movf ACUMC, w
    btfss ACUMC, 7 ; Skip if bit 3 is clear
    goto exit_int
    bsf PORTB, 4
    goto finish_startup
wait_sync:
    btfss PORTB, 1
    goto wait_sync
    bsf CSTATUS, SYNC_READY
exit_int:
    bsf INTCON, Gie   ; Enable global interrupts
    retfie
finish_startup:
;    bcf CSTATUS, 0
    bsf CSTATUS, RESET_READY ; Finish reset/startup sequence
    clrf ACUMC
;    incf ACUMC, f
    bcf INTCON, Inte   ; Disable external interrupt on port b
    bcf INTCON, Gie   ; Disable global interrupts
    retfie
app_interrupts:
    btfsc INTCON, Rbif
    goto rb_int ; Rb[RB4:RB7] port changes int
    btfsc INTCON, Intf
    goto ext_int ; External interrupt INT/RB0
    retfie
ext_int:
    bcf INTCON, Intf   ; Clear external interrupt flag
    bcf INTCON, Inte   ; Disable external interrupt on port b
    btfsc CSTATUS, RQ_ONE_CYCLE ; Check if there is a one cycle request
    call one_cycle
    bsf INTCON, Gie   ; Enable global interrupts
    retfie
one_cycle:
    PAGESEL $
    movf ACUM, w
    addwf PCL, f
    ; btfss ACUM, 0 ; Check which state will be executed
    goto set_ready_pin ; Set pin ready
    goto wait_ready_pin
    goto wait_ready_pin
    goto wait_ready_pin
    goto wait_ready_pin
    goto wait_ready_pin
    goto finish_ext_int ; Clear pin ready and finish
set_ready_pin:
    bsf PORTB, READY ; Set pin ready to execute one microprocessor instruction
wait_ready_pin:
    incf ACUM, f ; Set next step to finish
    bsf INTCON, Inte   ; Enable external interrupt on port b
    bcf INTCON, Intf   ; Clear external interrupt flag
    return
finish_ext_int:
    bcf PORTB, READY ; Clear pin ready to stop microprocessor
    clrf ACUM ; Reset register of states
    bcf CSTATUS, RQ_ONE_CYCLE ; Finish one cycle execution
    bsf INTCON, Rbie  ; Enable port b[RB4:RB7] changes
    bcf INTCON, Rbif ; Clear rb interrupt flag
    bcf INTCON, Intf   ; Clear external interrupt flag
    return
rb_int:
; bcf INTCON, Inte
    bcf INTCON, Rbie  ; Disable port b[RB4:RB7] changes
    movf PORTB, f
    nop
    nop
    bsf CSTATUS, RQ_ONE_CYCLE ; Request one cycle execution
    bsf INTCON, Inte   ; Enable external interrupt on port b
    ; We need to clear the flag after refresh port B due to avoid executing twice
    bcf INTCON, Rbif ; Clear rb interrupt flag
    bsf INTCON, Gie   ; Enable global interrupts
    retfie

PSECT code, delta=2
main:
;    movlw 0xF0
;    movwf FSR
;    movlw 15
;    movwf INDF
    
    bcf STATUS, Rp1
    bcf STATUS, Rp0 ; Used bank 0
    clrf PORTA	    ; Reset PORTA
    clrf PORTB	    ; Reset PORTB
    clrf PORTC
    bsf STATUS, Rp0 ; Used bank 1
    movlw 255	    ; Set all pin as input
    movwf PORTA	    ; Save working register into PORTA
    movlw 35
    movwf PORTB	    ; Set up PORTB for I/O
    clrf PORTC
;    clrf OPTION_REG
    bcf OPTION_REG, Intedg ; Configure INT/PB0 as falling edge
    bcf STATUS, Rp0   ; Used bank 0
    bcf INTCON, Intf   ; Clear external interrupt flag
    bsf INTCON, Inte   ; Enable external interrupt on port b
    bsf INTCON, Gie   ; Enable global interrupts
    movlw CYCLE
    movwf FSR	    ; Point to data memory
;    movlw STACK_LENGTH
;    movwf ACUM
clear_mem:
;    clrf CSTATUS
;    clrf INDF
;    incf FSR, f
;    decfsz ACUM, f
;    goto clear_mem
;    movlw CYCLE
;    movwf FSR
    movlw 1
    movwf ACUMC
reset_loop:
    btfss CSTATUS, RESET_READY
    goto reset_loop
app_config:
    clrf ACUM ; Reset register of states
    bcf PORTB, READY ; Prepare system for one cycle execution
    bcf INTCON, Rbif ; Clear rb interrupt flag
    bsf INTCON, Rbie  ; Enable port b[RB4:RB7] changes
    bsf INTCON, Gie   ; Enable global interrupts
app_loop:
    goto app_loop
goto $
    end resetVec
