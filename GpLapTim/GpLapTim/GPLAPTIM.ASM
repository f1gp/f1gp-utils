        page    66,132
;****************************************************************************
        TITLE   gptimlog.asm
        NAME    gptimlog
        .286

;****************************************************************************
;** Description: Lap Time Logger for MPS Grand Prix.
;****************************************************************************

TSR     segment  WORD PUBLIC 'TSR'
TSR     ends

_TEXT	segment  WORD PUBLIC 'CODE'
_TEXT	ends
_DATA	segment  WORD PUBLIC 'DATA'
_DATA	ends
_BSS	segment  WORD PUBLIC 'BSS'
_BSS	ends
DGROUP	group	 TSR, _TEXT, _BSS, _DATA

        assume  CS: DGROUP, DS: DGROUP, ES: NOTHING, SS: DGROUP

;****************************************************************************

TSR     segment

        org     100h
startup:
        jmp     install

;****************************************************************************
;**
;** Equates
;**

DOS             equ     21h                     ; Dos interrupt call
ROM_BIOS        equ     10h                     ; PC Rom bios interrupt call
CR              equ     13                      ; Caridge return
LF              equ     10                      ; Line Feed
BELL            equ     07h                     ; Bell Code
DOS_TERMINATE   equ     20h                     ; Terminate interrupt.

TICK_TIME_MS    equ     18                      ; Approx ms of a tick
DELAY_TIME      equ     (500 / TICK_TIME_MS)    ; Time to display hook msg

STACK_SIZE      equ     200

MAGIC_CX_IN     equ     0c0deh
MAGIC_DX_IN     equ     0deadh
MAGIC_CX_OUT    equ     05a5ah
MAGIC_DX_OUT    equ     03412h

SAVGAM_OFF_ITA  equ     -10h
SAVGAM_OFF_EUR  equ     0030h
SAVGAM_OFF_USA  equ     0000h
SAVGAM_OFF_SPA  equ     0000h

;**
;** Split Timer hooks.
;**
DIF_SEG         equ     466dh
DIF_SEG_MOD_EUR equ     (DIF_SEG - 0000h)
DIF_SEG_MOD_ITA equ     (DIF_SEG - 0000h)
DIF_SEG_MOD_USA equ     (DIF_SEG - 0003h)
DIF_SEG_MOD_SPA equ     (DIF_SEG - 0003h)

FUNC1_OFF_EUR   equ     14c0h
FUNC2_OFF_EUR   equ     15b6h

FUNC1_OFF_ITA   equ     14cch
FUNC2_OFF_ITA   equ     15c2h

FUNC1_OFF_USA   equ     1494h
FUNC2_OFF_USA   equ     158ah

FUNC1_OFF_SPA   equ     1494h
FUNC2_OFF_SPA   equ     158ah

include gplaptim.ash

;****************************************************************************
;**
;** Local data
;**
                even

psp_env         label   dword
psp_env_off     dw      002ch           ; Offset of env pointer in PSP
psp_seg         dw      ?               ; PSP segment

old8_hndlr      label   dword           ; IRQ 0 Timer tick
old8_off        dw      ?
old8_seg        dw      ?

old11_hndlr     label   dword           ; ROM BIOS equipment list
old11_off       dw      ?
old11_seg       dw      ?

old21_hndlr     label   dword           ; DOS Int 21 handler
old21_off       dw      ?
old21_seg       dw      ?

delay_count     db      0

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
GP_SPA_RET_ADDR equ     0b75bh
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

gp_code_usa     label
gp_code_spa     label
                db      0b0h, 00h, 0b4h, 30h, 0cdh, 21h, 0ah, 0c0h, 75h, 03h
MATCH_BYTES_USA equ     $ - gp_code_usa
MATCH_BYTES_SPA equ     $ - gp_code_spa

q_filename      db      64 + 14 dup(?)
fh              dw      ?

game_si         label   dword
game_si_off     dw      ?
game_si_seg     dw      ?
game_ss         dw      ?
game_sp         dw      ?
game_eur        db      0
game_usa        db      0
game_ita        db      0
game_spa        db      0

game_sg_off     dw      ?

game_hp         label   dword
game_hp_off     dw      ?
game_hp_seg     dw      ?

                public  _player_only_flag
_player_only_flag db    0

hook_code_start:
hook_lap_code           db      09ah
hook_lap_code_off       dw      ?
hook_lap_code_seg       dw      ?
hook_code_end:
HOOK_LENGTH     equ     (hook_code_end - hook_code_start)

hook_pit1_code_start:
hook_pit1_code          db      09ah
hook_pit1_code_off      dw      ?
hook_pit1_code_seg      dw      ?
                        db      90h
hook_pit1_code_end:
HOOK1_LENGTH    equ     (hook_pit1_code_end - hook_pit1_code_start)

hook_pit2_code_start:
hook_pit2_code          db      09ah
hook_pit2_code_off      dw      ?
hook_pit2_code_seg      dw      ?
hook_pit2_code_end:
HOOK2_LENGTH    equ     (hook_pit2_code_end - hook_pit2_code_start)

hook_aig_code_start:
hook_aig_code           db      09ah
hook_aig_code_off       dw      ?
hook_aig_code_seg       dw      ?
                        db      90h
hook_aig_code_end:
HOOK3_LENGTH    equ     (hook_aig_code_end - hook_aig_code_start)

hook_st_code_start:
hook_st_code           db      09ah
hook_st_code_off       dw      ?
hook_st_code_seg       dw      ?
                       db      90h
hook_st_code_end:
HOOK4_LENGTH    equ    (hook_st_code_end - hook_st_code_start)
HOOK5_LENGTH    equ    (hook_st_code_end - hook_st_code_start)

hook_bhp_code_start:
hook_bhp_code           db      09ah
hook_bhp_code_off       dw      ?
hook_bhp_code_seg       dw      ?
hook_bhp_code_end:
HOOK6_LENGTH    equ     (hook_bhp_code_end - hook_bhp_code_start)

;**
;** Note: Log record and Auth record *must* be adjacent!
;**
lr              LAP_RECORD              ?
ar              AUTH_RECORD             ?
rr              AUTH_RACE_RECORD        ?
sr              SETUP_RECORD            ?
dr              DATE_RECORD             ?
pi              PIT_IN_RECORD           ?
po              PIT_OUT_RECORD          ?
st              SPLIT_TIME_RECORD       ?

RECORD_LEN      equ     (OFFSET ar - OFFSET lr)
CRC_LEN         equ     (RECORD_LEN - 2)
SETUP_SRC_LEN   equ     10

;**
;** Last lap and current lap HH:MM:SS
;**
tag$secs        STRUC

secs_seconds    db      ?
secs_minutes    db      ?
secs_hours      db      ?
secs_total      dw      ?
                dw      ?

tag$secs        ENDS

SECS            TYPEDEF tag$0

ll_secs         SECS    ?
cl_secs         SECS    ?

grass           db      0

;**
;** Split Timer (split values must be adjacent).
;**
dif_seg         dw      ?
func1_off       dw      ?
func2_off       dw      ?

                public  _split1, _split2, _split3, _freeze_time
splits:
_split1         dw      25
_split2         dw      50
_split3         dw      75

_freeze_time    dw      10000
track_divs      dw      ?
last_value      dw      0


;**
;** End of race lap numbers.
;**
full_race_distance:
                db      81            ;** Phoenix             - United States
                db      71            ;** Interlagos          - Brazil
                db      61            ;** Imola               - San Marino
                db      78            ;** Monte-Carlo         - Monaco
                db      69            ;** Montreal            - Canada
                db      69            ;** Mexico City         - Mexico
                db      72            ;** Magny Cours         - France
                db      59            ;** Silverstone         - Great Britain
                db      45            ;** Hockenheim          - Germany
                db      77            ;** Hungaroring         - Hungary
                db      44            ;** Spa-Francorchamps   - Belgium
                db      53            ;** Monza               - Italy
                db      71            ;** Estoril             - Portugal
                db      65            ;** Barcelona           - Spain
                db      53            ;** Suzuka              - Japan
                db      81            ;** Adelaide            - Australia

half_race_distance:
                db      41            ;** Phoenix             - United States
                db      36            ;** Interlagos          - Brazil
                db      31            ;** Imola               - San Marino
                db      39            ;** Monte-Carlo         - Monaco
                db      35            ;** Montreal            - Canada
                db      35            ;** Mexico City         - Mexico
                db      36            ;** Magny Cours         - France
                db      30            ;** Silverstone         - Great Britain
                db      23            ;** Hockenheim          - Germany
                db      39            ;** Hungaroring         - Hungary
                db      22            ;** Spa-Francorchamps   - Belgium
                db      27            ;** Monza               - Italy
                db      36            ;** Estoril             - Portugal
                db      33            ;** Barcelona           - Spain
                db      27            ;** Suzuka              - Japan
                db      41            ;** Adelaide            - Australia
;**
;** Standard GP.EXE damage values (from DS:08CCh)
;**
damage_defaults:
                dw      1d00h, 2000h, 1000h, 0700h, 0700h
DAMAGE_COUNT    equ     $ - damage_defaults
DAMAGE_OFFSET   equ     08cch


hooked_msg      db      CR, LF, "GpLapTim activated OK", CR, LF, '$'

                even

                public  my_stack_bottom
                public  my_stack_top
my_stack_bottom db      STACK_SIZE dup('#')
my_stack_top:   dw      5a5ah

;****************************************************************************
;**
;** Our IRQ 0 tick timer
;**

        public  new8_hndlr
new8_hndlr      proc near

        pushf
        call    DWORD PTR cs:old8_hndlr

        ;**
        ;** Decrement delay count
        ;**
        cmp     cs:delay_count, 0
        jz      no_delay
        dec     cs:delay_count
no_delay:
        iret

new8_hndlr      endp

