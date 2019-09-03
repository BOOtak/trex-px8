PUBLIC px_getk
PUBLIC _px_getk

px_getk:
_px_getk:
    call    const
    cp      a,0
    jz      return
    call conin
.return
    ld      l,a
    ld      h,0
    ret

.conin
    ld      de,6   ; CONIN offset
    jr      console
.const
    ld      de,3   ; CONST offset
    jr      console
.conout
    ld      de,9    ; CONOUT offset
.console
    ld      hl,(1) ; WBOOT (BIOS)
    add     hl,de
    jp      (hl)
