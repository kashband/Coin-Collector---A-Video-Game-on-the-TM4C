; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix
	PRESERVE8

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

  

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
; R0=0,    then output "0"
; R0=3,    then output "3"
; R0=89,   then output "89"
; R0=123,  then output "123"
; R0=9999, then output "9999"
; R0=4294967295, then output "4294967295"

fp RN 11
count EQU 0x0
num EQU 0x4

LCD_OutDec
	PUSH {R4-R9, R11, LR}
	SUBS SP, #8; Allocation
	MOV fp, SP; fp <- SP
	MOV R4, #0
	STR R4, [fp, #count]
	STR R0, [fp, #num]
	
	MOV R5, #10
loop	
	LDR R4, [fp, #num]
	UDIV R6, R4, R5; Quotient
	MUL R7, R6, R5; Quotient * 10
	SUBS R8, R4, R7; Mod = Num - Quotient*10 
	PUSH {R8}
	STR R6, [fp, #num]
	LDR R6, [fp, #count]
	ADDS R6, #1; increments counter
	STR R6, [fp, #count]
	LDR R4, [fp, #num]; Checking done at bottom to allow for 0 case
	CMP R4, #0; Checks if run out of digits of original number
	BEQ next
	B loop
	
next
	LDR R4, [fp, #count]
	CMP R4, #0; checks if finished writing all digits
	BEQ done
	POP {R0}
	ADD R0, #0x30; converts decimal number to ASCII equivalent
	BL ST7735_OutChar; Writes char to screen
	SUBS R4, #1; Decrement count
	STR R4, [fp, #count]
	B next
	
done
	ADD SP, #8; deallocation
	POP {R4-R9, R11, LR}
	BX LR

;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000"
;       R0=3,    then output "0.003"
;       R0=89,   then output "0.089"
;       R0=123,  then output "0.123"
;       R0=9999, then output "9.999"
;       R0>9999, then output "*.***"
; Invariables: This function must not permanently modify registers R4 to R11

fp RN 11
star EQU -6; 0x2A - 30
period EQU -2; Ox2E - 30
	;count EQU 0x0 Same bindings as last time
	;num EQU 0x4 Same bindings as last time

LCD_OutFix
	PUSH {R4-R9, R11, LR}
	SUBS SP, #8; Allocation
	MOV R11, #9999
	SUBS R11, R0, R11
	BHI stars
	MOV fp, SP; fp <- SP
	MOV R4, #5
	STR R4, [fp, #count]
	MOV R4, #0
	STR R0, [fp, #num]

	MOV R5, #10
loop2	
	LDR R9, [fp, #count]
	CMP R9, #2
	BEQ addPeriod; subroutine will place the period on the stack and also decrement counter correctly/Returns to loop2
	CMP R9, #0; Checks if run out of digits of original number
	BEQ next2
	LDR R4, [fp, #num]
	UDIV R6, R4, R5; Quotient
	MUL R7, R6, R5; Quotient * 10
	SUBS R8, R4, R7; Mod = Num - Quotient*10 
	PUSH {R8}
	STR R6, [fp, #num]
	LDR R9, [fp, #count]
	SUBS R9, #1; decrements counter
	STR R9, [fp, #count]
	B loop2
	
addPeriod; Subroutine putting period on the stack and also decrement counter
	MOV R4, #period
	PUSH {R4}
	LDR R9, [fp, #count]
	SUBS R9, #1; decrements counter
	STR R9, [fp, #count]
	B loop2
	
stars
	MOV fp, SP; fp <- SP
	MOV R4, #5
	STR R4, [fp, #count]
	MOV R4, #0
	STR R0, [fp, #num]; Likely not necessary
	MOV R4, #star
	PUSH {R4}
	PUSH {R4}
	PUSH {R4}
	MOV R4, #period
	PUSH {R4}
	MOV R4, #star
	PUSH {R4}
	ADDS R5, #5; increments counter/redundant code likely
	STR R5, [fp, #count];redundant code likely
	B next2
	
next2
	MOV R9, #5
	STR R9, [fp, #count]
printing
	LDR R4, [fp, #count]
	CMP R4, #0; checks if finished writing all digits
	BEQ done2
	POP {R0}
	ADD R0, #0x30; converts decimal number to ASCII equivalent
	BL ST7735_OutChar; Writes char to screen
	SUBS R4, #1; Decrement count
	STR R4, [fp, #count]
	B printing
	
done2
	ADD SP, #8; deallocation
	POP {R4-R9, R11, LR}
	BX LR
 
    ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
