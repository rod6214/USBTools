
; PIC18F2550 Configuration Bit Settings

; Assembly source line config statements

; CONFIG1L
  CONFIG  PLLDIV = 1            ; PLL Prescaler Selection bits (No prescale (4 MHz oscillator input drives PLL directly))
  CONFIG  CPUDIV = OSC1_PLL2    ; System Clock Postscaler Selection bits ([Primary Oscillator Src: /1][96 MHz PLL Src: /2])
  CONFIG  USBDIV = 1            ; USB Clock Selection bit (used in Full-Speed USB mode only; UCFG:FSEN = 1) (USB clock source comes directly from the primary oscillator block with no postscale)

; CONFIG1H
  CONFIG  FOSC = HSPLL_HS       ; Oscillator Selection bits (HS oscillator, PLL enabled (HSPLL))
  CONFIG  FCMEN = OFF           ; Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
  CONFIG  IESO = OFF            ; Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

; CONFIG2L
  CONFIG  PWRT = OFF            ; Power-up Timer Enable bit (PWRT disabled)
  CONFIG  BOR = ON              ; Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
  CONFIG  BORV = 3              ; Brown-out Reset Voltage bits (Minimum setting 2.05V)
  CONFIG  VREGEN = OFF          ; USB Voltage Regulator Enable bit (USB voltage regulator disabled)

; CONFIG2H
  CONFIG  WDT = OFF             ; Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
  CONFIG  WDTPS = 32768         ; Watchdog Timer Postscale Select bits (1:32768)

; CONFIG3H
  CONFIG  CCP2MX = ON           ; CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
  CONFIG  PBADEN = OFF          ; PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
  CONFIG  LPT1OSC = OFF         ; Low-Power Timer 1 Oscillator Enable bit (Timer1 configured for higher power operation)
  CONFIG  MCLRE = ON            ; MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

; CONFIG4L
  CONFIG  STVREN = ON           ; Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
  CONFIG  LVP = OFF             ; Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
  CONFIG  XINST = OFF           ; Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

; CONFIG5L
  CONFIG  CP0 = OFF             ; Code Protection bit (Block 0 (000800-001FFFh) is not code-protected)
  CONFIG  CP1 = OFF             ; Code Protection bit (Block 1 (002000-003FFFh) is not code-protected)
  CONFIG  CP2 = OFF             ; Code Protection bit (Block 2 (004000-005FFFh) is not code-protected)
  CONFIG  CP3 = OFF             ; Code Protection bit (Block 3 (006000-007FFFh) is not code-protected)

; CONFIG5H
  CONFIG  CPB = OFF             ; Boot Block Code Protection bit (Boot block (000000-0007FFh) is not code-protected)
  CONFIG  CPD = OFF             ; Data EEPROM Code Protection bit (Data EEPROM is not code-protected)

; CONFIG6L
  CONFIG  WRT0 = OFF            ; Write Protection bit (Block 0 (000800-001FFFh) is not write-protected)
  CONFIG  WRT1 = OFF            ; Write Protection bit (Block 1 (002000-003FFFh) is not write-protected)
  CONFIG  WRT2 = OFF            ; Write Protection bit (Block 2 (004000-005FFFh) is not write-protected)
  CONFIG  WRT3 = OFF            ; Write Protection bit (Block 3 (006000-007FFFh) is not write-protected)

; CONFIG6H
  CONFIG  WRTC = OFF            ; Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) are not write-protected)
  CONFIG  WRTB = OFF            ; Boot Block Write Protection bit (Boot block (000000-0007FFh) is not write-protected)
  CONFIG  WRTD = OFF            ; Data EEPROM Write Protection bit (Data EEPROM is not write-protected)

; CONFIG7L
  CONFIG  EBTR0 = OFF           ; Table Read Protection bit (Block 0 (000800-001FFFh) is not protected from table reads executed in other blocks)
  CONFIG  EBTR1 = OFF           ; Table Read Protection bit (Block 1 (002000-003FFFh) is not protected from table reads executed in other blocks)
  CONFIG  EBTR2 = OFF           ; Table Read Protection bit (Block 2 (004000-005FFFh) is not protected from table reads executed in other blocks)
  CONFIG  EBTR3 = OFF           ; Table Read Protection bit (Block 3 (006000-007FFFh) is not protected from table reads executed in other blocks)

; CONFIG7H
  CONFIG  EBTRB = OFF           ; Boot Block Table Read Protection bit (Boot block (000000-0007FFh) is not protected from table reads executed in other blocks)

// config statements should precede project file includes.
#include <xc.inc>
#include <pic18f2550.inc>

PROG    equ 0x00
ACUMA    equ 0x01  
ACUMB   equ 0x02
_PORTB  equ 0x03
CSTATUS equ 0x04
;------------
Gie		  equ 7
Inte		equ 4
Intedg	equ 6	
Z		    equ 2	
Rbif		equ 0
Rbie		equ 3
Intf		equ 1
Rbif		equ 0
Rbie		equ 3
Intf		equ 1
;------------
Pin0    equ 0
Pin1    equ 1
Pin2    equ 2
Pin3    equ 3
Pin4    equ 4
Pin5    equ 5
Pin6    equ 6
Pin7    equ 7
;f    equ 1
RESET_STEP	    equ 0
ONE_CYCLE_STEP	equ 1
ERROR_F           equ 7
WAIT_SYNC       equ 0
SYNC_SUCCESS    equ 1
FINISH_F	equ 2
  
