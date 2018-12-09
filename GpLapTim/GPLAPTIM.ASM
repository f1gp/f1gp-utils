include gpdef.inc
include gplaptim.ash

Timestamp       struc
secs            db ?
mins            db ?
hours           db ?
total_secs      dd ?
field_7         db 37 dup(?)
Timestamp       ends


                .286
                .model tiny

TSR             segment para public 'CODE' use16
                assume cs:TSR
                org 100h
                assume es:nothing, ss:nothing, ds:nothing

LOCALS __

                public start
start           proc near
                jmp     install

start           endp


;*****************************************************************************
;**
;** Equates & Macros
;**

TRUE            equ     1
FALSE           equ     0

DOS             equ     21h                     ; Dos interrupt call
DOS_TERMINATE   equ     20h                     ; Terminate interrupt.
CR              equ     0Dh                     ; Caridge return
LF              equ     0Ah                     ; Line Feed
BELL            equ     07h                     ; Bell Code
TAB             equ     09h                     ; Tab

MAGIC_CX_IN     equ     0c0deh
MAGIC_DX_IN     equ     0deadh
MAGIC_CX_OUT    equ     5a5ah
MAGIC_DX_OUT    equ     3412h

SAVGAM_OFF_EUR  equ     0030h
SAVGAM_OFF_ITA  equ     -10h
SAVGAM_OFF_USA  equ     0000h

GP_EUR_RET_ADDR equ     0b74ch
GP_ITA_RET_ADDR equ     0b74ch
GP_USA_RET_ADDR equ     0b727h
MATCH_OFFSET    equ     6
CONV_BASE_SUB   equ     357h
CONV_BASE_3     equ     1e5eh - 1f7h + CONV_BASE_SUB
CONV_BASE_4     equ     2911h - 1f7h + CONV_BASE_SUB

; ---------------------------------------------------------------------------
                align 2

psp_env         dd      2Ch
old08_handler   dd      0
old11_handler   dd      0
old21_handler   dd      0

delay_count     db      0

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

logfile_path    db      78 dup(0)
logfile_handle  dw      0
pCar            dd      0
fpInitCarGrip   dd      0
gp_ver          db      GP_VER_UNK
game_ss         dw      0
game_sp         dw      0
game_sg_off     dw      0
gp_end_of_lap_off dd    0
hook4_dest      dd      0

log_player_times_only db 0

hook1_start     db      9Ah
hook1_off       dw      0, 0
hook2_start     db      9Ah
hook2_off       dw      0, 0
                db      90h
hook3_start     db      9Ah
hook3_off       dw      0, 0
hook4_start     db      9Ah
hook4_off       dw      0, 0
                db      90h
hook4_alt_start db      61h, 7h, 1Fh, 0CBh, 90h
hook5_start     db      9Ah
hook5_off       dw      0, 0
                db      90h
hook6_start     db      9Ah
hook6_off       dw      0, 0
hook7_start     db      9Ah
hook7_off       dw      0, 0
hook8_start     db      9Ah
hook8_off       dw      0, 0
hook9_start     db      9Ah
hook9_off       dw      0, 0

rec_lap         LAP_RECORD <0>
rec_auth        AUTH_RECORD <0>
rec_auth_race   AUTH_RACE_RECORD <0>
rec_setup       SETUP_RECORD <0>
rec_date        DATE_RECORD <0>
rec_pit_in      PIT_IN_RECORD <0>
rec_pit_out     PIT_OUT_RECORD <0>
rec_split       SPLIT_TIME_RECORD <0>

time            Timestamp <0>
new_time        Timestamp <0>

grass_touched_this_lap db 0

diff_seg002_seg008_seg dw 0

split1_perc     dw      25
split2_perc     dw      50
split3_perc     dw      75
stop_time       dw      10000
ccplayer_power  dw      4000h
trackseg_count  dw      0
car_trackseg    dw      0

circuit_lap_counts      db  81, 71, 61, 78, 69, 69, 72, 59, 45, 77, 44, 53, 71, 65, 53, 81
circuit_half_lap_counts db  41, 36, 31, 39, 35, 35, 36, 30, 23, 39, 22, 27, 36, 33, 27, 41

gp_exe_data_bytes1      db  0, 1Dh, 0, 20h, 0, 10h, 0, 7, 0, 7

hooked_msg      db      CR,LF,'GpLapTim activated OK',CR,LF,'$'

                align 2
                db      200 dup('#')          ; stack
stack_top       dw      5A5Ah


; =============== S U B R O U T I N E =======================================


new08_hndlr     proc far
                pushf
                call    cs:old08_handler

                cmp     cs:delay_count, 0
                jz      short __end

                dec     cs:delay_count

__end:
                iret

new08_hndlr     endp


; =============== S U B R O U T I N E =======================================


new11_hndlr     proc far
                cmp     cx, MAGIC_CX_IN
                jnz     short __skip

                cmp     dx, MAGIC_DX_IN
                jnz     short __skip

                mov     cx, MAGIC_CX_OUT
                mov     dx, MAGIC_DX_OUT
                mov     bx, word ptr cs:psp_env+2
                push    bx
                push    cx
                push    dx
                pushf
                call    cs:old11_handler

                pop     dx
                pop     cx
                pop     bx
                iret

__skip:
                pushf
                call    cs:old11_handler

                iret

new11_hndlr     endp


; =============== S U B R O U T I N E =======================================

PUSHED_WORDS equ (8 + 1 + 1 + 1)

new21_hndlr     proc near
                cmp     ah, 30h         ; Is it DOS Get Version?
                jz      __start
                jmp     __call_old21

__start:
                pushf
                pusha
                push    ds
                push    es

                mov     ax, cs
                mov     ds, ax
                assume ds:TSR
                mov     di, sp

                mov     cs:game_ss, ss
                mov     cs:game_sp, sp

                ; Get return address
                les     di, ss:[di + (PUSHED_WORDS * 2)]
                mov     gp_ver, GP_VER_UNK
                cmp     di, GP_EUR_RET_ADDR
                jnz     short __try_usa

                sub     di, MATCH_OFFSET
                mov     bx, di
                mov     si, offset gp_code_eur
                mov     cx, MATCH_BYTES_EUR
                cld
                repe cmpsb
                jz      short __its_eur

                mov     di, bx
                mov     si, offset gp_code_ita
                mov     cx, MATCH_BYTES_ITA
                cld
                repe cmpsb
                jnz     short __no_match

__its_ita:
                mov     gp_ver, GP_VER_ITA
                mov     game_sg_off, SAVGAM_OFF_ITA
                jmp     short __got_match
                nop

__its_eur:
                mov     gp_ver, GP_VER_EUR
                mov     game_sg_off, SAVGAM_OFF_EUR
                jmp     short __got_match
                nop

__try_usa:
                cmp     di, GP_USA_RET_ADDR
                jnz     short __no_match

                sub     di, MATCH_OFFSET
                mov     si, offset gp_code_usa
                mov     cx, MATCH_BYTES_USA
                cld
                repe cmpsb
                jnz     short __no_match

__its_usa:
                mov     gp_ver, GP_VER_USA
                mov     game_sg_off, SAVGAM_OFF_USA

__got_match:
                call    announce_hook
                call    hook_gp_exe
                call    log_date_record

__no_match:
                pop     es
                pop     ds
                assume ds:nothing
                popa
                popf

__call_old21:
                jmp     cs:old21_handler

new21_hndlr     endp


; =============== S U B R O U T I N E =======================================


hook_gp_exe     proc near
                assume ds:TSR
                mov     si, offset hook1_start
                mov     di, sp
                les     di, ss:[di + ((PUSHED_WORDS + 1) * 2)]
                cmp     gp_ver, GP_VER_USA
                jz      short __usa1

                cmp     gp_ver, GP_VER_ITA
                jz      short __ita1

__eur1:
                sub     di, (0BCECh - 1F77h)
                mov     ax, es
                sub     ax, 6000h
                nop
                mov     es, ax
                jmp     short __dohook1
                nop

__ita1:
                sub     di, 9D35h
                mov     ax, es
                sub     ax, 6000h
                nop
                mov     es, ax
                jmp     short __dohook1
                nop

__usa1:
                sub     di, 9D20h
                mov     ax, es
                sub     ax, 6000h
                nop
                mov     es, ax

__dohook1:
                mov     word ptr gp_end_of_lap_off+2, ax
                mov     word ptr gp_end_of_lap_off, di
                mov     cx, 5
__loop1:
                movsb
                loop    __loop1

; ---------------------------------------------------------------------------

                mov     si, offset hook2_start
                mov     di, sp
                les     di, ss:[di + ((PUSHED_WORDS + 1) * 2)]
                cmp     gp_ver, GP_VER_USA
                jz      short loc_1061F

                cmp     gp_ver, GP_VER_ITA
                jz      short loc_10601

                sub     di, (0BCECh - 411Eh)
                mov     ax, es
                sub     ax, 6000h
                nop
                mov     es, ax
                jmp     short loc_1062B
                nop

loc_10601:
                sub     di, 7B8Eh
                mov     ax, es
                sub     ax, 6000h
                nop
                mov     es, ax
                jmp     short loc_1062B
                nop

loc_1061F:
                sub     di, 7B79h
                mov     ax, es
                sub     ax, 6000h
                nop
                mov     es, ax

loc_1062B:
                mov     cx, 6
loc_1062E:
                movsb
                loop    loc_1062E

; ---------------------------------------------------------------------------

                mov     si, offset hook3_start
                mov     di, sp
                les     di, ss:[di + ((PUSHED_WORDS + 1) * 2)]
                cmp     gp_ver, GP_VER_USA
                jz      short loc_1067D

                cmp     gp_ver, GP_VER_ITA
                jz      short loc_1065F

                sub     di, (0BCECh - 431Fh)
                mov     ax, es
                sub     ax, 6000h
                nop
                mov     es, ax
                jmp     short loc_10689
                nop

loc_1065F:
                sub     di, 798Dh
                mov     ax, es
                sub     ax, 6000h
                nop
                mov     es, ax
                jmp     short loc_10689
                nop

loc_1067D:
                sub     di, 7978h
                mov     ax, es
                sub     ax, 6000h
                nop
                mov     es, ax

loc_10689:
                mov     cx, 5
loc_1068C:
                movsb
                loop    loc_1068C

; ---------------------------------------------------------------------------

                mov     si, offset hook4_start
                mov     di, sp
                les     di, ss:[di + ((PUSHED_WORDS + 1) * 2)]
                cmp     gp_ver, GP_VER_USA
                jz      short loc_106DB

                cmp     gp_ver, GP_VER_ITA
                jz      short loc_106BD

                sub     di, (0BCECh - 4CE0h)
                mov     ax, es
                sub     ax, 6000h
                nop
                mov     es, ax
                jmp     short loc_106E7
                nop

