;**
;** File   : gptest.asm
;**
        name    gptest
        title   'Car Performance'

LOCALS __

;*****************************************************************************
;**
;** Include files
;**

;*****************************************************************************
;**
;** Segments
;**

include segments.inc
include gptest.inc
include gptest.ash

;*****************************************************************************
;**
;** Equates & Macros
;**

TICK_TIME_MS    equ     18                      ; Approx ms of a tick
DELAY_TIME      equ     (500 / TICK_TIME_MS)    ; Time to display hook msg

DOS             equ     21h                     ; Dos interrupt call

RED             equ     0030h
GREEN           equ     0040h
BLUE            equ     0050h
YELLOW          equ     0060h

GP_VER_UNK      equ     -1
GP_VER_EUR      equ     0
GP_VER_USA      equ     1
GP_VER_ITA      equ     2

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

err_notify      db      0

game_ds         dw      ?
game_ss         dw      ?

q_filename      db      ?
                db      64 + 13 dup(?)
fh              dw      ?


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
GP_ITA_RET_ADDR equ     0b74ch
GP_USA_RET_ADDR equ     0b727h
MATCH_OFFSET    equ     6

gp_code_eur     db      0B0h, 000h
                db      0B4h, 030h
                db      0CDh, 021h
                db      00Ah, 0C0h
                db      075h, 003h
                db      0B8h, 001h, 000h
                db      036h, 0A3h, 0C6h, 005h
                db      03Ch, 003h
                db      073h, 009h
                db      036h, 0C6h, 006h, 031h, 009h, 007h
                db      0E9h, 067h, 001h
                db      00Eh
                db      0E8h, 062h, 0F7h
                db      00Eh
                db      0E8h, 0EDh, 0F8h
                db      032h, 0C0h
                db      09Ah, 043h, 019h
MATCH_BYTES_EUR equ     $ - gp_code_eur

gp_code_ita     db      0B0h, 000h
                db      0B4h, 030h
                db      0CDh, 021h
                db      00Ah, 0C0h
                db      075h, 003h
                db      0B8h, 001h, 000h
                db      036h, 0A3h, 0C6h, 005h
                db      03Ch, 003h
                db      073h, 009h
                db      036h, 0C6h, 006h, 031h, 009h, 007h
                db      0E9h, 067h, 001h
                db      00Eh
                db      0E8h, 062h, 0F7h
                db      00Eh
                db      0E8h, 0EDh, 0F8h
                db      032h, 0C0h
                db      09Ah, 0ebh, 018h
MATCH_BYTES_ITA equ     $ - gp_code_ita

gp_code_usa     db      0b0h, 00h, 0b4h, 30h, 0cdh, 21h, 0ah, 0c0h, 75h, 03h
MATCH_BYTES_USA equ     $ - gp_code_usa

SEG_COUNT       equ 14
eu_seg_base     dw 0, 0F47h, 19EDh, 1E61h, 2914h, 30CAh, 40C1h, 50BAh, 605Ah, 6BE7h, 7BCEh, 8B6Eh, 8CE6h, 8EAAh, 9151h
us_seg_base     dw 0, 0F47h, 19EDh, 1E5Eh, 2911h, 30C7h, 40BEh, 50B7h, 6057h, 6BE2h, 7BC4h, 8B64h, 8CDCh, 8EA0h, 9147h
it_seg_base     dw 0, 0F47h, 19EDh, 1E5Dh, 2910h, 30C6h, 40BDh, 50B6h, 6056h, 6BE3h, 7BC5h, 8B65h, 8CDDh, 8EA1h, 9148h
eu_to_us        dw 2, 00179h, -24h
                dw 2, 00C25h, -08h
                dw 8, 0A04Eh, -09h
                dw 8, 0A0D2h, -09h
                dw 8, 0A248h, -09h
                dw 8, 0A46Ch, -01h
                dw 8, 0A4A0h, -09h
                dw 9, 00576h, -4Eh
                dw 9, 005A6h, -0Ah
                dw 9, 07932h,  01h
                dw SEG_COUNT, 0, 0
eu_to_it        dw 2, 00179h, -2Ch
                dw 2, 00C25h, -08h
                dw 9, 00576h, -4Eh
                dw 9, 005A6h, -0Ah
                dw 9, 07932h,  01h
                dw SEG_COUNT, 0, 0
offset_diffs    dw eu_to_us, eu_to_it


;*****************************************************************************

gp_ver          db      GP_VER_UNK

hooked_msg      db      CR, LF, "GpTest activated OK", CR, LF, '$'

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
        jz      __start
        jmp     do_orig_int21

