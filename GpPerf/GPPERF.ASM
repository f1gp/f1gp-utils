;**
;** File   : gpperf.asm
;** Author : TK
;** Date   :  6/06/94
;**
        name    gpperf
        title   'Car Performance'

;*****************************************************************************
;**
;** Include files
;**

;*****************************************************************************
;**
;** Segments
;**

include segments.inc
include gpperf.inc
include gpperf.ash

;*****************************************************************************
;**
;** Equates & Macros
;**

TICK_TIME_MS    equ     18                      ; Approx ms of a tick
DELAY_TIME      equ     (500 / TICK_TIME_MS)    ; Time to display hook msg

SAVGAM_OFF_EUR  equ     0030h
SAVGAM_OFF_ITA  equ     -10h
SAVGAM_OFF_USA  equ     0000h

DOS             equ     21h                     ; Dos interrupt call

RED             equ     0030h
GREEN           equ     0040h
BLUE            equ     0050h
YELLOW          equ     0060h

;*****************************************************************************
;**
;** Externals.
;**

TSR_CODE        segment

                extrn   install:far
                extrn   LogEntry:near

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
beep_count      db      0
always_enabled  db      0
enabled         db      0
                public  pending_enable
pending_enable  db      1
                public  log_laps
log_laps        dw      1
log_period      dw      1
last_seg_id     dw      0ffffh

                public  rev_limit1
                public  rev_limit2
                public  rev_limit3
rev_limit1      dw      10000
rev_limit2      dw      11500
rev_limit3      dw      13000
force_led       db      1

                public  show_tyres
show_seg        db      0
show_tyres      db      1

                public  log_car_num
log_car_num     dw      0

err_notify      db      0

game_ds         dw      ?
game_ss         dw      ?

q_filename      db      ?
                db      64 + 13 dup(?)
fh              dw      ?

pe              PERFORMANCE_ENTRY       ?
pe_size         equ     $  - OFFSET pe

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

;*****************************************************************************

game_sg_off     dw      ?

game_eur        db      0
game_usa        db      0
game_ita        db      0

hooked_msg      db      CR, LF, "GpPerf activated OK", CR, LF, '$'

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

        mov     game_ds, 0
        mov     game_ss, ss

        ;**
        ;** Get return address
        ;**
        mov     di, sp
        les     di, ss:[di + ((PUSHA_SIZE + push_end - push_start) * 2)]

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
        mov     game_ita, 1
        mov     game_sg_off, SAVGAM_OFF_ITA
        jmp     got_match

its_eur:
        mov     game_eur, 1
        mov     game_sg_off, SAVGAM_OFF_EUR
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

        mov     game_usa, 1
        mov     game_sg_off, SAVGAM_OFF_USA

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

        mov     cs:err_notify, 0

        mov     cx, HOOK1_EUR_MSW
        mov     ax, HOOK1_EUR_LSW
        mov     dx, HOOK1_USA_MSW
        mov     bx, HOOK1_USA_LSW
        mov     si, HOOK1_ITA_MSW
        mov     di, HOOK1_ITA_LSW
        call    get_hook_offset
        mov     si, OFFSET cs:hook_code_start
        mov     di, OFFSET cs:hook_orig
        mov     cx, HOOK_LENGTH
        call    copy_patch_block

        mov     cx, HOOK2_EUR_MSW
        mov     ax, HOOK2_EUR_LSW
        mov     dx, HOOK2_USA_MSW
        mov     bx, HOOK2_USA_LSW
        mov     si, HOOK2_ITA_MSW
        mov     di, HOOK2_ITA_LSW
        call    get_hook_offset
        mov     si, OFFSET cs:hook_code_start
        mov     di, OFFSET cs:hook_orig
        mov     cx, HOOK_LENGTH
        call    copy_patch_block

        mov     cx, HOOK_EUR_KBD_MSW
        mov     ax, HOOK_EUR_KBD_LSW
        mov     dx, HOOK_USA_KBD_MSW
        mov     bx, HOOK_USA_KBD_LSW
        mov     si, HOOK_ITA_KBD_MSW
        mov     di, HOOK_ITA_KBD_LSW
        call    get_hook_offset
        mov     si, OFFSET cs:hook_kbd_start
        mov     di, OFFSET cs:hook_kbd_orig
        mov     cx, HOOK_KBD_LENGTH
        call    copy_patch_block

        mov     cx, HOOK_EUR_LED_MSW
        mov     ax, HOOK_EUR_LED_LSW
        mov     dx, HOOK_USA_LED_MSW
        mov     bx, HOOK_USA_LED_LSW
        mov     si, HOOK_ITA_LED_MSW
        mov     di, HOOK_ITA_LED_LSW
        call    get_hook_offset
        mov     si, OFFSET cs:hook_led_start
        mov     di, OFFSET cs:hook_led_orig
        mov     cx, HOOK_LED_LENGTH
        call    copy_patch_block

        mov     cx, HOOK_EUR_PIT_MSW
        mov     ax, HOOK_EUR_PIT_LSW
        mov     dx, HOOK_USA_PIT_MSW
        mov     bx, HOOK_USA_PIT_LSW
        mov     si, HOOK_ITA_PIT_MSW
        mov     di, HOOK_ITA_PIT_LSW
        call    get_hook_offset
        mov     si, OFFSET cs:hook_pit_start
        mov     di, OFFSET cs:hook_pit_orig
        mov     cx, HOOK_PIT_LENGTH
        call    copy_patch_block

        mov     cx, HOOK_EUR_SEG_MSW
        mov     ax, HOOK_EUR_SEG_LSW
        mov     dx, HOOK_USA_SEG_MSW
        mov     bx, HOOK_USA_SEG_LSW
        mov     si, HOOK_ITA_SEG_MSW
        mov     di, HOOK_ITA_SEG_LSW
        call    get_hook_offset
        mov     si, OFFSET cs:hook_seg_start
        mov     di, OFFSET cs:hook_seg_orig
        mov     cx, HOOK_SEG_LENGTH
        call    copy_patch_block

        ;**
        ;** Hook time bug fix removed.
        ;**
