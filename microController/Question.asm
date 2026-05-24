
// RAM Allocation
SUM_LO    EQU 0x20    // Lower two BCD digits of running sum (00-99)
RegB      EQU 0x21    // Upper two BCD digits / carry accumulator (00-99)
COUNTER   EQU 0x22    // Loop counter

          ORG 0x0000

START:
          CALL BCD_SUM_10
          BRA  START

//------------------------------------------------------------------------------
// Subroutine: BCD_SUM_10
// Adds 10 packed BCD bytes from address 0x50 onward.
// Returns: RegB:SUM_LO = final BCD sum (up to 9999 BCD)
//------------------------------------------------------------------------------
BCD_SUM_10:
          LFSR  0, 0x50       // FSR0 -> start of BCD data array
          MOVLW D'10'
          MOVWF COUNTER       // Loop 10 times
          CLRF  SUM_LO        // Clear low BCD sum
          CLRF  RegB          // Clear high BCD sum / carry register

ADD_LOOP:
          // --- Step 1: Fetch next BCD byte and add to current low sum ---
          MOVF  POSTINC0, W   // W = next BCD byte, FSR0 advances
          ADDWF SUM_LO, W     // W = W + SUM_LO  (result in W, not file)
                              // DAW requires the result to be in W

          // --- Step 2: BCD-correct the low byte ---
          DAW                 // Decimal Adjust W:
                              //   Adds 0x06 if lower nibble > 9 or DC=1
                              //   Adds 0x60 if result > 0x99 or C=1
                              //   Sets C=1 if adjusted result exceeded 99
          MOVWF SUM_LO        // Save corrected low BCD sum

          // --- Step 3: Propagate carry into high byte ---
          BNC   NO_CARRY      // If C=0, no overflow from low byte, skip

          INCF  RegB, F       // RegB++ (plain binary increment is safe here
                              // because RegB counts carries, range 0-9.
                              // No DAW needed — we never add two BCD values
                              // together here, just count +1 each overflow)

NO_CARRY:
          // --- Step 4: Loop control ---
          DECFSZ COUNTER, F   // Decrement counter, skip next if zero
          BRA    ADD_LOOP     // Not done yet, fetch next byte

          RETURN              // Done. Result = RegB:SUM_LO

          END
