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



        ; TODO: Keep MOVE_MATRIX in here somewhere

        mov rax,0

directionLoop1:
        mov r8, -1
directionLoop2:
        mov r9, -1

        mov     rbx,rsi ; idx rank in rbx
        shr     rbx,3
        mov     rcx,rsi ; idx file in rcx
        and     rcx,7

directionLoop3:
        mov     r11,8 ; count direction multiplier down from 8

multiplierLoop:

; check if both directions are 0, if so inc r9 and continue

        add rbx,r8
        add rcx,r9
        mov r10,rbx
        or  r10,rcx
        and r10,0FFFFFFFFFFFFFFF8h
        ; jnz skipCheck

        inc rax

        dec r11
        jnz multiplierLoop

skipCheck:

        inc r9
        cmp r9,2
        jne directionLoop3


        inc r8
        cmp r8,2
        jne directionLoop2

        je end
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
