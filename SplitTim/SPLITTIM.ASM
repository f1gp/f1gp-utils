;**
;** File   : splittim.asm
;** Author : TK 
;** Date   :  6/06/94
;**
        name    splittim
        title   'Split Section Timer'

;*****************************************************************************
;**
;** Include files
;**

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

TICK_TIME_MS    equ     18                      ; Approx ms of a tick
DELAY_TIME      equ     (500 / TICK_TIME_MS)    ; Time to display hook msg

DIF_SEG         equ     466dh
DIF_SEG_MOD_EUR equ     (DIF_SEG - 0000h)
DIF_SEG_MOD_US  equ     (DIF_SEG - 0003h)

FUNC1_OFF_EUR   equ     14c0h
FUNC2_OFF_EUR   equ     15b6h
FUNC1_OFF_US    equ     1494h
FUNC2_OFF_US    equ     158ah

;*****************************************************************************
;**
;** Externals.
;**

TSR_CODE        segment
                extrn   install:far
TSR_CODE        ends

;*****************************************************************************
;**
;** Data
;**

TSR_DATA        segment

psp_env         label   dword
psp_env_off     dw      002ch           ; Offset of env pointer in PSP
psp_seg         dw      ?               ; PSP segment

old21_hndlr     label   dword           ; DOS Int 21 handler
old21_off       dw      ?
old21_seg       dw      ?

delay_count     db      0
dif_seg         dw      ?
func1_off       dw      ?
func2_off       dw      ?

split1          dw      25
split2          dw      50
split3          dw      75
freeze_time     dw      10000
track_divs      dw      ?

;*****************************************************************************
;** 
;** European V1.05 GP.EXE - DOS Version code fragment
;** 
;** 92D2:5C76  B0 00 mov	al,0
;** 92D2:5C78  B4 30 mov	ah,30h
;** 92D2:5C7A  CD 21 int	21h	        @06d2eaH in GPU.EXE
;** 92D2:5C7C  0A C0 or	        al,al	
;** 92D2:5C7E  75 03 jnz	loc_5593
;** 

GP_EUR_RET_ADDR equ     0b74ch
GP_US_RET_ADDR  equ     0b727h
MATCH_BYTES     equ     10
MATCH_OFFSET    equ     6

gp_code         db      0b0h, 00h, 0b4h, 30h, 0cdh, 21h, 0ah, 0c0h, 75h, 03h
        
;*****************************************************************************

game_eur        db      ?

hooked_msg      db      CR, LF, "SplitTim activated OK", CR, LF, '$'

TSR_DATA        ends

;****************************************************************************
;** 
;** Our DOS Int 21 handler
;** 

TSR_CODE        segment

        public  new21_hndlr
new21_hndlr     proc near

        ;**
        ;** Is it DOS Get Version?
        ;** 
        cmp     ah, 30h
        jz      push_start
        jmp     do_orig_int21

        PUSHA_SIZE equ (8 - 1)
push_start:
        pushf
        pusha
        push    ds
        push    es
push_end:

        mov     ax, cs
        mov     ds, ax
        cld

        ;**
        ;** Get return address
        ;**
        mov     di, sp
        les     di, ss:[di + ((PUSHA_SIZE + push_end - push_start) * 2)]

        mov     si, OFFSET TSR:gp_code
        mov     cx, MATCH_BYTES

        ;**
        ;** Is it European GP.EXE calling us?
        ;**
        cmp     di, GP_EUR_RET_ADDR
        jnz     try_us
        sub     di, MATCH_OFFSET
        repe    cmpsb
        jnz     no_match

        mov     game_eur, 1
        mov     dif_seg, DIF_SEG_MOD_EUR
        mov     WORD PTR cs:[fo1], FUNC1_OFF_EUR
        mov     WORD PTR cs:[fo2], FUNC2_OFF_EUR
        jmp     got_match

        ;**
        ;** Is it US GP.EXE calling us?
        ;**
