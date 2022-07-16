section .asm

global insb
global insw
global outb
global outw

; The stack pointer (SP) points to the top element in the stack
; ebp register stores the base address of the stack frame
; eax, ebx, ecx, edx are four 32-bit registers used for arithmetic, logical,
; and other operations

; DATA REGISTERS
; ax is the primary accumulator ->  used for io and most arithmetic instructions
; bx is the base register -> used in indexed addressing
; cx is the count register -> stores the loop count in iterative operations
; dx is the data register -> also used for io. used with the ax register

insb:
    push ebp
    mov ebp, esp

    xor eax, eax
    mov edx, [ebp+8]
    in al, dx
    pop ebp
    ret

insw:
    push ebp
    mov ebp, esp

; Set eax to 0
    xor eax, eax
    mov edx, [ebp+8]
    in ax, dx
    pop ebp
    ret

outb:
    push ebp
    mov ebp, esp

    mov eax, [ebp+12]
    mov edx, [ebp+8]
    out dx, al

    pop ebp
    ret

outw:
    push ebp
    mov ebp, esp

    mov eax, [ebp+12]
    mov edx, [ebp+8]
    out dx, ax

    pop ebp
    ret