;****************************************************************************
;**
;** Our ROM BIOS equipment list int
;**

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

;****************************************************************************
;**
;** Our DOS Int 21 handler
;**

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

        ;**
        ;** Get return address
        ;**
        mov     di, sp
        les     di, ss:[di + ((PUSHA_SIZE + push_end - push_start) * 2)]

        ;**
        ;** Clear all detection flags
        ;**
        mov     game_eur, 0
        mov     game_usa, 0
        mov     game_ita, 0
        mov     game_spa, 0

        ;**
        ;** Is it European GP.EXE calling us?
        ;**
        cmp     di, GP_EUR_RET_ADDR
        jnz     try_us
        sub     di, MATCH_OFFSET
        mov     bx, di
        mov     si, OFFSET gp_code_eur
        mov     cx, MATCH_BYTES_EUR
        cld
        repe    cmpsb
        jz      its_eur

        mov     di, bx
        mov     si, OFFSET gp_code_ita
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
        jnz     try_spa
        sub     di, MATCH_OFFSET
        mov     si, OFFSET gp_code_usa
        mov     cx, MATCH_BYTES_USA
        cld
        repe    cmpsb
        jnz     try_spa

        mov     game_usa, 1
        mov     game_sg_off, SAVGAM_OFF_USA
        jmp     got_match

        ;**
        ;** Is it Spanish GP.EXE calling us?
        ;**
try_spa: cmp    di, GP_SPA_RET_ADDR
        jnz     no_match
        sub     di, MATCH_OFFSET
        mov     si, OFFSET gp_code_spa
        mov     cx, MATCH_BYTES_SPA
        cld
        repe    cmpsb
        jnz     no_match

        mov     game_spa, 1
        mov     game_sg_off, SAVGAM_OFF_SPA

got_match:
        call    announce_hook
        call    hook_gp_exe
        call    add_date_entry

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

;*****************************************************************************
;** Purpose: Hook lap code.
;**
;** Returns: Nothing.
;**
;** Notes  :
;**
        public  hook_gp_exe
hook_gp_exe proc near

        mov     si, OFFSET cs:hook_lap_code
        mov     di, sp
        les     di, ss:[di + ((PUSHA_SIZE + push_end - push_start) * 2) + 2]
        cmp     game_usa, 1
        jz      do_us
        cmp     game_ita, 1
        jz      do_ita
        cmp     game_spa, 1
        jz      do_spa

        sub     di, 2 + HOOK_EUR_LSW
        mov     ax, es
        sub     ax, HOOK_EUR_MSW
        mov     es, ax
        jmp     hook_both

do_ita: sub     di, 2 + HOOK_ITA_LSW
        mov     ax, es
        sub     ax, HOOK_ITA_MSW
        mov     es, ax
        jmp     hook_both

do_spa: sub     di, 2 + HOOK_SPA_LSW
        mov     ax, es
        sub     ax, HOOK_SPA_MSW
        mov     es, ax
        jmp     hook_both

do_us:  sub     di, 2 + HOOK_USA_LSW
        mov     ax, es
        sub     ax, HOOK_USA_MSW
        mov     es, ax

hook_both:
        mov     game_hp_seg, ax
        mov     game_hp_off, di
        mov     cx, HOOK_LENGTH
hook_next_lap_code:
        lodsb
        stosb
        loop    hook_next_lap_code

        ;**
        ;** Pit 1
        ;**
        mov     si, OFFSET cs:hook_pit1_code
        mov     di, sp
        les     di, ss:[di + ((PUSHA_SIZE + push_end - push_start) * 2) + 2]
        cmp     game_usa, 1
        jz      do_us1
        cmp     game_ita, 1
        jz      do_it1
        cmp     game_spa, 1
        jz      do_sp1

        sub     di, 2 + HOOK1_EUR_LSW
        mov     ax, es
        sub     ax, HOOK1_EUR_MSW
        mov     es, ax
        jmp     hook_both1

do_it1: sub     di, 2 + HOOK1_ITA_LSW
        mov     ax, es
        sub     ax, HOOK1_ITA_MSW
        mov     es, ax
        jmp     hook_both1

do_sp1: sub     di, 2 + HOOK1_SPA_LSW
        mov     ax, es
        sub     ax, HOOK1_SPA_MSW
        mov     es, ax
        jmp     hook_both1

do_us1: sub     di, 2 + HOOK1_USA_LSW
        mov     ax, es
        sub     ax, HOOK1_USA_MSW
        mov     es, ax

hook_both1:
        mov     cx, HOOK1_LENGTH
hook_next_lap_code1:
        lodsb
        stosb
        loop    hook_next_lap_code1

        ;**
        ;** Pit 2
        ;**
        mov     si, OFFSET cs:hook_pit2_code
        mov     di, sp
        les     di, ss:[di + ((PUSHA_SIZE + push_end - push_start) * 2) + 2]
        cmp     game_usa, 1
        jz      do_us2
        cmp     game_ita, 1
        jz      do_it2
        cmp     game_spa, 1
        jz      do_sp2

        sub     di, 2 + HOOK2_EUR_LSW
        mov     ax, es
        sub     ax, HOOK2_EUR_MSW
        mov     es, ax
        jmp     hook_both2

do_it2: sub     di, 2 + HOOK2_ITA_LSW
        mov     ax, es
        sub     ax, HOOK2_ITA_MSW
        mov     es, ax
        jmp     hook_both2

do_sp2: sub     di, 2 + HOOK2_SPA_LSW
        mov     ax, es
        sub     ax, HOOK2_SPA_MSW
        mov     es, ax
        jmp     hook_both2

do_us2: sub     di, 2 + HOOK2_USA_LSW
        mov     ax, es
        sub     ax, HOOK2_USA_MSW
        mov     es, ax

hook_both2:
        mov     cx, HOOK2_LENGTH
hook_next_lap_code2:
        lodsb
        stosb
        loop    hook_next_lap_code2

        ;**
        ;** Players AIG
        ;**
        mov     si, OFFSET cs:hook_aig_code
        mov     di, sp
        les     di, ss:[di + ((PUSHA_SIZE + push_end - push_start) * 2) + 2]
        cmp     game_usa, 1
        jz      do_us3
        cmp     game_ita, 1
        jz      do_it3
        cmp     game_spa, 1
        jz      do_sp3

        sub     di, 2 + HOOK3_EUR_LSW
        mov     ax, es
        sub     ax, HOOK3_EUR_MSW
        mov     es, ax
        jmp     hook_both3

do_it3: sub     di, 2 + HOOK3_ITA_LSW
        mov     ax, es
        sub     ax, HOOK3_ITA_MSW
        mov     es, ax
        jmp     hook_both3

do_sp3: sub     di, 2 + HOOK3_SPA_LSW
        mov     ax, es
        sub     ax, HOOK3_SPA_MSW
        mov     es, ax
        jmp     hook_both3

do_us3: sub     di, 2 + HOOK3_USA_LSW
        mov     ax, es
        sub     ax, HOOK3_USA_MSW
        mov     es, ax

hook_both3:
        mov     cx, HOOK3_LENGTH
hook_next_lap_code3:
        lodsb
        stosb
        loop    hook_next_lap_code3

        ;**
        ;** Split Timer Player
        ;**
        mov     si, OFFSET cs:hook_st_code
        mov     di, sp
        les     di, ss:[di + ((PUSHA_SIZE + push_end - push_start) * 2) + 2]
        cmp     game_usa, 1
        jz      do_us4
        cmp     game_ita, 1
        jz      do_it4
        cmp     game_spa, 1
        jz      do_sp4

        mov     dif_seg, DIF_SEG_MOD_EUR
        mov     WORD PTR cs:[fo1], FUNC1_OFF_EUR
        mov     WORD PTR cs:[fo2], FUNC2_OFF_EUR
        sub     di, 2 + HOOK4_EUR_LSW
        mov     ax, es
        sub     ax, HOOK4_EUR_MSW
        mov     es, ax
        jmp     hook_both4


do_it4: mov     dif_seg, DIF_SEG_MOD_ITA
        mov     WORD PTR cs:[fo1], FUNC1_OFF_ITA
        mov     WORD PTR cs:[fo2], FUNC2_OFF_ITA
        sub     di, 2 + HOOK4_ITA_LSW
        mov     ax, es
        sub     ax, HOOK4_ITA_MSW
        mov     es, ax
        jmp     hook_both4

do_sp4: mov     dif_seg, DIF_SEG_MOD_SPA
        mov     WORD PTR cs:[fo1], FUNC1_OFF_SPA
        mov     WORD PTR cs:[fo2], FUNC2_OFF_SPA
        sub     di, 2 + HOOK4_SPA_LSW
        mov     ax, es
        sub     ax, HOOK4_SPA_MSW
        mov     es, ax
        jmp     hook_both4

do_us4: mov     dif_seg, DIF_SEG_MOD_USA
        mov     WORD PTR cs:[fo1], FUNC1_OFF_USA
        mov     WORD PTR cs:[fo2], FUNC2_OFF_USA
        sub     di, 2 + HOOK4_USA_LSW
        mov     ax, es
        sub     ax, HOOK4_USA_MSW
        mov     es, ax

hook_both4:
        push    si
        push    di
        push    ds
        push    es
        mov     si, di
        mov     di, OFFSET cs:hook_st_orig
        mov     ax, es
        mov     ds, ax
        mov     ax, cs
        mov     es, ax
        mov     cx, HOOK4_LENGTH
hook_next_lap_code4a:
        lodsb
        stosb
        loop    hook_next_lap_code4a
        pop     es
        pop     ds
        pop     di
        pop     si
        mov     cx, HOOK4_LENGTH
