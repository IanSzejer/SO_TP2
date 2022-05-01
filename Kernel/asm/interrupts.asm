
GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler

GLOBAL _exception00Handler
GLOBAL _exception06Handler

GLOBAL _sysCallHandlerMaster

EXTERN irqDispatcher
EXTERN exceptionDispatcher

EXTERN sysCallDispatcher

SECTION .text

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

%macro pushStateSysCall 0
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

%macro popStateSysCall 0
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

%macro irqHandlerMaster 1
	push rsp
	pushState

	mov rdi, %1 ; pasaje de parametro
	mov rsi, rsp
	call irqDispatcher

	; ; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	pop rsp
	iretq
%endmacro

_sysCallHandlerMaster:
	cli
	pushStateSysCall
	;sti
	mov r8, rax ; el 4to argumento de sysCallDispatcher tendra el valor de rax

	call sysCallDispatcher ; me llamaron a la syscall int 80h con los argumentos en registros rdi, rsi, rdx, rax. Llamo a la funcion 'sysCallDispatcher' que se encarga de manejar la llamada y retorna en rax el valor de retorno

	popStateSysCall

	;cli
	sti
	iretq


%macro exceptionHandler 1
	push rsp
	pushState

	mov rdi, %1 ; pasaje de parametro
	mov rsi, rsp ; puntero a los registros
	call exceptionDispatcher

	popState
	pop rsp
	iretq
%endmacro


_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret


_sti:
	sti
	ret

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn


;8254 Timer (Timer Tick)
_irq00Handler:
	irqHandlerMaster 0

;Keyboard
_irq01Handler:
	irqHandlerMaster 1

; ;Cascade pic never called
; _irq02Handler:
; 	irqHandlerMaster 2

; ;Serial Port 2 and 4
; _irq03Handler:
; 	irqHandlerMaster 3

; ;Serial Port 1 and 3
; _irq04Handler:
; 	irqHandlerMaster 4

; ;USB
; _irq05Handler:
; 	irqHandlerMaster 5


;Zero Division Exception
_exception00Handler:
	exceptionHandler 0

_exception06Handler:
	exceptionHandler 6

haltcpu:
	cli
	hlt
	ret

SECTION .bss
	aux resq 1