; -----------------------------------------------------------------------------
; A 64-bit program that displays its command line arguments, one per line.
;
; On entry, rdi will contain argc and rsi will contain argv.
; nasm -fmacho64 echo.asm && gcc echo.o && lldb a.out
; nasm -fmacho64 echo.asm && gcc echo.o && ./a.out
; -----------------------------------------------------------------------------

        global  _main

        section .text
_main:
beginning:
        push    rdi                     ; save registers that puts uses
        push    rsi

        mov rax,10
        mov rbx,0
        add rax,rbx
        js signed
        mov rax,0

signed:

        pop     rsi                     ; restore registers puts used
        pop     rdi



        ret
