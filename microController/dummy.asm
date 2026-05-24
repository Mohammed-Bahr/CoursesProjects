LIST p=18f4550
#include <p18f4550.inc>

Value EQU 0x20

ORG 0x00

MOVLW 0xFF
MOVWF TRISB

CLRF TRISD

MOVFF PORTB, Value

MainLoop:

    MOVF Value, W

    CALL Display_7Seg

    MOVWF LATD

    CALL Delay_1s

    DECFSZ Value, F
    GOTO MainLoop

    MOVLW 0x3F
    MOVWF LATD

Stop:
    GOTO Stop

Display_7Seg:
    ADDWF PCL, F

    RETLW 0x3F
    RETLW 0x06
    RETLW 0x5B
    RETLW 0x4F
    RETLW 0x66
    RETLW 0x6D
    RETLW 0x7D
    RETLW 0x07
    RETLW 0x7F
    RETLW 0x6F

Delay_1s:

    BCF T0CON, TMR0ON

    MOVLW b'00000110'
    MOVWF T0CON

    MOVLW 0xE1
    MOVWF TMR0H

    MOVLW 0x7C
    MOVWF TMR0L

    BCF INTCON, TMR0IF

    BSF T0CON, TMR0ON

CheckFlag:
    BTFSC INTCON, TMR0IF
    GOTO FinishDelay

    GOTO CheckFlag

FinishDelay:
    BCF T0CON, TMR0ON

    RETURN

END