;        mov     cx, HOOK_EUR_TIM_MSW
;        mov     ax, HOOK_EUR_TIM_LSW
;        mov     dx, HOOK_USA_TIM_MSW
;        mov     bx, HOOK_USA_TIM_LSW
;        mov     si, HOOK_ITA_TIM_MSW
;        mov     di, HOOK_ITA_TIM_LSW
;        call    get_hook_offset
;        mov     si, OFFSET cs:hook_tim_start
;        mov     di, OFFSET cs:hook_tim_orig
;        mov     cx, HOOK_TIM_LENGTH
;        call    copy_patch_block

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
;**             SI:DI = Italian hook.
;**

TSR_CODE        segment

get_hook_offset proc near

        cmp     game_usa, 1
        jz      get_hook_offset_do_usa
        cmp     game_ita, 1
        jz      get_hook_offset_do_ita

        mov     bx, ax
        mov     dx, cx
get_hook_offset_do_usa:
        ret

get_hook_offset_do_ita:
        mov     bx, di
        mov     dx, si
        ret

get_hook_offset endp

TSR_CODE        ends

;*****************************************************************************
;** Purpose:    Copy patch block to correct destination.
;**
;** Returns:    Nothing.
;**
;** Notes  :    SI = offset of local code fragment.
;**             DI = offset of local store of original code fragment.
;**             DX = Hook segment
;**             BX = Hook offset
;**             CX = Hook block length.

TSR_CODE        segment

copy_patch_block proc near

        mov     ax, di
        mov     di, sp
        les     di, ss:[di + ((PUSHA_SIZE + push_end - push_start) * 2) + 2 + 2]
        add     bx, 2
        sub     di, bx
        mov     bx, es
        sub     bx, dx
        mov     es, bx

        push    si
        push    di
        push    cx
        push    ds
        push    es

        xchg    si, di
        mov     di, ax
        mov     ax, es
        push    ds
        pop     es
        mov     ds, ax

copy_patch_block1:
        lodsb
        stosb
        loop    copy_patch_block1

        pop     es
        pop     ds
        pop     cx
        pop     di
        pop     si

copy_patch_block2:
        lodsb
        stosb
        loop    copy_patch_block2

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
;** Purpose:    Code executed as cars move on track.
;**
;** Returns:    Nothing.
;**
;** Notes  :
;**
;** GPU.EXE:
;**
;** Player
;** 002D17  C47C12         les    di,[si+12]
;** 002D1A  E80700         call   002D24
;** 002D1D  897C12         mov    [si+12],di                    <----
;** 002D20  8C4414         mov    [si+14],es
;** 002D23  C3             ret
;**
;** CC
;** 007CC6  B81000         mov    ax,10
;** 007CC9  894416         mov    [si+16],ax
;** 007CCC  897C12         mov    [si+12],di
;** 007CCF  8C4414         mov    [si+14],es
;** 007CD2  894C1C         mov    [si+1C],cx
;** 007CD5  8B541C         mov    dx,[si+1C]
;**
;** (DOS VER hook point = 06d2eaH in GPU.EXE)
;** (DOS VER hook point = 06d2aaH in ITU.EXE)
;** (DOS VER hook point = 06d295H in USU.EXE)
;**