loc_106BD:
                sub     di, 6FCCh
                mov     ax, es
                sub     ax, 6000h
                nop
                mov     es, ax
                jmp     short loc_106E7
                nop

loc_106DB:
                sub     di, 6FB7h
                mov     ax, es
                sub     ax, 6000h
                nop
                mov     es, ax

loc_106E7:
                mov     word ptr hook4_dest, di
                mov     word ptr hook4_dest+2, es
                mov     cx, 6

loc_106EA:
                movsb
                loop    loc_106EA

; ---------------------------------------------------------------------------

                mov     si, offset hook5_start
                mov     di, sp
                les     di, ss:[di + ((PUSHED_WORDS + 1) * 2)]
                cmp     gp_ver, GP_VER_USA
                jz      short loc_10775

                cmp     gp_ver, GP_VER_ITA
                jz      short loc_1072F

                mov     diff_seg002_seg008_seg, (605AH - 19EDH) ; seg008.start - seg002.start
                mov     cs:func_lcdinit_off, 14C0h ; LCDInit
                mov     cs:func_renderlaptime_off, 15B6h ; RenderLCDLapTime
                sub     di, (0BCECh - 171Dh)
                mov     ax, es
                sub     ax, 6000h
                nop
                mov     es, ax
                jmp     short loc_10795
                nop

loc_1072F:
                mov     diff_seg002_seg008_seg, 466Dh
                mov     cs:func_lcdinit_off, 14CCh
                mov     cs:func_renderlaptime_off, 15C2h
                sub     di, 0A58Fh
                mov     ax, es
                sub     ax, 6000h
                nop
                mov     es, ax
                jmp     short loc_10795
                nop

loc_10775:
                mov     diff_seg002_seg008_seg, 466Ah
                mov     cs:func_lcdinit_off, 1494h
                mov     cs:func_renderlaptime_off, 158Ah
                sub     di, 0A57Ah
                mov     ax, es
                sub     ax, 6000h
                nop
                mov     es, ax

loc_10795:
                push    si
                push    di
                push    ds
                push    es
                mov     si, di
                mov     di, offset hook5_orig_code
                mov     ax, es
                mov     ds, ax
                mov     ax, cs
                mov     es, ax
                mov     cx, 6
loc_107A9:
                movsb
                loop    loc_107A9

; ---------------------------------------------------------------------------

                pop     es
                pop     ds
                pop     di
                pop     si
                mov     cx, 6
loc_107B4:
                movsb
                loop    loc_107B4

; ---------------------------------------------------------------------------

                mov     di, sp
                les     di, ss:[di + ((PUSHED_WORDS + 1) * 2)]
                mov     ax, es
                sub     ax, diff_seg002_seg008_seg
                mov     cs:func_lcdinit_seg, ax
                mov     cs:func_renderlaptime_seg, ax
                mov     si, offset hook5_start
                mov     di, sp
                les     di, ss:[di + ((PUSHED_WORDS + 1) * 2)]
                cmp     gp_ver, GP_VER_USA
                jz      short loc_10817

                cmp     gp_ver, GP_VER_ITA
                jz      short loc_107F9

                sub     di, (0BCECh - 66CCh)
                mov     ax, es
                sub     ax, 6000h
                nop
                mov     es, ax
                jmp     short loc_10823
                nop

loc_107F9:
                sub     di, 55E0h
                mov     ax, es
                sub     ax, 6000h
                nop
                mov     es, ax
                jmp     short loc_10823
                nop

loc_10817:
                sub     di, 55CBh
                mov     ax, es
                sub     ax, 6000h
                nop
                mov     es, ax

loc_10823:
                mov     cx, 6
loc_10826:
                movsb
                loop    loc_10826

; ---------------------------------------------------------------------------

                mov     si, offset hook6_start
                mov     di, sp
                les     di, ss:[di + ((PUSHED_WORDS + 1) * 2)]
                cmp     gp_ver, GP_VER_USA
                jz      short loc_10875

                cmp     gp_ver, GP_VER_ITA
                jz      short loc_10857

                sub     di, (0BCECh - 4B8Ah)
                mov     ax, es
                sub     ax, 6000h
                nop
                mov     es, ax
                jmp     short loc_10881
                nop

loc_10857:
                sub     di, 7122h
                mov     ax, es
                sub     ax, 6000h
                nop
                mov     es, ax
                jmp     short loc_10881
                nop

loc_10875:
                sub     di, 710Dh
                mov     ax, es
                sub     ax, 6000h
                nop
                mov     es, ax

loc_10881:
                mov     cx, 5
loc_10884:
                movsb
                loop    loc_10884

; ---------------------------------------------------------------------------

                mov     si, offset hook7_start
                mov     di, sp
                les     di, ss:[di + ((PUSHED_WORDS + 1) * 2)]
                cmp     gp_ver, GP_VER_USA
                jz      short __usa7

                cmp     gp_ver, GP_VER_ITA
                jz      short __ita7

                ; use 4B94h to keep the randomization of CCPlayer car power
                sub     di, (0BCECh - 4B99h)
                mov     ax, es
                sub     ax, 6000h
                nop
                mov     es, ax
                jmp     short __dohook7
                nop

__ita7:
                sub     di, 7113h
                mov     ax, es
                sub     ax, 6000h
                nop
                mov     es, ax
                jmp     short __dohook7
                nop

__usa7:
                sub     di, 70FEh
                mov     ax, es
                sub     ax, 6000h
                nop
                mov     es, ax

__dohook7:
                mov     cx, 5
__loop7:
                movsb
                loop    __loop7

; ---------------------------------------------------------------------------

                mov     si, offset hook8_start
                mov     di, sp
                les     di, ss:[di + ((PUSHED_WORDS + 1) * 2)]
                sub     di, (0BCECh - 04C85h)
                mov     ax, es
                sub     ax, 6000h
                mov     word ptr cs:fpInitCarGrip, di
                mov     word ptr cs:fpInitCarGrip+2, ax

                mov     di, sp
                les     di, ss:[di + ((PUSHED_WORDS + 1) * 2)]
                sub     di, (0BCECh - 0D83Ah)
                mov     ax, es
                sub     ax, 6000h
                mov     es, ax

                mov     cx, 5
__loop8:
                movsb
                loop    __loop8

; ---------------------------------------------------------------------------

                mov     si, offset hook9_start
                mov     di, sp
                les     di, ss:[di + ((PUSHED_WORDS + 1) * 2)]
                sub     di, (0BCECh - 0D8EBh)
                mov     ax, es
                sub     ax, 6000h
                nop
                mov     es, ax

                mov     cx, 5
__loop9:
                movsb
                loop    __loop9

; ---------------------------------------------------------------------------

                mov     si, offset func_renderlcdimage_off
                mov     di, sp
                les     di, ss:[di + ((PUSHED_WORDS + 1) * 2)]
                cmp     gp_ver, GP_VER_USA
                jz      short loc_108DC

                cmp     gp_ver, GP_VER_ITA
                jz      short loc_108B8

                mov     bx, 3830h       ; RenderLCDImage
                sub     di, (0BCECh - 1E24h)    ; just to read out segment
                mov     ax, es
                sub     ax, 6000h
                nop
                mov     es, ax
                jmp     short loc_108EB
                nop

loc_108B8:
                mov     bx, 37FCh
                sub     di, 9E88h
                mov     ax, es
                sub     ax, 6000h
                nop
                mov     es, ax
                jmp     short loc_108EB
                nop

loc_108DC:
                mov     bx, 3804h
                sub     di, 9E73h
                mov     ax, es
                sub     ax, 6000h
                nop
                mov     es, ax

loc_108EB:
                mov     [si], bx
                add     si, 2
                mov     ax, es:[di]
                mov     [si], ax
                assume ds:nothing
                retn

hook_gp_exe     endp


; =============== S U B R O U T I N E =======================================


announce_hook   proc near
                assume ds:TSR
                mov     dx, offset hooked_msg
                mov     ah, 9
                int     DOS             ; DOS - PRINT STRING
                                        ; DS:DX -> string terminated by "$"

                sti
                mov     delay_count, 27


do_delay:
                cmp     delay_count, 0
                jnz     short do_delay

                assume ds:nothing
                retn

announce_hook   endp


; =============== S U B R O U T I N E =======================================


hook2_pit_in    proc far
                add     [si+Car.timerPitSeq_lo], 400 ; also used for pulling away
                pushf
                push    ax
                mov     word ptr cs:pCar, si
                mov     word ptr cs:pCar+2, ds
                cli
                mov     cs:game_ss, ss
                mov     cs:game_sp, sp
                mov     ax, cs
                mov     ss, ax
                assume ss:TSR
                mov     sp, offset stack_top
                sti
                pusha
                push    es
                push    ds
                mov     ax, ds
                mov     es, ax
                mov     ax, cs
                mov     ds, ax
                assume ds:TSR
                cld
                test    byte ptr es:5Ah, 80h ; seg003:byte_1E66A
                jnz     short __end

                mov     al, log_player_times_only
                or      al, al
                jz      short loc_10957

                mov     al, es:[si+Car.bID]
                and     al, 80h
                jnz     short loc_10957

                jmp     short __end
                nop

loc_10957:
                mov     al, REC_TYPE_PIT_IN
                mov     rec_pit_in.pi_record_type, al
                call    get_game_mode

                mov     rec_pit_in.pi_game_mode, ah
                call    get_tracknr_in_high_nibble

                or      rec_pit_in.pi_game_mode, al
                call    get_car_id

                mov     rec_pit_in.pi_car_number, al
                mov     al, es:[si+Car.bCurLap]
                dec     al
                mov     rec_pit_in.pi_lap_number, al
                mov     al, rec_pit_in.pi_car_number
                call    get_car_race_pos

                mov     rec_pit_in.pi_car_position, cl
                mov     ax, es:[si+Car.timerPitSeq_lo] ; also used for pulling away
                mov     word ptr rec_pit_in.pi_in_time, ax
                mov     ax, es:[si+Car.timerPitSeq_hi]
                mov     word ptr rec_pit_in.pi_in_time+2, ax
                mov     al, rec_pit_in.pi_car_number
                mov     di, offset rec_pit_in.pi_driver
                call    get_driver_name

                mov     cx, 44
                mov     dx, offset rec_pit_in
                call    log_record

