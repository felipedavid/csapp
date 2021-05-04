section .text

string_length:
    xor rax, rax
.loop:
    cmp byte [rdi+rax], 0
    je .end

    inc rax
    jmp .loop
.end:
    ret

print_string:
    mov rsi, rdi
    call string_length
    mov rdx, rax
    mov rax, 1
    mov rdi, 1
    syscall
.end:
    ret


print_char:
    push rdi

    mov rax, 1
    mov rdi, 1
    mov rsi, rsp
    mov rdx, 1
    syscall

    add rsp, 8
    ret

print_newline:
    xor rax, rax
    ret


print_uint:
    push r12
    push r13
    mov r12, rsp
    mov r13, 10

    cmp rdi, 0
    je .zero

    sub rsp, 1
    mov byte [rsp], 0

    mov rax, rdi
.loop:
    mov rdx, 0
    cmp rax, 0
    je .end
    div r13
    add rdx, 0x30

    sub rsp, 1
    mov byte [rsp], dl

    jmp .loop
.zero:
    sub rsp, 1
    mov byte[rsp], 0x30
.end:
    mov rdi, rsp
    call print_string
    mov rsp, r12
    pop r13
    pop r12
    ret


print_int:
    push r12

    cmp rdi, 0
    jl .negative
    
    jmp .print_un
.negative:
    neg rdi
    mov r12, rdi
    mov rdi, '-'
    call print_char
    mov rdi, r12 

.print_un:
    call print_uint

    pop r12
    ret

string_equals:
    xor rax, rax
.loop:
    mov rcx, byte [rdi]
    cmp rcx, byte [rsi]
    jne .not_equal

.not_equal:
    mov rax, 1
    jmp .end
.end:
    ret


read_char:
    push 0

    xor rax, rax
    xor rdi, rdi
    mov rsi, rsp
    mov rdx, 1
    syscall

    pop rax
    ret 

read_word:
    push r12
    push r13
    mov r13, rdi 
    xor r12, r12 ; r12 -> n of letters printed
.loop:
    cmp rsi, r12 ; check if we still have space on the buffer
    jz .too_big
    
    push rdi
    push rsi
    call read_char
    pop rsi
    pop rdi

    cmp al, 0     ; check if it was a EOT signal
    je .return_addr
    cmp al, 32
    je .return_addr
    cmp al, 9
    je .loop
    cmp al, 10
    je .loop

    mov byte [r13], al ; move char to the buffer

    inc r12
    inc r13
    jmp .loop
.too_big:
    xor rax, rax
    jmp .end
.return_addr:
    mov byte [r13], 0
    mov rax, rdi
.end:
    mov rdx, r12
    pop r13
    pop r12
    ret


; rdi points to a string
; returns rax: number, rdx: length
parse_uint:
    push r11
    push r12
    push r13
    push r14 
    push r15
    mov r13, 1
    mov r15, 10
    xor r14, r14
    xor rax, rax
    xor rdx, rdx
.push_numbers:
    cmp byte [rdi], 0
    je .pop_numbers

    mov cl, byte [rdi]
    inc rdi
    cmp cl, 0x39
    jg .push_numbers

    inc rdx ; inc number of characters

    sub cl, 0x30
    dec rsp
    mov byte [rsp], cl

    jmp .push_numbers
.pop_numbers:
    mov r11, rdx
    mov r12, rdx
.loop:
    cmp r12, 0
    je .end
    
    xor rax, rax
    mov al, byte [rsp]
    inc rsp
    mul r13
    add r14, rax

    mov rax, r13
    mul r15
    mov r13, rax

    dec r12
    jmp .loop
.end:
    mov rdx, r11
    mov rax, r14
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    ret

; rdi points to a string
; returns rax: number, rdx : length
parse_int:
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    xor r10, r10
    xor r11, r11
    mov r12, 1
    mov r13, 10

    xor rax, rax
    xor rdx, rdx
.push_stuff:
    cmp byte [rdi], 0
    je .pop_stuff

    mov al, byte[rdi]
    inc rdi

    cmp al, 0x2d
    je .negative
.back:
    cmp al, 0x39
    jg .push_stuff
    cmp al, 0x30
    jl .push_stuff

    inc rdx

    dec rsp
    sub al, 0x30
    mov [rsp], al
    jmp .push_stuff
.pop_stuff:
    add r15, rdx
    add r14, rdx
.loop:
    cmp r15, 0
    je .end

    xor rax, rax
    mov al, byte [rsp]
    inc rsp

    mul r12
    add r11, rax

    mov rax, r12
    mul r13
    mov r12, rax

    dec r15
    jmp .loop
.end:    
    mov rax, r11
    cmp r10, 0
    jne .negate
.back2:
    mov rdx, r14
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    ret 
.negative:
    inc r14
    mov r10, 1
    jmp .back
.negate:
    mov r13, -1
    imul r13
    jmp .back2


string_copy:
    push r11
    push r12
    push r13

    xor r12, r12

    call string_length
    mov r11, rax
    cmp rdx, r11
    jl .return_zero
.copy:
    cmp r11, r12
    jl .end

    mov r13, qword[rdi+r12]
    mov qword[rsi+r12], r13
    inc r12
    
    jmp .copy
.return_zero:
    xor rax, rax
.end:
    pop r13
    pop r12
    pop r11
    ret