hook_next_lap_code4:
        lodsb
        stosb
        loop    hook_next_lap_code4

        ;**
        ;** Calculate segment offset to far functions we want to call.
        ;**
        mov     di, sp
        les     di, ss:[di + ((PUSHA_SIZE + push_end - push_start) * 2) + 2]
        mov     ax, es
        sub     ax, dif_seg

        mov     WORD PTR cs:[fs1], ax
        mov     WORD PTR cs:[fs2], ax

        ;**
        ;** Split Timer Computer Cars
        ;**
        mov     si, OFFSET cs:hook_st_code
        mov     di, sp
        les     di, ss:[di + ((PUSHA_SIZE + push_end - push_start) * 2) + 2]
        cmp     game_usa, 1
        jz      do_us5
        cmp     game_ita, 1
        jz      do_it5
        cmp     game_spa, 1
        jz      do_sp5

        sub     di, 2 + HOOK5_EUR_LSW
        mov     ax, es
        sub     ax, HOOK5_EUR_MSW
        mov     es, ax
        jmp     hook_both5

do_it5: sub     di, 2 + HOOK5_ITA_LSW
        mov     ax, es
        sub     ax, HOOK5_ITA_MSW
        mov     es, ax
        jmp     hook_both5

do_sp5: sub     di, 2 + HOOK5_SPA_LSW
        mov     ax, es
        sub     ax, HOOK5_SPA_MSW
        mov     es, ax
        jmp     hook_both5

do_us5: sub     di, 2 + HOOK5_USA_LSW
        mov     ax, es
        sub     ax, HOOK5_USA_MSW
        mov     es, ax

hook_both5:
        mov     cx, HOOK5_LENGTH
hook_next_lap_code5:
        lodsb
        stosb
        loop    hook_next_lap_code5

        ;**
        ;** Players BHP.
        ;**
        mov     si, OFFSET cs:hook_bhp_code
        mov     di, sp
        les     di, ss:[di + ((PUSHA_SIZE + push_end - push_start) * 2) + 2]
        cmp     game_usa, 1
        jz      do_us6
        cmp     game_ita, 1
        jz      do_it6
        cmp     game_spa, 1
        jz      do_sp6

        sub     di, 2 + HOOK6_EUR_LSW
        mov     ax, es
        sub     ax, HOOK6_EUR_MSW
        mov     es, ax
        jmp     hook_both6

do_it6: sub     di, 2 + HOOK6_ITA_LSW
        mov     ax, es
        sub     ax, HOOK6_ITA_MSW
        mov     es, ax
        jmp     hook_both6

do_sp6: sub     di, 2 + HOOK6_SPA_LSW
        mov     ax, es
        sub     ax, HOOK6_SPA_MSW
        mov     es, ax
        jmp     hook_both6

do_us6: sub     di, 2 + HOOK6_USA_LSW
        mov     ax, es
        sub     ax, HOOK6_USA_MSW
        mov     es, ax

hook_both6:
        mov     cx, HOOK6_LENGTH
hook_next_lap_code6:
        lodsb
        stosb
        loop    hook_next_lap_code6

        ;**
        ;** Pit graphics hook.
        ;**
        mov     si, OFFSET cs:update_pit_status_off
        mov     di, sp
        les     di, ss:[di + ((PUSHA_SIZE + push_end - push_start) * 2) + 2]
        cmp     game_usa, 1
        jz      do_us7
        cmp     game_ita, 1
        jz      do_it7
        cmp     game_spa, 1
        jz      do_sp7

        mov     bx, 3830h
        sub     di, 2 + HOOK7_EUR_LSW
        mov     ax, es
        sub     ax, HOOK7_EUR_MSW
        mov     es, ax
        jmp     hook_both7

do_it7: mov     bx, 37fch
        sub     di, 2 + HOOK7_ITA_LSW
        mov     ax, es
        sub     ax, HOOK7_ITA_MSW
        mov     es, ax
        jmp     hook_both7

do_sp7: mov     bx, 3804h
        sub     di, 2 + HOOK7_SPA_LSW
        mov     ax, es
        sub     ax, HOOK7_SPA_MSW
        mov     es, ax
        jmp     hook_both7

do_us7: mov     bx, 3804h
        sub     di, 2 + HOOK7_USA_LSW
        mov     ax, es
        sub     ax, HOOK7_USA_MSW
        mov     es, ax

hook_both7:
        mov     ds:[si], bx
        add     si, 2
        mov     ax, es:[di]
        mov     ds:[si], ax

        ret

hook_gp_exe endp

;*****************************************************************************
;** Purpose: Show user that we've hooked in OK.
;**
;** Returns:
;**
;** Notes  :
;**

        public  announce_hook
announce_hook proc near
        mov     dx, OFFSET hooked_msg
        mov     ah, 09h
        int     DOS
        sti
        mov     delay_count, DELAY_TIME
do_delay:
        cmp     delay_count, 0
        jnz     do_delay
        ret

announce_hook endp

;*****************************************************************************
;** Purpose: Up onto jacks.
;**
;** Returns: Nothing.
;**
;** Notes  :
;**
;** European V1.05 = 06d2eaH - 00571eH = 067bcch
;** Italian  V1.05 = 06d2aaH - 00571eH = 067b8ch
;** US       V1.05 = 06d295H - 00571eH = 067b77h
;** Spanish  V1.05 = 06d2c9H - 00571eH = 067babh
;**

HOOK1_EUR_MSW   equ     6000h
HOOK1_EUR_LSW   equ     7bcch

HOOK1_ITA_MSW   equ     6000h
HOOK1_ITA_LSW   equ     7b8ch

HOOK1_USA_MSW   equ     6000h
HOOK1_USA_LSW   equ     7b77h

HOOK1_SPA_MSW   equ     6000h
HOOK1_SPA_LSW   equ     7babh

        public  hook_pit1
hook_pit1:
        ;**
        ;** Original code.
        ;**
        add     WORD PTR [si+92h], 0190h

        ;**
        ;** Save games flags.
        ;**
        pushf
        push    ax

        ;**
        ;** Save GP.EXE's current DS:SI - Pointer to current car data.
        ;**
        mov     cs:game_si_off, si
        mov     cs:game_si_seg, ds

        ;**
        ;** Switch stacks.
        ;**
        cli
        mov     cs:game_ss, ss
        mov     cs:game_sp, sp
        mov     ax, cs
        mov     ss, ax
        mov     sp, OFFSET my_stack_top
        sti

        ;**
        ;** Save rest of registers.
        ;**
        pusha
        push    es
        push    ds

        ;**
        ;** Our data segment and ensure direction flag is correct.
        ;**
        mov     ax, ds
        mov     es, ax
        mov     ax, cs
        mov     ds, ax
        cld

        ;**
        ;** Replay?
        ;**
        test    BYTE PTR es:[005ah], 80h
        jnz     dont_log1

        ;**
        ;** Are we only logging player's entries?
        ;**
        mov     al, _player_only_flag
        or      al, al
        jz      do_log1
        mov     al, es:[si + 0ach]
        and     al, PLAYERS_CAR
        jnz     do_log1
        jmp     dont_log1

do_log1:
        mov     al, REC_TYPE_PIT_IN
        mov     pi.pi_record_type, al

        call    get_game_mode
        mov     pi.pi_game_mode, ah
        call    get_circuit
        or      pi.pi_game_mode, al

        call    get_car_number
        mov     pi.pi_car_number, al

        mov     al, es:[si + 022h]
        dec     al
        mov     pi.pi_lap_number, al

        mov     al, pi.pi_car_number
        call    find_car_position
        mov     pi.pi_car_position, cl

        mov     ax, es:[si + 092h]
        mov     WORD PTR [pi.pi_in_time], ax
        mov     ax, es:[si + 092h + 2]
        mov     WORD PTR [pi.pi_in_time + 2], ax

        mov     al, pi.pi_car_number
        mov     di, OFFSET cs:pi.pi_driver
        call    copy_drivers_name

        mov     cx, RECORD_LEN
        mov     dx, OFFSET pi
        call    write_entry

dont_log1:
        ;**
        ;** Restore rest of registers.
        ;**
        pop     ds
        pop     es
        popa

        ;**
        ;** Switch stacks.
        ;**
        cli
        mov     ss, cs:game_ss
        mov     sp, cs:game_sp
        sti

        pop     ax
        popf

        retf

;*****************************************************************************
;** Purpose: Off jacks.
;**
;** Returns: Nothing.
;**
;** Notes  :
;**
;** European V1.05 = 06d2eaH - 00591fH = 0679cbh
;** Italian  V1.05 = 06d2aaH - 00591fH = 06798bh
;** US       V1.05 = 06d295H - 00591fH = 067976h
;** Spanish  V1.05 = 06d2c9H - 00591fH = 0679aah
;**

HOOK2_EUR_MSW   equ     6000h
HOOK2_EUR_LSW   equ     79cbh

HOOK2_ITA_MSW   equ     6000h
HOOK2_ITA_LSW   equ     798bh

HOOK2_USA_MSW   equ     6000h
HOOK2_USA_LSW   equ     7976h

HOOK2_SPA_MSW   equ     6000h
HOOK2_SPA_LSW   equ     79aah


        public  hook_pit2
hook_pit2:
        ;**
        ;** Original code.
        ;**
        or      BYTE PTR [si+0b3h], 20h
        ;**
        ;** Save games flags.
        ;**
        pushf
        push    ax

        ;**
        ;** Save GP.EXE's current DS:SI - Pointer to current car data.
        ;**
        mov     cs:game_si_off, si
        mov     cs:game_si_seg, ds

        ;**
        ;** Switch stacks.
        ;**
        cli
        mov     cs:game_ss, ss
        mov     cs:game_sp, sp
        mov     ax, cs
        mov     ss, ax
        mov     sp, OFFSET my_stack_top
        sti

        ;**
        ;** Save rest of registers.
        ;**
        pusha
        push    es
        push    ds

        ;**
        ;** Our data segment and ensure direction flag is correct.
        ;**
        mov     ax, ds
        mov     es, ax
        mov     ax, cs
        mov     ds, ax
        cld

        ;**
        ;** Replay?
        ;**
        test    BYTE PTR es:[005ah], 80h
        jnz     dont_log2

        ;**
        ;** Are we only logging player's entries?
        ;**
        mov     al, _player_only_flag
        or      al, al
        jz      do_log2
        mov     al, es:[si + 0ach]
        and     al, PLAYERS_CAR
        jnz     do_log2
        jmp     dont_log2