__end:
                pop     ds
                assume ds:nothing
                pop     es
                popa
                cli
                mov     ss, cs:game_ss
                assume ss:nothing
                mov     sp, cs:game_sp
                sti
                pop     ax
                popf
                retf

hook2_pit_in    endp


; =============== S U B R O U T I N E =======================================


hook3_pit_out   proc far
                or      [si+Car.flags_B3], 20h
                pushf
                push    ax
                mov     word ptr cs:pCar, si
                mov     word ptr cs:pCar+2, ds
                cli
                mov     cs:game_ss, ss
                mov     cs:game_sp, sp
                mov     ax, cs
                mov     ss, ax
                assume ss:TSR
                mov     sp, offset stack_top
                sti
                pusha
                push    es
                push    ds
                mov     ax, ds
                mov     es, ax
                mov     ax, cs
                mov     ds, ax
                assume ds:TSR
                cld
                test    byte ptr es:5Ah, 80h ; seg003:byte_1E66A
                jnz     short loc_10A50

                mov     al, log_player_times_only
                or      al, al
                jz      short loc_10A02

                mov     al, es:[si+Car.bID]
                and     al, 80h
                jnz     short loc_10A02

                jmp     short loc_10A50
                nop


loc_10A02:
                mov     al, REC_TYPE_PIT_OUT
                mov     rec_pit_out.po_record_type, al
                call    get_game_mode

                mov     rec_pit_out.po_game_mode, ah
                call    get_tracknr_in_high_nibble

                or      rec_pit_out.po_game_mode, al
                call    get_car_id

                mov     rec_pit_out.po_car_number, al
                mov     al, es:[si+Car.bCurLap]
                dec     al
                mov     rec_pit_out.po_lap_number, al
                mov     al, rec_pit_out.po_car_number
                call    get_car_race_pos

                mov     rec_pit_out.po_car_position, cl
                mov     ax, es:[si+Car.timerPitSeq_lo] ; also used for pulling away
                mov     word ptr rec_pit_out.po_out_time, ax
                mov     ax, es:[si+Car.timerPitSeq_hi]
                mov     word ptr rec_pit_out.po_out_time+2, ax
                mov     al, rec_pit_out.po_car_number
                mov     di, offset rec_pit_out.po_driver
                call    get_driver_name

                mov     cx, size ALL_RECORDS
                mov     dx, offset rec_pit_out
                call    log_record

loc_10A50:
                pop     ds
                assume ds:nothing
                pop     es
                popa
                cli
                mov     ss, cs:game_ss
                assume ss:nothing
                mov     sp, cs:game_sp
                sti
                pop     ax
                popf
                retf

hook3_pit_out   endp


; =============== S U B R O U T I N E =======================================


hook4_player_grip proc far
                push    ds
                push    es
                pusha

                ; read bActivePlayerCarID
                mov     dx, CONV_BASE_3 + 296h
                mov     ax, 1
                push    ds
                push    cs
                pop     ds
                assume ds:TSR
                call    near ptr read_some_byte_from_gp
                pop     ds
                assume ds:nothing
                mov     dl, al

                mov     al, [si+Car.bID]
                test    al, 80h
                jz      short __repeat_orig_code

                cmp     al, dl
                jne     __repeat_orig_code

                mov     [si+Car.wCornerSpeedFactor], 4000h

__repeat_orig_code:
                popa
                pop     es
                pop     ds
                mov     di, [bp+1222h]  ; wOppLevel
                shl     di, 1
                retf

hook4_player_grip endp


; =============== S U B R O U T I N E =======================================


hook6_randomize_player_car_power proc far
                retf

hook6_randomize_player_car_power endp


; =============== S U B R O U T I N E =======================================


; out cx -> power value
hook7_randomize_ccplayer_car_power proc far
                mov     cx, cs:ccplayer_power
                retf

hook7_randomize_ccplayer_car_power endp


; =============== S U B R O U T I N E =======================================


; set CCPlayer grip to CC grip
hook8_hotseat_automatic proc far
                push    ds
                push    es
                push    si
                push    di
                push    cx
                push    dx

                push    ds
                push    es
                push    si

                push    cs
                pop     ds
                assume ds:TSR

                ; skip pitstop strategy code
                les     di, hook4_dest
                mov     byte ptr es:[di-53h], 0EBh  ; JNS -> JMP
                ; skip all code in InitCarGrip right after grip is set (same location as hook4)
                mov     si, offset hook4_alt_start
                mov     cx, 5
__loop1:
                movsb
                loop    __loop1

                pop     si
                pop     es
                pop     ds
                assume ds:nothing

                ; call patched InitCarGrip
                call    [cs:fpInitCarGrip]

                push    cs
                pop     ds
                assume ds:TSR

                ; restore original pitstop strategy code
                les     di, hook4_dest
                mov     byte ptr es:[di-53h], 79h
                ; restore original hook4
                mov     si, offset hook4_start
                mov     cx, 5
__loop2:
                movsb
                loop    __loop2

__repeat_orig_code:
                pop     dx
                pop     cx
                pop     di
                pop     si
                pop     es
                pop     ds
                assume ds:nothing
                add     [si+Car.bNumHotseatSwitchesDone], 1
                retf

hook8_hotseat_automatic endp


; =============== S U B R O U T I N E =======================================


; set Player grip to 1
hook9_hotseat_control proc far
                mov     [si+Car.wCornerSpeedFactor], 4000h

__repeat_orig_code:
                mov     byte ptr [bp+192h], 0FFh
                retf

hook9_hotseat_control endp


; ---------------------------------------------------------------------------


loc_10A77:
                jmp     loc_10C24

hook5_seg:
                pusha
                push    ds
                push    es
                test    [si+Car.bID], 80h ; is Player?
                jz      short loc_10AB3

                test    [si+Car.bID], 40h ; is CCPlayer?
                jnz     short loc_10AB3

                cmp     byte ptr ds:22FBh, SURFACE_TYPE_GRASS ; surface type left?
                jz      short on_grass

                cmp     byte ptr ds:22FCh, SURFACE_TYPE_GRASS ; surface type right?
                jnz     short loc_10AB3


on_grass:
                mov     cs:grass_touched_this_lap, 1
                test    byte ptr ss:124Ah, 80h ; session mode race?
                jnz     short loc_10AB3

                cmp     si, ds:97Dh     ; seg003:pCarCockpit
                jnz     short loc_10AB3

                mov     ax, 0Eh
                call    call_render_lcd_image


loc_10AB3:
                cmp     si, ds:97Dh     ; seg003:pCarCockpit
                jnz     short loc_10A77

                test    byte ptr ss:124Ah, 80h ; session mode race?
                jnz     short loc_10A77

                mov     ax, word ptr [si+Car.pSeg]
                cmp     ax, cs:car_trackseg
                jnz     short loc_10AD1


loc_10ACB:
                jmp     loc_10C24

loc_10ACE:
                jmp     short loc_10B31
                nop

loc_10AD1:
                mov     cs:car_trackseg, ax
                test    [si+Car.timeAbsLast_hi], 0C000h
                jnz     short loc_10A77

                mov     cx, 1
                mov     ax, ss:15Ch     ; pAfterLastSeg
                sub     ax, 30h
                mov     bx, size Segment
                mov     di, 100
                xor     dx, dx
                div     bx
                mov     cs:trackseg_count, ax
                imul    cs:split1_perc
                div     di
                imul    bx
                add     ax, 30h
                cmp     ax, word ptr [si+Car.pSeg]
                jz      short loc_10ACE

                inc     cx
                mov     ax, cs:trackseg_count
                imul    cs:split2_perc
                div     di
                imul    bx
                add     ax, 30h
                cmp     ax, word ptr [si+Car.pSeg]
                jz      short loc_10ACE

                inc     cx
                mov     ax, cs:trackseg_count
                imul    cs:split3_perc
                div     di
                imul    bx
                add     ax, 30h
                cmp     ax, word ptr [si+Car.pSeg]
                jnz     short loc_10ACB


loc_10B31:
                test    [si+Car.bID], 80h
                jnz     short loc_10B3B

                jmp     loc_10BE8

loc_10B3B:
                mov     word ptr cs:pCar, si
                mov     word ptr cs:pCar+2, ds
                cli
                mov     cs:game_ss, ss
                mov     cs:game_sp, sp
                mov     ax, cs
                mov     ss, ax
                assume ss:TSR
                mov     sp, offset stack_top
                sti
                pusha
                push    ds
                push    es
                cld
                mov     ax, ds
                mov     es, ax
                mov     ax, cs
                mov     ds, ax
                assume ds:TSR
                mov     rec_split.st_point, cl
                dec     cx
                add     cx, cx
                mov     bx, offset split1_perc
                add     bx, cx
                mov     ax, cs:[bx]
                mov     rec_split.st_percentage, ax
                mov     al, REC_TYPE_SPLIT
                mov     rec_split.st_record_type, al
                call    get_game_mode

                mov     rec_split.st_game_mode, ah
                call    get_tracknr_in_high_nibble

                or      rec_split.st_game_mode, al
                call    get_car_id

                mov     rec_split.st_car_number, al
                mov     al, es:[si+Car.bCurLap]
                dec     al
                mov     rec_split.st_lap_number, al
                mov     al, rec_split.st_car_number
                call    get_car_race_pos

                mov     rec_split.st_car_position, cl
                mov     al, rec_split.st_car_number
                call    get_car_fastlap_pos

                mov     rec_split.st_qual_position, cl
                mov     ax, es:294Fh    ; timeCurrent_lo
                mov     dx, es:2951h    ; timeCurrent_hi
                sub     ax, es:[si+Car.timeAbsLast_lo]
                sbb     dx, es:[si+Car.timeAbsLast_hi]
                mov     word ptr rec_split.st_split_time, ax
                mov     word ptr rec_split.st_split_time+2, dx
                call    adjust_laptime_with_segdist

                mov     al, rec_split.st_car_number
                mov     di, offset rec_split.st_driver
                call    get_driver_name

                mov     cx, size ALL_RECORDS
                mov     dx, offset rec_split
                call    log_record

                pop     es
                pop     ds
                assume ds:nothing
                popa
                cli
                mov     ss, cs:game_ss
                assume ss:nothing
                mov     sp, cs:game_sp
                sti

loc_10BE8:
                push    word ptr ds:294Fh ; timeCurrent_lo
                pop     word ptr ds:1Ah ; split timer lo
                push    word ptr ds:2951h ; timeCurrent_hi
                pop     word ptr ds:1Ch ; split timer hi
                mov     cx, cs:stop_time
                add     word ptr ds:1Ah, cx ; split timer lo
                adc     word ptr ds:1Ch, 0 ; split timer hi
                mov     al, 25
                mov     cl, 2