PSECT resetVec, class=CODE, delta=1
resetVec:
    goto main
    nop
    nop
    nop
    org 0x08
perifVec:
    movf PROG, w, a 
    addlw 0
    bz startup
    movf PROG, w, a 
    sublw 1
    bz onecycle1 
    movf PROG, w, a 
    sublw 2
    bz onecycle2 
    retfie
onecycle1:
    bcf INTCON, Gie, a   ; Disable global interrupts
    bcf INTCON, Rbie, a ; Disable RB7:RB4 int changes
    movf PORTB, f, a
    nop
    nop
    nop
    bcf INTCON, Rbif, a   ; Clear external interrupt flag
    incf PROG, f, a
    bcf INTCON, Intf, a   ; Clear external interrupt flag
    bcf INTCON, Rbif, a   ; Clear external interrupt flag
    bsf INTCON, Inte, a ; Enable int/RB0
    bsf INTCON, Gie, a   ; Enable global interrupts
    retfie
onecycle2:
    bcf INTCON, Gie, a   ; Disable global interrupts
    bcf INTCON, Intf, a   ; Clear external interrupt flag
    movf ACUMA, w, a 
    sublw 1
    bz exit_onecycle2 
    incf ACUMA, f, a
    bsf PORTB, Pin2, a ; We need to select this line in the future
    nop
    nop
    ;movlw 5
    ;movwf ACUMB, a
;wait_tpcs:
    ;nop
    ;decfsz ACUMB, f, a
    ;bra wait_tpcs
    bra exit_onecycle2
    bcf INTCON, Intf, a   ; Clear external interrupt flag
    bsf INTCON, Gie, a   ; Enaable global interrupts
    retfie
exit_onecycle2:
    bcf INTCON, Gie, a   ; Disable global interrupts
    bcf INTCON, Intf, a   ; Clear external interrupt flag
    bcf INTCON, Inte, a ; Enable int/RB0
    clrf ACUMA, a
    bcf PORTB, Pin2, a ; We need to select this line in the future
    movf PORTB, f, a
    bcf INTCON, Rbif, a   ; Clear external interrupt flag
    bsf INTCON, Rbie, a
    bsf INTCON, Gie, a   ; Disable global interrupts
    clrf PROG, a
    incf PROG, f, a ; Restar states
    retfie
startup:
    bcf INTCON, Gie, a   ; Disable global interrupts
    bcf INTCON, Intf, a   ; Clear external interrupt flag
    movf ACUMA, w, a
    sublw 2
    btfsc STATUS, Z, a
    bcf PORTB, Pin2, a
    movf ACUMA, w, a
    sublw 4
    btfsc STATUS, Z, a
    bsf PORTB, Pin3, a
    movf ACUMA, w, a
    sublw 10
    btfsc STATUS, Z, a
    bra savePortValue
    incf ACUMA, f, a
    bcf INTCON, Intf, a   ; Clear external interrupt flag
    bsf INTCON, Gie, a   ; Enable global interrupts
    retfie
savePortValue:
    bsf PORTB, Pin4, a
    bcf INTCON, Intf, a   ; Clear external interrupt flag
    incf PROG, f, a
    retfie

main:
    clrf PORTB, a ; Clear portb register
    clrf PORTC, a ; Clear portc register
    ;movf PORTC, f, a
    ;clrf LATB, a
    movlw 35 ; 00100011
    movwf TRISB, a ; Set port b I/O pins
    ;clrf LATB, a
    clrf PORTB, a ; Clear portb register
    clrf PORTC, a ; Clear portc register
    ;movf PORTC, f, a
    clrf TRISC, a ; Set port c as ouput
;    bcf PORTB, Pin2, a
;test:
;    bra test
    movlw 10
    movwf ACUMA, a
wait_s:
    decfsz ACUMA, f, a
    bra wait_s
    clrf PROG, a ; Clear program pointer
    clrf ACUMA, a ; Clear acumulator A
    clrf ACUMB, a ; Clear acumulator B
    clrf CSTATUS, a ; Clear cstatus
    clrf PORTB, a
    nop
    nop
    bsf INTCON, Inte, a ; Enable int/RB0
    bcf INTCON2, Intedg, a
    bcf INTCON, Intf, a ; Clear int/RB0  flag
    bsf INTCON, Gie, a ; Enable global interrupts
reset_loop:
    movf PROG, w, a 
    sublw 1
    bz app_config
    bra reset_loop
app_config:
    bcf INTCON, Inte, a ; Disable int/RB0
    bcf INTCON, Intf, a   ; Clear external interrupt flag
    bsf INTCON, Gie, a   ; Enable global interrupts
    bcf PORTB, 2, a
    movf PORTB, f, a
    nop
    nop
    nop
    bcf INTCON, Rbif, a
    bsf INTCON, Rbie, a
    clrf ACUMA, a
app_loop:
    bra app_loop
    goto $

    end resetVec