try_us: cmp     di, GP_US_RET_ADDR
        jnz     no_match
        sub     di, MATCH_OFFSET
        repe    cmpsb

        mov     game_eur, 0
        mov     dif_seg, DIF_SEG_MOD_US
        mov     WORD PTR cs:[fo1], FUNC1_OFF_US
        mov     WORD PTR cs:[fo2], FUNC2_OFF_US

got_match:
        call    announce_hook
        call    hook_gp_exe

no_match:
        pop     es
        pop     ds
        popa
        popf

do_orig_int21:
        ;**
        ;** Perform normal DOS call.
        ;**
        jmp     dword ptr cs:old21_hndlr

new21_hndlr     endp

TSR_CODE        ends

;*****************************************************************************
;** Purpose: Hook our code.
;**
;** Returns: Nothing.
;**
;** Notes  : 
;**

TSR_CODE        segment

        public  hook_gp_exe
hook_gp_exe proc near

        mov     cx, HOOK1_EUR_MSW 
        mov     ax, HOOK1_EUR_LSW
        mov     dx, HOOK1_US_MSW
        mov     bx, HOOK1_US_LSW
        call    get_hook_offset
        mov     si, OFFSET cs:hook_code_start
        mov     cx, HOOK_LENGTH
        call    copy_patch_block

        mov     cx, HOOK2_EUR_MSW 
        mov     ax, HOOK2_EUR_LSW
        mov     dx, HOOK2_US_MSW
        mov     bx, HOOK2_US_LSW
        call    get_hook_offset
        mov     si, OFFSET cs:hook_code_start
        mov     cx, HOOK_LENGTH
        call    copy_patch_block

        ret

hook_gp_exe endp

TSR_CODE        ends

;*****************************************************************************
;** Purpose:    Determines which hook address to use, European or US.
;**
;** Returns:    DX:BX = Hook
;**
;** Notes  :    CX:AX = European hook.
;**             DX:BX = US hook.
;**

TSR_CODE        segment

                public get_hook_offset
get_hook_offset proc near

        cmp     game_eur, 1
        jnz     get_hook_offset_do_us

        mov     bx, ax
        mov     dx, cx
get_hook_offset_do_us:
        ret

get_hook_offset endp

TSR_CODE        ends

;*****************************************************************************
;** Purpose:    Copy patch block to correct destination.
;**
;** Returns:    Nothing.
;**
;** Notes  :    SI = offset of local code fragment.
;**             DX = Hook segment
;**             BX = Hook offset
;**             CX = Hook block length.

TSR_CODE        segment

                public copy_patch_block
copy_patch_block proc near

        mov     di, sp
        les     di, ss:[di + ((PUSHA_SIZE + push_end - push_start) * 2) + 2 + 2]
        add     bx, 2
        sub     di, bx
        mov     ax, es
        sub     ax, dx
        mov     es, ax
copy_patch_block1:
        lodsb
        stosb
        loop    copy_patch_block1

        ;**
        ;** Calculate segment offset to far functions we want to call.
        ;**
        mov     di, sp
        les     di, ss:[di + ((PUSHA_SIZE + push_end - push_start) * 2) + 2 + 2]
        mov     ax, es
        sub     ax, dif_seg

        mov     WORD PTR cs:[fs1], ax
        mov     WORD PTR cs:[fs2], ax
        ret

copy_patch_block endp

TSR_CODE        ends

;*****************************************************************************
;** Purpose:    Show user that we've hooked in OK.
;**
;** Returns:    Nothing.
;**
;** Notes  : 
;**

TSR_CODE        segment

        public  announce_hook
announce_hook proc near
        mov     dx, OFFSET TSR:hooked_msg
        mov     ah, 09h
        int     DOS
        sti
        mov     delay_count, DELAY_TIME
do_delay:        
        cmp     delay_count, 0
        jnz     do_delay
        ret

announce_hook endp

TSR_CODE        ends

