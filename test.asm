; /usr/local/bin/nasm -f macho64 test.asm && ld -macosx_version_min 10.7.0 -lSystem -o test test.o && ./test

global start


section .text

start:
    mov     rax, 0x2000004 ; write
    mov     rdi, 1 ; stdout
    mov     rsi, msg
    mov     rdx, msg.len
    syscall

    call testfunc

    mov     rax, 0x2000001 ; exit
    mov     rdi, 0
    syscall


testfunc:
    mov rbx,92487922
    mov rcx,92374924
    and rbx,rcx
    ret

section .data

msg:    db      "Hello, world!", 10
.len:   equ     $ - msg
