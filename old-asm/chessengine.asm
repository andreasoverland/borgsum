; /usr/local/bin/nasm -f macho64 test.asm && ld -macosx_version_min 10.7.0 -lSystem -o test test.o && ./test

global start


section .text

start:
    call testfunc

    mov     rax, 0x2000004 ; write
    mov     rdi, 1 ; stdout
    mov     rsi, originalBoard
    mov     rdx, originalBoard.len
    syscall

    mov     rax, 0x2000001 ; exit
    mov     rdi, 0
    syscall

testfunc:
    mov     rax, 0x2000004 ; write
    mov     rdi, 1 ; stdout
    mov     rsi, welcomeMessage
    mov     rdx, welcomeMessage.len2
    syscall
    ret

diagToBoard:



    ret

section .data

welcomeMessage:   db      "********** Borgsummer ChessEngine - Assembly Version ************", 10
.len2:   equ     $ - welcomeMessage
originalBoard:    db      "rnbqkbnr pppppppp ........ ........ ........ ........ PPPPPPPP RNBQKBNR",10
.len:   equ     $ - originalBoard
