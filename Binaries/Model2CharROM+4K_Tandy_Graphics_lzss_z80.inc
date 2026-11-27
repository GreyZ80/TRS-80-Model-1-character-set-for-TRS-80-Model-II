; =============================================================================
; Compressed 256-Character 8x10 Bitmap Font (2,560 bytes -> 764 bytes LZSS)
; Blank pixels pre-cleaned (0x20 XORed to 0x00) for IDT7134 Character RAM
; =============================================================================

FONT_COMP_SIZE  EQU     764
FONT_RAW_SIZE   EQU     2560

; -----------------------------------------------------------------------------
; STEP 1: Unpack 764-byte ROM stream into 2,560-byte RAM buffer
; Entry:  HL = Source address in ROM (FONT_DATA)
;         DE = Destination address in system RAM (2,560 bytes)
; Exit:   All 2,560 bytes unpacked; AF, BC, DE, HL, IX modified
; -----------------------------------------------------------------------------
LZSS_UNPACK:
        LD      BC, 2560        ; Total uncompressed byte count
        LD      IXL, 1          ; Trigger flag byte fetch immediately
LZ_LOOP:
        DEC     IXL
        JR      NZ, LZ_NEXT_BIT
        LD      A, (HL)         ; Fetch next 8-bit flag byte
        INC     HL
        LD      IXH, A
        LD      IXL, 8
LZ_NEXT_BIT:
        RRC     IXH             ; Shift next flag bit into Carry
        JR      C, LZ_MATCH
        LDI                     ; Literal byte: (DE++) = (HL++), BC--
        JP      PE, LZ_LOOP
        RET

LZ_MATCH:
        PUSH    BC              ; Save remaining total byte count
        LD      C, (HL)         ; C = low 8 bits of (offset - 1)
        INC     HL
        LD      A, (HL)         ; A = high nibble (offset-1)[11:8] | (len-3)[3:0]
        INC     HL
        PUSH    HL              ; Save ROM source pointer
        LD      L, A
        AND     0F0H
        RRCA
        RRCA
        RRCA
        RRCA
        LD      B, A            ; BC = offset - 1
        LD      A, L
        AND     0FH
        ADD     A, 3            ; A = match length (3..18)
        LD      H, D
        LD      L, E
        SCF                     ; Set Carry so SBC HL, BC computes DE - offset
        SBC     HL, BC          ; HL = DE - (offset - 1) - 1 = DE - offset
        LD      C, A
        LD      B, 0
        LDIR                    ; Copy match bytes into (DE++)
        POP     HL              ; Restore ROM source pointer
        POP     BC              ; Restore remaining total byte count
        SUB     C               ; Compute BC = BC - A (match length)
        NEG
        LD      C, A
        JR      NC, LZ_NO_BORROW
        DEC     B
LZ_NO_BORROW:
        LD      A, B
        OR      C
        JR      NZ, LZ_LOOP
        RET

; -----------------------------------------------------------------------------
; STEP 2: Load unpacked 2,560-byte font into IDT7134 (16-byte stride/char)
; Entry:  HL = Unpacked 2,560-byte RAM buffer
;         DE = IDT7134 base address (e.g., 0x8000)
; -----------------------------------------------------------------------------
LOAD_IDT7134:
        LD      B, 0            ; 256 characters
LOAD_CHAR_LOOP:
        PUSH    BC
        LD      BC, 10          ; 10 scanlines per glyph
        LDIR                    ; Copy 10 bytes to IDT7134
        LD      A, E            ; Advance DE by +6 to reach next 16-byte boundary
        ADD     A, 6
        LD      E, A
        JR      NC, LOAD_NEXT
        INC     D
LOAD_NEXT:
        POP     BC
        DJNZ    LOAD_CHAR_LOOP
        RET

