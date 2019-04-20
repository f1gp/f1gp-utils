;**
;** File   : int11.asm
;** Author : TK
;** Date   :  8/06/94
;**
;** $Header:   L:/splittim/vcs/int11.as_   1.1   09 Oct 1994 16:40:48   tk  $
;**
;** INT 11H ROM BIOS equipment list
;**

                name    int11
                title   'INT 11H ROM BIOS equipment list'

;*****************************************************************************
;**
;** Segments
;**

include segments.inc
include splittim.inc

;*****************************************************************************
;**
;** Equates & Macros
;**

;*****************************************************************************
;**
;** Data
;**

TSR_DATA        segment

old11_hndlr     label   dword           ; ROM BIOS equipment list
old11_off       dw      ?
old11_seg       dw      ?

TSR_DATA        ends

;****************************************************************************
;** 
;** Our ROM BIOS equipment list int
;** 

TSR_CODE        segment

        public  new11_hndlr
new11_hndlr     proc near

        cmp     cx, MAGIC_CX_IN
        jne     I11_1
        cmp     dx, MAGIC_DX_IN
        jne     I11_1
        mov     cx, MAGIC_CX_OUT
        mov     dx, MAGIC_DX_OUT
        mov     bx, cs:psp_seg

        push    bx
        push    cx
        push    dx
        pushf                                   ; Simulate int
        call    DWORD PTR cs:old11_hndlr
        pop     dx
        pop     cx
        pop     bx
        iret

I11_1:  pushf                                   ; Simulate int
        call    DWORD PTR cs:old11_hndlr
        iret

new11_hndlr     endp

TSR_CODE        ends

;*****************************************************************************

                end


