    org 0000h

    ld a, (0xffbb)
    ld a, l
    ld a, 0xff
    ld l, (hl)
    ld a, (IX - 0x7f) ;not right
    ld a, (IY - 0x7e) ;not right
    ld (IX + 0x1a), a
    ld (IY + 0x1e), a
    ld (hl), 0xfe
    ld (IX + 0x1a), 0xfe
    ld (IY + 0x1a), 0xfe
    ld a, (de)
    ld a, (bc)
    ld (bc), a
    ld (de), a
    ld a, (0x1fae)
    ld (0xf1ea), a
    ld a, i
    ld a, r
    ld r, a
    ld i, a