; -----------------------------------------------------------------------------
; Compressed Font Payload (764 bytes)
; -----------------------------------------------------------------------------
FONT_DATA:
        DB      0C0H, 000H, 000H, 000H, 01FH, 01FH, 01CH, 000H, 000H, 008H, 000H, 098H, 000H, 0CCH, 0CCH, 009H
        DB      002H, 00EH, 000H, 0CCH, 0CCH, 00FH, 001H, 099H, 009H, 001H, 01FH, 01FH, 009H, 002H, 027H, 000H
        DB      0CFH, 0CFH, 01DH, 007H, 0F9H, 027H, 005H, 0CFH, 0CFH, 027H, 007H, 045H, 005H, 013H, 007H, 04EH
        DB      002H, 013H, 00CH, 0FFH, 06CH, 002H, 03BH, 004H, 045H, 006H, 027H, 00DH, 06DH, 001H, 076H, 005H
        DB      088H, 004H, 007H, 005H, 0FFH, 0A9H, 003H, 058H, 002H, 0B3H, 000H, 007H, 003H, 0B3H, 003H, 013H
        DB      005H, 038H, 009H, 04FH, 00FH, 07DH, 017H, 00BH, 0CFH, 000H, 000H, 007H, 005H, 0CBH, 001H, 007H
        DB      003H, 0D5H, 000H, 0A0H, 079H, 000H, 005H, 000H, 0C0H, 000H, 005H, 01EH, 010H, 001H, 004H, 034H
        DB      010H, 00AH, 0A3H, 067H, 00BH, 0ACH, 00AH, 004H, 004H, 004H, 009H, 006H, 05EH, 001H, 000H, 011H
        DB      00DH, 003H, 008H, 000H, 02AH, 0C4H, 003H, 042H, 044H, 000H, 008H, 000H, 006H, 066H, 078H, 010H
        DB      010H, 068H, 068H, 010H, 018H, 06AH, 064H, 01AH, 049H, 00FH, 05BH, 00DH, 00AH, 000H, 01EH, 065H
        DB      070H, 006H, 01EH, 078H, 00FH, 000H, 05EH, 08CH, 00FH, 0CEH, 011H, 060H, 041H, 0C1H, 011H, 062H
        DB      066H, 07AH, 042H, 062H, 03DH, 013H, 008H, 081H, 03EH, 004H, 01CH, 062H, 000H, 02CH, 010H, 060H
        DB      034H, 001H, 089H, 009H, 000H, 000H, 000H, 01DH, 003H, 02CH, 000H, 004H, 046H, 003H, 080H, 05EH
        DB      060H, 058H, 000H, 000H, 064H, 018H, 041H, 003H, 0F4H, 05CH, 062H, 03BH, 002H, 05EH, 027H, 001H
        DB      011H, 023H, 010H, 000H, 013H, 003H, 0DFH, 009H, 000H, 08BH, 000H, 027H, 003H, 00CH, 01FH, 04FH
        DB      000H, 010H, 053H, 000H, 09EH, 004H, 065H, 0A0H, 003H, 040H, 011H, 001H, 010H, 040H, 081H, 004H
        DB      056H, 024H, 002H, 008H, 06AH, 076H, 06CH, 036H, 014H, 062H, 05EH, 062H, 062H, 001H, 090H, 001H
        DB      05CH, 002H, 002H, 01CH, 002H, 002H, 05CH, 021H, 01DH, 002H, 060H, 060H, 060H, 002H, 09AH, 021H
        DB      058H, 004H, 0F0H, 002H, 002H, 002H, 004H, 099H, 000H, 0ECH, 001H, 09CH, 000H, 0F2H, 001H, 0E3H
        DB      009H, 003H, 045H, 003H, 060H, 06EH, 062H, 027H, 002H, 041H, 000H, 045H, 004H, 003H, 0A0H, 01CH
        DB      0D1H, 002H, 062H, 064H, 068H, 050H, 068H, 064H, 00DH, 0F4H, 001H, 050H, 07BH, 002H, 038H, 011H
        DB      062H, 046H, 07AH, 07AH, 0C1H, 077H, 003H, 062H, 042H, 072H, 06AH, 066H, 081H, 002H, 0E5H, 000H
        DB      0F7H, 08AH, 000H, 013H, 031H, 000H, 010H, 05CH, 083H, 000H, 013H, 004H, 0ADH, 013H, 013H, 001H
        DB      0E1H, 045H, 003H, 01CH, 062H, 060H, 01CH, 03FH, 013H, 096H, 017H, 03AH, 002H, 097H, 071H, 012H
        DB      0CDH, 000H, 009H, 024H, 061H, 000H, 000H, 069H, 075H, 0C8H, 001H, 0FFH, 012H, 000H, 0E8H, 000H
        DB      076H, 011H, 0CFH, 000H, 0BEH, 019H, 08FH, 013H, 075H, 033H, 0BCH, 015H, 01FH, 00DH, 007H, 019H
        DB      004H, 031H, 005H, 023H, 03FH, 0BEH, 031H, 018H, 000H, 01CH, 001H, 04DH, 022H, 060H, 060H, 07CH
        DB      042H, 062H, 042H, 07CH, 085H, 0AFH, 014H, 060H, 0FDH, 014H, 01AH, 066H, 062H, 066H, 06BH, 023H
        DB      0D8H, 01CH, 062H, 05EH, 0B6H, 000H, 0F2H, 011H, 000H, 064H, 012H, 01DH, 003H, 03FH, 01CH, 000H
        DB      004H, 040H, 03BH, 002H, 03FH, 01EH, 0C8H, 011H, 012H, 023H, 004H, 008H, 0CCH, 010H, 008H, 0DDH
        DB      025H, 018H, 045H, 056H, 069H, 000H, 000H, 02FH, 042H, 087H, 03BH, 007H, 041H, 011H, 0B3H, 033H
        DB      07CH, 042H, 042H, 07CH, 040H, 012H, 0FFH, 063H, 003H, 027H, 004H, 053H, 013H, 0CEH, 020H, 040H
        DB      010H, 084H, 003H, 0EFH, 011H, 030H, 023H, 0AFH, 005H, 020H, 0A9H, 004H, 041H, 011H, 07EH, 043H
        DB      061H, 063H, 000H, 016H, 05FH, 013H, 07FH, 040H, 011H, 069H, 014H, 0B3H, 003H, 00DH, 026H, 070H
        DB      024H, 05AH, 025H, 0B8H, 043H, 010H, 0FDH, 005H, 006H, 010H, 0A4H, 003H, 046H, 036H, 04AH, 031H
        DB      0F0H, 035H, 0C7H, 041H, 0FBH, 035H, 07CH, 0C0H, 0C0H, 013H, 00FH, 020H, 006H, 037H, 048H, 012H
        DB      040H, 013H, 00EH, 0CFH, 0FEH, 0CFH, 02FH, 040H, 055H, 044H, 009H, 005H, 041H, 042H, 013H, 00CH
        DB      04FH, 00FH, 04FH, 00FH, 0FFH, 077H, 003H, 073H, 006H, 0ACH, 041H, 095H, 052H, 09FH, 002H, 013H
        DB      00CH, 038H, 057H, 037H, 057H, 0FFH, 09FH, 002H, 0A7H, 04CH, 09FH, 002H, 01FH, 006H, 020H, 003H
        DB      0DDH, 043H, 013H, 00BH, 04FH, 00FH, 0FFH, 04FH, 00FH, 038H, 056H, 03FH, 004H, 009H, 005H, 03FH
        DB      012H, 013H, 00CH, 010H, 015H, 092H, 004H, 0FFH, 027H, 004H, 013H, 00FH, 04CH, 003H, 0A3H, 003H
        DB      077H, 055H, 081H, 057H, 013H, 007H, 04FH, 00FH, 0FFH, 04FH, 00FH, 077H, 003H, 03FH, 01FH, 03FH
        DB      01FH, 03FH, 01FH, 03FH, 01FH, 03FH, 01FH, 03FH, 01FH, 0FFH, 03FH, 01FH, 08FH, 01FH, 038H, 06FH
        DB      03FH, 06FH, 03FH, 06FH, 03FH, 06FH, 03FH, 06FH, 092H, 06FH, 0FFH, 03FH, 06FH, 03FH, 06FH, 03FH
        DB      06FH, 03FH, 06FH, 03FH, 06FH, 03FH, 06FH, 03FH, 06FH, 03FH, 06FH, 0FFH, 03FH, 06FH, 03FH, 06FH
        DB      03FH, 06FH, 03FH, 06FH, 03FH, 06FH, 03FH, 06FH, 03FH, 06FH, 03FH, 06FH, 0FFH, 03FH, 06FH, 03FH
        DB      06FH, 03FH, 06FH, 03FH, 06FH, 03FH, 06FH, 03FH, 06FH, 03FH, 06FH, 03FH, 06FH, 07FH, 03FH, 06FH
        DB      03FH, 06FH, 03FH, 06FH, 03FH, 06FH, 03FH, 06FH, 03FH, 06DH, 0FFH, 045H