HOOK1_EUR_MSW   equ     6000h
HOOK1_EUR_LSW   equ     (0d2eah - 7CCCh)
HOOK1_ITA_MSW   equ     6000h
HOOK1_ITA_LSW   equ     (0d2aah - 7CCCh)
HOOK1_USA_MSW   equ     6000h
HOOK1_USA_LSW   equ     (0d295h - 7CCCh)

HOOK2_EUR_MSW   equ     6000h
HOOK2_EUR_LSW   equ     (0d2eah - 2D1Dh)
HOOK2_ITA_MSW   equ     6000h
HOOK2_ITA_LSW   equ     (0d2aah - 2D1Dh)
HOOK2_USA_MSW   equ     6000h
HOOK2_USA_LSW   equ     (0d295h - 2D1Dh)

TSR_DATA                segment

hook_code_start:
                        nop
                        db     09ah
hook_code_off           dw     ?
hook_code_seg           dw     ?
hook_code_end:
HOOK_LENGTH             equ     (hook_code_end - hook_code_start)

TSR_DATA                ends

TSR_CODE                segment

hskip2: jmp     hskip

        public  hook
hook:
        pushf
        pusha
        push    ds
        push    es
        cld

        mov     cs:game_ds, ds
        mov     cs:game_ss, ss

        push    cs
        pop     ds

        ;**
        ;** Replay?
        ;**
        mov     es, game_ds
        test    byte ptr es:[005ah], 80h
        jnz     hskip2

        ;**
        ;** Specified CC, or local player's car (not linked player)?
        ;**
        mov     ax, log_car_num
        or      ax, ax
        jz      hk_1
        cmp     al, es:[si+00ach]
        je      hk_2
        jmp     hskip1

hk_1:   test    byte ptr es:[si+00ach], 80h
        jz      hskip1
        test    byte ptr es:[si+00ach], 40h
        jnz     hskip1

        ;**
        ;** Same track segment id as last time?
        ;**
hk_2:   mov     ax, es:[si+0012h]
        cmp     ax, last_seg_id
        jz      hskip1
        mov     last_seg_id, ax

        ;**
        ;** Perform normal enabled log?
        ;**
        mov     al, always_enabled
        or      al, al
        jnz     hlogit

        ;**
        ;** Don't perform auto-cancel if this is a race.
        ;**
        mov     es, game_ss
        test    byte ptr es:[124ah], 80h
        mov     es, game_ds
        jnz     hchksf

        ;**
        ;** If re-entered pit then cancel logging.
        ;**
        mov     al, enabled
        and     al, pending_enable
        jz      hchksf
        test    byte ptr es:[si+0023h], 20h
        jnz     hpittted

        ;**
        ;** Crossing s/f line on the track (not in the pit)?
        ;**
hchksf: cmp     word ptr es:[si+0012h], 0030h
        jnz     hchk
        test    byte ptr es:[si+0023h], 20h
        jnz     hchk

        ;**
        ;** Both pending & enabled?
        ;**
        mov     al, enabled
        and     al, pending_enable
        jz      hnoenbl

        ;**
        ;** Don't perform auto-disable if this is a race.
        ;**
        mov     es, game_ss
        test    byte ptr es:[124ah], 80h
        mov     es, game_ds
        jnz     hlogit

        ;**
        ;** If Logging period expired then go back to pending mode.
        ;**
        dec     log_period
        jnz     hlogit
hpittted:
        mov     enabled, 0
        jmp     hdoled
hskip1: jmp     hskip

        ;**
        ;** If pending then go from pending mode to enabled (& pending)
        ;**
hnoenbl:mov     al, pending_enable
        or      al, al
        jz      hskip1
        mov     enabled, 1
        mov     ax, log_laps
        mov     log_period, ax

hdoled: push    cs
        call    update_led
        jmp     hlogit

        ;**
        ;** Both pending & enabled?
        ;**
hchk:   mov     al, enabled
        and     al, pending_enable
        jz      hskip

hlogit: mov     es, game_ds

        cmp     word ptr es:[si+0012h], 0030h
        jnz     hnotsf

        test    es:[si+0042h], 0c000h
        jnz     hnotime
        push    es:[si+0042h]           ; Last lap time (exactly right)
        push    es:[si+0040h]
        jmp     hlog

hnotime:push    0
        push    0
        jmp     hlog

hnotsf: mov     ax, es:[294fh]          ; Game's main timer (approx value)
        mov     dx, es:[2951h]
        sub     ax, es:[si+54h]
        sbb     dx, es:[si+56h]

        call    modtime

        push    dx
        push    ax

