include segments.inc
include render.inc

LOCALS  __

TSR_DATA                segment

public gp_hex2bcd
public gp_lcd_init
public gp_render_char
public gp_led_pos_ptr

gp_hex2bcd              dd      ?
gp_lcd_init             dd      ?
gp_render_char          dd      ?
gp_led_pos_ptr          dd      ?

bak_transparent         db      ?
bak_color_fg            db      ?
bak_color_bg            db      ?

TSR_DATA                ends



TSR_CODE                segment

;*****************************************************************************
;** Purpose:    Save the colors used for rendering. Cannot nest calls.
;**

save_led_render_state   proc    near
                        push    di
                        push    es
                        les     di, cs:gp_led_pos_ptr
                        mov     al, BYTE PTR es:[di + LED_TRANSPARENT]
                        mov     cs:bak_transparent, al
                        mov     al, BYTE PTR es:[di + LED_COLOR_FG]
                        mov     cs:bak_color_fg, al
                        mov     al, BYTE PTR es:[di + LED_COLOR_BG]
                        mov     cs:bak_color_bg, al
                        pop     es
                        pop     di
                        ret
save_led_render_state   endp

;*****************************************************************************
;** Purpose:    Restore the colors used for rendering.
;**

restore_led_render_state proc    near
                        push    di
                        push    es
                        les     di, cs:gp_led_pos_ptr
                        mov     al, cs:bak_color_bg
                        mov     BYTE PTR es:[di + LED_COLOR_BG], al
                        mov     al, cs:bak_color_fg
                        mov     BYTE PTR es:[di + LED_COLOR_FG], al
                        mov     al, cs:bak_transparent
                        mov     BYTE PTR es:[di + LED_TRANSPARENT], al
                        pop     es
                        pop     di
                        ret
restore_led_render_state endp


TSR_CODE                ends


;*****************************************************************************
;** Purpose:    Render a cockpit character at a specific location.
;**
;** Notes  :    BL = character to render
;**             column pixel offset = (AL & 7Fh) * 7 + (CL & 7)
;**             row                 = (AL & 80h) ? bottom row : top row
;**
;**             e.g. bottom left -> AL = 128, CL = 0
;**             number of columns = 88 (7 pixels each)

TSR_CODE                segment

render_cockpit_char     proc    near
                        push    es
                        push    si
                        les     di, cs:gp_led_pos_ptr
                        mov     BYTE PTR es:[di + LED_TRANSPARENT], 0
                        mov     BYTE PTR es:[di + LED_COLOR_FG], COLOR_BLACK
                        mov     BYTE PTR es:[di + LED_COLOR_BG], COLOR_CPGREEN
                        pop     si
                        pop     es
                        call    cs:[gp_lcd_init]
                        mov     al, bl
                        call    cs:[gp_render_char]
                        ret
render_cockpit_char     endp

TSR_CODE                ends


;*****************************************************************************
;** Purpose:    Prepare to render text at a specific location.
;**
;** Notes  :    AL = row index
;**             AH = row sub index
;**             CL = column index (negative aligns right)
;**             CH = column sub index
;**             BL = 0: bg opaque, 80h: bg transparent
;**             DL = fg color
;**             DH = bg color (unused if bg transparent)

TSR_CODE                segment

lcd_init                proc    near
                        cmp     cl, 0
                        jge     __left
                        add     cl, 45
__left:
                        push    es
                        push    si
                        les     di, cs:gp_led_pos_ptr
                        mov     BYTE PTR es:[di + LED_ROWIDX], al
                        mov     BYTE PTR es:[di + LED_ROWSUB], ah
                        mov     BYTE PTR es:[di + LED_COLIDX], cl
                        mov     BYTE PTR es:[di + LED_COLSUB], ch
                        mov     BYTE PTR es:[di + LED_TRANSPARENT], bl
                        mov     BYTE PTR es:[di + LED_COLOR_FG], dl
                        cmp     bl, 80h
                        jz      __end
                        mov     BYTE PTR es:[di + LED_COLOR_BG], dh
__end:
                        pop     si
                        pop     es
                        ret
lcd_init                endp


;** Notes  :    BX = number to render
render_hex_word         proc    near
                        mov     al, bh
                        shr     al, 4
                        call    render_nibble

                        mov     al, bh
                        and     al, 0Fh
                        call    render_nibble

                        mov     al, bl
                        shr     al, 4
                        call    render_nibble

                        mov     al, bl
                        and     al, 0Fh
                        call    render_nibble
                        ret
render_hex_word         endp


;** Notes  :    BL = number to render
render_hex_byte         proc    near
                        mov     al, bl
                        shr     al, 4
                        call    render_nibble

                        mov     al, bl
                        and     al, 0Fh
                        call    render_nibble
                        ret
render_hex_byte         endp


;** Notes  :    BX = number to render
render_dec_word         proc    near
                        push    dx
                        push    ax
                        xor     dx, dx
                        mov     ax, bx
                        call    cs:[gp_hex2bcd]
                        mov     bx, ax
                        pop     ax
                        pop     dx
                        call    render_hex_word
                        ret
render_dec_word         endp


;** Notes  :    BL = number to render
render_dec_byte         proc    near
                        push    dx
                        push    ax
                        xor     dx, dx
                        xor     ax, ax
                        mov     al, bl
                        call    cs:[gp_hex2bcd]
                        mov     bx, ax
                        pop     ax
                        pop     dx
                        call    render_hex_byte
                        ret
render_dec_byte         endp


;** Notes  :    AL = digit to render
render_nibble           proc    near
                        cmp     al, 10
                        jb      __num
                        add     al, 'A' - 10
                        jmp     __render
__num:
                        add     al, '0'
__render:
                        call    cs:[gp_render_char]
                        ret
render_nibble           endp

TSR_CODE                ends

                        end
