; Lab-2 Loops, & Procedures Template To Follow
            
; Label     ; OpCode/Directive      ; Operands              ; Comment
            LIST                    p=18f4550               ; Tells The Assembler The Controller Type Being Used
            INCLUDE                 <p18f4550.inc>          ; This Includes Some Controller Specific Macros, Ex: Register Name 'STATUS', Flag Name 'Z'
            

Initial     EQU                     D'100'                  ; Initial Loop Counter Value
Counter     EQU                     0x11                    ; Counter Address
Counter_2   EQU                     0x10                    ; Counter Address

SUM_Counter EQU                     0x00                    ; Sum Counter Address
SUM_Value   EQU                     0x20                    ; Sum Value Address
SUM_Temp    EQU                     0x21                    ; Sum Temporary Value Address
     
            
;###############################################################################
	    MOVLW                   0x00                    ; Sets 'WREG' Register With Value Equals To Zeroes
	    MOVWF                   TRISB                   ; Sets 'PORTB' To Output mode
	    MOVLW                   0x55                    ; Sets 'WREG' Register With Value Equals To '55H'
	    MOVWF                   PORTB                   ; Sets 'PORTB' To 55H
;###############################################################################
	    CLRF                    PORTB
	    CLRF                    PORTC
	    MOVLW                   B'00000000'             ; Sets 'WREG' Register With Value Equals To Zeroes
	    MOVWF                   TRISB                   ; Sets 'PORTB' To Output mode
	    MOVLW                   B'11111111'             ; Sets 'WREG' Register With Value Equals To Ones
	    MOVWF                   TRISC                   ; Sets 'PORTC' To Input mode
	    MOVF                    PORTC,W                 ; Sets 'WREG' Register With Value From 'PORTC' 
	    ADDLW                   0x05                    ; Adds '05H' To The 'WREG' Register 
	    MOVWF                   PORTB                   ; Sets 'PORTB' To Value Of 'WREG' Register
;###############################################################################
            MOVLW                   Initial                 ; Sets 'WREG' Register With Value Equals To 'Initial'
            MOVWF                   Counter                 ; Moves 'WREG' Value To 'Counter' Address
            
LOOP_EX1                                                    ; Loop Body Should Be Starting From Here
                                                            ; This Style Is Like do {} while () in c
            
            DECFSZ                  Counter, F              ; Decrements 'Counter' Address Value By One, If Value Reached Zero, Then Skips Next Instruction
                                                            ; Otherwise, No Skipping
            GOTO                    LOOP_EX1                ; Jump To 'LOOP_EX1' Label Address
            
;###############################################################################
            MOVLW                   Initial                 ; Sets 'WREG' Register With Value Equals To 'Initial'
            MOVWF                   Counter                 ; Moves 'WREG' Value To 'Counter' Address
            
            BCF                     STATUS, Z               ; Clears 'Z' Zero Flag Bit In Status Register
LOOP_EX2    BZ                      LOOP_EX2_END            ; If 'Z' Zero Flag Bit Is Set In The Status Register, Then Jump
                                                            ; Otherwise, No Jump
                                                            
                                                            ; Loop Body Should Be Starting From Here
                                                            ; This Style Is Like while (){} in c
            
            DECF                    Counter, F              ; Decrements 'Counter' Address Value By One
            GOTO LOOP_EX2                                   ; Jump To 'LOOP_EX2' Label Address
LOOP_EX2_END                                                ; The Loop With 'LOOP_EX2' Label Address Has Ended
            
;###############################################################################
            MOVLW                   Initial                 ; Sets 'WREG' Register With Value Equals To 'Initial'
            MOVWF                   Counter                 ; Moves 'WREG' Value To 'Counter' Address
            
LOOP_EX3                                                    ; Loop Body Should Be Starting From Here
                                                            ; This Style Is Like do {} while () in c
                                                            
            MOVLW                   Initial                 ; Sets 'WREG' Register With Value Equals To 'Initial'
            MOVWF                   Counter_2               ; Moves 'WREG' Value To 'Counter_2' Address
                                                            
LOOP_NESTED                                                 ; Loop Body Should Be Starting From Here
                                                            ; This Style Is Like do {} while () in c
                                                            
            DECFSZ                  Counter_2, F            ; Decrements 'Counter' Address Value By One, If Value Reached Zero, Then Skips Next Instruction
                                                            ; Otherwise, No Skipping
            GOTO                    LOOP_NESTED             ; Jump To 'LOOP_NESTED' Label Address
                                                            
            DECFSZ                  Counter, F              ; Decrements 'Counter' Address Value By One, If Value Reached Zero, Then Skips Next Instruction
                                                            ; Otherwise, No Skipping
            GOTO                    LOOP_EX3                ; Jump To 'LOOP_EX3' Label Address
            
;###############################################################################
            GOTO                    MAIN                    ; Jump To 'MAIN', We Can Consider This As A New Program
PROC_X10    
            MOVLW                   D'10'                   ; Sets 'WREG' Register With Value Equals To 10 Decimal
            MOVWF                   SUM_Counter             ; Moves 'WREG' Value To 'SUM_Counter' Address
            MOVFF                   SUM_Value, SUM_Temp     ; Moves 'SUM_Value' Address Value To 'SUM_Temp' Address
            MOVLW                   D'0'                    ; Sets 'WREG' Register With Value Equals To 0 Decimal
ADD_AGAIN   ADDWF                   SUM_Value, W             ; Adds 'WREG' Register And 'SUM_Temp' Address Value, Then Store The Result In 'WREG' Register
	    movwf		    SUM_Temp
            DECFSZ                  SUM_Counter, F          ; Decrements 'Counter' Address Value By One, Store The Value In 'SUM_Counter'
                                                            ; If Value Reached Zero, Then Skips Next Instruction
                                                            ; Otherwise, No Skipping
            GOTO                    ADD_AGAIN               ; Jump To 'ADD_AGAIN' Label Address
            MOVWF                   SUM_Value               ; Moves 'WREG' Value To 'SUM_Value' Address
            RETURN                                          ; Invoke A Procedure Return, Which Restore The Program Counter Value, Then Jump To It
            
MAIN        MOVLW                   D'1'                    ; Sets 'WREG' Register With Value Equals To 1 Decimal
            MOVWF                   SUM_Value               ; Moves 'WREG' Value To 'SUM_Value' Address
            CALL                    PROC_X10                ; Invoke A Procedure Call, Which Saves The Program Counter Value, Then Jump To Procedure Address
            
            MOVF                    SUM_Value, W            ; Moves The Value In 'Value' Address Into Working Register
            
;###############################################################################


            GOTO                    $                       ; This Line Is To Keep The Program Running, Not To Terminate
                                                            ; $ is the current instruction address
                                                            ; so goto $ same as while(1){} in c
            END                                             ; Program End