__start:
        pushf
        pusha
        push    ds
        push    es
        PUSHED_WORDS equ (1 + 8 + 1 + 1)

        mov     ax, cs
        mov     ds, ax

        mov     game_ds, 0
        mov     game_ss, ss

        ;**
        ;** Get return address
        ;**
        mov     di, sp
        les     di, ss:[di + (PUSHED_WORDS * 2)]

        mov     gp_ver, GP_VER_UNK

        ;**
        ;** Is it European GP.EXE calling us?
        ;**
        cmp     di, GP_EUR_RET_ADDR
        jnz     try_us
        sub     di, MATCH_OFFSET
        mov     bx, di
        mov     si, OFFSET TSR_CODE:gp_code_eur
        mov     cx, MATCH_BYTES_EUR
        cld
        repe    cmpsb
        jz      its_eur

        mov     di, bx
        mov     si, OFFSET TSR_CODE:gp_code_ita
        mov     cx, MATCH_BYTES_ITA
        cld
        repe    cmpsb
        jnz     no_match
its_ita:
        mov     gp_ver, GP_VER_ITA
        jmp     got_match

its_eur:
        mov     gp_ver, GP_VER_EUR
        jmp     got_match

        ;**
        ;** Is it US GP.EXE calling us?
        ;**
try_us: cmp     di, GP_USA_RET_ADDR
        jnz     no_match
        sub     di, MATCH_OFFSET
        mov     si, OFFSET TSR_CODE:gp_code_usa
        mov     cx, MATCH_BYTES_USA
        cld
        repe    cmpsb
        jnz     no_match

        mov     gp_ver, GP_VER_USA

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

TSR_CODE        segment

        public  hook_gp_exe
hook_gp_exe proc near

        mov     cs:err_notify, 0

        mov     si, OFFSET TSR:hook_random_seed_start
        push    2
        push    0C45h
        call    calc_offset_esdi
        mov     cx, (hook_random_seed_end - hook_random_seed_start)
__loop1:
        lodsb
        stosb
        loop    __loop1

        ret

hook_gp_exe endp

TSR_CODE        ends



;*****************************************************************************
;** Purpose:    Calculate memory offset inside the game.
;**
;** Returns:    dx:ax -> translated memory address of actual exe version.
;**
;** Notes  :    AX = eu memory offset.
;**             DX = segnr (0...13).

TSR_CODE        segment

calc_offset     proc near
                push si
                push di
                push bx
                push cx
                push ds

                mov cx, cs
                mov ds, cx
                mov cx, dx
                mov bx, ax
                mov si, OFFSET TSR:eu_seg_base
                xor ax, ax
                mov al, gp_ver
                mov dx, ((SEG_COUNT + 1) * 2)
                imul dx
                add si, ax

                ; set dx to runtime value of seg000
                mov dx, game_ss
                sub dx, [si + 4 * 2]

                add si, cx
                add si, cx
                add dx, [si]

                xor ax, ax
                mov al, gp_ver
                cmp ax, GP_VER_EUR
                ja __not_eu
                mov ax, bx
                jmp __end
__not_eu:
                dec ax
                shl ax, 1
                mov si, OFFSET TSR:offset_diffs
                add si, ax
                mov ax, bx
__loop:
                mov di, [si]   ; seg nr
                cmp di, cx
                ja __end
                jne __cont

                mov di, [si+2] ; seg offset
                cmp di, bx
                ja __end

                mov di, [si+4] ; diff
                add ax, di
__cont:
                add si, 6
                jmp __loop
__end:
                pop ds
                pop cx
                pop bx
                pop di
                pop si
                retn

calc_offset     endp


calc_offset_esdi proc near

arg_offset      = word ptr  4
arg_segnr       = word ptr  6

                push    bp
                mov     bp, sp
                push    ax
                push    dx

                mov     dx, [bp+arg_segnr]
                mov     ax, [bp+arg_offset]
                call    calc_offset
                mov     es, dx
                mov     di, ax

                pop     dx
                pop     ax
                pop     bp
                retn    (2 * 2)

calc_offset_esdi endp

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
;** Purpose: Disables setting a random seed from the timer.
;**

TSR_DATA        segment

hook_random_seed_start: db     09ah
hook_random_seed_off    dw     ?
hook_random_seed_seg    dw     ?
hook_random_seed_end:

TSR_DATA        ends

TSR_CODE        segment

        public  hook_random_seed
hook_random_seed proc far
        retf

hook_random_seed endp

TSR_CODE        ends


;****************************************************************************

        end