; ---------------------------------------------------------------------------
                db      9Ah
func_lcdinit_off dw     0
func_lcdinit_seg dw     0
; ---------------------------------------------------------------------------
                mov     ax, ds:294Fh    ; timeCurrent_lo
                mov     dx, ds:2951h    ; timeCurrent_hi
                sub     ax, [si+Car.timeAbsLast_lo]
                sbb     dx, [si+Car.timeAbsLast_hi]
                call    adjust_laptime_with_segdist

; ---------------------------------------------------------------------------
                db      9Ah
func_renderlaptime_off dw 0
func_renderlaptime_seg dw 0
; ---------------------------------------------------------------------------

loc_10C24:
                pop     es
                pop     ds
                assume ds:nothing
                popa
hook5_orig_code:
                db      6 dup(0)
                retf


; =============== S U B R O U T I N E =======================================


call_render_lcd_image proc near
                db      9Ah
func_renderlcdimage_off dw 0
func_renderlcdimage_seg dw 0
                retn

call_render_lcd_image endp

; =============== S U B R O U T I N E =======================================


hook1_end_of_lap proc far
                test    byte ptr [bp+124Ah], 0FFh ; session mode race?
                pushf
                push    ax
                mov     word ptr cs:pCar, si
                mov     word ptr cs:pCar+2, ds
                cli
                mov     cs:game_ss, ss
                mov     cs:game_sp, sp
                mov     ax, cs
                mov     ss, ax
                assume ss:TSR
                mov     sp, offset stack_top
                sti
                pusha
                push    es
                push    ds
                mov     ax, ds
                mov     es, ax
                mov     ax, cs
                mov     ds, ax
                assume ds:TSR
                cld
                test    byte ptr es:5Ah, 80h ; seg003:byte_1E66A
                jz      short loc_10C6F

                jmp     loc_110DC

loc_10C6F:
                les     si, pCar
                test    es:[si+Car.bID], 80h
                jz      short loc_10C93

                test    es:[si+Car.bID], 40h
                jnz     short loc_10C93

                mov     ah, 2Ch
                int     DOS             ; DOS - GET CURRENT TIME
                                        ; Return: CH = hours, CL = minutes, DH = seconds
                                        ; DL = hundredths of seconds

                mov     new_time.hours, ch
                mov     new_time.mins, cl
                mov     new_time.secs, dh


loc_10C93:
                les     si, pCar
                mov     al, log_player_times_only
                or      al, al
                jz      short loc_10CAA

                mov     al, es:[si+Car.bID]
                and     al, 80h
                jnz     short loc_10CAA

                jmp     loc_11074

loc_10CAA:
                mov     al, 1
                mov     rec_lap.lr_record_type, al
                call    get_game_mode

                mov     rec_lap.lr_game_mode, ah
                call    get_tracknr_in_high_nibble

                or      rec_lap.lr_game_mode, al
                call    get_car_id

                mov     rec_lap.lr_car_number, al
                mov     al, es:[si+Car.bCurLap]
                dec     al
                mov     rec_lap.lr_lap_number, al
                mov     al, rec_lap.lr_car_number
                call    get_car_race_pos

                mov     rec_lap.lr_car_position, cl
                mov     al, rec_lap.lr_car_number
                call    get_car_fastlap_pos

                mov     rec_lap.lr_qual_position, cl
                mov     ax, es:[si+Car.timeLast_lo]
                mov     word ptr rec_lap.lr_lap_time, ax
                mov     ax, es:[si+Car.timeLast_hi]
                mov     word ptr rec_lap.lr_lap_time+2, ax
                mov     ax, es:[si+Car.timeBest_lo]
                mov     word ptr rec_lap.lr_best_time, ax
                mov     ax, es:[si+Car.timeBest_hi]
                mov     word ptr rec_lap.lr_best_time+2, ax
                mov     al, rec_lap.lr_car_number
                and     al, 3Fh
                xor     ah, ah
                sub     al, 1
                shl     ax, 2
                mov     di, ax
                add     di, 1982h       ; arCarIDRaceTimes
                push    es
                mov     es, game_ss
                mov     ax, es:[di]
                mov     word ptr rec_lap.lr_race_time, ax
                mov     ax, es:[di+2]
                mov     word ptr rec_lap.lr_race_time+2, ax
                pop     es
                mov     al, rec_lap.lr_car_number
                mov     di, offset rec_lap.lr_driver
                call    get_driver_name

                mov     si, offset rec_lap
                mov     cx, 42
                call    calc_crc

                mov     rec_lap.lr_crc, ax
                mov     cx, 44
                les     si, pCar
                test    es:[si+Car.bID], 80h
                jz      short loc_10D52

                test    es:[si+Car.bID], 40h
                jnz     short loc_10D52

                jmp     short loc_10D55
                nop


loc_10D52:
                jmp     loc_1106E

loc_10D55:
                test    word ptr rec_lap.lr_lap_time+2, 0C000h
                jz      short laptime_valid

                jmp     loc_1101C

laptime_valid:
                call    get_tracknr_in_high_nibble

                shr     al, 4
                and     al, 0Fh
                xor     ah, ah
                mov     bx, offset circuit_lap_counts
                add     bx, ax
                mov     al, [bx]
                inc     al
                les     si, pCar
                cmp     al, es:[si+Car.bCurLap]
                jz      short loc_10DB5

                call    get_tracknr_in_high_nibble

                shr     al, 4
                and     al, 0Fh
                xor     ah, ah
                mov     bx, offset circuit_half_lap_counts
                add     bx, ax
                mov     al, [bx]
                inc     al
                les     si, pCar
                cmp     al, es:[si+Car.bCurLap]
                jz      short loc_10DB5

                mov     ax, word ptr rec_lap.lr_lap_time
                cmp     ax, word ptr rec_lap.lr_best_time
                jnz     short loc_10DAF

                mov     ax, word ptr rec_lap.lr_lap_time+2
                cmp     ax, word ptr rec_lap.lr_best_time+2
                jnz     short loc_10DAF

                jmp     short loc_10DB5
                nop


loc_10DAF:
                mov     cx, size ALL_RECORDS
                jmp     loc_1106E

loc_10DB5:
                mov     al, rec_lap.lr_record_type
                and     al, 0F0h
                or      al, REC_TYPE_AUTH
                mov     rec_auth.ar_record_type, al
                mov     ax, word ptr rec_lap.lr_best_time
                mov     word ptr rec_auth.ar_lap_time, ax
                mov     ax, word ptr rec_lap.lr_best_time+2
                mov     word ptr rec_auth.ar_lap_time+2, ax
                mov     al, rec_lap.lr_game_mode
                mov     rec_auth.ar_game_mode, al

;                les     bx, gp_end_of_lap_off
;                add     bx, (4B88h - 1F77h)  ; player car power
;                mov     ax, es:[bx]
                les     si, pCar
                mov     ax, es:[si+Car.wPower]

                sub     ax, 632
                xor     dx, dx
                mov     cx, 22
                div     cx
                mov     rec_auth.ar_bhp, ax

                les     bx, gp_end_of_lap_off
                add     bx, (4CDAh - 1F77h)  ; player car grip
                xor     dx, dx
                mov     ax, es:[bx]
                cmp     ax, 4000h
                jb      short loc_10DFE

                sub     ax, 4000h
loc_10DFE:
                mov     cx, 100
                div     cx
                add     ax, 1
                mov     rec_auth.ar_ai_grip, al

                mov     dx, CONV_BASE_4 + 122h
                mov     ax, 2
                call    read_some_byte_from_gp
                mov     rec_auth.ar_skill, al
                mov     rec_auth.ar_drv_aids, 0

                mov     dx, CONV_BASE_4 + 111h
                mov     ax, 0Dh
                call    read_some_byte_from_gp
                and     al, 80h
                or      rec_auth.ar_drv_aids, al

                mov     dx, CONV_BASE_4 + 122h
                mov     ax, 0
                call    read_some_byte_from_gp
                and     al, 3Fh
                or      rec_auth.ar_drv_aids, al
                xor     bh, bh
                mov     bl, rec_auth.ar_skill

                mov     dx, CONV_BASE_3 + 12h
                mov     ax, 6
                add     ax, bx
                call    read_some_byte_from_gp
                or      al, 0C0h
                and     rec_auth.ar_drv_aids, al

                mov     dx, CONV_BASE_4 + 111h
                mov     ax, 0Ch
                call    read_some_byte_from_gp
                and     al, 80h
                shr     al, 1
                or      rec_auth.ar_drv_aids, al

                mov     ah, 2Ah
                int     DOS             ; DOS - GET CURRENT DATE
                                        ; Return: DL = day, DH = month, CX = year
                                        ; AL = day of the week (0=Sunday, 1=Monday, etc.)

                mov     rec_auth.ar_year, cx
                mov     rec_auth.ar_month, dh
                mov     rec_auth.ar_day, dl
                mov     ah, 2Ch
                int     DOS             ; DOS - GET CURRENT TIME
                                        ; Return: CH = hours, CL = minutes, DH = seconds
                                        ; DL = hundredths of seconds

                mov     rec_auth.ar_hours, ch
                mov     rec_auth.ar_minutes, cl
                and     dh, 3Fh
                mov     rec_auth.ar_seconds, dh
                push    ds
                pop     es
                assume es:TSR
                mov     si, offset rec_lap.lr_driver
                mov     di, offset rec_auth.ar_driver
                mov     cx, 24


loc_10E8E:
                movsb
                loop    loc_10E8E

                les     si, pCar
                assume es:nothing
                cmp     es:[si+Car.bFrontWingSetup], 64
                jg      short loc_10EA6

                cmp     es:[si+Car.bRearWingSetup], 64
                jle     short loc_10EAE


loc_10EA6:
                call    maybe_init_auth_flags

                or      rec_auth.ar_seconds, 1 ; illegal wing setting


loc_10EAE:
                cmp     es:[si+Car.tyreCompound], 5
                jle     short loc_10EBE

                call    maybe_init_auth_flags

                or      rec_auth.ar_seconds, 2


loc_10EBE:
                cmp     time.hours, -1
                jz      short loc_10F27

                mov     di, offset time
                call    calc_total_secs

                mov     di, offset new_time
                call    calc_total_secs

                cmp     time.hours, 23
                jnz     short loc_10EEA

                cmp     new_time.hours, 0

new_day:
                jnz     short loc_10EEA

                add     word ptr time.total_secs, 5180h ; add 1 day (15180h = 86400 = 24*60*60 secs)
                adc     word ptr time.total_secs+2, 1


