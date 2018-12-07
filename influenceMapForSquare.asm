; -----------------------------------------------------------------------------
; A 64-bit function that calculates the influence map for a square on the board
;
;   int64_t _influenceMapForSquare(int64_t[] board, int32_t idx)
;
; -----------------------------------------------------------------------------

        global  _influenceMapForSquare2
        section .text
_influenceMapForSquare2:

        push rbx
        push rcx
        push rdi

      ;  mov     rax,[rdi+rsi*8] ; returns piece at position rsi
        ; add     rdi,74*8 ; point to influence map work space
        mov     rbx,16 ; prepare to clear 16 longs on adress

clearMap:
        mov     qword [rdi+74*8+rbx],0
        dec     rbx
        jnz     clearMap ; continue clearing, until rbx == 0

        ; calculate row and column based on index
        mov     rbx,rsi ; idx rank in rbx
        shr     rbx,3
        mov     rcx,rsi ; idx file in rcx
        and     rcx,7

        ; TODO: Keep MOVE_MATRIX in here somewhere

directionLoop1:
        mov r8, -1
directionLoop2:
        add rbx,r8
        mov r9, -1
multiplierLoop:
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


cont:   ; rank&file are ok
        ; do the check
        ; calculate index with new row and column
        mov r10,rbx
        shl r10,3
        or  r10,rcx
        mov rax,r10

        inc r8
        cmp r8,2
        jne directionLoop2
end:

        pop rdi
        pop rcx
        pop rbx

        ret

breakMultiplierLoop:
        jmp end

        section .data
MOVE_MATRIX: db -1, -1, 0, -1, 1, -1,-1, 0, 1,  0, -1,  1, 0,  1, 1,  1
KNIGHT_MOVE_MATRIX : db -1, -2, -1, 2, -2, -1, -2, 1, 1, -2,  1, 2,  2, -1,  2, 1
