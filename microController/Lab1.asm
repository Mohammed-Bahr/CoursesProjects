; Assembly Template To Follow

; Label     ; OpCode/Directive      ; Operands              ; Comment

Initial     EQU                     D'11'                   ; Note: Literal Value (By Default) is Hex 
                                                            ;       Such as 12 or 0x12 or H'12' or 12H
                                                            ; Note: Literal Value Could Be Used In Another Forms Also
                                                            ;       Decimal Such as D'12' or .12 (Dot Before The Number)
                                                            ;       Binary Such as B'10010011'
                                                            ;       ASCII Such as A'9'

RegAddress  EQU                     0x20                    ; Use Macro For Register Address To Be Used Later
InRegFile   EQU                     0x01                    ; Use Macro For 'd' Destination is 'RegFile'
InWReg      EQU                     0x00                    ; Use Macro For 'd' Destination is 'WREG'

            ORG                     0x00                    ; Program Origin/Start Address
            
            MOVLW                   Initial                 ; Sets 'WREG' Register With Value 0x05 = 5H
            
            ADDLW                   0x02                    ; Adds To 'WREG' Register Value Of 0x07 = 7H

            MOVWF                   RegAddress              ; Puts The 'WREG' Register Content To 'RegAddress' Address In Register File
            
            ADDWF                   RegAddress, InWReg      ; Adds 'RegAddress' Address Value To 'WREG' And Stores The Result In 'WREG'
            
            ADDWF                   RegAddress, InRegFile   ; Adds 'RegAddress' Address Value To 'WREG' And Stores The Result In 'RegFile'
            
            MOVLW                   0xFF
            MOVWF                   0x00
            MOVLW                   0x01
            ADDWF                   0x00, 0 ; WREG = 0, RegFile = 255
            MOVLW                   0x01
            ADDWFC                  0x00, 0 ; WREG = 1
            
FINISH        GOTO                    FINISH                    ; $ is the current instruction address
                                                            ; so goto $ same as while(1){} in c
            
            END                                             ; Program End
