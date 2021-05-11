%include "lib.inc"

%define O_RDONLY    0x0
%define PROT_READ   0x1
%define MAP_PRIVATE 0x2

section .data
file_name: db "file.txt", 0x0

section .text
global _start

factorial:
    mov rax, 1
    xor rcx, rcx
.loop:
    inc rcx
    mov rdx, 0
    imul rcx
    cmp rcx, rdi
    je .end

    jmp .loop
.end:
    ret

_start:
    mov rax, 2         ;opening the file for read
    mov rdi, file_name
    mov rsi, O_RDONLY
    mov rdx, 0
    syscall

    mov r8, rax 
    mov rax, 9 ; mmap syscall
    mov rdi, 0
    mov rsi, 4096
    mov rdx, PROT_READ
    mov r10, MAP_PRIVATE
    mov r9, 0 ; offset inside file
    syscall

    mov rdi, rax
    call parse_int    ; -> rax: number read (x)

    push rax
    mov rdi, rax      ; find factorial
    call factorial    
    mov rdi, rax
    call print_int 
    pop rax

    ;mov rdi, r15
    ;call is_prime     ; 0 if the input number is prime, 1 otherwise

    ;mov rdi, r15
    ;call sum_digits   ; sum of all number's digits

    ;mov rdi, r15
    ;call fibonacci    ; x-th Fibonacci number

    ;mov rdi, r15
    ;call is_fibonacci ; Checks if x is a Fibonacci number


    mov rax, 60
    mov rdi, 0
    syscall