hlog:   mov     es, game_ds
        push    es:[22f3h]              ; Braking
        mov     ax, ss:[121ch+1ah]      ; Circuit
        push    ax
        push    game_ds                 ; DS:SI = Current car
        push    si
        push    OFFSET TSR:pe           ; Entry to fill in
        call    LogEntry
        add     sp, 14

        mov     dx, OFFSET TSR:pe
        mov     cx, pe_size
        call    write_entry

hskip:  pop     es
        pop     ds
        popa
        popf

        ;**
        ;** Update revs
        ;**
        cmp     si, ds:[097dh]
        jne     hnotus
        push    cs
        call    update_revs
hnotus:

        ;**
        ;** Original code (could be SPLITTIM patch code).
        ;**
        public  hook_orig
hook_orig:
        db      HOOK_LENGTH dup(0)
        retf

TSR_CODE        ends

;*****************************************************************************
;** Purpose: Write an entry to file.
;**
;** Returns:
;**
;** Notes  :
;**

TSR_CODE        segment

;****************************************************************************
;**
;** DS:DX points to data.
;** CX    is length.

data_dx dw      ?
data_cx dw      ?

        public  write_entry
write_entry proc near
        mov     data_dx, dx
        mov     data_cx, cx

        ;**
        ;** Open file
        ;**
        mov     dx, OFFSET TSR:q_filename
        mov     ah, 03dh
        mov     al, 00100010B           ; R/W (share deny write)
        int     DOS
        jnc     do_seek

        ;**
        ;** Try and Create the file.
        ;**
open_failed:
        mov     dx, OFFSET TSR:q_filename
        mov     ah, 03ch
        mov     cx, 0                   ; Normal attributes
        int     DOS
        jc      creat_failed

        ;**
        ;** Seek to EOF
        ;**
do_seek:
        mov     fh, ax                  ; Save File Handle
        mov     ah, 42h
        mov     al, 02h
        mov     bx, fh
        xor     cx, cx
        xor     dx, dx
        INT     DOS
        jc      do_close

        ;**
        ;** Write to file
        ;**
        mov     ah, 40h
        mov     bx, fh
        mov     cx, data_cx
        mov     dx, data_dx
        INT     DOS
        jc      do_error
        cmp     ax, data_cx
        jz      do_close

        ;**
        ;** Write failed error (disk full)
        ;**
do_error:
        cmp     err_notify, 1
        jz      do_close
        mov     beep_count, BEEP_ERROR
        mov     err_notify, 1

        mov     always_enabled, 0
        mov     enabled, 0
        mov     pending_enable, 0
        push    cs
        call    update_led

        ;**
        ;** Close file
        ;**
do_close:
        mov     bx, fh
        mov     ah, 3eh
        INT     DOS

creat_failed:
        ret

write_entry     endp

TSR_CODE        ends

;*****************************************************************************
;** Purpose: Code executed at s/f line to test if car should pit in qual.
;**
;** Returns:
;**
;** Notes  :
;**
;** 0035A4  0E             push   cs
;** 0035A5  E82FE5         call   001AD7
;** 0035A8  2407           and    al,7
;** 0035AA  751A           jnz    0035C6
;** 0035AC  B003           mov    al,3                          <----
;** 0035AE  3A4422         cmp    al,[si+22]
;** 0035B1  7F13           jg     0035C6
;** 0035B3  804C2380       or     byte ptr [si+23],80
;** 0035B7  80A49700FB     and    byte ptr [si+97],0FB
;** 0035BC  808CB30004     or     byte ptr [si+0B3],4
;** 0035C1  9ACB1BED19     call   19ED:1BCB
;** 0035C6  C3             ret
;**
;**

HOOK_EUR_PIT_MSW equ    6000h
HOOK_EUR_PIT_LSW equ    (0d2eah - 35ach)
HOOK_ITA_PIT_MSW equ    6000h
HOOK_ITA_PIT_LSW equ    (0d2aah - 35ach)
HOOK_USA_PIT_MSW equ    6000h
HOOK_USA_PIT_LSW equ    (0d295h - 35ach)

TSR_DATA                segment

hook_pit_start:         db     09ah
hook_pit_off            dw     ?
hook_pit_seg            dw     ?
hook_pit_end:
HOOK_PIT_LENGTH         equ     (hook_pit_end - hook_pit_start)

TSR_DATA                ends

TSR_CODE        segment

        public  hook_pit_orig
hook_pit_orig:
        db      HOOK_PIT_LENGTH dup(0)
        retf

        public  hook_pit