do_log2:
        mov     al, REC_TYPE_PIT_OUT
        mov     po.po_record_type, al

        call    get_game_mode
        mov     po.po_game_mode, ah
        call    get_circuit
        or      po.po_game_mode, al

        call    get_car_number
        mov     po.po_car_number, al

        mov     al, es:[si + 022h]
        dec     al
        mov     po.po_lap_number, al


        mov     al, po.po_car_number
        call    find_car_position
        mov     po.po_car_position, cl

        mov     ax, es:[si + 092h]
        mov     WORD PTR [po.po_out_time], ax
        mov     ax, es:[si + 092h + 2]
        mov     WORD PTR [po.po_out_time + 2], ax

        mov     al, po.po_car_number
        mov     di, OFFSET cs:po.po_driver
        call    copy_drivers_name

        mov     cx, RECORD_LEN
        mov     dx, OFFSET po
        call    write_entry

dont_log2:
        ;**
        ;** Restore rest of registers.
        ;**
        pop     ds
        pop     es
        popa

        ;**
        ;** Switch stacks.
        ;**
        cli
        mov     ss, cs:game_ss
        mov     sp, cs:game_sp
        sti

        pop     ax
        popf

        retf

;*****************************************************************************
;** Purpose: AIG.
;**
;** Returns: Nothing.
;**
;** Notes  :
;**
;** European V1.05 = 06d2eaH - 0062e0H = 06700ah
;** Italian  V1.05 = 06d2aaH - 0062e0H = 066fcah
;** US       V1.05 = 06d295H - 0062e0H = 066fb5h
;** Spanish  V1.05 = 06d2c9H - 0062e0H = 066fe9h
;**

HOOK3_EUR_MSW   equ     6000h
HOOK3_EUR_LSW   equ     700ah

HOOK3_ITA_MSW   equ     6000h
HOOK3_ITA_LSW   equ     6fcah

HOOK3_USA_MSW   equ     6000h
HOOK3_USA_LSW   equ     6fb5h

HOOK3_SPA_MSW   equ     6000h
HOOK3_SPA_LSW   equ     6fe9h

        public  hook_aig
hook_aig:
        ;**
        ;** Check to see if player's car?
        ;** If it is then ensure standard braking value, not enhanced AIG.
        ;**
        test    BYTE PTR [si+0ach], 80h
        jz      not_players_aig
        mov     WORD PTR [si+0beh], 4000h
not_players_aig:
        ;**
        ;** Original code.
        ;**
        mov     di, [bp+1222h]
        shl     di, 1
        retf

;*****************************************************************************
;** Purpose: BHP.
;**
;** Returns: Nothing.
;**
;** Notes  :
;**
;** European V1.05 = 06d2eaH - 00618aH = 67160h
;** Italian  V1.05 = 06d2aaH - 00618aH = 67120h
;** US       V1.05 = 06d295H - 00618aH = 6710bh
;** Spanish  V1.05 = 06d2c9H - 00618aH = 6713fh
;**

HOOK6_EUR_MSW   equ     6000h
HOOK6_EUR_LSW   equ     7160h

HOOK6_ITA_MSW   equ     6000h
HOOK6_ITA_LSW   equ     7120h

HOOK6_USA_MSW   equ     6000h
HOOK6_USA_LSW   equ     710bh

HOOK6_SPA_MSW   equ     6000h
HOOK6_SPA_LSW   equ     713fh

        public  hook_bhp
hook_bhp:
        ;**
        ;** Do nothing as our patch removes the players randomisation.
        ;**
        retf

;*****************************************************************************
;** Purpose: Split Timer.
;**
;** Returns: Nothing.
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
;** European V1.05 = 06d2eaH - 002d1dH = 06a5cdh
;** Italian  V1.05 = 06d2aaH - 002d1dH = 06a58dh
;** US       V1.05 = 06d295H - 002d1dH = 06a578h
;** Spanish  V1.05 = 06d2c9H - 002d1dH = 06a5ach
;**
;** European V1.05 = 06d2eaH - 007cccH = 06561eh
;** Italian  V1.05 = 06d2aaH - 007cccH = 0655deh
;** US       V1.05 = 06d295H - 007cccH = 0655c9h
;** Spanish  V1.05 = 06d2c9H - 007cccH = 0655fdh
;**

HOOK4_EUR_MSW   equ     6000h
HOOK4_EUR_LSW   equ     0a5cdh

HOOK4_ITA_MSW   equ     6000h
HOOK4_ITA_LSW   equ     0a58dh

HOOK4_USA_MSW   equ     6000h
HOOK4_USA_LSW   equ     0a578h

HOOK4_SPA_MSW   equ     6000h
HOOK4_SPA_LSW   equ     0a5ach

HOOK5_EUR_MSW   equ     6000h
HOOK5_EUR_LSW   equ     561eh

HOOK5_ITA_MSW   equ     6000h
HOOK5_ITA_LSW   equ     55deh

HOOK5_USA_MSW   equ     6000h
HOOK5_USA_LSW   equ     55c9h

HOOK5_SPA_MSW   equ     6000h
HOOK5_SPA_LSW   equ     55fdh

h3:     jmp     h1

        public  hook_st
hook_st:
        pusha
        push    ds
        push    es

        ;**
        ;** Player's car on grass? (Ignore linked player).
        ;**
        test    BYTE PTR [si + 0ach], 80h
        jz      st_not_players_car
        test    BYTE PTR [si + 0ach], 40h
        jnz     st_not_players_car
        cmp     BYTE PTR ds:[22fbh], 3
        je      on_grass
        cmp     BYTE PTR ds:[22fch], 3
        jne     st_not_players_car
on_grass:
        mov     cs:grass, 1

        ;**
        ;** Turn on red cross to indicate invalid lap.
        ;** Ignore if this is a race or not currently displayed car.
        ;**
        test    byte ptr ss:[124ah], 80h
        jnz     st_not_players_car
        cmp     si, ds:[097dh]
        jne     st_not_players_car

        mov     ax, 0eh
        call    update_pit_status

st_not_players_car:

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
h1f:    jmp     h1
h2f:    jmp     h2
h4:
        mov     cs:last_value, ax

        ;**
        ;** Ignore if this an out lap
        ;**
        test    WORD PTR [si+56h], 0c000h
        jnz     h3

        ;**
        ;** Are we at split section time point?
        ;**
        mov     cx, 1
        mov     ax, ss:[015ch]
        sub     ax, 0030h
        mov     bx, 002eh
        mov     di, 100
        xor     dx, dx
        div     bx
        mov     cs:track_divs,ax

        imul    cs:_split1
        div     di
        imul    bx
        add     ax, 0030h
        cmp     ax, [si+12h]
        je      h2f

        inc     cx
        mov     ax, cs:track_divs
        imul    cs:_split2
        div     di
        imul    bx
        add     ax, 0030h
        cmp     ax, [si+12h]
        je      h2f

        inc     cx
        mov     ax, cs:track_divs
        imul    cs:_split3
        div     di
        imul    bx
        add     ax, 0030h
        cmp     ax, [si+12h]
        jne     h1f

        ;**
        ;** Log split time if player, CX=1,2,3 split index.
        ;**
h2:     test    BYTE PTR [si+0ach], 80h
        jnz     h6
        jmp     h5

h6:
        ;**
        ;** Save GP.EXE's current DS:SI - Pointer to current car data.
        ;**
        mov     cs:game_si_off, si
        mov     cs:game_si_seg, ds

        ;**
        ;** Switch stacks.
        ;**
        cli
        mov     cs:game_ss, ss
        mov     cs:game_sp, sp
        mov     ax, cs
        mov     ss, ax
        mov     sp, OFFSET my_stack_top
        sti

        ;**
        ;** Save rest of registers.
        ;**
        pusha
        push    ds
        push    es
        cld

        mov     ax, ds
        mov     es, ax
        mov     ax, cs
        mov     ds, ax

        mov     st.st_point, cl
        dec     cx
        add     cx, cx
        mov     bx, OFFSET cs:splits
        add     bx, cx
        mov     ax, cs:[bx]
        mov     st.st_percentage, ax

        mov     al, REC_TYPE_SPLIT
        mov     st.st_record_type, al

        call    get_game_mode
        mov     st.st_game_mode, ah
        call    get_circuit
        or      st.st_game_mode, al

        call    get_car_number
        mov     st.st_car_number, al

        mov     al, es:[si + 022h]
        dec     al
        mov     st.st_lap_number, al

        mov     al, st.st_car_number
        call    find_car_position
        mov     st.st_car_position, cl

        mov     al, st.st_car_number
        call    find_qual_position
        mov     st.st_qual_position, cl

        mov     ax, es:[294fh]
        mov     dx, es:[2951h]
        sub     ax, es:[si+54h]
        sbb     dx, es:[si+56h]
        mov     WORD PTR [st.st_split_time], ax
        mov     WORD PTR [st.st_split_time + 2], dx
        call    modtime

        mov     al, st.st_car_number
        mov     di, OFFSET cs:st.st_driver
        call    copy_drivers_name

        mov     cx, RECORD_LEN
        mov     dx, OFFSET st
        call    write_entry

        ;**
        ;** Restore rest of registers.
        ;**
        pop     es
        pop     ds
        popa

        ;**
        ;** Switch stacks.
        ;**
        cli
        mov     ss, cs:game_ss
        mov     sp, cs:game_sp
        sti

        ;**
        ;** Hold lap-time display.
        ;**
