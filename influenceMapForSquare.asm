; -----------------------------------------------------------------------------
; A 64-bit function that calculates the influence map for a square on the board
;
;   int64_t _influenceMapForSquare(int64_t[] board, int32_t idx)
;
; -----------------------------------------------------------------------------

Pieces_PpKk equ 1 | 64 | 32 | 2048
Pieces_Nn equ   4 | 256


        global  _influenceMapForSquare2
        section .text
_influenceMapForSquare2:

        push rbx
        push rdi
        push r12

      ;  mov     rax,[rdi+rsi*8] ; returns piece at position rsi
        ;add     rdi,74*8 ; point to influence map work space
        mov     rbx,16 ; prepare to clear 16 longs on adress
        ;add     rdi,73*8 ; why 73 and not 74??

clearMap:
        dec     rbx
        mov     qword [rdi+74*8+rbx*8],0 ; kanskje xor,xor
        jnz     clearMap ; continue clearing, until rbx == 0

        ; TODO: Keep MOVE_MATRIX in here somewhere

        mov rax,[rdi+rsi*8]

directionLoop1: ; vi skal aldri hoppe hit egentlig.
        mov r8, -1 ; for r8 = -1 .. 1
directionLoop2:
        mov r9, -1 ; for r9 = -1 .. 1
directionLoop3:

        ; hvis r9 og r8 == 0, hopp til directionLoop2
        mov   r12,r8
        or    r12,r9
        jz    nextR9

        mov     rbx,rsi ; idx rank in rbx
        shr     rbx,3
        mov     rcx,rsi ; idx file in rcx
        and     rcx,7

        mov     r11,0 ; count direction multiplier down from 8

multiplierLoop:

        add rbx,r8    ; is rank or file smaller than 0 or bigger than 7
        add rcx,r9
        mov r10,rbx
        or  r10,rcx
        and r10,-8h

        jnz nextR9 ; if so, jump to next direction

theInnerLoop:

        mov r10,rbx
        shl r10,3
        or  r10,rcx
        mov rax,[rdi+r10*8] ; returns piece at r10
        cmp rax,0
        jz nextMultiplier ; if piece == 0, go further

        and rax, Pieces_PpKk
        jz notPpKk
        cmp r11,1
        jg nextR9

notPpKk:
        mov rax,[rdi+r10*8] ; returns piece at position rsi
        and rax, Pieces_Nn
        jz setAndNextDirection
        jmp nextMultiplier

setAndNextDirection:
        mov rax,[rdi+r10*8]
        mov qword [rdi+74*8+r11*8],rax; flytt rax inn i riktig plass i m
        jmp nextR9

nextMultiplier:
        inc r11
        cmp r11,8
        jl multiplierLoop
nextR9:
        inc r9
        cmp r9,2
        jl directionLoop3

        inc r8
        cmp r8,2
        jl directionLoop2
end:

        ; do knight-checks

        sub rdi,74*8
        pop r12
        pop rdi
        pop rbx

        ret


        section .data
; MOVE_MATRIX: db -1, -1, 0, -1, 1, -1,-1, 0, 1,  0, -1,  1, 0,  1, 1,  1
;KNIGHT_MOVE_MATRIX : db -1, -2, -1, 2, -2, -1, -2, 1, 1, -2,  1, 2,  2, -1,  2, 1
