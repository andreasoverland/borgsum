; -----------------------------------------------------------------------------
; A 64-bit function that calculates the influence map for a square on the board
;
;   int64_t _influenceMapForSquare(int64_t[] board, int32_t idx)
;
; -----------------------------------------------------------------------------

Pieces_PpKk equ 1 | 64 | 32 | 2048
Pieces_Nn   equ   4 | 256
Piece_Test  equ  4096

        global  _influenceMapForSquare2

        section .text
_influenceMapForSquare2:

        push rbx
        push rdi
        push r12
        push r11
        push r13

        lea rdx,[rel COUNTER]
        inc qword [rdx]

clearMap:
        mov     qword [rdi+74*8],0
        mov     qword [rdi+74*8+8],0
        mov     qword [rdi+74*8+16],0
        mov     qword [rdi+74*8+24],0
        mov     qword [rdi+74*8+32],0
        mov     qword [rdi+74*8+40],0
        mov     qword [rdi+74*8+48],0
        mov     qword [rdi+74*8+56],0
        mov     qword [rdi+74*8+64],0
        mov     qword [rdi+74*8+72],0
        mov     qword [rdi+74*8+80],0
        mov     qword [rdi+74*8+88],0
        mov     qword [rdi+74*8+96],0
        mov     qword [rdi+74*8+104],0
        mov     qword [rdi+74*8+112],0
        mov     qword [rdi+74*8+120],0

directionLoop1: ; vi skal aldri hoppe hit egentlig.
        mov r13, 0
        mov r8, -1 ; for r8 = -1 .. 1
directionLoop2:
        mov r9, -1 ; for r9 = -1 .. 1

directionLoop3:

        mov   r12,r8
        or    r12,r9
        jnz   notZero   ; hvis r9 og r8 == 0, hopp til nextR9
        dec   r13
        jmp   nextR9
notZero:
        mov     rbx,rsi ; idx rank in rbx
        shr     rbx,3
        mov     rcx,rsi ; idx file in rcx
        and     rcx,7

        mov     r11,0 ; count direction multiplier up to 8

multiplierLoop:

        add rbx,r8    ; is rank or file smaller than 0 or bigger than 7
        add rcx,r9
        mov r10,rbx
        or  r10,rcx
        and r10,-8h

        jnz nextR9 ; if so, jump to next direction

theInnerLoop:

        mov r10,rbx ; move rank into r10
        shl r10,3   ; multiply by 8
        or  r10,rcx ; add file into r10
        mov rax,[rdi+r10*8] ; move piece at index r10 into rax
        cmp rax,0 ; is it empty ?
        jz nextMultiplier ; if piece == 0, go further

        and rax, Pieces_PpKk
        jz notPpKk
        cmp r11,1
        jge nextR9

notPpKk:
        mov rax,[rdi+r10*8] ; returns piece at position rsi
        and rax, Pieces_Nn
        jz setAndNextDirection
        jmp nextR9

setAndNextDirection:
        mov rax,[rdi+r10*8]
        ; mov qword [rdi+r10*8], Piece_Test ; for debugging
        mov qword [rdi+74*8+r13*8],rax; flytt rax inn i riktig plass i map
        jmp nextR9

nextMultiplier:
        inc r11
        cmp r11,8
        jl multiplierLoop
nextR9:
        inc r13
        inc r9
        cmp r9,2
        jl directionLoop3

        inc r8
        cmp r8,2
        jl directionLoop2

;; ------------------------- KNIGHTS ------------------------

checkForKnights:
        lea rdx,[rel KNIGHT_MOVE_MATRIX]
        mov r12,0
        mov r8,0
        mov r13,8
knightLoop:
        mov r8,[rdx+r12*8] ; rank-offset
        inc r12
        mov r9,[rdx+r12*8] ; file-offset
        inc r12

        mov     rbx,rsi ; idx rank in rbx
        shr     rbx,3
        mov     rcx,rsi ; idx file in rcx
        and     rcx,7

        add rbx,r8    ; add offsets to rank&file
        add rcx,r9

        mov r10,rbx
        or  r10,rcx
        and r10,-8h
        jnz knightIndexOutOfBoard ; if so, jump to next direction

        mov r10,rbx ; move rank into r10
        shl r10,3   ; multiply by 8
        or  r10,rcx ; add file into r10
        mov rax,[rdi+r10*8] ; move piece at index r10 into rax
        ; mov qword [rdi+r10*8],Piece_Test

        and rax, Pieces_Nn ; is piece at position a knight?
        jz noKnightOnIndex

        ; yes a knight was there
        mov rax,[rdi+r10*8]
        mov [rdi+74*8+r13*8],rax
        mov rax, r10

noKnightOnIndex:
knightIndexOutOfBoard:
        inc r13
        cmp r13,16
        jl knightLoop

end:
        lea rdx,[rel COUNTER]
        mov rax,[rdx]

        pop r13
        pop r11
        pop r12
        pop rdi
        pop rbx

        ret

section .data
; MOVE_MATRIX         dq -1, -1, 0, -1, 1, -1,-1, 0, 1,  0, -1,  1, 0,  1, 1,  1
KNIGHT_MOVE_MATRIX  dq -1, -2, -1, 2, -2, -1, -2, 1, 1, -2,  1, 2,  2, -1,  2, 1
COUNTER             dq 0
