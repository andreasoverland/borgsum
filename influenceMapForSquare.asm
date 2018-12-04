; -----------------------------------------------------------------------------
; A 64-bit function that calculates the influence map for a square on the board
;
;   int64_t _influenceMapForSquare(int64_t[] board, int32_t idx)
;
; Note that the parameters have already been passed in rdi, rsi, and rdx.  We
; just have to return the value in rax.
; -----------------------------------------------------------------------------

        global  _influenceMapForSquare2
        section .text
_influenceMapForSquare2:
        mov     rax,[rdi+rsi*8] ; returns piece at position rsi
        add     rdi,74*8
        mov     rbx,16

clearMap:
        mov     qword [rdi+rbx],0
        dec     rbx
        jnz     clearMap

        mov     rbx,rsi ; idx rank in rbx
        shr     rbx,3
        mov     rcx,rsi ; idx file in rcx
        and     rcx,7


        ; TODO: Keep MOVE_MATRIX in here somewhere

directionLoop1:
        mov r8, -1
directionLoop2:
        mov r9, -1
multiplierLoop:
        add rbx,r8
        add rcx,r9

        mov r10,rbx
        or  r10,rcx
        and r10,0FFFFFFFFFFFFFFF8h
        jnz breakMultiplierLoop

        inc r9
        cmp r9,2
        jne multiplierLoop

        cmp r8,0
        jne cont
        cmp r9,0
        inc r9
        jmp multiplierLoop
cont:
        inc r8
        cmp r8,2
        jne directionLoop2
end:
        ret

breakMultiplierLoop:
        mov rax,10
        jmp end

        section .data
MOVE_MATRIX: db -1, -1, 0, -1, 1, -1,-1, 0, 1,  0, -1,  1, 0,  1, 1,  1
KNIGHT_MOVE_MATRIX : db -1, -2, -1, 2, -2, -1, -2, 1, 1, -2,  1, 2,  2, -1,  2, 1
