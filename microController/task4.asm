; ==============================================================================
; Program: 7-Segment Countdown Timer via Port B Input
; Processor: PIC18F4550
; Description: Reads an initial value from PORTB, then counts down to 1,
;              displaying each number on a 7-segment display connected to PORTD
;              with a 1-second delay between decrements. Displays '0' at the end.
; ==============================================================================

    LIST p=18f4550                  ; Define the target processor
    #include <p18f4550.inc>         ; Include processor-specific register definitions

Value EQU 0x20                      ; Allocate a RAM variable named 'Value' at address 0x20

    ORG 0x00                        ; Reset vector (where the CPU starts executing after power-on/reset)

; ------------------------------------------------------------------------------
; Initialization Section
; ------------------------------------------------------------------------------
    MOVLW 0xFF                      ; Load literal 0xFF (binary 11111111) into W (Working Register)
    MOVWF TRISB                     ; Copy W to TRISB -> Configures all PORTB pins as INPUTS

    CLRF TRISD                      ; Clear TRISD register -> Configures all PORTD pins as OUTPUTS

    MOVFF PORTB, Value              ; Read the logic states of PORTB pins and save into 'Value' variable

; ------------------------------------------------------------------------------
; Main Countdown Loop
; ------------------------------------------------------------------------------
MainLoop:
    MOVF Value, W                   ; Copy the current countdown number from 'Value' into W register

    CALL Display_7Seg               ; Call look-up table subroutine to convert the number to 7-segment code

    MOVWF LATD                      ; Output the 7-segment code from W to the PORTD latches (turns on segments)

    CALL Delay_1s                   ; Call the timer subroutine to pause the execution for exactly 1 second

    DECFSZ Value, F                 ; Decrement 'Value' by 1 and store result back in 'Value'.
                                    ; Skip the next instruction (GOTO) if the result equals 0.
    GOTO MainLoop                   ; If 'Value' is not 0, loop back to display and decrement the next number

    ; This segment executes only after the loop completes (when 'Value' reaches 0)
    MOVLW 0x3F                      ; Load 7-segment code for '0' (0x3F) into W register
    MOVWF LATD                      ; Output '0' to PORTD so the countdown doesn't end on a blank/wrong display

; ------------------------------------------------------------------------------
; Infinite Halt Loop
; ------------------------------------------------------------------------------
Stop:
    GOTO Stop                       ; Trap the MCU here infinitely to prevent it from executing random memory

; ------------------------------------------------------------------------------
; Subroutine: 7-Segment Look-up Table (Computed GOTO)
; Input: W register holds the index (0 to 9)
; Output: W register returns the active-high 7-segment cathode pattern
; ------------------------------------------------------------------------------
Display_7Seg:
    ADDWF PCL, F                    ; Add the index in W to the Program Counter Low (PCL) byte.
                                    ; This forces a jump directly to one of the RETLW instructions below.

    RETLW 0x3F                      ; Index 0: Returns 0x3F (Displays '0')
    RETLW 0x06                      ; Index 1: Returns 0x06 (Displays '1')
    RETLW 0x5B                      ; Index 2: Returns 0x5B (Displays '2')
    RETLW 0x4F                      ; Index 3: Returns 0x4F (Displays '3')
    RETLW 0x66                      ; Index 4: Returns 0x66 (Displays '4')
    RETLW 0x6D                      ; Index 5: Returns 0x6D (Displays '5')
    RETLW 0x7D                      ; Index 6: Returns 0x7D (Displays '6')
    RETLW 0x07                      ; Index 7: Returns 0x07 (Displays '7')
    RETLW 0x7F                      ; Index 8: Returns 0x7F (Displays '8')
    RETLW 0x6F                      ; Index 9: Returns 0x6F (Displays '9')

; ------------------------------------------------------------------------------
; Subroutine: 1-Second Delay Generator (Using Hardware Timer0)
; ------------------------------------------------------------------------------
Delay_1s:
    BCF T0CON, TMR0ON               ; Stop Timer0 (turn it off) to configure it safely

    MOVLW b'00000110'               ; Configure Timer0: 16-bit mode, internal clock (Fosc/4),
    MOVWF T0CON                     ; and enable Prescaler at 1:128 ratio

    ; Preload Timer0 registers to achieve exactly a 1-second delay based on the clock frequency
    MOVLW 0xE1
    MOVWF TMR0H                     ; Write high byte of preload value
    MOVLW 0x7C
    MOVWF TMR0L                     ; Write low byte of preload value

    BCF INTCON, TMR0IF              ; Clear the Timer0 Overflow Interrupt Flag to start fresh

    BSF T0CON, TMR0ON               ; Start Timer0 counting up

; Polling Loop
CheckFlag:
    BTFSC INTCON, TMR0IF            ; Bit Test INTCON: Skip next instruction if TMR0IF is 1 (Timer overflowed)
    GOTO FinishDelay                ; If flag is 1 (Time's up!), jump out of the loop
    GOTO CheckFlag                  ; If flag is 0, loop back and keep checking the flag

FinishDelay:
    BCF T0CON, TMR0ON               ; Turn off Timer0 now that the 1 second has elapsed

    RETURN                          ; Return to the main loop

END                                 ; End of code directive