hook_pit:
        mov     ax, 3
        test    byte ptr [si+0ach], 80h
        jz      hpnorm
        test    byte ptr [si+0ach], 40h
        jnz     hpnorm
        mov     ax, cs:log_laps
        add     ax, 2
hpnorm: cmp     al, [si+22h]
        retf

TSR_CODE        ends

;*****************************************************************************
;** Purpose: Normally MPH display, flipped to segment display.
;**
;** Returns:
;**
;** Notes  :
;**
;** 01CD5D  50             push   ax
;** 01CD5E  B006           mov    al,6
;** 01CD60  E807FC         call   01C96A
;** 01CD63  58             pop    ax
;** 01CD64  8BC2           mov    ax,dx
;** 01CD66  9A170E0000     call   0:0E17        <----
;** 01CD6B  50             push   ax
;** 01CD6C �C1E808         shr    ax,8
;** 01CD6F  240F           and    al,0F
;** 01CD71  0430           add    al,30
;**

HOOK_EUR_SEG_MSW equ    5000h
HOOK_EUR_SEG_LSW equ    (0d2eah - 0cd66h)
HOOK_ITA_SEG_MSW equ    5000h
HOOK_ITA_SEG_LSW equ    (0d2aah - 0cd32h)
HOOK_USA_SEG_MSW equ    5000h
HOOK_USA_SEG_LSW equ    (0d295h - 0cd3ah)

TSR_DATA                segment

hook_seg_start:         db     09ah
hook_seg_off            dw     ?
hook_seg_seg            dw     ?
hook_seg_end:
HOOK_SEG_LENGTH         equ     (hook_seg_end - hook_seg_start)

TSR_DATA                ends

BAR_COLOUR_WARN         equ     02h
BAR_COLOUR_GOOD         equ     03h
BAR_COLOUR_BACK         equ     98h
BAR_NUM_SEGS            equ     136
BAR_SEGMENT             equ     0a000h
BAR_OFFSET              equ     0ef1ch

TSR_CODE        segment

        public  hook_seg
hook_seg:
        public  hook_seg_orig
        cmp     cs:show_seg, 1
        jne     hs1
        push    bx
        push    dx
        mov     ax, [si+12h]
        sub     ax, 0030h
        mov     bx, 002eh
        xor     dx, dx
        div     bx
        pop     dx
        pop     bx

        ;**
        ;** Display tyre wear.
        ;**
hs1:    push    es
        push    di
        push    dx
        push    cx
        push    bx
        push    ax

        cmp     cs:show_tyres, 1
        je      hs6
        xor     ax, ax
        jmp     hs4

hs6:    mov     dl, BAR_COLOUR_WARN
        xor     ax, ax
        mov     al, [si+0b2h]
        and     al, 07h
        shl     ax, 1
        mov     bx, OFFSET TSR_CODE:tyres
        add     bx, ax
        mov     ax, cs:[bx]
        sub     ax, [si+46h]
        jns     hs2
        neg     ax
        shr     ax, 1
        mov     dl, BAR_COLOUR_GOOD
        jmp     hs3
hs2:    shr     ax, 3
hs3:    cmp     ax, BAR_NUM_SEGS
        jle     hs4
        mov     ax, BAR_NUM_SEGS
hs4:    mov     cx, ax
        mov     bx, ax

        mov     ax, BAR_SEGMENT
        mov     es, ax
        mov     di, BAR_OFFSET

        mov     al, dl
        rep     stosb
        mov     cx, BAR_NUM_SEGS
        sub     cx, bx
        jz      hs5
        mov     al, BAR_COLOUR_BACK
        rep     stosb

hs5:    pop     ax
        pop     bx
        pop     cx
        pop     dx
        pop     di
        pop     es

hook_seg_orig:
        db      HOOK_SEG_LENGTH dup(0)
        retf

tyres:  dw      3a40h, 3b40h, 3c40h, 3d00h, 3e80h, 3900h, 3900h

TSR_CODE        ends

;*****************************************************************************
;** Purpose:    Code when key pressed and driving car.
;**
;** Returns:    Nothing.
;**
;** Notes  :
;**
;** GPU.EXE:
;**
;** 08D208  50             push   ax
;** 08D209  53             push   bx
;** 08D20A  51             push   cx
;** 08D20B  1E             push   ds
;** 08D20C  9C             pushf
;** 08D20D  BB1429         mov    bx,2914               <----
;** 08D210  8EDB           mov    ds,bx
;** 08D212  50             push   ax
;** 08D213  3C80           cmp    al,80
;**
;** (DOS VER hook point = 06d2eaH in GPU.EXE)
;** (DOS VER hook point = 06d2aaH in GPU.EXE)
;** (DOS VER hook point = 06d295H in USU.EXE)
;**

