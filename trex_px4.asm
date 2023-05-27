org 100h

; Constants
RBIOS1      equ 00000h
RBIOS2      equ 0EB03h

; BIOS calls
CONIN       equ 06h
CONOUT      equ 09h

; Starting address of the VRAM
; Should be 8000h or higher and should be in 2k boundary
LSCRVRAM    equ 0F294h
; VRAM Y offset (0 .. 63)
LVRAMYOF    equ 0F2ADh

; XUSRSCRN
; XSYSSCRN

; Character codes
ESC         equ 01Bh
ASTERISK    equ 02Ah
TWO         equ 032h
THREE       equ 033h

start:
    ; disable cursor
    ld a, CONOUT
    ld c, ESC
    call bios_call
    ld a, CONOUT
    ld c, TWO
    call bios_call

    ; clear screen
    ld a, CONOUT
    ld c, ESC
    call bios_call
    ld a, CONOUT
    ld c, ASTERISK
    call bios_call

    ; draw sprite
    ld hl, sprite
    ld ix, (LSCRVRAM)   ; y coord
    ld de, 32           ; y offset
    ld c, 23            ; sprite height
again:
    ld a, (hl)
    ld (ix+0), a
    inc hl
    ld a, (hl)
    ld (ix+1), a
    inc hl
    ld a, (hl)
    ld (ix+2), a
    inc hl
    add ix, de

    dec c
    jr nz, again

    ; wait for user input
    ld a, CONIN
    call bios_call

    ; clear screen
    ld a, CONOUT
    ld c, ESC
    call bios_call
    ld a, CONOUT
    ld c, ASTERISK
    call bios_call

    ; enable cursor
    ld a, CONOUT
    ld c, ESC
    call bios_call
    ld a, CONOUT
    ld c, THREE
    call bios_call
    ret

; Bios call routine
; A: Bios function number * 3
; return: Depending on BIOS function
; Caution: ROM executable programs should use RBIOS2 instead of RBIOS1
bios_call:
    push hl
    push de
    ld hl, (RBIOS1+1)
    ld e, a
    ld d, 00h
    add hl, de
    push hl
    pop iy
    pop de
    pop hl
    jp (iy)

sprite:
    defb    %"--------", %"--------", %"--------"
    defb    %"--------", %"-------#", %"#######-"
    defb    %"--------", %"------##", %"-#######"
    defb    %"--------", %"------##", %"########"
    defb    %"--------", %"------##", %"########"
    defb    %"--------", %"------##", %"########"
    defb    %"--------", %"------##", %"###-----"
    defb    %"--------", %"------##", %"######--"
    defb    %"----#---", %"-----###", %"##------"
    defb    %"----#---", %"---#####", %"##------"
    defb    %"----##--", %"--######", %"####----"
    defb    %"----###-", %"-#######", %"##-#----"
    defb    %"----####", %"########", %"##------"
    defb    %"----####", %"########", %"##------"
    defb    %"-----###", %"########", %"#-------"
    defb    %"------##", %"########", %"#-------"
    defb    %"-------#", %"########", %"--------"
    defb    %"--------", %"#######-", %"--------"
    defb    %"--------", %"-###-##-", %"--------"
    defb    %"--------", %"-##---#-", %"--------"
    defb    %"--------", %"-#----#-", %"--------"
    defb    %"--------", %"-##---##", %"--------"
    defb    %"--------", %"--------", %"--------"