h5:     push    ds:[294fh]      ; Game's main timer
        pop     ds:[001ah]
        push    ds:[2951h]
        pop     ds:[001ch]

        mov     cx, cs:_freeze_time
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
        call    modtime

        ;**
        ;** call ????:???? (func2)
        ;**
        db      9ah
fo2:    dw      ?
fs2:    dw      ?

h1:     pop     es
        pop     ds
        popa

        ;**
        ;** Original code (could be GPPERF patch code).
        ;**
        public  hook_st_orig
hook_st_orig:
        db      HOOK4_LENGTH dup(0)

        retf

;*****************************************************************************
;** Purpose: Pit status graphics.
;**
;** Returns: Nothing.
;**
;** Notes  :
;**
;** European V1.05 = 06d2eaH - 003424H = 69ec6h
;** Italian  V1.05 = 06d2aaH - 003424H = 69e86h
;** US       V1.05 = 06d295H - 003424H = 69e71h
;** Spanish  V1.05 = 06d2c9H - 003424H = 69ea5h
;**

HOOK7_EUR_MSW   equ     6000h
HOOK7_EUR_LSW   equ     9ec6h

HOOK7_ITA_MSW   equ     6000h
HOOK7_ITA_LSW   equ     9e86h

HOOK7_USA_MSW   equ     6000h
HOOK7_USA_LSW   equ     9e71h

HOOK7_SPA_MSW   equ     6000h
HOOK7_SPA_LSW   equ     9ea5h

update_pit_status proc near

        ;**
        ;** CALLF ????:3830H
        ;**
        db      09ah
update_pit_status_off:
        dw      ?
update_pit_status_seg:
        dw      ?
        ret

update_pit_status endp