;*****************************************************************************
;** Purpose:    Code executed as each car crosses s/f line.
;**
;** Returns:    Nothing.
;**
;** Notes  : 
;**
;** GPU.EXE:
;**
;** Player?
;** 002D17  C47C12         les    di,[si+12]
;** 002D1A  E80700         call   002D24
;** 002D1D  897C12         mov    [si+12],di                    <----
;** 002D20  8C4414         mov    [si+14],es
;** 002D23  C3             ret
;**
;** CC?
;** 007CC6  B81000         mov    ax,10
;** 007CC9  894416         mov    [si+16],ax
;** 007CCC  897C12         mov    [si+12],di                    <----
;** 007CCF  8C4414         mov    [si+14],es
;** 007CD2  894C1C         mov    [si+1C],cx
;** 007CD5  8B541C         mov    dx,[si+1C]
;**
;** (DOS VER hook point = 06d2eaH in GPU.EXE)
;** (DOS VER hook point = 06d295H in GPU.EXE)
;**

HOOK1_EUR_MSW   equ     6000h
HOOK1_EUR_LSW   equ     (0d2eah - 7CCCh)
HOOK1_US_MSW    equ     6000h
HOOK1_US_LSW    equ     (0d295h - 7CCCh)        ;/*<TK>*/ Check!

HOOK2_EUR_MSW   equ     6000h
HOOK2_EUR_LSW   equ     (0d2eah - 2D1Dh)
HOOK2_US_MSW    equ     6000h
HOOK2_US_LSW    equ     (0d295h - 2D1Dh)        ;/*<TK>*/ Check!

TSR_DATA                segment

hook_code_start:
                        nop
                        db     09ah
hook_code_off           dw     ?
hook_code_seg           dw     ?
hook_code_end:
HOOK_LENGTH             equ     (hook_code_end - hook_code_start)

last_value              dw      0

TSR_DATA                ends

TSR_CODE                segment

h3:     jmp     h1

        public  hook
hook:
        pushf
        pusha
        push    ds
        push    es
        cld

        ;**
        ;** Is it the currently displayed car?
        ;**
        cmp     si, ds:[097dh]
        jne     h3

        ;**
        ;** Ignore if this is a race.
        ;**
        test    byte ptr ss:[124ah], 80h
        jnz     h3

        ;**
        ;** Are we still on same section of track?
        ;**
        mov     ax, [si+12h] 
        cmp     ax, cs:last_value
        jne     h4
        jmp     h1
h4:     
        mov     cs:last_value, ax

        ;**
        ;** Ignore if this an out lap
        ;**
        test    ds:[si+56h], 0c000h
        jnz     h3

        ;**
        ;** Are we at split section time point?
        ;**
        mov     ax, ss:[015ch]
        sub     ax, 0030h
        mov     bx, 002eh
        mov     di, 100
        xor     dx, dx
        div     bx
        mov     cs:track_divs,ax

        imul    cs:split1
        div     di
        imul    bx
        add     ax, 0030h
        cmp     ax, [si+12h]
        je      h2

        mov     ax, cs:track_divs
        imul    cs:split2
        div     di
        imul    bx
        add     ax, 0030h
        cmp     ax, [si+12h]
        je      h2

        mov     ax, cs:track_divs
        imul    cs:split3
        div     di
        imul    bx
        add     ax, 0030h
        cmp     ax, [si+12h]
        jne     h1

        ;**
        ;** Hold lap-time display.
        ;**
h2:     push    ds:[294fh]      ; Game's main timer
        pop     ds:[001ah]
        push    ds:[2951h]
        pop     ds:[001ch]
        mov     cx, cs:freeze_time
        add     word ptr ds:[001ah], cx
        adc     word ptr ds:[001ch], 0

        mov     al, 19h
        mov     cl, 02h
        ;**
        ;** call ????:???? (func1)
        ;**
        db      9ah
fo1:    dw      ?
fs1:    dw      ?

        ;**
        ;** Display elasped section time.
        ;**
        mov     ax, ds:[294fh]
        mov     dx, ds:[2951h]
        sub     ax, ds:[si+54h]
        sbb     dx, ds:[si+56h]

        ;**
        ;** call ????:???? (func2)
        ;**
        db      9ah
fo2:    dw      ?
fs2:    dw      ?

h1:     pop     es
        pop     ds
        popa
        popf

        ;**
        ;** Original code
        ;**
        mov    [si+12h],di
        mov    [si+14h],es

        retf

TSR_CODE        ends

;****************************************************************************

        end     