HOOK_EUR_KBD_MSW equ    0e000h                  ; (-2000h) 8d20dh
HOOK_EUR_KBD_LSW equ    (0d2eah - 0d20dh)
HOOK_ITA_KBD_MSW equ    0e000h                  ; (-2000h) 8d17dh
HOOK_ITA_KBD_LSW equ    (0d2aah - 0d17dh)
HOOK_USA_KBD_MSW equ    0e000h                  ; (-2000h) 8d16dh
HOOK_USA_KBD_LSW equ    (0d295h - 0d16dh)

TSR_DATA                segment

hook_kbd_start:         db     09ah
hook_kbd_off            dw     ?
hook_kbd_seg            dw     ?
hook_kbd_end:
HOOK_KBD_LENGTH         equ     (hook_kbd_end - hook_kbd_start)

TSR_DATA                ends

TSR_CODE                segment

        public  hook_kbd
hook_kbd:
        pushf
        push    es
        push    bx
        cld

        cmp     cs:beep_count, 0
        jnz     hk8

        mov     bx, cs:game_ds
        or      bx, bx
        jz      hk8
        mov     es, cs:game_ss
        test    byte ptr es:[1108h], 0ffh
        jnz     hk8

        cmp     al, 41h                         ; Scan code for <F7>
        jz      hk4
        cmp     al, 42h                         ; Scan code for <F8>
        jz      hk6
        cmp     al, 44h                         ; Scan code for <F10>
        jz      hk9
        cmp     al, 43h                         ; Scan code for <F9>
        jnz     hk8

        ;**
        ;** <F9>
        ;**
        cmp     cs:show_seg, 0
        jz      hk7
        mov     cs:show_seg, 0
        jmp     hk3
hk7:    mov     cs:show_seg, 1
        jmp     hk3
hk8:    jmp     hk2

        ;**
        ;** <F8>
        ;**
hk6:    mov     cs:always_enabled, 0
        mov     cs:enabled, 0
        cmp     cs:pending_enable, 0
        jz      hk5
        mov     cs:pending_enable, 0
        jmp     hk3
hk5:    mov     cs:pending_enable, 1
        mov     bx, cs:log_laps
        mov     cs:log_period, bx
        jmp     hk3

        ;**
        ;** <F10>
        ;**
hk9:    cmp     cs:show_tyres, 0
        jz      hk10
        mov     cs:show_tyres, 0
        jmp     hk3
hk10:   mov     cs:show_tyres, 1
        jmp     hk3

        ;**
        ;** <F7>
        ;**
hk4:    mov     cs:enabled, 0
        mov     cs:pending_enable, 0
        cmp     cs:always_enabled, 0
        jz      hk1
        mov     cs:always_enabled, 0
        jmp     hk3
hk1:    mov     cs:always_enabled, 1

hk3:    push    cs
        call    update_led

hk2:    pop     bx
        pop     es
        popf

        ;**
        ;** Original code
        ;**
        public  hook_kbd_orig
hook_kbd_orig:
        db      HOOK_KBD_LENGTH dup(0)
        retf

TSR_CODE        ends

;*****************************************************************************
;** Purpose:    Update left-hand opposition level LEDs.
;**
;** Returns:    Nothing.
;**
;** Notes  :
;**
;** GPU.EXE:
;**
;** 00E4AA  9AFC1AED19     call   19ED:1AFC             <----
;** 00E4AF  9A1D1BED19     call   19ED:1B1D
;** 00E4B4  9ACF1AED19     call   19ED:1ACF
;** 00E4B9  9A641BED19     call   19ED:1B64
;** 00E4BE  E82000         call   00E4E1
;**
;** (DOS VER hook point = 06d2eaH in GPU.EXE)
;** (DOS VER hook point = 06d2aaH in ITU.EXE)
;** (DOS VER hook point = 06d295H in USU.EXE)
;**

HOOK_EUR_LED_MSW equ    06000h
HOOK_EUR_LED_LSW equ    (0d2eah - 0e4aah)
HOOK_ITA_LED_MSW equ    06000h
HOOK_ITA_LED_LSW equ    (0d2aah - 0e4aah)
HOOK_USA_LED_MSW equ    06000h
HOOK_USA_LED_LSW equ    (0d295h - 0e4aah)

TSR_DATA                segment

hook_led_start:         db     09ah
hook_led_off            dw     ?
hook_led_seg            dw     ?
hook_led_end:
HOOK_LED_LENGTH         equ     (hook_led_end - hook_led_start)

TSR_DATA                ends

TSR_CODE                segment

        public  hook_led