;****************************************************************************
;**
;** 2CEB:1D77  F6 86 124A FF test byte ptr [bp+124Ah],0FFh @003577h
;**
;** European V1.05 = 06d2eaH - 003577h = 069d73h
;** Italian  V1.05 = 06d2aaH - 003577h = 069d33h
;** US       V1.05 = 06d295H - 003577h = 069d1eh
;** Spanish  V1.05 = 06d2c9H - 003577H = 069d52h
;**
;** Players BHP - European/US V1.05 = 006188h - 003577h = 002c11h
;**
;** AI grip     - European/US V1.05 = 0062dah - 003577h = 002d63h
;**
;** Save Game   - European    V1.05 = 02b95ch
;** Save Game   - US          V1.05 = 02b92ch
;**
;** Skill       - European    V1.05 = 02b962h
;** Skill       - US          V1.05 = 02b932h
;**
;** Traction    - European    V1.05 = 02b85dh
;** Traction    - US          V1.05 = 02b82dh
;**
;** Aids Help   - European    V1.05 = 02b960h
;** Aids Help   - US          V1.05 = 02b930h
;**
;** Aids Mask   - European    V1.05 = 01fd36h
;** Aids Mask   - US          V1.05 = 01fd06h
;**
;** Steering As - European    V1.05 = 02b85ch
;** Steering As - US          V1.05 = 02b82ch
;**
;** Circuit     - European    V1.05 = 02b976h
;** Circuit     - US          V1.05 = 02b946h
;**
;** Play Mode   - European    V1.05 = 02b98ah
;** Play Mode   - US          V1.05 = 02b95ah
;**
;** Car Setup   - European    V1.05 = 2b9c6h + (# * 10t)
;** Car Setup   - US          V1.05 = 2b996h + (# * 10t)
;**

HOOK_EUR_MSW    equ     6000h
HOOK_EUR_LSW    equ     9d73h

HOOK_ITA_MSW    equ     6000h
HOOK_ITA_LSW    equ     9d33h

HOOK_USA_MSW    equ     6000h
HOOK_USA_LSW    equ     9d1eh

HOOK_SPA_MSW    equ     6000h
HOOK_SPA_LSW    equ     9d52h

BHP_OFFSET      equ     2c11h
AIG_OFFSET      equ     2d63h

;**
;** Specify all as US values.
;**
HOOK_SEG        equ     0357h
HOOK_OFF        equ     0007h
SKILL_SEG       equ     2b93h
SKILL_OFF       equ     0002h
TRACT_SEG       equ     2b82h
TRACT_OFF       equ     000dh
AIDS_SEG        equ     2b93h
AIDS_OFF        equ     0000h
AIDS_MASK_SEG   equ     1fd0h
AIDS_MASK_OFF   equ     0006h
SA_SEG          equ     2b82h
SA_OFF          equ     000ch
CIRCUIT_SEG     equ     2b94h
CIRCUIT_OFF     equ     0006h
GAME_MODE_SEG   equ     2b95h
GAME_MODE_OFF   equ     000ah
CAR_SETUP_SEG   equ     2b99h
CAR_SETUP_OFF   equ     0006h

        public  hook_lap
hook_lap:
        ;**
        ;** GP.EXE original code
        ;**
        test byte ptr [bp+124Ah],0FFh

        ;**
        ;** Save games flags.
        ;**
        pushf
        push    ax

        ;**
        ;** Save GP.EXE's current DS:SI - Pointer to current car data.
        ;**
        mov     cs:game_si_off, si
        mov     cs:game_si_seg, ds

        ;**
        ;** Switch stacks.
        ;**
        cli
        mov     cs:game_ss, ss
        mov     cs:game_sp, sp
        mov     ax, cs
        mov     ss, ax
        mov     sp, OFFSET my_stack_top
        sti

        ;**
        ;** Save rest of registers.
        ;**
        pusha
        push    es
        push    ds

        ;**
        ;** Our data segment and ensure direction flag is correct.
        ;**
        mov     ax, ds
        mov     es, ax
        mov     ax, cs
        mov     ds, ax
        cld

        ;**
        ;** Replay?
        ;**
        test    BYTE PTR es:[005ah], 80h
        jz      not_replay
        jmp     replay
not_replay:

        ;**
        ;** Record RTC time for end of this lap (ignore linked player).
        ;**
        les     si, game_si
        test    BYTE PTR es:[si + 0ach], 80h
        jz      no_rtc1
        test    BYTE PTR es:[si + 0ach], 40h
        jnz     no_rtc1
        mov     ah, 2ch
        int     DOS
        mov     cl_secs.secs_hours, ch
        mov     cl_secs.secs_minutes, cl
        mov     cl_secs.secs_seconds, dh
no_rtc1:

        ;**
        ;** Fill in record.
        ;**
        les     si, game_si

        ;**
        ;** Are we only logging player's entries?
        ;**
        mov     al, _player_only_flag
        or      al, al
        jz      do_log
        mov     al, es:[si + 0ach]
        and     al, PLAYERS_CAR
        jnz     do_log
        jmp     dont_log

do_log: mov     al, REC_TYPE_LAP
        mov     lr.lr_record_type, al

        call    get_game_mode
        mov     lr.lr_game_mode, ah
        call    get_circuit
        or      lr.lr_game_mode, al

        call    get_car_number
        mov     lr.lr_car_number, al

        mov     al, es:[si + 022h]
        dec     al
        mov     lr.lr_lap_number, al

        mov     al, lr.lr_car_number
        call    find_car_position
        mov     lr.lr_car_position, cl

        mov     al, lr.lr_car_number
        call    find_qual_position
        mov     lr.lr_qual_position, cl

        mov     ax, es:[si + 040h]
        mov     WORD PTR [lr.lr_lap_time], ax
        mov     ax, es:[si + 040h + 2]
        mov     WORD PTR [lr.lr_lap_time + 2], ax

        mov     ax, es:[si + 0aeh]
        mov     WORD PTR [lr.lr_best_time], ax
        mov     ax, es:[si + 0aeh + 2]
        mov     WORD PTR [lr.lr_best_time + 2], ax

        mov     al, lr.lr_car_number
        and     al, 3fh
        xor     ah, ah
        sub     al, 1
        shl     ax, 2
        mov     di, ax
        add     di, 1982h
        push    es
        mov     es, game_ss
        mov     ax, es:[di]
        mov     WORD PTR [lr.lr_race_time], ax
        mov     ax, es:[di + 2]
        mov     WORD PTR [lr.lr_race_time + 2], ax
        pop     es

        mov     al, lr.lr_car_number
        mov     di, OFFSET cs:lr.lr_driver
        call    copy_drivers_name

        mov     si, OFFSET lr
        mov     cx, CRC_LEN
        call    sum_check
        mov     lr.lr_crc, ax

        ;**
        ;** Is it player's car and a new best lap?
        ;** Don't log authentication records for linked player.
        ;**
        mov     cx, RECORD_LEN
        les     si, game_si
        test    BYTE PTR es:[si + 0ach], 80h
        jz      npc
        test    BYTE PTR es:[si + 0ach], 40h
        jnz     npc
        jmp     is_player
npc:    jmp     not_players_car
is_player:
        test    WORD PTR [lr.lr_lap_time + 2], 0C000h
        jz      is_best_lap
        jmp     log_setup
is_best_lap:
        ;**
        ;** Is it 100% race distance?
        ;**
        call    get_circuit
        shr     al, 4
        and     al, 0fh
        xor     ah, ah
        mov     bx, OFFSET cs:full_race_distance
        add     bx, ax
        mov     al, [bx]
        inc     al
        les     si, game_si
        cmp     al, es:[si + 022h]
        je      best_lap

        ;**
        ;** Is it 50% race distance?
        ;**
        call    get_circuit
        shr     al, 4
        and     al, 0fh
        xor     ah, ah
        mov     bx, OFFSET cs:half_race_distance
        add     bx, ax
        mov     al, [bx]
        inc     al
        les     si, game_si
        cmp     al, es:[si + 022h]
        je      best_lap

        ;**
        ;** Is it new best time.
        ;**
        mov     ax, WORD PTR [lr.lr_lap_time]
        cmp     ax, WORD PTR [lr.lr_best_time]
        jnz     not_best_lap
        mov     ax, WORD PTR [lr.lr_lap_time + 2]
        cmp     ax, WORD PTR [lr.lr_best_time + 2]
        jnz     not_best_lap
        jmp     best_lap
not_best_lap:
        mov     cx, RECORD_LEN
        jmp     not_best_lap1

        ;**
        ;** Best lap, write authenticated entry.
        ;**
best_lap:
        mov     al, lr.lr_record_type
        and     al, CIR_MASK
        or      al, REC_TYPE_AUTH
        mov     ar.ar_record_type, al
        mov     ax, WORD PTR [lr.lr_best_time]
        mov     WORD PTR [ar.ar_lap_time], ax
        mov     ax, WORD PTR [lr.lr_best_time + 2]
        mov     WORD PTR [ar.ar_lap_time + 2], ax
        mov     al, lr.lr_game_mode
        mov     ar.ar_game_mode, al

        les     bx, game_hp
        add     bx, BHP_OFFSET
        mov     ax, es:[bx]
        sub     ax, 632
        xor     dx, dx
        mov     cx, 22
        div     cx
        mov     ar.ar_bhp, ax

        les     bx, game_hp
        add     bx, AIG_OFFSET
        xor     dx, dx
        mov     ax, es:[bx]
        cmp     ax, 4000h
        jb      new_aig_mod
        sub     ax, 4000h
new_aig_mod:
        mov     cx, 100
        div     cx
        add     ax, 1
        mov     ar.ar_ai_grip, al

        mov     dx, SKILL_SEG
        mov     ax, SKILL_OFF
        call    get_sg_byte
        mov     ar.ar_skill, al

        mov     ar.ar_drv_aids, 0
        mov     dx, TRACT_SEG
        mov     ax, TRACT_OFF
        call    get_sg_byte
        and     al, 80h
        or      ar.ar_drv_aids, al

        mov     dx, AIDS_SEG
        mov     ax, AIDS_OFF
        call    get_sg_byte
        and     al, 3fh
        or      ar.ar_drv_aids, al

        xor     bh, bh
        mov     bl, ar.ar_skill

        mov     dx, AIDS_MASK_SEG
        mov     ax, AIDS_MASK_OFF
        add     ax, bx
        call    get_sg_byte
        or      al, 0c0h
        and     ar.ar_drv_aids, al

        mov     dx, SA_SEG
        mov     ax, SA_OFF
        call    get_sg_byte
        and     al, 80h
        shr     al, 1
        or      ar.ar_drv_aids, al

        mov     ah, 2ah
        int     DOS
        mov     ar.ar_year, cx
        mov     ar.ar_month, dh
        mov     ar.ar_day, dl
        mov     ah, 2ch
        int     DOS
        mov     ar.ar_hours, ch
        mov     ar.ar_minutes, cl
        and     dh, 03fh
        mov     ar.ar_seconds, dh

        push    ds
        pop     es
        mov     si, OFFSET lr.lr_driver
        mov     di, OFFSET ar.ar_driver
        mov     cx, 24
copy_auth_driver:
        lodsb
        stosb
        loop    copy_auth_driver

        ;**
        ;** Determine if an illegal front/rear wing was used.
        ;**
        les     si, game_si
        cmp     BYTE PTR es:[si + 9eh], 40h
        jg      illegal_wing
        cmp     BYTE PTR es:[si + 9fh], 40h
        jle     wings_ok
illegal_wing:
        call    clear_secs
        or      ar.ar_seconds, 01h

wings_ok:
        ;**
        ;** Determine if illegal tyres were used.
        ;**
        cmp     BYTE PTR es:[si + 0b2h], 05h
        jle     tyres_ok
        call    clear_secs
        or      ar.ar_seconds, 02h

tyres_ok:
        ;**
        ;** Ensure game wasn't paused or reloaded.
        ;** Last lap should always be valid for a flying lap time,
        ;** if it isn't it must be a reload after just loading us.
        ;**
        cmp     ll_secs.secs_hours, 255
        je      illegal_reload

        ;**
        ;** Convert values to seconds, if time wrapped over midnight
        ;** then bump it up.
        ;**
        mov     di, OFFSET ll_secs
        call    convert_rtc
        mov     di, OFFSET cl_secs
        call    convert_rtc

        cmp     ll_secs.secs_hours, 23
        jne     not_midnight
        cmp     cl_secs.secs_hours, 00
        jne     not_midnight

        ;**
        ;** 86400 secs per day = 00015180H
        ;**
        add     ll_secs.secs_total, 5180H
        adc     ll_secs.secs_total+2, 0001H
not_midnight:

        ;**
        ;** Subtract the times then convert it to ms
        ;**
        mov     ax, cl_secs.secs_total
        mov     dx, cl_secs.secs_total+2
        sub     ax, ll_secs.secs_total
        sbb     dx, ll_secs.secs_total+2
        mov     bx, 1000
        mul     bx

        ;**
        ;** And compare it to games time, it must be within 10s.
        ;**
        sub     ax, WORD PTR [lr.lr_best_time]
        sbb     dx, WORD PTR [lr.lr_best_time + 2]
        test    dx, 8000h
        jz      no_invert_diff
        mov     bx, ax
        mov     cx, dx
        xor     ax, ax
        xor     dx, dx
        sub     ax, bx
        sbb     dx, cx

no_invert_diff:
        cmp     dx, 0
        jne     gt_30s
        test    ax, 8000h
        jnz     gt_30s
        cmp     ax, (10 * 1000)
        jle     no_illegal_reload
illegal_reload:
        call    clear_secs
        or      ar.ar_seconds, 04h
        jmp     reload_check_done
no_illegal_reload:
        cmp     ax, (30 * 1000)
        jle     reload_check_done
gt_30s: call    clear_secs
        or      ar.ar_seconds, 08h
reload_check_done:

        ;**
        ;** Check BHP
        ;**
        les     si, game_si
        cmp     es:[si+ 0a8h], 4000h
        jle     bhp_ok
        call    clear_secs
        or      ar.ar_seconds, 10h
bhp_ok:

        ;**
        ;** Check if car went onto grass during lap.
        ;**
        cmp     grass, 1
        jne     grass_ok
        call    clear_secs
        or      ar.ar_seconds, 20h
grass_ok:

        ;**
        ;** Check damage values.
        ;**
        mov     cx, DAMAGE_COUNT
        mov     si, OFFSET damage_defaults
        les     di, game_si
        mov     di, DAMAGE_OFFSET
        cld
        repe    cmpsb
        jz      damage_ok
        call    clear_secs
        or      ar.ar_seconds, 40h
damage_ok:

        ;**
        ;** Calculate auth record CRC.
        ;**
        mov     si, OFFSET ar
        mov     cx, CRC_LEN
        call    sum_check
        mov     ar.ar_crc, ax

        ;**
        ;** Is it 100% race distance?
        ;**
        call    get_circuit
        shr     al, 4
        and     al, 0fh
        xor     ah, ah
        mov     bx, OFFSET cs:full_race_distance
        add     bx, ax
        mov     al, [bx]
        inc     al
        les     si, game_si
        cmp     al, es:[si + 022h]
        je      end_of_race

        ;**
        ;** Is it 50% race distance?
        ;**
        call    get_circuit
        shr     al, 4
        and     al, 0fh
        xor     ah, ah
        mov     bx, OFFSET cs:half_race_distance
        add     bx, ax
        mov     al, [bx]
        inc     al
        les     si, game_si
        cmp     al, es:[si + 022h]
        je      end_of_race

        ;**
        ;** Write lap & auth record in one go.
        ;**
        mov     cx, RECORD_LEN * 2
        jmp     common_write

        ;**
        ;** End of race, copy dummy FL and log race time,
        ;** and log fastest race lap as well.
        ;**
end_of_race:
        push    ds
        pop     es
        mov     si, OFFSET ar
        mov     di, OFFSET rr
        mov     cx, RECORD_LEN
eor1:   lodsb
        stosb
        loop    eor1

        mov     al, rr.rr_record_type
        and     al, CIR_MASK
        or      al, REC_TYPE_AUTH_RACE
        mov     rr.rr_record_type, al
        mov     ax, WORD PTR [lr.lr_race_time]
        mov     dx, WORD PTR [lr.lr_race_time + 2]
        mov     WORD PTR [rr.rr_race_time], ax
        mov     WORD PTR [rr.rr_race_time + 2], dx
        mov     si, OFFSET rr
        mov     cx, CRC_LEN
        call    sum_check
        mov     rr.ar_crc, ax

        mov     ax, WORD PTR [lr.lr_best_time]
        mov     dx, WORD PTR [lr.lr_best_time + 2]
        mov     WORD PTR [ar.ar_lap_time], ax
        mov     WORD PTR [ar.ar_lap_time + 2], dx
        mov     si, OFFSET ar
        mov     cx, CRC_LEN
        call    sum_check
        mov     ar.ar_crc, ax

        mov     cx, RECORD_LEN * 3
        jmp     common_write

        ;**
        ;** Copy car setup.
        ;**
log_setup:
        mov     al, REC_TYPE_SETUP
        mov     sr.sr_record_type, al

        call    get_game_mode
        mov     sr.sr_game_mode, ah
        call    get_circuit
        or      sr.sr_game_mode, al

        mov     al, lr.lr_car_number
        and     al, 3fh
        xor     ah, ah
        sub     al, 1
        mov     dx, SETUP_SRC_LEN
        mul     dx

        mov     dx, CAR_SETUP_SEG
        add     ax, CAR_SETUP_OFF

        mov     di, OFFSET sr.sr_front_wing
        mov     cx, SETUP_SRC_LEN
copy_setup:
        push    ax
        call    get_sg_byte
        mov     [di], al
        pop     ax
        inc     ax
        inc     di
        loop    copy_setup

        ;**
        ;** Current tyres.
        ;**
        les     si, game_si
        mov     al, es:[si + 0b2h]
        and     al, 07h
        mov     sr.sr_using_tyres, al

        ;**
        ;** Write setup entry, then fall through and write lap entry.
        ;**
        mov     cx, RECORD_LEN
        mov     dx, OFFSET sr
        call    write_entry

        mov     cx, RECORD_LEN

not_players_car:
not_best_lap1:
common_write:
        ;**
        ;** Common write of entries.
        ;**
        mov     dx, OFFSET lr
        call    write_entry

dont_log:
        ;**
        ;** Move current RTC lap time to last lap time (ignore linked player).
        ;**
        les     si, game_si
        test    BYTE PTR es:[si + 0ach], 80h
        jz      no_rtc2
        test    BYTE PTR es:[si + 0ach], 40h
        jnz     no_rtc2
        mov     ch, cl_secs.secs_hours
        mov     ll_secs.secs_hours, ch
        mov     cl, cl_secs.secs_minutes
        mov     ll_secs.secs_minutes, cl
        mov     dh, cl_secs.secs_seconds
        mov     ll_secs.secs_seconds, dh

        ;**
        ;** Clear on the grass flag.
        ;**
        cmp     grass, 1
        jne     no_rtc2
        mov     grass, 0

        ;**
        ;** Restore rest of registers.
        ;**
        pop     ds
        pop     es
        popa

        ;**
        ;** Switch stacks.
        ;**
        cli
        mov     ss, cs:game_ss
        mov     sp, cs:game_sp
        sti

        test    byte ptr ss:[124ah], 80h
        jnz     not_race_grass
        cmp     si, ds:[097dh]
        jne     not_race_grass

        mov     ax, 03h
        test    BYTE PTR ds:[si + 0b3h], 04h
        jz      pit_request_off
        mov     ax, 0fh
pit_request_off:
        call    update_pit_status

        jmp     not_race_grass

no_rtc2:
replay:
        ;**
        ;** Restore rest of registers.
        ;**
        pop     ds
        pop     es
        popa

        ;**
        ;** Switch stacks.
        ;**
        cli
        mov     ss, cs:game_ss
        mov     sp, cs:game_sp
        sti

not_race_grass:
        pop     ax
        popf

        retf

;*****************************************************************************
;** Purpose:    To clear seconds field if this is the first failure.
;**
;** Returns:    Nothing.
;**
;** Notes  :
;**
        public  clear_secs

clear_secs proc near

        test    BYTE PTR ar.ar_seconds, 80h
        jnz     already_set
        mov     BYTE PTR ar.ar_seconds, 80h

already_set:
        ret

clear_secs endp

;*****************************************************************************
;** Purpose:    Convert RTC HH:MM:SS to seconds.
;**
;** Returns:    Nothing.
;**
;** Notes  :
;**
        public  convert_rtc

convert_rtc proc near
        xor     dx, dx
        xor     ax, ax
        mov     al, [di+secs_hours]
        mov     bx, 3600
        mul     bx
        mov     [di+secs_total], ax
        mov     [di+secs_total+2], dx
        xor     dx, dx
        xor     ax, ax
        mov     al, [di+secs_minutes]
        mov     bx, 60
        mul     bx
        add     [di+secs_total], ax
        adc     [di+secs_total+2], dx
        xor     ax, ax
        mov     al, [di+secs_seconds]
        add     [di+secs_total], ax
        adc     [di+secs_total+2], 0
        ret

convert_rtc endp

;*****************************************************************************
;** Purpose: Get car number.
;**
;** Returns: Car number is AL (with player and CC control)
;**
;** Notes  : Leaves ES:SI pointing at car.
;**

        public  get_car_number
get_car_number proc near

        les     si, game_si
        mov     al, es:[si + 0ach]
        and     al, 0bfh
        mov     ah, es:[si + 7eh]
        and     ah, 04h
        shl     ah, 4
        or      al, ah
        ret

get_car_number endp

;*****************************************************************************
;** Purpose: Obtains the current game mode.
;**
;** Returns: GM_* in AH.
;**
;** Notes  :
;**

        public  get_game_mode
get_game_mode proc near

        mov     dx, GAME_MODE_SEG
        mov     ax, GAME_MODE_OFF
        call    get_sg_byte
        test    al, 01h                         ; 01h = Free Practice
        jz      tst1
        mov     ah, GM_FREE_PRACTICE
        jmp     tstend
tst1:   test    al, 40h                         ; 40h = Qualifying
        jz      tst2
        mov     ah, GM_QUALIFYING
        jmp     tstend
tst2:   test    al, 02h                         ; 02h = Pre-Race Practice
        jz      tst3
        mov     ah, GM_PRE_RACE_PRACTICE
        jmp     tstend
tst3:   test    al, 80h
        jz      tst4
        mov     ah, GM_RACE
        jmp     tstend
tst4:   mov     ah, GM_PRACTICE
tstend: ret

get_game_mode endp

;*****************************************************************************
;** Purpose: Find car position.
;**
;** Returns: Car position in CL
;**
;** Notes  : Input AL = Car number.
;**

        public  find_car_position
find_car_position proc near

        push    es
        and     al, 3fh
        mov     es, game_ss
        mov     di, 1940h
        mov     cl, 1
fcp1:   mov     ah, es:[di]
        and     ah, 3fh
        cmp     al, ah
        jz      fcp2
        inc     di
        inc     cl
        cmp     cl, 26
        jle     fcp1
        xor     cl, cl
fcp2:   pop     es
        ret

find_car_position endp

;*****************************************************************************
;** Purpose: Find car qualifying position.
;**
;** Returns: Car position in CL
;**
;** Notes  : Input AL = Car number.
;**

        public  find_qual_position
find_qual_position proc near

        push    es
        and     al, 3fh
        mov     es, game_ss
        mov     di, 17d8h
        mov     cl, 1
fqp1:   mov     ah, es:[di]
        and     ah, 3fh
        cmp     al, ah
        jz      fqp2
        inc     di
        inc     cl
        cmp     cl, 26
        jle     fqp1
        xor     cl, cl
fqp2:   pop     es
        ret

find_qual_position endp

;*****************************************************************************
;** Purpose: Copy driver's name.
;**
;** Returns: Nothing.
;**
;** Notes  : Input AL = car number, DI = destination.
;**

        public  copy_drivers_name
copy_drivers_name proc near

        and     al, 3fh
        xor     ah, ah
        sub     al, 1
        mov     dx, 24
        mul     dx
        mov     si, ax
        add     si, 1a4ah
        push    ds

        push    ds
        pop     es

        mov     ds, cs:game_ss

        mov     cx, 24
cdn1:   lodsb
        stosb
        loop    cdn1

        pop     ds
        ret

copy_drivers_name endp

;*****************************************************************************
;** Purpose: Get circuit.
;**
;** Returns: AL = Circuit
;**
;** Notes  :
;**

        public  get_circuit
get_circuit proc near

        mov     dx, CIRCUIT_SEG
        mov     ax, CIRCUIT_OFF
        call    get_sg_byte
        shl     al, 4
        and     al, CIR_MASK
        ret

get_circuit endp

;*****************************************************************************
;** Purpose: Get a byte from the saved game area (or near it).
;**
;** Returns: AL = retrieved byte.
;**
;** Notes  : DX:AX offset within unpacked EXE.
;**          Adds 0030h for European version.
;**
        public  get_sg_byte
get_sg_byte proc near

        les     bx, game_hp
        add     bx, game_sg_off
        add     bx, ax
        sub     bx, HOOK_OFF
        mov     ax, es
        add     ax, dx
        sbb     ax, HOOK_SEG
        mov     es, ax
        mov     al, es:[bx]
        ret

get_sg_byte endp

;*****************************************************************************
;** Purpose: Add date/time entry to log file.
;**
;** Returns: Nothing.
;**
;** Notes  :
;**
        public  add_date_entry
add_date_entry proc near

        mov     dr.dr_record_type, REC_TYPE_DATE
        mov     ah, 2ah
        int     DOS
        mov     dr.dr_year, cx
        mov     dr.dr_month, dh
        mov     dr.dr_day, dl
        mov     dr.dr_week_day, al
        mov     ah, 2ch
        int     DOS
        mov     dr.dr_hours, ch
        mov     dr.dr_minutes, cl
        mov     dr.dr_seconds, dh
        mov     cx, RECORD_LEN
        mov     dx, OFFSET dr
        call    write_entry
        ret

add_date_entry endp

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
        mov     dx, OFFSET q_filename
        mov     ah, 03dh
        mov     al, 00100010B           ; R/W (share deny write)
        int     DOS
        jnc     do_seek

        ;**
        ;** Try and Create the file.
        ;**
open_failed:
        mov     dx, OFFSET q_filename
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
        jc      do_close

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

;*****************************************************************************
;** Purpose: Perform modified Sum-Check.
;**
;** Returns: 16-bit result in AX.
;**
;** Notes  : I/P DS:SI points to buffer, CX length in bytes.
;**

        public  sum_check
sum_check proc  near
        call    crypt
        call    do_sum_check
        call    crypt
        ret

        public  encrypt_start
encrypt_start   dd      12345678h

sum_check_done:
        xchg    ax, bx
        add     ax, 6060h
        ret
        lodsb
        add     al, 52
        jmp     next_sum_check

do_sum_check:
        xor     bx, bx
next_sum_check:
        lodsb
        add     bh, al
        add     bl, bh
        loop    next_sum_check
        clc
        jnc     sum_check_done
        lodsb
        xor     al, 55h
        jmp     next_sum_check

sum_check endp

        public  encrypt_end
encrypt_end     dd      12345678h

        public  crypt
crypt   proc    near
        push    ax
        push    cx
        push    si
        push    di
        les     di, game_hp
        add     di, 0066h
        mov     si, OFFSET encrypt_start
        mov     cx, (OFFSET encrypt_end) - (OFFSET encrypt_start)
        xor     ah, ah
crypt_next:
        mov     al, es:[di]
        xor     [si], al
        inc     si
        and     al, 03h
        add     di, ax
        inc     di
        loop    crypt_next
        pop     di
        pop     si
        pop     cx
        pop     ax
        ret
crypt   endp

;*****************************************************************************
;** Purpose: Modify time allowing for extra sub-distance travelled.
;**
;** Returns:
;**
;** Notes  : DX:AX = time on i/p & o/p
;**

modtime:
        push    ds
        mov     ds, cs:game_si_seg
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

;****************************************************************************

        public  end_resident
end_resident:

;****************************************************************************
;**
;** Non-residient data
;**

load_msg        db      "Installed OK", CR, LF, '$'
already_msg     db      "Already installed !", CR, LF, "$"
parse_msg       db      "Failed to determine absolute pathname.", BELL, CR, LF, "$"
cant_unload_msg db      "Unable to unload as another TSR was loaded after GPLAPTIM.", BELL, CR, LF, "$"
no_unload_msg   db      "Can't unload as GPLAPTIM isn't loaded!", BELL, CR, LF, "$"
unloaded_msg    db      "GPLAPTIM unloaded OK.", CR, LF, "$"

                public  _fname_ptr
_fname_ptr      dw      OFFSET filename
filename        db      "GPLAPTIM.LOG", 0

public          _cmd_line
public          _cmd_line_len
_cmd_line       label   dword
_cmd_line_off   dw      0081h           ; Point to PSP command line.
_cmd_line_seg   dw      ?
_cmd_line_len   db      ?

public          _msg_text
_msg_text       dw      ?

public          _unload_flag
_unload_flag    db      0

tsrs_cs         dw      ?

extrn           _parse:PROC

;****************************************************************************
;**
;** Program entry point
;**

;*****************************************************************************
;** Purpose: Install TSR.
;**
;** Returns: Nothing.
;**
;** Notes  :
;**
        public  install
install proc    near

        ;**
        ;** Switch to our stack.
        ;**
        cli
        mov     ax, cs
        mov     ss, ax
        mov     sp, OFFSET cs:my_stack_top
        sti

        ;***
        ;*** Get Environment variable info.
        ;***
        mov     ah, 62h
        int     DOS                             ; Get PSP segment address
        mov     psp_seg, bx
        mov     cs:_cmd_line_seg, bx
        mov     bx, 0080h                       ; PSP Command line length
        mov     al, [bx]
        mov     cs:_cmd_line_len, al

        ;**
        ;** Point DS at our common segment before calling C code.
        ;**
        mov     ax, cs
        mov     ds, ax
        call    _parse
        or      al, al
        jnz     parse_ok
        int     DOS_TERMINATE

parse_ok:
        ;**
        ;** Check to see if we are already installed
        ;** Note: BX is currently loaded TSR's CS:
        ;**
        mov     cx, MAGIC_CX_IN
        mov     dx, MAGIC_DX_IN
        int     11h
        cmp     cx, MAGIC_CX_OUT
        jne     not_installed
        cmp     dx, MAGIC_DX_OUT
        jne     not_installed
        jmp     installed

not_installed:
        mov     al, _unload_flag
        or      al, al
        jz      not_installed1

nothing_to_unload:
        ;**
        ;** User is trying to unload non-existant resident portion!
        ;**
        push    cs
        pop     ds
        mov     dx, OFFSET no_unload_msg
        mov     ah, 09h
        int     DOS
        int     DOS_TERMINATE

installed:
        mov     al, _unload_flag
        or      al, al
        jz      no_unload

        call    tsr_unload

        ;**
        ;** Can't unload as someone else has hooked one of our vectors.
        ;**
        push    cs
        pop     ds
        mov     dx, OFFSET cant_unload_msg
        mov     ah, 09h
        int     DOS
        int     DOS_TERMINATE

        ;**
        ;** Already installed !
        ;**
no_unload:
        mov     dx, OFFSET already_msg
        mov     ah, 09h
        int     DOS
        int     DOS_TERMINATE

not_installed1:
        ;**
        ;** Convert current filename to fully qualified name.
        ;**
        mov     ax, ds
        mov     es, ax
        mov     si, _fname_ptr
        mov     di, OFFSET q_filename
        mov     ah, 60h
        int     DOS
        jnc     name_q_ok
        mov     dx, OFFSET parse_msg
        mov     ah, 09h
        int     DOS
        int     DOS_TERMINATE

name_q_ok:
        call    tsr_vectors_install

        ;**
        ;** Ensure last lap time is initialised.
        ;**
        mov     al, 0ffh
        mov     ll_secs.secs_seconds, al
        mov     ll_secs.secs_minutes, al
        mov     ll_secs.secs_hours, al

        ;**
        ;** Patch our entry points patches!
        ;**
        mov     bx, OFFSET cs:hook_lap_code_off
        mov     [bx], OFFSET cs:hook_lap
        mov     ax, cs
        mov     [bx+2], ax

        mov     bx, OFFSET cs:hook_pit1_code_off
        mov     [bx], OFFSET cs:hook_pit1
        mov     ax, cs
        mov     [bx+2], ax

        mov     bx, OFFSET cs:hook_pit2_code_off
        mov     [bx], OFFSET cs:hook_pit2
        mov     ax, cs
        mov     [bx+2], ax

        mov     bx, OFFSET cs:hook_aig_code_off
        mov     [bx], OFFSET cs:hook_aig
        mov     ax, cs
        mov     [bx+2], ax

        mov     bx, OFFSET cs:hook_st_code_off
        mov     [bx], OFFSET cs:hook_st
        mov     ax, cs
        mov     [bx+2], ax

        mov     bx, OFFSET cs:hook_bhp_code_off
        mov     [bx], OFFSET cs:hook_bhp
        mov     ax, cs
        mov     [bx+2], ax

        ;**
        ;** Display message
        ;**
        mov     dx, OFFSET load_msg
        mov     ah, 09h
        int     DOS

        ;***
        ;*** Free environment memory
        ;***
        les     bx, psp_env                     ; Point to ENV ptr in PSP
        mov     ax, es:[bx]                     ; Get ENV segment pointer
        mov     es, ax
        mov     ah, 49h
        int     DOS

        ;***
        ;*** Terminate and stay resident for our resident routines only.
        ;***
        mov     ax, OFFSET end_resident
        mov     cl, 4
        shr     ax, cl
        inc     ax
        mov     dx, ax
        mov     ax, 3100h
        int     DOS
        ;***
        ;*** Not reached
        ;***
quit:   ret

install endp

;*****************************************************************************
;** Purpose: Uninstall (if possible). Check vectors still point to us.
;**
;** Returns: If unistall fails, terminates if successful.
;**
;** Notes  :
;**
        public  tsr_unload
tsr_unload proc near

        mov     tsrs_cs, bx

        mov     ax, 3508h
        int     DOS
        mov     ax, es
        cmp     ax, tsrs_cs
        jnz     cant_unload
        cmp     bx, OFFSET new8_hndlr
        jnz     cant_unload

        mov     ax, 3511h
        int     DOS
        mov     ax, es
        cmp     ax, tsrs_cs
        jnz     cant_unload
        cmp     bx, OFFSET new11_hndlr
        jnz     cant_unload

        mov     ax, 3521h
        int     DOS
        mov     ax, es
        cmp     ax, tsrs_cs
        jnz     cant_unload
        cmp     bx, OFFSET new21_hndlr
        jnz     cant_unload

        ;**
        ;** Restore original vectors, point DS at currently installed TSR.
        ;**
        mov     es, tsrs_cs
        lds     dx, es:old8_hndlr
        mov     ax, 2508h
        int     DOS
        lds     dx, es:old11_hndlr
        mov     ax, 2511h
        int     DOS
        lds     dx, es:old21_hndlr
        mov     ax, 2521h
        int     DOS

        ;**
        ;** Free memory. Set original PSP, then terminate process.
        ;**
        push    cs
        pop     ds
        mov     dx, OFFSET unloaded_msg
        mov     ah, 09h
        int     DOS

        mov     es, tsrs_cs
        mov     ah, 49h
        INT     DOS
        int     DOS_TERMINATE
cant_unload:
        ret

tsr_unload      endp

;*****************************************************************************
;** Purpose: Install our interrupt vectors.
;**
;** Returns: Nothing.
;**
;** Notes  : Hooked interrupts, INT 08H, INT 11H, INT 21H.
;**
        public  tsr_vectors_install
tsr_vectors_install proc near

        ;***
        ;*** Install new timer tick vector
        ;***
        mov     ax, 3508h
        int     DOS
        mov     old8_off, bx
        mov     old8_seg, es
        mov     dx, OFFSET new8_hndlr
        mov     ax, 2508h
        int     DOS

        ;***
        ;*** Install new equipment list vector
        ;***
        mov     ax, 3511h
        int     DOS
        mov     old11_off, bx
        mov     old11_seg, es
        mov     dx, OFFSET new11_hndlr
        mov     ax, 2511h
        int     DOS

        ;***
        ;*** Install new DOS Int 21 handler
        ;***
        mov     ax, 3521h
        int     DOS
        mov     old21_off, bx
        mov     old21_seg, es
        mov     dx, OFFSET new21_hndlr
        mov     ax, 2521h
        int     DOS
        ret

tsr_vectors_install endp

;*****************************************************************************
;** Purpose: Write message string to display.
;**
;** Returns: Nothing.
;**
;** Notes  : Message should be terminated with a '$', not '\0'!
;**
        public  _wrt_msg
_wrt_msg proc   near

        mov     dx, _msg_text
        mov     ah, 09h
        int     DOS
        ret

_wrt_msg endp

;****************************************************************************

TSR     ends

END startup