loc_10EEA:
                mov     ax, word ptr new_time.total_secs
                mov     dx, word ptr new_time.total_secs+2
                sub     ax, word ptr time.total_secs
                sbb     dx, word ptr time.total_secs+2
                mov     bx, 1000
                mul     bx
                sub     ax, word ptr rec_lap.lr_best_time
                sbb     dx, word ptr rec_lap.lr_best_time+2
                test    dx, 8000h
                jz      short loc_10F18

                mov     bx, ax
                mov     cx, dx
                xor     ax, ax
                xor     dx, dx
                sub     ax, bx
                sbb     dx, cx


loc_10F18:
                cmp     dx, 0
                jnz     short loc_10F37

                test    ax, 8000h
                jnz     short loc_10F37

                cmp     ax, 10000
                jle     short loc_10F32


loc_10F27:
                call    maybe_init_auth_flags

                or      rec_auth.ar_seconds, 4
                jmp     short loc_10F3F
                nop


loc_10F32:
                cmp     ax, 30000
                jle     short loc_10F3F


loc_10F37:
                call    maybe_init_auth_flags

                or      rec_auth.ar_seconds, 8


loc_10F3F:
                les     si, pCar
                cmp     es:[si+Car.wPower], 4000h ; car power
                jle     short loc_10F54

                call    maybe_init_auth_flags

                or      rec_auth.ar_seconds, 10h


loc_10F54:
                cmp     grass_touched_this_lap, 1
                jnz     short loc_10F63

                call    maybe_init_auth_flags

                or      rec_auth.ar_seconds, 20h


loc_10F63:
                mov     cx, 10
                mov     si, offset gp_exe_data_bytes1
                les     di, pCar
                mov     di, 8CCh    ; data in seg003 to be authenticated
                cld
                repe cmpsb
                jz      short loc_10F7D

                call    maybe_init_auth_flags

                or      rec_auth.ar_seconds, 40h


loc_10F7D:
                mov     si, offset rec_auth
                mov     cx, 42
                call    calc_crc

                mov     rec_auth.ar_crc, ax
                call    get_tracknr_in_high_nibble

                shr     al, 4
                and     al, 0Fh
                xor     ah, ah
                mov     bx, offset circuit_lap_counts
                add     bx, ax
                mov     al, [bx]
                inc     al
                les     si, pCar
                cmp     al, es:[si+Car.bCurLap]
                jz      short loc_10FC9

                call    get_tracknr_in_high_nibble

                shr     al, 4
                and     al, 0Fh
                xor     ah, ah
                mov     bx, offset circuit_half_lap_counts
                add     bx, ax
                mov     al, [bx]
                inc     al
                les     si, pCar
                cmp     al, es:[si+Car.bCurLap]
                jz      short loc_10FC9

                mov     cx, 88
                jmp     loc_1106E

loc_10FC9:
                push    ds
                pop     es
                assume es:TSR
                mov     si, offset rec_auth
                mov     di, offset rec_auth_race
                mov     cx, size AUTH_RECORD


loc_10FD4:
                movsb
                loop    loc_10FD4

                mov     al, rec_auth_race.rr_record_type
                and     al, 0F0h
                or      al, REC_TYPE_AUTH_RACE
                mov     rec_auth_race.rr_record_type, al
                mov     ax, word ptr rec_lap.lr_race_time
                mov     dx, word ptr rec_lap.lr_race_time+2
                mov     word ptr rec_auth_race.rr_race_time, ax
                mov     word ptr rec_auth_race.rr_race_time+2, dx
                mov     si, offset rec_auth_race
                mov     cx, AUTH_RACE_RECORD.rr_crc
                call    calc_crc

                mov     rec_auth_race.rr_crc, ax
                mov     ax, word ptr rec_lap.lr_best_time
                mov     dx, word ptr rec_lap.lr_best_time+2
                mov     word ptr rec_auth.ar_lap_time, ax
                mov     word ptr rec_auth.ar_lap_time+2, dx
                mov     si, offset rec_auth
                mov     cx, AUTH_RACE_RECORD.rr_crc
                call    calc_crc

                mov     rec_auth.ar_crc, ax
                mov     cx, 132
                jmp     short loc_1106E
                nop


loc_1101C:
                mov     al, REC_TYPE_SETUP
                mov     rec_setup.sr_record_type, al
                call    get_game_mode

                mov     rec_setup.sr_game_mode, ah
                call    get_tracknr_in_high_nibble

                or      rec_setup.sr_game_mode, al
                mov     al, rec_lap.lr_car_number
                and     al, 3Fh
                xor     ah, ah
                sub     al, 1
                mov     dx, 10
                mul     dx
                mov     dx, CONV_BASE_4 + 128h       ; osCarSetups
                add     ax, 6
                mov     di, offset rec_setup.sr_front_wing
                mov     cx, 10
loc_11049:
                push    ax
                call    read_some_byte_from_gp
                mov     [di], al
                pop     ax
                inc     ax
                inc     di
                loop    loc_11049

                les     si, pCar
                assume es:nothing
                mov     al, es:[si+Car.tyreCompound]
                and     al, 7
                mov     rec_setup.sr_using_tyres, al
                mov     cx, size ALL_RECORDS
                mov     dx, offset rec_setup
                call    log_record

                mov     cx, size ALL_RECORDS


loc_1106E:
                mov     dx, offset rec_lap
                call    log_record


loc_11074:
                les     si, pCar
                assume es:nothing
                test    es:[si+Car.bID], 80h
                jz      short loc_110DC

                test    es:[si+Car.bID], 40h
                jnz     short loc_110DC

                mov     ch, new_time.hours
                mov     time.hours, ch
                mov     cl, new_time.mins
                mov     time.mins, cl
                mov     dh, new_time.secs
                mov     time.secs, dh
                cmp     grass_touched_this_lap, 1
                jnz     short loc_110DC

                mov     grass_touched_this_lap, 0
                pop     ds
                assume ds:nothing
                pop     es
                assume es:nothing
                popa
                cli
                mov     ss, cs:game_ss
                assume ss:nothing
                mov     sp, cs:game_sp
                sti
                test    byte ptr ss:124Ah, 80h ; session mode race?
                jnz     short loc_110EB

                cmp     si, ds:97Dh     ; seg003:pCarCockpit
                jnz     short loc_110EB

                mov     ax, 3
                test    [si+Car.flags_B3], 4 ; pitlight
                jz      short loc_110D6

                mov     ax, 0Fh

loc_110D6:
                call    near ptr call_render_lcd_image

                jmp     short loc_110EB
                nop
loc_110DC:
                pop     ds
                assume ds:nothing
                pop     es
                assume es:nothing
                popa
                cli
                mov     ss, cs:game_ss
                assume ss:nothing
                mov     sp, cs:game_sp
                sti

loc_110EB:
                pop     ax
                popf
                retf

hook1_end_of_lap endp


; =============== S U B R O U T I N E =======================================


maybe_init_auth_flags proc near
                test    rec_auth.ar_seconds, 80h
                jnz     short locret_110FA

                mov     rec_auth.ar_seconds, 80h
locret_110FA:
                retn

maybe_init_auth_flags endp


; =============== S U B R O U T I N E =======================================


calc_total_secs proc near
                xor     dx, dx
                xor     ax, ax
                mov     al, [di+Timestamp.hours]
                mov     bx, 3600
                mul     bx
                mov     word ptr [di+Timestamp.total_secs], ax
                mov     word ptr [di+(Timestamp.total_secs+2)], dx
                xor     dx, dx
                xor     ax, ax
                mov     al, [di+Timestamp.mins]
                mov     bx, 60
                mul     bx
                add     word ptr [di+Timestamp.total_secs], ax
                adc     word ptr [di+(Timestamp.total_secs+2)], dx
                xor     ax, ax
                mov     al, [di+Timestamp.secs]
                add     word ptr [di+Timestamp.total_secs], ax
                adc     word ptr [di+(Timestamp.total_secs+2)], 0
                retn

calc_total_secs endp


; =============== S U B R O U T I N E =======================================


get_car_id      proc near
                les     si, pCar
                assume es:nothing
                mov     al, es:[si+Car.bID]
                and     al, not 40h
                mov     ah, es:[si+Car.autoFlags]
                and     ah, 4
                shl     ah, 4
                or      al, ah
                retn

get_car_id      endp


; =============== S U B R O U T I N E =======================================


get_game_mode   proc near
                mov     dx, CONV_BASE_4 + 124h       ; session mode
                mov     ax, 0Ah
                call    read_some_byte_from_gp
                test    al, 1
                jz      short loc_11155

                mov     ah, GM_FREE_PRACTICE
                jmp     short locret_11172
                nop
loc_11155:
                test    al, 40h
                jz      short loc_1115E

                mov     ah, GM_QUALIFYING
                jmp     short locret_11172
                nop
loc_1115E:
                test    al, 2
                jz      short loc_11167

                mov     ah, GM_PRE_RACE_PRACTICE
                jmp     short locret_11172
                nop
loc_11167:
                test    al, 80h
                jz      short loc_11170

                mov     ah, GM_RACE
                jmp     short locret_11172
                nop
loc_11170:
                mov     ah, 0
locret_11172:
                retn

get_game_mode   endp


; =============== S U B R O U T I N E =======================================


get_car_race_pos proc near
                push    es
                and     al, 3Fh
                mov     es, game_ss
                assume es:nothing
                mov     di, 1940h       ; abRacePosCarIDTable
                mov     cl, 1


loc_1117F:
                mov     ah, es:[di]
                and     ah, 3Fh
                cmp     al, ah
                jz      short loc_11193

                inc     di
                inc     cl
                cmp     cl, 26
                jle     short loc_1117F

                xor     cl, cl


loc_11193:
                pop     es
                assume es:nothing
                retn

get_car_race_pos endp


; =============== S U B R O U T I N E =======================================


get_car_fastlap_pos proc near
                push    es
                and     al, 3Fh
                mov     es, game_ss
                assume es:nothing
                mov     di, 17D8h       ; abFastLapPosCarIDTable
                mov     cl, 1


loc_111A1:
                mov     ah, es:[di]
                and     ah, 3Fh
                cmp     al, ah
                jz      short loc_111B5

                inc     di
                inc     cl
                cmp     cl, 26
                jle     short loc_111A1

                xor     cl, cl


loc_111B5:
                pop     es
                assume es:nothing
                retn

get_car_fastlap_pos endp


; =============== S U B R O U T I N E =======================================


get_driver_name proc near
                assume ds:TSR
                and     al, 3Fh
                xor     ah, ah
                sub     al, 1
                mov     dx, 24
                mul     dx
                mov     si, ax
                add     si, 1A4Ah       ; osDriverNames
                push    ds
                push    ds
                pop     es
                mov     ds, cs:game_ss
                assume ds:nothing
                mov     cx, 24


