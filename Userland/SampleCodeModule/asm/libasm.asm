GLOBAL system_write
GLOBAL system_read

GLOBAL console_clear

GLOBAL get_char

GLOBAL get_time

GLOBAL timer_tick

GLOBAL set_kb_target

GLOBAL get_date

GLOBAL get_regs

GLOBAL get_memory

GLOBAL malloc

GLOBAL free

GLOBAL mem_state

GLOBAL new_process

GLOBAL end_process

GLOBAL kill

GLOBAL get_all_processes

GLOBAL nice

GLOBAL change_state

GLOBAL changeProcesses

GLOBAL createSemaphore

GLOBAL openSemaphore

GLOBAL closeSemaphore

GLOBAL getSemaphores

GLOBAL waitSem

GLOBAL postSem

GLOBAL createPipe

GLOBAL openPipe


GLOBAL get_pipes

GLOBAL get_pid_sys

GLOBAL dup2

GLOBAL my_yield
GLOBAL exit

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

%macro sysCall 1
    pushStateSysCall
    mov rax, %1
    int 80h
    popStateSysCall
    ret

%endmacro


system_write:
    sysCall 1
    ; push rbp
    ; mov rbp,rsp
    ; mov rax, 1
    ; push rbx
    ; int 80h
    ; pop rbx
    ; mov rsp, rbp
    ; pop rbp
    ; ret

system_read:
    sysCall 0
    ; push rbp
    ; mov rbp, rsp
    ; push rbx
    ; mov rax, 0
    ; int 80h
    ; pop rbx
    ; mov rsp, rbp
    ; pop rbp
    ; ret

console_clear:
    sysCall 2
    ; push rbp
    ; mov rbp, rsp
    ; push rbx
    ; mov rax, 2
    ; int 80h
    ; pop rbx
    ; mov rsp, rbp
    ; pop rbp
    ; ret

get_char:
    sysCall 3
    ; push rbp
    ; mov rbp, rsp
    ; push rbx
    ; mov rax, 5
    ; int 80h
    ; pop rbx
    ; mov rsp, rbp
    ; pop rbp
    ; ret



get_time:
    sysCall 4
    ; push rbp
    ; mov rbp, rsp
    ; push rbx
    ; mov rax, 7
    ; int 80h
    ; pop rbx
    ; mov rsp, rbp
    ; pop rbp
    ; ret

timer_tick:
    sysCall 5
    ; push rbp
    ; mov rbp, rsp
    ; push rbx
    ; mov rax, 8
    ; int 80h
    ; pop rbx
    ; mov rsp, rbp
    ; pop rbp
    ; ret

set_kb_target:
    sysCall 6
    ; push rbp
    ; mov rbp, rsp
    ; push rbx
    ; mov rax, 9
    ; int 80h
    ; pop rbx
    ; mov rsp, rbp
    ; pop rbp
    ; ret

get_date:
    sysCall 7


get_regs:
    sysCall 8


malloc:
    sysCall 9

free:
    sysCall 10

mem_state:
    sysCall 11

new_process:
    sysCall 12

end_process:
    sysCall 13

kill:
    sysCall 14

get_all_processes:
    sysCall 15

nice:
    sysCall 16

change_state:
    sysCall 17

changeProcesses:
    sysCall 18

createSemaphore:
    sysCall 19

openSemaphore:
    sysCall 20

closeSemaphore:
    sysCall 21

getSemaphores:
    sysCall 22

waitSem:
    sysCall 23

postSem:
    sysCall 24

createPipe:
    sysCall 25

openPipe:
    sysCall 26

get_pipes:
    sysCall 27

get_pid_sys:
    sysCall 28

dup2:
    sysCall 29

my_yield:
    sysCall 30
exit:
    sysCall 31