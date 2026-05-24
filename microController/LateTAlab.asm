LIST                    p=18f4550               ; Tells The Assembler The Controller Type Being Used
INCLUDE                 <p18f4550.inc>          ; This Includes Some Controller Specific Macros, Ex: Register Name 'STATUS', Flag Name 'Z'
            


	    
	    
	    
	    
	    
	    ORG                     0x00                    ; Program Origin/Start Address
	    
	    
	    ;**********************************
	    ;Direct Addressing Mode
	    MOVLW 0x55
	    
	    MOVWF 0x40
	    MOVWF 0x41
	    MOVWF 0x42
	    MOVWF 0x43
	    MOVWF 0x44

	    
	    ;**********************************
	    ;Register Indirect Addressing Mode without Loop
	    MOVLW 0x66
	    
	    LFSR  0, 0x50    ;Load File Select Register
	    MOVWF INDF0      ;Indirect File-Register
	    INCF FSR0L, F
	    MOVWF INDF0
	    INCF FSR0L, F
	    MOVWF INDF0
	    INCF FSR0L, F
	    MOVWF INDF0
	    INCF FSR0L, F
	    MOVWF INDF0
	    INCF FSR0L, F
	    
	    
	    ;**********************************
	    ;Register Indirect Addressing Mode with Loop
	    COUNT EQU 0x10
	    MOVLW 0x5
	    MOVWF COUNT
	    LFSR 0, 0x60
	    
	    MOVLW 0x77
LOOP:	    
	    MOVWF INDF0
	    INCF FSR0L
	    DECF COUNT
	    BNZ  LOOP
	    
	    
	    ;**********************************
	    ;Register Indirect Addressing Mode with Loop and Autoincrement
	    MOVLW 0x5
	    MOVWF COUNT
	    LFSR 1, 0x30
	    LFSR 0, 0x60
	    
COPY_LOOP:	    
	    MOVF POSTINC0, W
	    MOVWF POSTINC1
	    DECF COUNT
	    BNZ  COPY_LOOP
	    
	    
	    ;**********************************
	    ;Memory Swapping using Indirect Addressing
	    
	    NUM1 EQU 0x70
	    NUM2 EQU 0x71
	    MOVLW 0x10
	    MOVWF NUM1
	    MOVLW 0x20
	    MOVWF NUM2
	    
	    LFSR 0, 0x70
	    LFSR 1, 0x71
	    
	    CALL SWAP_FUN
	    
	    
	    
	    GOTO                    $
	    
	    
SWAP_FUN:
	    TEMP EQU 0x20
	    MOVFF INDF0, TEMP
	    MOVFF INDF1, INDF0
	    MOVFF TEMP, INDF1
	    RETURN
	    
	    
	    END