loc_111D3:
                movsb
                loop    loc_111D3

                pop     ds
                assume ds:nothing
                retn

get_driver_name endp


; =============== S U B R O U T I N E =======================================


get_tracknr_in_high_nibble proc near
                mov     dx, CONV_BASE_4 + 123h
                mov     ax, 6
                call    read_some_byte_from_gp
                shl     al, 4
                and     al, 0F0h
                retn

get_tracknr_in_high_nibble endp


; =============== S U B R O U T I N E =======================================


read_some_byte_from_gp proc near
                les     bx, gp_end_of_lap_off ; seg000:1F77
                assume es:nothing
                add     bx, game_sg_off
                add     bx, ax
                sub     bx, 7
                mov     ax, es
                add     ax, dx   ; e.g. 2b95a -> seg004:124a
                sbb     ax, 357h ; not offset unk2
                mov     es, ax
                assume es:nothing
                mov     al, es:[bx]
                retn

read_some_byte_from_gp endp


; =============== S U B R O U T I N E =======================================


log_date_record proc near
                assume ds:TSR
                mov     rec_date.dr_record_type, REC_TYPE_DATE
                mov     ah, 2Ah
                int     DOS             ; DOS - GET CURRENT DATE
                                        ; Return: DL = day, DH = month, CX = year
                                        ; AL = day of the week (0=Sunday, 1=Monday, etc.)

                mov     rec_date.dr_year, cx
                mov     rec_date.dr_month, dh
                mov     rec_date.dr_day, dl
                mov     rec_date.dr_week_day, al
                mov     ah, 2Ch
                int     DOS             ; DOS - GET CURRENT TIME
                                        ; Return: CH = hours, CL = minutes, DH = seconds
                                        ; DL = hundredths of seconds

                mov     rec_date.dr_hours, ch
                mov     rec_date.dr_minutes, cl
                mov     rec_date.dr_seconds, dh
                mov     cx, 44
                mov     dx, offset rec_date
                call    log_record

                assume ds:nothing
                retn

log_date_record endp

; ---------------------------------------------------------------------------
record_off      dw 0
record_size     dw 0

; =============== S U B R O U T I N E =======================================


log_record      proc near
                assume ds:TSR
                mov     record_off, dx
                mov     record_size, cx
                mov     dx, offset logfile_path
                mov     ah, 3Dh
                mov     al, 22h
                int     DOS             ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
                                        ; DS:DX -> ASCIZ filename
                                        ; AL = access mode
                                        ; 0 - read, 1 - write, 2 - read & write


loc_11249:
                jnb     short loc_11257

                mov     dx, offset logfile_path
                mov     ah, 3Ch

loc_11250:
                mov     cx, 0
                int     DOS             ; DOS - 2+ - CREATE A FILE WITH HANDLE (CREAT)
                                        ; CX = attributes for file
                                        ; DS:DX -> ASCIZ filename (may include drive and path)

                jb      short locret_11284


loc_11257:
                mov     logfile_handle, ax
                mov     ah, 42h
                mov     al, 2
                mov     bx, logfile_handle
                xor     cx, cx
                xor     dx, dx
                int     DOS             ; DOS - 2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
                                        ; AL = method: offset from end of file

                jb      short loc_1127C

                mov     ah, 40h
                mov     bx, logfile_handle
                mov     cx, record_size
                mov     dx, record_off
                int     DOS             ; DOS - 2+ - WRITE TO FILE WITH HANDLE
                                        ; BX = file handle, CX = number of bytes to write, DS:DX -> buffer

                jb      short $+2


loc_1127C:
                mov     bx, logfile_handle
                mov     ah, 3Eh
                int     DOS             ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                                        ; BX = file handle


locret_11284:
                assume ds:nothing
                retn

log_record      endp


; =============== S U B R O U T I N E =======================================

; Attributes: noreturn

calc_crc        proc near
                call    xor_crc_code

                call    near ptr crc_func

                call    xor_crc_code

                retn

calc_crc        endp

; ---------------------------------------------------------------------------
;--------- decoded code:
;0202:128F  7856                js   000012E7 ($+56)        (no jmp)
;0202:1291  3412                xor  al,12
;0202:1293  93                  xchg bx,ax
;0202:1294  056060              add  ax,6060
;0202:1297  C3                  ret
;0202:1298  AC                  lodsb
;0202:1299  0434                add  al,34
;0202:129B  EB03                jmp  short 000012A0 ($+3)   (down)
;0202:129D  90                  nop
;0202:129E  33DB                xor  bx,bx
;0202:12A0  AC                  lodsb
;0202:12A1  02F8                add  bh,al
;0202:12A3  02DF                add  bl,bh
;0202:12A5  E2F9                loop 000012A0 ($-7)
;0202:12A7  F8                  clc
;0202:12A8  73E9                jnc  00001293 ($-17)        (up)
;0202:12AA  AC                  lodsb
;0202:12AB  3455                xor  al,55
;0202:12AD  EBF1                jmp  short 000012A0 ($-f)   (up)

xored_crc_code  db 18h
                db 0F7h
                db 1Dh
                db 4
                db 0B8h
                db 1Eh
                db 4Bh
                db 0EAh
                db 0C3h
                db 2Dh
                db 3Bh
                db 35h
                db 2Ah
                db 1
                db 89h
crc_func        db 0EAh
                db 52h
                db 9Ah
                db 0
                db 59h
                db 2Bh
                db 0C9h
                db 0C9h
                db 0ADh
                db 0AEh
                db 3Ch
                db 0BDh
                db 53h
                db 0BBh
                db 7
                db 0AEh
                db 0D4h
                dd 0AB28A17Fh

; =============== S U B R O U T I N E =======================================


xor_crc_code    proc near
                assume ds:TSR
                push    ax
                push    cx
                push    si
                push    di
                les     di, gp_end_of_lap_off ; seg000:1F77
                add     di, 66h         ; CalcAndStoreRaceTime (seg000:1FDD)
; es:di points to:
; 60 A1 4F 29 8B 16 51 29 2B 06 2B 29 1B 16 2D 29
; 2B C3 1B D1 8A 8C AC 00 81 E1 3F 00 81 E9 01 00
; C1 E1 02 BB 82 19 03 D9 36 89 07 36 89 57 02 61
; C3 A1 4F 29 8B 16 51 29 2B 44 54 1B 54 56 FF 36
; 4F 29 8F 44 54 FF 36 51 29 8F 44 56 50 52 26 8B
; 45 1A 25 FF 07
                mov     si, offset xored_crc_code
                mov     cx, 32
                xor     ah, ah


loc_112C6:
                mov     al, es:[di]
                xor     [si], al
                inc     si
                and     al, 3
                add     di, ax
                inc     di
                loop    loc_112C6

                pop     di
                pop     si
                pop     cx
                pop     ax
                assume ds:nothing
                retn

xor_crc_code    endp


; =============== S U B R O U T I N E =======================================


adjust_laptime_with_segdist proc near
                push    ds
                mov     ds, word ptr cs:pCar+2
                assume ds:nothing
                les     di, [si+Car.pSeg]
                push    ax
                push    dx
                xor     dx, dx
                mov     ax, [si+Car.wSegDist]
                mov     cx, 0
                mov     bx, 1000
                mul     bx
                mov     bx, ds:160h ; seg003:word_1E770
                shr     bx, 3
                test    [si+Car.wSpeed], 0FFFFh
                js      short loc_11318

                jz      short loc_11318

                mov     byte ptr ss:0C0h, 0 ; seg004:byte_29200
                div     [si+Car.wSpeed]
                test    byte ptr ss:0C0h, 0FFh ; seg004:byte_29200
                jnz     short loc_11318

                mov     dx, 0
                cmp     ax, bx
                jb      short loc_1131C


loc_11318:
                mov     ax, bx
                mov     dx, cx


loc_1131C:
                mov     bx, ax
                mov     cx, dx
                pop     dx
                pop     ax
                sub     ax, bx
                sbb     dx, cx
                pop     ds
                assume ds:nothing
                retn

adjust_laptime_with_segdist endp

; ---------------------------------------------------------------------------
; ----------- END OF TSR, everything below does not stay resident -----------
; ---------------------------------------------------------------------------

load_msg        db      'Installed OK',CR,LF,'$'
already_msg     db      'Already installed !',CR,LF,'$'
parse_msg       db      'Failed to determine absolute pathname.',BELL,CR,LF,'$'
cant_unload_msg db      'Unable to unload as another TSR was loaded after GPLAPTIM.',BELL,CR,LF,'$'
no_unload_msg   db      'Can''t unload as GPLAPTIM isn''t loaded!',BELL,CR,LF,'$'
unloaded_msg    db      'GPLAPTIM unloaded OK.',CR,LF,'$'
filename_offset dw      offset filename      ; "GPLAPTIM.LOG"
filename        db      'GPLAPTIM.LOG',0
cmd_line        dd      81h           ; Point to PSP command line.
cmd_line_len    db      0
msg_text        dw      0
unload_flag     db      0
tsrs_cs         dw      0

; =============== S U B R O U T I N E =======================================

; Attributes: noreturn

install         proc near

                ;**
                ;** Switch to our stack.
                ;**
                cli
                mov     ax, cs
                mov     ss, ax
                assume ss:TSR
                mov     sp, offset stack_top
                sti

                ;***
                ;*** Get Environment variable info.
                ;***
                mov     ah, 62h
                int     DOS             ; DOS - 3+ - GET PSP ADDRESS

                mov     word ptr psp_env+2, bx
                mov     word ptr cs:cmd_line+2, bx
                mov     bx, 80h
                mov     al, [bx]
                mov     cs:cmd_line_len, al

                ;**
                ;** Point DS at our common segment before calling C code.
                ;**
                mov     ax, cs
                mov     ds, ax
                assume ds:TSR
                call    parse

                or      al, al
                jnz     short parse_ok

                int     20h             ; DOS - PROGRAM TERMINATION
                                        ; returns to DOS--identical to INT 21/AH=00h

                ;**
                ;** Check to see if we are already installed
                ;** Note: BX is currently loaded TSR's CS:
                ;**
parse_ok:
                mov     cx, MAGIC_CX_IN
                mov     dx, MAGIC_DX_IN
                int     11h             ; EQUIPMENT DETERMINATION
                                        ; Return: AX = equipment flag bits

                cmp     cx, MAGIC_CX_OUT
                jnz     short not_installed

                cmp     dx, MAGIC_DX_OUT
                jnz     short not_installed

                jmp     short installed
                nop


