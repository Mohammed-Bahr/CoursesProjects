; Delay task for PIC18F4550
; Blinks an LED on PORTB,2 five times using a triple nested loop delay

LIST        p=18f4550    ; Specify processor type for assembly
INCLUDE     <p18f4550.inc>          ; Include PIC18F4550 register definitions
;------------------------- Register Definitions -------------------
R1 EQU 0x06                     ; Define R1 as general purpose register at 0x06
R2 EQU 0x07                     ; Define R2 as general purpose register at 0x07
R3 EQU 0x08                     ; Define R3 as general purpose register at 0x08
Count EQU 0x05                  ; Define Count as general purpose register at 0x05

;------------------------- Program Start -------------------------
    ;MOVLW 0x0F
    ;MOVWF ADCON1             ; (Commented) Configure analog pins as digital

    ORG 0x00                  ; Program start address (reset vector)

    ClRF TRISB                ; Set PORTB pins as outputs
    BSF PORTB , 2             ; Set RB2 high initially
    MOVLW d'5'                ; Load number of blinks (5) into W
    MOVWF Count               ; Store blink count in Count register

Again BTG PORTB , 2           ; Toggle RB2 (blink LED)
      CALL Delay              ; Call the delay subroutine
U      DECF Count , F         ; Decrement blink counter
      BNZ Again               ; If count != 0, repeat blinking

    GOTO $                    ; Infinite loop (trap)

;------------------------- Delay Subroutine ----------------------
     ORG 0x300                ; Place Delay subroutine at address 0x300
Delay                        ; Delay subroutine: triple nested loop
     MOVLW d'100'             ; Outer loop count (R1 = 100)
     MOVWF R1
loop1
     MOVLW d'100'             ; Middle loop count (R2 = 100)
     MOVWF R2
loop2
     MOVLW d'40'              ; Inner loop count (R3 = 40)
     MOVWF R3
loop3
     NOP                      ; No-operation (waste 1 cycle)
     NOP                      ; No-operation (waste 1 cycle)
     DECF R3, F               ; Decrement inner loop counter R3
     BNZ loop3                ; If R3 != 0, repeat inner loop
     DECF R2, F               ; Decrement middle loop counter R2
     BNZ loop2                ; If R2 != 0, repeat middle loop
     DECF R1, F               ; Decrement outer loop counter R1
     BNZ loop1                ; If R1 != 0, repeat outer loop

     RETURN                   ; Return from subroutine
     END                      ; End of program
