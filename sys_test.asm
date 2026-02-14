; System Call Benchmark Test Program (v1.0, February 1 2026)
; Written by Ian Seyler
;
; BareMetal compile:
; nasm sys_test.asm -o sys_test.app

[BITS 64]
DEFAULT ABS

%INCLUDE "libBareMetal.asm"

start:					; Start of program label
	lea rsi, [rel msg_start]
	mov ecx, 23
	call [b_output]

	; Gather timer to timer delay
	mov ecx, TIMECOUNTER
	call [b_system]
	mov r8, rax
	call [b_system]
	mov r9, rax
	mov r15, rax
	sub r15, r8			; R10 contains timer to timer delay

	mov r12, 1000000		; Iterations
	xor r14, r14			; Cumulative time
	xor r13, r13			; Bytes counter (for network test)

loop1:
	; Gather start time of iteration
	mov ecx, TIMECOUNTER
	call [b_system]
	mov r8, rax			; t0

;-------------------------
; Code to benchmark
;-------------------------
	mov ecx, FREE_MEMORY
	call [b_system]
;-------------------------
;	call [b_net_rx]
;	add r13, rcx
;-------------------------
;	cpuid
;-------------------------

	; Gather end time of iteration
	mov ecx, TIMECOUNTER
	call [b_system]
	mov r9, rax			; t1

	; Check for start time > end time
	cmp r9, r8			; t0 > t1?
	jb error

	; Calculate elapsed time
	sub r9, r8			; end time (t1) - start time (t0)
	sub r9, r10			; subtract timer delay calculated earlier

	add r14, r9			; Add elapsed time to cumulative time
	dec r12				; Decrement iterations counter
	cmp r12, 0			; 0 yet?
	jne loop1			; If not, continue loop

	; Divide cumulative time by iterations
	xor edx, edx
	mov rax, r14
	mov ecx, 1000000
	div rcx				; RDX:RAX / RCX (quotient in RAX, remainder in RDX)

	ud2				; Dump registers

	ret

error:
	lea rsi, [rel msg_err]
	mov ecx, 3
	call [b_output]
	ud2				; Dump registers
	ret				; Return to OS

msg_start: db "Starting benchmark...", 13, 10,0
msg_err: db "err", 0