not_installed:
                mov     al, unload_flag
                or      al, al
                jz      short not_installed1


nothing_to_unload:
                push    cs
                pop     ds
                assume ds:TSR
                mov     dx, offset no_unload_msg ; "Can't unload as GPLAPTIM isn't loaded!"...
                mov     ah, 9
                int     DOS             ; DOS - PRINT STRING
                                        ; DS:DX -> string terminated by "$"

                int     20h             ; DOS - PROGRAM TERMINATION
                                        ; returns to DOS--identical to INT 21/AH=00h


installed:
                mov     al, unload_flag
                or      al, al
                jz      short no_unload

                call    tsr_unload

        ;**
        ;** Can't unload as someone else has hooked one of our vectors.
        ;**
                push    cs
                pop     ds
                assume ds:TSR
                mov     dx, offset cant_unload_msg ; "Unable to unload as another TSR was loa"...
                mov     ah, 9
                int     DOS             ; DOS - PRINT STRING
                                        ; DS:DX -> string terminated by "$"

                int     20h             ; DOS - PROGRAM TERMINATION
                                        ; returns to DOS--identical to INT 21/AH=00h

        ;**
        ;** Already installed !
        ;**

no_unload:
                mov     dx, offset already_msg ; "Already installed !\r\n$"
                mov     ah, 9
                int     DOS             ; DOS - PRINT STRING
                                        ; DS:DX -> string terminated by "$"

                int     20h             ; DOS - PROGRAM TERMINATION
                                        ; returns to DOS--identical to INT 21/AH=00h

        ;**
        ;** Convert current filename to fully qualified name.
        ;**

not_installed1:
                mov     ax, ds
                mov     es, ax
                mov     si, filename_offset
                mov     di, offset logfile_path
                mov     ah, 60h
                int     DOS             ; DOS - RESOLVE PATH STRING TO CANONICAL PATH STRING
                                        ; DS:SI -> ASCIZ relative path string or directory name
                                        ; ES:DI -> 128-byte buffer for ASCIZ canonical fully qualified name

                jnb     short name_q_ok

                mov     dx, offset parse_msg ; "Failed to determine absolute pathname."...
                mov     ah, 9
                int     DOS             ; DOS - PRINT STRING
                                        ; DS:DX -> string terminated by "$"

                int     20h             ; DOS - PROGRAM TERMINATION
                                        ; returns to DOS--identical to INT 21/AH=00h

        ;**
        ;** Install our vectors
        ;**

name_q_ok:
                call    tsr_vectors_install

        ;**
        ;** Patch our entry points patches!
        ;**
                mov     al, 0FFh
                mov     time.secs, al
                mov     time.mins, al
                mov     time.hours, al

                mov     bx, offset hook1_off
                mov     ax, cs
                mov     word ptr [bx], offset hook1_end_of_lap
                mov     [bx+2], ax

                mov     bx, offset hook2_off
                mov     word ptr [bx], offset hook2_pit_in
                mov     [bx+2], ax

                mov     bx, offset hook3_off
                mov     word ptr [bx], offset hook3_pit_out
                mov     [bx+2], ax

                mov     bx, offset hook4_off
                mov     word ptr [bx], offset hook4_player_grip
                mov     [bx+2], ax

                mov     bx, offset hook5_off
                mov     word ptr [bx], offset hook5_seg
                mov     [bx+2], ax

                mov     bx, offset hook6_off
                mov     word ptr [bx], offset hook6_randomize_player_car_power
                mov     [bx+2], ax

                mov     bx, offset hook7_off
                mov     word ptr [bx], offset hook7_randomize_ccplayer_car_power
                mov     [bx+2], ax

                mov     bx, offset hook8_off
                mov     word ptr [bx], offset hook8_hotseat_automatic
                mov     [bx+2], ax

                mov     bx, offset hook9_off
                mov     word ptr [bx], offset hook9_hotseat_control
                mov     [bx+2], ax

        ;**
        ;** Display message
        ;**
                mov     dx, offset load_msg ; "Installed OK\r\n$"
                mov     ah, 9
                int     DOS             ; DOS - PRINT STRING
                                        ; DS:DX -> string terminated by "$"

        ;***
        ;*** Free environment memory
        ;***
                les     bx, psp_env
                mov     ax, es:[bx]
                mov     es, ax
                mov     ah, 49h
                int     DOS             ; DOS - 2+ - FREE MEMORY
                                        ; ES = segment address of area to be freed

        ;***
        ;*** Terminate and stay resident for our resident routines only.
        ;***
                mov     ax, offset load_msg ; "Installed OK\r\n$"
                mov     cl, 4
                shr     ax, cl
                inc     ax
                mov     dx, ax
                mov     ax, 3100h
                int     DOS             ; DOS - DOS 2+ - TERMINATE BUT STAY RESIDENT
                                        ; AL = exit code, DX = program size, in paragraphs

        ;***
        ;*** Not reached
        ;***

quit:
                retn

install         endp

;*****************************************************************************
;** Purpose: Uninstall (if possible). Check vectors still point to us.
;**
;** Returns: If unistall fails, terminates if successful.
;**


; =============== S U B R O U T I N E =======================================


tsr_unload      proc near
                mov     tsrs_cs, bx
                mov     ax, 3508h
                int     DOS             ; DOS - 2+ - GET INTERRUPT VECTOR
                                        ; AL = interrupt number
                                        ; Return: ES:BX = value of interrupt vector

                mov     ax, es
                cmp     ax, tsrs_cs
                jnz     short cant_unload

                cmp     bx, offset new08_hndlr
                jnz     short cant_unload

                mov     ax, 3511h
                int     DOS             ; DOS - 2+ - GET INTERRUPT VECTOR
                                        ; AL = interrupt number
                                        ; Return: ES:BX = value of interrupt vector

                mov     ax, es
                cmp     ax, tsrs_cs
                jnz     short cant_unload

                cmp     bx, offset new11_hndlr
                jnz     short cant_unload

                mov     ax, 3521h
                int     DOS             ; DOS - 2+ - GET INTERRUPT VECTOR
                                        ; AL = interrupt number
                                        ; Return: ES:BX = value of interrupt vector

                mov     ax, es
                cmp     ax, tsrs_cs
                jnz     short cant_unload

                cmp     bx, offset new21_hndlr
                jnz     short cant_unload

        ;**
        ;** Restore original vectors, point DS at currently installed TSR.
        ;**
                mov     es, tsrs_cs
                lds     dx, es:old08_handler
                mov     ax, 2508h
                int     DOS             ; DOS - SET INTERRUPT VECTOR
                                        ; AL = interrupt number
                                        ; DS:DX = new vector to be used for specified interrupt

                lds     dx, es:old11_handler
                mov     ax, 2511h
                int     DOS             ; DOS - SET INTERRUPT VECTOR
                                        ; AL = interrupt number
                                        ; DS:DX = new vector to be used for specified interrupt

                lds     dx, es:old21_handler
                mov     ax, 2521h
                int     DOS             ; DOS - SET INTERRUPT VECTOR
                                        ; AL = interrupt number
                                        ; DS:DX = new vector to be used for specified interrupt

        ;**
        ;** Display message.
        ;**
                push    cs
                pop     ds
                mov     dx, offset unloaded_msg ; "GPLAPTIM unloaded OK.\r\n$"
                mov     ah, 9
                int     DOS             ; DOS - PRINT STRING
                                        ; DS:DX -> string terminated by "$"

        ;**
        ;** Free memory. Set original PSP, then terminate process.
        ;**
                mov     es, tsrs_cs
                mov     ah, 49h
                int     DOS             ; DOS - 2+ - FREE MEMORY
                                        ; ES = segment address of area to be freed

                int     DOS_TERMINATE   ; DOS - PROGRAM TERMINATION
                                        ; returns to DOS--identical to INT 21/AH=00h

cant_unload:
                retn

tsr_unload      endp


; =============== S U B R O U T I N E =======================================


tsr_vectors_install proc near
                mov     ax, 3508h
                int     DOS             ; DOS - 2+ - GET INTERRUPT VECTOR
                                        ; AL = interrupt number
                                        ; Return: ES:BX = value of interrupt vector

                mov     word ptr old08_handler, bx
                mov     word ptr old08_handler+2, es
                mov     dx, offset new08_hndlr
                mov     ax, 2508h
                int     DOS             ; DOS - SET INTERRUPT VECTOR
                                        ; AL = interrupt number
                                        ; DS:DX = new vector to be used for specified interrupt

                mov     ax, 3511h
                int     DOS             ; DOS - 2+ - GET INTERRUPT VECTOR
                                        ; AL = interrupt number
                                        ; Return: ES:BX = value of interrupt vector

                mov     word ptr old11_handler, bx
                mov     word ptr old11_handler+2, es
                mov     dx, offset new11_hndlr
                mov     ax, 2511h
                int     DOS             ; DOS - SET INTERRUPT VECTOR
                                        ; AL = interrupt number
                                        ; DS:DX = new vector to be used for specified interrupt


loc_115B4:
                mov     ax, 3521h
                int     DOS             ; DOS - 2+ - GET INTERRUPT VECTOR
                                        ; AL = interrupt number
                                        ; Return: ES:BX = value of interrupt vector

                mov     word ptr old21_handler, bx
                mov     word ptr old21_handler+2, es
                mov     dx, offset new21_hndlr
                mov     ax, 2521h
                int     DOS             ; DOS - SET INTERRUPT VECTOR
                                        ; AL = interrupt number
                                        ; DS:DX = new vector to be used for specified interrupt

                retn

tsr_vectors_install endp


; =============== S U B R O U T I N E =======================================


wrt_msg         proc near
                mov     dx, msg_text
                mov     ah, 9
                int     DOS             ; DOS - PRINT STRING
                                        ; DS:DX -> string terminated by "$"

                retn

wrt_msg         endp

                align 2

; =============== S U B R O U T I N E =======================================

; Attributes: bp-based frame

parse           proc near

len             = word ptr -2

                push    bp
                mov     bp, sp
                sub     sp, 2
                push    si
                push    di
                push    (offset title_msg+4)
                call    display_msg

                add     sp, 2
                xor     cx, cx
                xor     dx, dx
                mov     al, cmd_line_len
                cbw
                mov     [bp+len], ax
                jmp     short loc_1161D

loc_115F2:
                les     bx, cmd_line
                add     bx, dx
                mov     ax, bx
                cmp     byte ptr es:[bx], CR
                jz      short loc_11612

                cmp     byte ptr es:[bx], LF
                jz      short loc_11612

                cmp     byte ptr es:[bx], ' '
                jz      short loc_11612

                cmp     byte ptr es:[bx], TAB
                jnz     short loc_1161C


