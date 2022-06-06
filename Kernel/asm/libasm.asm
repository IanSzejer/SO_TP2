GLOBAL cpuVendor
GLOBAL getRTC

GLOBAL kbReadUntilCode

GLOBAL getKbCode

GLOBAL keyboardActivated
GLOBAL createContext

EXTERN handler
EXTERN updateRegs
; EXTERN updateRegsEx
EXTERN sysCallDispatcher
EXTERN getRegs

section .text

%macro pushState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

%macro pushStateNoRax 0
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popStateNoRax 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
%endmacro


createContext:
	push rbp
	push rbx
	mov rbx,rsp		;Guardo en rbx el stack apuntando a push rbx
    mov rsp,rdi		;Pongo el stack start en rsp
    ; <-rsp
    push qword 0x0   ; ss
    push qword rdi   ; rsp
    push qword 0x202 ; rflags
    push qword 0x8   ; cs
    push qword rsi   ; rip
    ; registros generales
    push qword 0x1
	push qword 0x2
	push qword 0x3
	push qword 0x4
	push qword 0x5
	push qword rdx ; argc
	push qword rcx ; argv
	push qword 0x6
	push qword 0x7
	push qword 0x8
	push qword 0x9
	push qword 0xA
	push qword 0xB
	push qword 0xC
	push qword 0xD

    mov rax, rsp
	mov rsp,rbx
    pop rbx			 ;De esta forma restauro el rsp como estaba cuadno llamaron a createContext y el rbp
    pop rbp
    ret



cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

kbd_wait_till_data:
.loop:	in al, 64h		
		and al, 00000001
		jz .loop

.iscomplete:
		xor rax, rax
		in al, 60h
		test al, 0x80
		jnz .iscomplete
		ret

kbReadUntilCode:
		pushStateNoRax
		mov al, 0D0h
		out 64h, al
		call kbd_wait_till_data	
		popStateNoRax			
		ret

getKbCode:
	mov rax, 0
    in al, 60h
	ret

; -----------------------------------
; Detecta si hay un codigo de teclado en el buffer.
; Retorna 1 de haber, 0 si no.
; -----------------------------------
keyboardActivated:
    mov rax, 0
    in al, 64h
    and al, 01h 
    ret


getRTC:
	push rbp	
	mov rbp, rsp
	mov rax, rdi
	out 70h, al
	in al, 71h
	mov rsp, rbp
    pop rbp
    ret

GLOBAL getStack
getStack:
	mov rax, rsp
	ret

GLOBAL _syscallHandler
EXTERN syscallDispatcher
_syscallHandler:
    pushState
	call sysCallDispatcher 
	popState
	iretq

GLOBAL forceTimer
forceTimer:
	int 20h
	ret

GLOBAL _xchg
_xchg:
	mov rax, rsi
	xchg [rdi], eax
	ret

; GLOBAL infoReg

; infoReg:
; 	; push rsp
; 	pushState

; 	mov [ptr], rdi
; 	mov rdi, rsp
; 	call updateRegs
; 	mov rdi, [ptr]
; 	call getRegs
; 	popState
; 	; pop rsp
; 	ret

section .bss
;ptr resq 1

size resd 1



