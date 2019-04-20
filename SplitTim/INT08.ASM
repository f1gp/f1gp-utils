;**
;** File   : int08.asm
;** Author : TK
;** Date   :  8/06/94
;**
;** $Header:   L:/splittim/vcs/int08.as_   1.1   09 Oct 1994 16:40:48   tk  $
;**
;** IRQ 0 tick timer
;**

                name    int08
                title   'IRQ 0 tick timer'

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

old08_hndlr     label   dword           ; IRQ 0 Timer tick
old08_off       dw      ?
old08_seg       dw      ?

TSR_DATA        ends

;****************************************************************************
;**        
;** Our IRQ 0 tick timer
;**        

TSR_CODE        segment

        public  new08_hndlr
new08_hndlr     proc near

        pushf
        call    DWORD PTR cs:old08_hndlr

        ;**
        ;** Decrement delay count
        ;**
        cmp     cs:delay_count, 0
        jz      no_delay
        dec     cs:delay_count
no_delay:

        iret

new08_hndlr     endp

TSR_CODE        ends

;*****************************************************************************

                end


