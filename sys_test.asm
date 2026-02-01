; System Call Benchmark Test Program (v1.0, February 1 2026)
; Written by Ian Seyler
;
; BareMetal compile:
; nasm sys_test.asm -o sys_test.app


[BITS 64]
DEFAULT ABS

%INCLUDE "libBareMetal.asm"

start:					; Start of program label
	mov eax, 0x80000007
	cpuid
	bt edx, 8			; Check for invariant tsc
	jnc error
	mov eax, 0x15
	cpuid
	cmp ecx, 0
	je error
	;tsc_hz = ECX * EBX / EAX
	push rax
	mov eax, ecx
	mul rbx				; RDX:RAX = RAX * RBX
	pop rcx
	div rcx				; RDX:RAX / RCX
	; RAX contains TSC Hz		; On Framework 13 0x90321000 (2419200000 aka 2419.2 MHz)
	mov r12, rax			; tsc_hz

	; Benchmark end-to-end TSC reads
	rdtscp
	shl rdx, 32
	add rax, rdx
	mov r10, rax			; R10 = T0 (Start)
	rdtscp
	shl rdx, 32
	add rax, rdx
	mov r11, rax			; R11 = T1 (end)
	sub rax, r10
	mov r9, rax			; R9 = T1 - T0

	; Convert to ns
	xor edx, edx
        mov rbx, 1000000000
        mul rbx                         ; RDX:RAX = RAX * RBX
        div r12                         ; RAX = RDX:RAX / R12
	mov r8, rax			; R8 = R9 (nanoseconds)

	rdtscp
	shl rdx, 32
	add rax, rdx
	mov r14, rax

	; code to test
	mov ecx, FREE_MEMORY
        call [b_system]

        rdtscp
        shl rdx, 32
        add rax, rdx
        mov r15, rax
	sub rax, r14

	xor edx, edx
	mov rbx, 1000000000
	mul rbx				; RDX:RAX = RAX * RBX
	div r12				; RAX = RDX:RAX / R12

	; Calculate timer difference in nanoseconds
	sub rax, r8			; Subtract latency of timer call
	mov ecx, DUMP_RAX
	call [b_system]

	; Dump registers
	ud2
error:
	ret				; Return to OS