hook_led:
        mov     cs:game_ds, ds
        mov     cs:game_ss, ss

update_led:
        pushf
        pusha
        push    ds
        push    es
        cld

        ;**
        ;** Force rev LEDs to be updated.
        ;**
        mov     cs:force_led, 1

        cmp     cs:game_ds, 0
        jz      hook_not_init

        mov     es, cs:game_ss
        test    byte ptr es:[1108h], 0ffh
        jnz     not_in_car

        push    ds
        mov     ds, cs:game_ds
        xor     bp, bp
        call    hook_led_update
        pop     ds

not_in_car:
hook_not_init:
        pop     es
        pop     ds
        popa
        popf
        retf

update_revs:
        pushf
        pusha
        push    ds
        push    es
        cld

        cmp     cs:game_ds, 0
        jz      hook_not_init

        mov     es, cs:game_ss
        test    byte ptr es:[1108h], 0ffh
        jnz     not_in_car

        push    ds
        mov     ds, cs:game_ds
        xor     bp, bp
        call    update_rev_leds
        pop     ds
        jmp     hook_not_init

;****************************************************************************

hook_led_update:
        pusha

        ;**
        ;** Change bitmap to blue if always enabled.
        ;**
        mov     al, cs:always_enabled
        or      al, al
        jz      hlu2
        mov     bx, BLUE
        call    change_colour
        jmp     hlu1

        ;**
        ;** Change bitmap to red if pending and enabled.
        ;**
hlu2:   mov     al, cs:enabled
        or      al, al
        jz      hlu4
        mov     bx, RED
        call    change_colour
        jmp     hlu1

        ;**
        ;** Change bitmap to yellow if pending enable.
        ;**
hlu4:   mov     al, cs:pending_enable
        or      al, al
        jz      hlu3
        mov     bx, YELLOW
        call    change_colour
        jmp     hlu1

        ;**
        ;** Change bitmap to green if nothing enabled.
        ;**
hlu3:   mov     bx, GREEN
        call    change_colour

hlu1:   popa

        ;**
        ;** Call (our modified) original bitmap code
        ;**
        call    update_opp_leds

        ;**
        ;** Change bitmap back to green (as bitmap is used by other LEDs)
        ;**
        mov     bx, GREEN
        call    change_colour
        ret

;****************************************************************************

        ;**
        ;** Original code
        ;**
        public  hook_led_orig
hook_led_orig:
        db      HOOK_LED_LENGTH dup(0)
        retf

;****************************************************************************

change_colour:
        ;**
        ;** Change bitmap colour
        ;**
        ;** BL = high nibble value
        ;**
        pusha
        push    bx

        ;**
        ;** Calculate bitmap location
        ;**
        mov     es, cs:game_ss
        mov     bx, es:[1222h]          ; opposition standard
        add     bx, 5                   ; offset of foreground LEDs
        shl     bx, 3
        add     bx, 2f39h
        push    ds
        mov     ds, cs:game_ds
        les     di, ds:[8783h]
        add     di, ds:[bx]
        pop     ds

        pop     bx

        mov     cx, 7
hl4:    push    cx
        mov     cx, 7
hl2:    mov     al, es:[di]
        test    al, 0f0h
        jz      hl3
        and     al, 0fh
        or      al, bl
        mov     byte ptr es:[di], al
hl3:    inc     di
        loop    hl2

        add     di, 0139h
        pop     cx
        loop    hl4
        popa
        ret

;****************************************************************************

        public  update_opp_leds
update_opp_leds:
        test    byte ptr ds:[0981h], 0FFh
        jnz     uol1
        mov     ax, 5+4
        call    bitblt
uol1:   ret

bp_2c   dw      ?
bp_34   dw      ?
bp_04   dw      ?

        public  bitblt
bitblt:
        push    ds
        push    es
        pusha
        shl     ax, 3
        mov     bx, 2F39h
        add     bx, ax

        mov     ax, [bx]
        mov     cs:bp_2c, ax

        mov     ax, [bx+2]
        mov     cs:bp_34, ax

        mov     ax, [bx+4]
        mov     cs:bp_04, ax

        mov     dx, 140h
        mov     bx, [bx+6]
        sub     dx, bx
        les     di, ds:[04C0h]
        lds     si, ds:[8783h]
        add     di, cs:bp_34
        add     si, cs:bp_2c
        mov     ax, cs:bp_04
        sub     ax, +1
bb1:    mov     cx, bx
        repz    movsb
        add     si, dx
        add     di, dx
        sub     ax, +1
        jns     bb1
        popa
        pop     es
        pop     ds
        ret

old_rev dw      0
new_rev dw      0

        public  update_rev_leds