loc_11612:
                mov     es, word ptr cmd_line+2
                mov     bx, ax
                mov     byte ptr es:[bx], 0


loc_1161C:
                inc     dx


loc_1161D:
                cmp     [bp+len], dx
                jge     short loc_115F2

                jmp     loc_117A8

loc_11625:
                or      cx, cx
                jnz     short loc_1162C

                jmp     loc_11791

loc_1162C:
                les     bx, cmd_line
                cmp     byte ptr es:[bx], 'p'
                jnz     short loc_1163E

                mov     log_player_times_only, 1
                jmp     loc_1178D

loc_1163E:
                les     bx, cmd_line
                cmp     byte ptr es:[bx], 'f'
                jnz     short loc_11673

                mov     ax, word ptr cmd_line
                inc     ax
                mov     filename_offset, ax
                cmp     cmd_line_len, 0
                jz      short loc_11662

                mov     bx, filename_offset
                cmp     byte ptr [bx], 0
                jz      short loc_11662

                jmp     loc_1178D

loc_11662:
                push    offset aGplaptimMissin ; "GpLapTim: missing log filename.\n"
                call    display_msg

                add     sp, 2
                call    Usage

                xor     ax, ax
                jmp     loc_117B9

loc_11673:
                les     bx, cmd_line
                cmp     byte ptr es:[bx], 'u'
                jnz     short loc_11685

                mov     unload_flag, 1
                jmp     loc_1178D

loc_11685:
                les     bx, cmd_line
                cmp     byte ptr es:[bx], 'a'
                jnz     short loc_116C0

                mov     ax, word ptr cmd_line
                inc     ax
                push    word ptr cmd_line+2
                push    ax
                call    atoi

                add     sp, 4
                mov     split1_perc, ax
                cmp     split1_perc, 1
                jl      short loc_116B2

                cmp     split1_perc, 99
                jg      short loc_116B2

                jmp     loc_1178D

loc_116B2:
                push    offset aGplaptimInvali ; "GpLapTim: Invalid 1st split percentage "...
                call    display_msg

                add     sp, 2
                xor     ax, ax
                jmp     loc_117B9

loc_116C0:
                les     bx, cmd_line
                cmp     byte ptr es:[bx], 'b'
                jnz     short loc_116FB

                mov     ax, word ptr cmd_line
                inc     ax
                push    word ptr cmd_line+2
                push    ax
                call    atoi

                add     sp, 4
                mov     split2_perc, ax
                cmp     split2_perc, 1
                jl      short loc_116ED

                cmp     split2_perc, 99
                jg      short loc_116ED

                jmp     loc_1178D

loc_116ED:
                push    offset aGplaptimInvali_0 ; "GpLapTim: Invalid 2nd split percentage "...
                call    display_msg

                add     sp, 2
                xor     ax, ax
                jmp     loc_117B9

loc_116FB:
                les     bx, cmd_line
                cmp     byte ptr es:[bx], 'c'
                jnz     short loc_11733

                mov     ax, word ptr cmd_line
                inc     ax
                push    word ptr cmd_line+2
                push    ax
                call    atoi

                add     sp, 4
                mov     split3_perc, ax
                cmp     split3_perc, 1
                jl      short loc_11725

                cmp     split3_perc, 99
                jg     short loc_11725

                jmp     loc_1178D

loc_11725:
                push    offset aGplaptimInvali_1 ; "GpLapTim: Invalid 3rd split percentage "...
                call    display_msg

                add     sp, 2
                xor     ax, ax
                jmp     loc_117B9

loc_11733:
                les     bx, cmd_line
                cmp     byte ptr es:[bx], 's'
                jnz     short loc_11776

                mov     ax, word ptr cmd_line
                inc     ax
                push    word ptr cmd_line+2
                push    ax
                call    atoi

                add     sp, 4
                mov     stop_time, ax
                cmp     stop_time, 1
                jl      short loc_1175D

                cmp     stop_time, 15
                jle     short loc_1176A


loc_1175D:
                push    offset aGplaptimSStopT ; "GpLapTim: -s Stop time must be between "...
                call    display_msg
                add     sp, 2

                xor     ax, ax
                jmp     short loc_117B9

loc_1176A:
                mov     ax, 1000
                imul    stop_time
                mov     stop_time, ax
                jmp     short loc_1178D

loc_11776:
                les     bx, cmd_line
                cmp     byte ptr es:[bx], 'w'
                jnz     short __end_w

                mov     ax, word ptr cmd_line
                inc     ax
                push    word ptr cmd_line+2
                push    ax
                call    atoi
                add     sp, 4

                mov     cx, 22
                mul     cx
                add     ax, 632
                mov     ccplayer_power, ax

                jmp     short loc_1178D

__end_w:
                les     bx, cmd_line
                cmp     byte ptr es:[bx], '?'
                jz      short loc_11786

                cmp     byte ptr es:[bx], 'h'
                jnz     short loc_1178D


loc_11786:
                call    Usage

                xor     ax, ax
                jmp     short loc_117B9

loc_1178D:
                xor     cx, cx
                jmp     short loc_117A4

loc_11791:
                les     bx, cmd_line
                cmp     byte ptr es:[bx], '-'
                jz      short loc_117A1

                cmp     byte ptr es:[bx], '/'
                jnz     short loc_117A4


loc_117A1:
                mov     cx, 1


loc_117A4:
                inc     word ptr cmd_line


loc_117A8:
                mov     al, cmd_line_len
                dec     cmd_line_len
                or      al, al
                jz      short loc_117B6

                jmp     loc_11625

loc_117B6:
                mov     ax, 1


loc_117B9:
                pop     di
                pop     si
                leave
                retn

parse           endp


; =============== S U B R O U T I N E =======================================


Usage           proc near
                push    si
                push    di
                push    offset aUsageGplaptimH ; "\nUsage: gplaptim [-h?]  [-aN -bN -cN -"...
                call    display_msg

                add     sp, 2
                pop     di
                pop     si
                retn

Usage           endp


; =============== S U B R O U T I N E =======================================

; Attributes: bp-based frame

display_msg     proc near

arg_0           = word ptr  4

                push    bp
                mov     bp, sp
                push    si
                push    di
                jmp     short loc_117F0

loc_117D2:
                mov     bx, [bp+arg_0]
                cmp     byte ptr [bx], LF
                jnz     short loc_117E2

                push    CR
                call    display_chr

                add     sp, 2


loc_117E2:
                mov     bx, [bp+arg_0]
                inc     [bp+arg_0]
                push    word ptr [bx]
                call    display_chr

                add     sp, 2


loc_117F0:
                mov     bx, [bp+arg_0]
                cmp     byte ptr [bx], 0
                jnz     short loc_117D2

                pop     di
                pop     si
                pop     bp
                retn

display_msg     endp


; =============== S U B R O U T I N E =======================================

; Attributes: bp-based frame

display_chr     proc near

s               = byte ptr -2
c               = byte ptr  4

                push    bp
                mov     bp, sp
                sub     sp, 2
                push    si
                push    di
                mov     al, [bp+c]
                mov     [bp+s], al
                mov     [bp+s+1], '$'
                lea     ax, [bp+s]
                mov     msg_text, ax
                call    wrt_msg

                pop     di
                pop     si
                leave
                retn

display_chr     endp


; =============== S U B R O U T I N E =======================================

; Attributes: bp-based frame

atoi            proc near

f               = word ptr -2
p               = dword ptr  4

                push    bp
                mov     bp, sp
                sub     sp, 2
                push    si
                push    di
                xor     cx, cx
                mov     [bp+f], 0


loc_1182A:
                les     bx, [bp+p]
                mov     al, es:[bx]
                cbw
                cmp     ax, '+'
                jz      short loc_11862

                jg      short loc_11844

                cmp     ax, 9
                jz      short loc_1184E

                cmp     ax, ' '
                jz      short loc_1184E

                jmp     short loc_11865

loc_11844:
                cmp     ax, '-'
                jnz     short loc_11865

                inc     [bp+f]
                jmp     short loc_11862

loc_1184E:
                inc     word ptr [bp+p]
                jmp     short loc_1182A

loc_11853:
                imul    cx, 10
                les     bx, [bp+p]
                mov     al, es:[bx]
                cbw
                add     cx, ax
                add     cx, -'0'


loc_11862:
                inc     word ptr [bp+p]


loc_11865:
                les     bx, [bp+p]
                cmp     byte ptr es:[bx], '0'
                jl      short loc_11874

                cmp     byte ptr es:[bx], '9'
                jle     short loc_11853


loc_11874:
                cmp     [bp+f], 0
                jz      short loc_11880

                mov     ax, cx
                neg     ax
                jmp     short loc_11882

loc_11880:
                mov     ax, cx


loc_11882:
                pop     di
                pop     si
                leave
                retn

atoi            endp

; ---------------------------------------------------------------------------
title_msg       db      '@(#)GpLapTim V7.1 9th Dec 2018 - Grand Prix/World Circuit Lap Time Logger.',LF
                db      'Copyright (c) Trevor Kellaway (CIS:100331,2330) 1995 - All Rights Reserved.',LF
                db      'Copyright (c) Rene Smit 2018 - All Rights Reserved.',LF,LF,0
aGplaptimMissin db      'GpLapTim: missing log filename.',LF,0
aGplaptimInvali db      'GpLapTim: Invalid 1st split percentage (-a)',LF,0
aGplaptimInvali_0 db    'GpLapTim: Invalid 2nd split percentage (-b)',LF,0
aGplaptimInvali_1 db    'GpLapTim: Invalid 3rd split percentage (-c)',LF,0
aGplaptimSStopT db      'GpLapTim: -s Stop time must be between 1s and 15s',LF,0
aUsageGplaptimH db      LF
                db      'Usage: gplaptim [-h?]  [-aN -bN -cN -s] [-p] [-u] [-f(filename)]',LF
                db      '       -f(name)  Specify log filename''s location.',LF
                db      '       -p        Log player''s times only.',LF
                db      LF
                db      '       -aN       1st split time percentage (default 25%).',LF
                db      '       -bN       2nd split time percentage (default 50%).',LF
                db      '       -cN       3rd split time percentage (default 75%).',LF
                db      LF
                db      '       -sN       Stop (freeze) delay in seconds (default 10s).',LF
                db      LF
                db      '       -wN       Set BHP for CC player during hotseat (default 716).',LF
                db      LF
                db      '       -h,-?     This help message.',LF
                db      '       -u        Unload TSR.',LF,0
TSR             ends


                end start