update_rev_leds:
        test    byte ptr ds:[0981h], 0FFh
        jz      uview
        mov     cs:force_led, 1
        ret

uview:  cmp     cs:force_led, 1
        jne     unoforce
        mov     cs:force_led, 0

        mov     cs:old_rev, 0
        xor     ax, ax
        call    bitblt
        inc     ax
        call    bitblt
        inc     ax
        call    bitblt

unoforce:
        mov     ax, 0
        mov     es, cs:game_ds
        mov     dx, es:[si+62h]
        cmp     dx, cs:rev_limit1
        jl      umatch
        inc     ax
        cmp     dx, cs:rev_limit2
        jl      umatch
        inc     ax
        cmp     dx, cs:rev_limit3
        jl      umatch
        inc     ax

umatch: cmp     ax, cs:old_rev
        mov     cs:new_rev, ax
        jl      uless
        jg      umore
        ret

        ;**
        ;** RPM increased
        ;**
umore:  mov     ax, cs:old_rev
umore1: inc     ax
        cmp     ax, 3
        jnz     unotred
        mov     bx, RED
        jmp     uupdate
unotred:
        mov     bx, YELLOW
uupdate:call    change_colour
        add     ax, 5-1
        call    bitblt
        sub     ax, 5-1
        cmp     ax, cs:new_rev
        jl      umore1
        mov     bx, GREEN
        call    change_colour
        jmp     uret

        ;**
        ;** RPM decreased
        ;**
uless:  mov     ax, cs:new_rev
uless1: inc     ax
        dec     ax
        call    bitblt
        inc     ax
        cmp     ax, cs:old_rev
        jl      uless1

uret:   mov     ax, cs:new_rev
        mov     cs:old_rev, ax
        ret

TSR_CODE        ends

;****************************************************************************
;** Purpose:    Hook time update code.
;**
;** Returns:
;**
;** Notes  :
;**
;** GPU.EXE:
;**
;** 00363A  B90000         mov    cx,0
;** 00363D  8B1E6001       mov    bx,[160]
;** 003641  F7E3           mul    bx                            <---
;** 003643  C1EB03         shr    bx,3
;** 003646  F74410FFFF     test   word ptr [si+10],0FFFF
;**
;** (DOS VER hook point = 06d2eaH in GPU.EXE)
;** (DOS VER hook point = 06d2aaH in ITU.EXE)
;** (DOS VER hook point = 06d295H in USU.EXE)

HOOK_EUR_TIM_MSW equ    6000h
HOOK_EUR_TIM_LSW equ    (0d2eah - 03641h)
HOOK_ITA_TIM_MSW equ    6000h
HOOK_ITA_TIM_LSW equ    (0d2aah - 03641h)
HOOK_USA_TIM_MSW equ    6000h
HOOK_USA_TIM_LSW equ    (0d295h - 03641h)

TSR_DATA                segment

hook_tim_start:         db     09ah
hook_tim_off            dw     ?
hook_tim_tim            dw     ?
hook_tim_end:
HOOK_TIM_LENGTH         equ     (hook_tim_end - hook_tim_start)

TSR_DATA                ends

TSR_CODE        segment

        ;**
        ;** Modified code
        ;**
        public  hook_tim
hook_tim:
        push    bx
        mov     bx, 1000
        mul     bx
        pop     bx
        shr     bx, 3
        retf

        public  hook_tim_orig
hook_tim_orig:
        db      HOOK_TIM_LENGTH dup(0)

;*****************************************************************************
;** Purpose: Modify time allowing for extra sub-distance travelled.
;**
;** Returns:
;**
;** Notes  : DX:AX = time on i/p & o/p
;**

modtime:
        push    ds
        mov     ds, cs:game_ds
        les     di, [si+12h]
        push    ax
        push    dx
        xor     dx, dx
        mov     ax, [si+1Ch]
        mov     cx, 0
	mov	bx, 1000				;<-- Bug fix for game!
        mul     bx
        mov     bx, word ptr ds:[0160h]
        shr     bx, 3
        test    word ptr [si+10h], 0FFFFh
        js      mt1
        jz      mt1
        mov     byte ptr ss:[0C0h], 00h
        div     word ptr [si+10h]
        test    byte ptr ss:[0C0h], 0FFh
        jnz     mt1
        mov     dx, 0
        cmp     ax, bx
        jc      mt2
mt1:    mov     ax, bx
        mov     dx, cx
mt2:    mov     bx, ax
        mov     cx, dx
        pop     dx
        pop     ax
        sub     ax, bx
        sbb     dx, cx
        pop     ds
        ret

TSR_CODE        ends

;****************************************************************************

        end

