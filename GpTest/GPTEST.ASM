;**
;** File   : gptest.asm
;**
                        name    gptest
                        title   'Test Support Tool'

;*****************************************************************************
;**
;** Include files
;**

                        include segments.inc
                        include render.inc
                        include gpdef.inc
                        include gptest.inc
                        include gptest.ash

                        LOCALS  __

;*****************************************************************************
;**
;** Equates & Macros
;**

TICK_TIME_MS            equ     18                      ; Approx ms of a tick
DELAY_TIME              equ     (500 / TICK_TIME_MS)    ; Time to display hook msg



;*****************************************************************************
;**
;** Externals
;**

TSR_CODE                segment

                        extrn   install:far

TSR_CODE                ends


;*****************************************************************************
;**
;** Exports
;**

                        public  limit_cars_race
                        public  rng_mode
                        public  rng_seed
                        public  keep_fw
                        public  no_drivers

                        public  new21_hndlr
                        public  hook_gp_exe
                        public  announce_hook
                        public  hook_load_track
                        public  hook_new_session
                        public  hook_pausing
                        public  hook_frame


;*****************************************************************************
;**
;** Data
;**

TSR_DATA                segment

psp_env                 label   dword
psp_env_off             dw      002Ch           ; Offset of env pointer in PSP
psp_seg                 dw      ?               ; PSP segment

old21_hndlr             label   dword           ; DOS Int 21 handler
old21_off               dw      ?
old21_seg               dw      ?

delay_count             db      0

limit_cars_race         db      26
rng_mode                db      0
rng_seed                db      0
keep_fw                 db      0
no_drivers              db      0

err_notify              db      0

game_ds                 dw      ?
game_ss                 dw      ?

gp_car_base_weight      dd      ?
gp_sel_car_ptr          dd      ?
gp_sestime_left         dd      ?
gp_rnd1                 dd      ?
gp_rnd2                 dd      ?
gp_rnd1_next            dd      ?
gp_rnd_car_next         dd      ?
gp_rnd2_next            dd      ?
gp_display_paused       dd      ?
gp_copy_screen_buf      dd      ?
gp_update_keys          dd      ?
gp_mute_audio           dd      ?
gp_play_engine_sound    dd      ?

q_filename              db      ?
                        db      64 + 13 dup(?)
filehandle              dw      ?


;*****************************************************************************
;**
;** European V1.05 GP.EXE - DOS Version code fragment
;**
;** 92D2:5C76  B0 00 mov    al,0
;** 92D2:5C78  B4 30 mov    ah,30h
;** 92D2:5C7A  CD 21 int    21h	        @06d2eaH in GPU.EXE
;** 92D2:5C7C  0A C0 or	    al,al
;** 92D2:5C7E  75 03 jnz    loc_5593
;**

GP_EUR_RET_ADDR         equ     0B74Ch
GP_ITA_RET_ADDR         equ     0B74Ch
GP_USA_RET_ADDR         equ     0B727h
MATCH_OFFSET            equ     6

gp_code_eur             db      0B0h, 000h
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
MATCH_BYTES_EUR         equ     $ - gp_code_eur

gp_code_ita             db      0B0h, 000h
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
                        db      09Ah, 0EBh, 018h
MATCH_BYTES_ITA         equ     $ - gp_code_ita

gp_code_usa             db      0B0h, 00h, 0B4h, 30h, 0CDh, 21h, 0Ah, 0C0h, 75h, 03h
MATCH_BYTES_USA         equ     $ - gp_code_usa

SEG_COUNT               equ     14
eu_seg_base             dw      0, 0F47h, 19EDh, 1E61h, 2914h, 30CAh, 40C1h, 50BAh, 605Ah, 6BE7h, 7BCEh, 8B6Eh, 8CE6h, 8EAAh, 9151h
us_seg_base             dw      0, 0F47h, 19EDh, 1E5Eh, 2911h, 30C7h, 40BEh, 50B7h, 6057h, 6BE2h, 7BC4h, 8B64h, 8CDCh, 8EA0h, 9147h
it_seg_base             dw      0, 0F47h, 19EDh, 1E5Dh, 2910h, 30C6h, 40BDh, 50B6h, 6056h, 6BE3h, 7BC5h, 8B65h, 8CDDh, 8EA1h, 9148h
eu_to_us                dw      2, 00179h, -24h
                        dw      2, 00C25h, -08h
                        dw      8, 0A04Eh, -09h
                        dw      8, 0A0D2h, -09h
                        dw      8, 0A248h, -09h
                        dw      8, 0A46Ch, -01h
                        dw      8, 0A4A0h, -09h
                        dw      9, 00576h, -4Eh
                        dw      9, 005A6h, -0Ah
                        dw      9, 07932h,  01h
                        dw      SEG_COUNT, 0, 0
eu_to_it                dw      2, 00179h, -2Ch
                        dw      2, 00C25h, -08h
                        dw      9, 00576h, -4Eh
                        dw      9, 005A6h, -0Ah
                        dw      9, 07932h,  01h
                        dw      SEG_COUNT, 0, 0
offset_diffs            dw      eu_to_us, eu_to_it


gp_ver                  db      GP_VER_UNK
hooked_msg              db      CR, LF, "GpTest activated OK", CR, LF, '$'
tyre_compound_chars     db      "ABCDQW?"

TSR_DATA                ends

;****************************************************************************
;**
;** Our DOS Int 21 handler
;**

TSR_CODE                segment

new21_hndlr             proc    near

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
PUSHED_WORDS            equ     (1 + 8 + 1 + 1)

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
try_us:                 cmp     di, GP_USA_RET_ADDR
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
                        jmp     DWORD PTR cs:old21_hndlr
new21_hndlr             endp

TSR_CODE                ends


;*****************************************************************************
;** Purpose: Hook our code.
;**
;** Returns: Nothing.
;**

TSR_CODE                segment

hook_gp_exe             proc    near
                        mov     cs:err_notify, 0

                        call    disable_default_driver
                        call    lower_race_dist_limit
                        call    init_rng
                        call    init_hook_load_track
                        call    init_hook_new_session
                        call    init_hook_pausing
                        call    init_hook_frame
                        call    patch_menu
                        call    init_race_car_limit
                        call    init_ccsetup
                        call    fix_max_fuel_load
                        ; call    disable_overtaking
                        ; call    disable_cc_driving

                        ret
hook_gp_exe             endp

TSR_CODE                ends


;-------------------------------------------------------

TSR_CODE                segment

disable_default_driver  proc    near
                        mov     al, no_drivers
                        cmp     al, 0
                        jz      __end
                        push    2
                        push    0C28h
                        call    calc_offset_esdi
                        mov     al, 90h
                        stosb
                        stosb
                        stosb
__end:
                        ret
disable_default_driver  endp

TSR_CODE                ends


;-------------------------------------------------------

TSR_CODE                segment

lower_race_dist_limit   proc    near
                        push    13
                        push    05A0h
                        call    calc_offset_esdi
                        mov     ax, 1001h
                        stosw
                        add     di, 3
                        stosw
                        ret
lower_race_dist_limit   endp

TSR_CODE                ends


;-------------------------------------------------------

TSR_DATA                segment

hook_random_seed_start  db      0BBh    ; mov bx, imm
hook_random_seed_value  dw      ?
HOOK_RANDOM_SEED_LENGTH equ     $ - hook_random_seed_start

; these offsets are all in seg000
; more can be found in other segments, but they don't seem to be too relevant
;                         ; ----- RandomByte1 -----
; hook_random_num_offs    dw      3CACh, 3CFCh, 3D05h     ; pitstop timers
;                         dw      1030h                   ; 50% chance: or [si+Car.flags_BC], 2
;                         dw      1FA4h                   ; 14% chance: CC pitting in practice
;                         dw      0FBEh, 0FD2h            ; car grip related
;                         dw      4C97h, 4CEFh, 4D23h     ; car grip related
;                         dw      4B20h, 4C13h, 4C44h     ; car power
;                         dw      526Fh,                  ; non-race car in pitbox
;                         dw      50B7h, 528Dh            ; starting grid related
;                         dw      7E62h                   ; wobbling (mostly on grass)
;                         dw      9D1Eh, 9D31h, 9D59h     ; revs (not just pitch, does affect times)
;                         dw      0A873h                  ; wing setup variation
;                         dw      0A964h                  ; tyre compound variation
;                         dw      0D96Ch                  ; race start car launch speed (very low value)
;                         dw      0D9E4h                  ; race start driver reaction time
;                         dw      0DC6Eh                  ; start lights duration
;                         dw      0E212h                  ; 6.25% chance: wet weather
;                         ; dw      059Dh, 05ACh            ; track related, hangs with rng_value 127
;                         ; ----- RandomByte2 -----
;                         dw      3ECEh, 3F26h            ; pit stop timing
;                         dw      0C147h, 0C182h, 0C192h  ; damage related
;                         dw      0C1A8h, 0C1BDh          ; damage related
;                         dw      0C5F6                   ; front/rear camera turning clockwise or not?
;                         dw      0CE02                   ; related to marshalls or something
;
; HOOK_RANDOM_NUM_COUNT    equ    ($ - hook_random_num_offs) / 2
;
; hook_random_num_start   db      0B8h, 0, 0  ; mov al, 0
;                         db      90h         ; nop
; HOOK_RANDOM_NUM_LENGTH  equ     $ - hook_random_num_start

TSR_DATA                ends


TSR_CODE                segment

init_rng                proc    near
                        mov     al, rng_mode
                        or      al, al
                        jz      __end
                        cmp     al, 1
                        je      __seed
                        ; cmp     al, 2
                        ; je      __seed
                        ; cmp     al, 3
                        ; je      __rng
                        jmp     __end
; __rng:
;                         ; disable RNG's
;                         mov     al, rng_value
;                         mov     BYTE PTR [hook_random_num_start + 1], al
;                         mov     si, OFFSET TSR:hook_random_num_offs
;                         mov     cx, HOOK_RANDOM_NUM_COUNT
; __loop0:
;                         lodsw
;                         push    0
;                         push    ax
;                         call    calc_offset_esdi
;
;                         push    si
;                         push    cx
;                         mov     si, OFFSET TSR:hook_random_num_start
;                         mov     cx, HOOK_RANDOM_NUM_LENGTH
; __loop1:
;                         movsb
;                         loop    __loop1
;                         pop     cx
;                         pop     si
;
;                         loop    __loop0
;
;                         jmp     __end
__seed:
                        ; seed from user supplied value instead of timer
                        xor     ax, ax
                        mov     al, rng_seed
                        mov     hook_random_seed_value, ax
                        mov     si, OFFSET TSR:hook_random_seed_start
                        push    2
                        push    0C3Ch
                        call    calc_offset_esdi
                        mov     cx, HOOK_RANDOM_SEED_LENGTH
__loop:
                        movsb
                        loop    __loop
__end:
                        ret
init_rng                endp

TSR_CODE                ends


;-------------------------------------------------------

TSR_CODE                segment

init_hook_load_track    proc    near
                        mov     dx, 3
                        mov     ax, 2983h
                        mov     bx, OFFSET TSR:gp_rnd1
                        call    store_offset

                        mov     dx, 3
                        mov     ax, 8C2h
                        mov     bx, OFFSET TSR:gp_rnd2
                        call    store_offset

                        mov     dx, 0
                        mov     ax, 4D7h
                        mov     bx, OFFSET TSR:gp_rnd1_next
                        call    store_offset

                        mov     dx, 0
                        mov     ax, 5297h
                        mov     bx, OFFSET TSR:gp_rnd_car_next
                        call    store_offset

                        mov     dx, 0
                        mov     ax, 509h
                        mov     bx, OFFSET TSR:gp_rnd2_next
                        call    store_offset

                        mov     si, OFFSET TSR:hook_load_track_start
                        push    2
                        push    029Fh
                        call    calc_offset_esdi
                        mov     cx, HOOK_LOAD_TRACK_LENGTH
                        mov     bx, OFFSET TSR:hook_load_track_orig
__loop:
                        mov     al, es:[di]
                        mov     cs:[bx], al
                        add     bx, 1
                        movsb
                        loop    __loop
                        ret
init_hook_load_track    endp

TSR_CODE                ends


;-------------------------------------------------------

TSR_CODE                segment

init_hook_new_session   proc    near
                        mov     si, OFFSET TSR:hook_new_session_start
                        push    0
                        push    0EE0Ch
                        call    calc_offset_esdi
                        mov     cx, HOOK_NEW_SESSION_LENGTH
                        mov     bx, OFFSET TSR:hook_new_session_orig
__loop:
                        mov     al, es:[di]
                        mov     cs:[bx], al
                        add     bx, 1
                        movsb
                        loop    __loop
                        ret
init_hook_new_session   endp

TSR_CODE                ends


;-------------------------------------------------------

TSR_CODE                segment

init_hook_pausing       proc    near
                        mov     dx, 0
                        mov     ax, 0EB6Eh
                        mov     bx, OFFSET TSR:gp_display_paused
                        call    store_offset

                        mov     dx, 2
                        mov     ax, 31FAh
                        mov     bx, OFFSET TSR:gp_copy_screen_buf
                        call    store_offset

                        mov     dx, 2
                        mov     ax, 2D8Ch
                        mov     bx, OFFSET TSR:gp_mute_audio
                        call    store_offset

                        mov     dx, 2
                        mov     ax, 2CFDh
                        mov     bx, OFFSET TSR:gp_play_engine_sound
                        call    store_offset

                        mov     dx, 0
                        mov     ax, 5h
                        mov     bx, OFFSET TSR:gp_update_keys
                        call    store_offset

                        mov     si, OFFSET TSR:hook_pausing_start
                        push    0
                        push    0DDEDh ;0DE6Dh
                        call    calc_offset_esdi
                        mov     cx, HOOK_PAUSING_LENGTH
                        mov     bx, OFFSET TSR:hook_pausing_orig
__loop:
                        mov     al, es:[di]
                        mov     cs:[bx], al
                        add     bx, 1
                        movsb
                        loop    __loop
                        ret
init_hook_pausing       endp

TSR_CODE                ends


;-------------------------------------------------------

TSR_CODE                segment

init_hook_frame         proc    near
                        mov     dx, 3
                        mov     ax, 2A6Fh
                        mov     bx, OFFSET TSR:gp_car_base_weight
                        call    store_offset

                        mov     dx, 0
                        mov     ax, 0E17h
                        mov     bx, OFFSET TSR:gp_hex2bcd
                        call    store_offset

                        mov     dx, 2
                        mov     ax, 14C0h
                        mov     bx, OFFSET TSR:gp_lcd_init
                        call    store_offset

                        mov     dx, 2
                        mov     ax, 38C9h
                        mov     bx, OFFSET TSR:gp_render_char
                        call    store_offset

                        mov     dx, 3
                        mov     ax, 97Fh
                        mov     bx, OFFSET TSR:gp_sel_car_ptr
                        call    store_offset

                        mov     dx, 3
                        mov     ax, 0958h
                        mov     bx, OFFSET TSR:gp_led_pos_ptr
                        call    store_offset

                        mov     dx, 3
                        mov     ax, 298Dh
                        mov     bx, OFFSET TSR:gp_sestime_left
                        call    store_offset

                        mov     si, OFFSET TSR:hook_frame_start
                        mov     bx, OFFSET TSR:hook_frame_orig
                        push    2
                        push    2127h
                        call    calc_offset_esdi
                        mov     cx, HOOK_FRAME_LENGTH
__loop:
                        mov     dl, es:[di]
                        mov     [bx], dl    ; copy original code to hook_frame_orig
                        inc     bx
                        movsb
                        loop    __loop

                        ret
init_hook_frame         endp

TSR_CODE                ends


;-------------------------------------------------------

TSR_CODE                segment

patch_menu              proc    near
                        ; skip language menu
                        push    9
                        push    534h
                        call    calc_offset_esdi
                        mov     WORD PTR es:[di], 0 ; English
                        mov     WORD PTR es:[di+2], 9090h
                        mov     BYTE PTR es:[di+4], 90h

                        ; skip startup menu
                        push    9
                        push    433h
                        call    calc_offset_esdi
                        mov     WORD PTR es:[di], 1 ; Main menu
                        mov     WORD PTR es:[di+2], 9090h
                        mov     BYTE PTR es:[di+4], 90h
                        ret
patch_menu              endp

TSR_CODE                ends


;-------------------------------------------------------

TSR_CODE                segment

init_race_car_limit     proc    near
                        xor     dx, dx
                        mov     dl, limit_cars_race
                        cmp     dl, 0
                        jz      __end
                        cmp     dl, 25
                        ja      __end
                        push    0
                        push    0ECB9h
                        call    calc_offset_esdi
                        mov     BYTE PTR es:[di], 24h
                        push    0
                        push    5856h
                        call    calc_offset_esdi
                        mov     ax, WORD PTR es:[di]
                        push    dx
                        mov     ax, dx
                        mov     dx, 0C0h
                        mul     dx
                        sub     ax, 180h
                        add     WORD PTR es:[di], ax ; first car offset
                        pop     dx
                        mov     ax, 26
                        sub     ax, dx
                        mov     WORD PTR es:[di+3], ax ; #cars to be removed
__end:
                        ret
init_race_car_limit     endp

TSR_CODE                ends


;-------------------------------------------------------

TSR_CODE                segment

fix_max_fuel_load       proc    near
                        push    13
                        push    05A2h
                        call    calc_offset_esdi
                        mov     BYTE PTR es:[di], 73h   ; JGE -> JAE (signed to unsigned)
                        push    0
                        push    5B67h
                        call    calc_offset_esdi
                        ; following fix has drawback that fuel load in the victory lap goes to max
                        mov     BYTE PTR es:[di], 0E3h  ; IMUL BX -> MUL BX (signed to unsigned)
                        ret
fix_max_fuel_load       endp

TSR_CODE                ends


;-------------------------------------------------------

TSR_CODE                segment

init_ccsetup            proc    near
                        xor     dx, dx
                        mov     dl, keep_fw
                        cmp     dl, 0
                        jz      __end
                        push    0
                        push    0A897h
                        call    calc_offset_esdi
                        mov     WORD PTR es:[di], 9090h
__end:
                        ret
init_ccsetup            endp

TSR_CODE                ends

;-------------------------------------------------------


TSR_CODE                segment

disable_overtaking      proc    near
                        push    0
                        push    586Dh
                        call    calc_offset_esdi
                        mov     al, 90h
                        stosb
                        stosb
                        stosb
                        ret
disable_overtaking      endp

TSR_CODE                ends

TSR_CODE                segment


;-------------------------------------------------------

disable_cc_driving      proc    near
                        push    0
                        push    9AFEh
                        call    calc_offset_esdi
                        mov     al, 90h
                        stosb
                        stosb
                        stosb
                        ret
disable_cc_driving      endp

TSR_CODE                ends


;*****************************************************************************
;** Purpose:    Calculate memory offset inside the game.
;**
;** Returns:    dx:ax -> translated memory address of actual exe version.
;**
;** Notes  :    AX = eu memory offset.
;**             DX = segnr (0...13).

TSR_CODE                segment

calc_offset             proc    near
                        push    si
                        push    di
                        push    bx
                        push    cx
                        push    ds

                        mov     cx, cs
                        mov     ds, cx
                        mov     cx, dx
                        mov     bx, ax
                        mov     si, OFFSET TSR:eu_seg_base
                        xor     ax, ax
                        mov     al, gp_ver
                        mov     dx, ((SEG_COUNT + 1) * 2)
                        imul    dx
                        add     si, ax

                        ; set dx to runtime value of seg000
                        mov     dx, game_ss
                        sub     dx, [si + 4 * 2]

                        add     si, cx
                        add     si, cx
                        add     dx, [si]

                        xor     ax, ax
                        mov     al, gp_ver
                        cmp     ax, GP_VER_EUR
                        ja      __not_eu
                        mov     ax, bx
                        jmp     __end
__not_eu:
                        dec     ax
                        shl     ax, 1
                        mov     si, OFFSET TSR:offset_diffs
                        add     si, ax
                        mov     ax, bx
__loop:
                        mov     di, [si]   ; seg nr
                        cmp     di, cx
                        ja      __end
                        jne     __cont

                        mov     di, [si+2] ; seg offset
                        cmp     di, bx
                        ja      __end

                        mov     di, [si+4] ; diff
                        add     ax, di
__cont:
                        add     si, 6
                        jmp     __loop
__end:
                        pop     ds
                        pop     cx
                        pop     bx
                        pop     di
                        pop     si
                        ret
calc_offset             endp


calc_offset_esdi        proc    near

arg_offset              = WORD PTR  4
arg_segnr               = WORD PTR  6

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
                        ret     (2 * 2)
calc_offset_esdi        endp


;** Notes  :    AX = eu memory offset.
;**             DX = segnr (0...13).
;**             DS:BX = near address to store far result
store_offset            proc    near
                        call    calc_offset
                        mov     [bx], ax
                        mov     [bx+2], dx
                        ret
store_offset            endp

TSR_CODE                ends


;*****************************************************************************
;** Purpose:    Convert GP's speed in 1/64 foot/s to km/h
;**
;** Notes  :    AX = GP's speed
;** Returns:    AX = speed in km/h
;**
TSR_CODE                segment

speed_to_kph            proc    near
                        push    bx
                        push    dx
                        mov     bx, 16000
                        mul     bx
                        mov     bx, 52493
                        div     bx
                        mov     bx, 36
                        mul     bx
                        mov     bx, 640
                        div     bx
                        pop     dx
                        pop     bx
                        ret
speed_to_kph            endp

TSR_CODE                ends


;*****************************************************************************
;** Purpose:    Show user that we've hooked in OK.
;**
;** Returns:    Nothing.
;**
;** Notes  :
;**

TSR_CODE                segment

announce_hook           proc    near
                        mov     dx, OFFSET TSR:hooked_msg
                        mov     ah, 09h
                        int     DOS
                        sti
                        mov     delay_count, DELAY_TIME
__do_delay:
                        cmp     delay_count, 0
                        jnz     __do_delay
                        ret
announce_hook           endp

TSR_CODE                ends


;*****************************************************************************
;** Purpose: Called when a track is loaded.
;**

TSR_DATA                segment

hook_load_track_start   db      9Ah
hook_load_track_off     dw      ?
hook_load_track_seg     dw      ?
HOOK_LOAD_TRACK_LENGTH  equ     $ - hook_load_track_start

rnd_seed_start          dw      173Bh, 8984h, 0DA3Ch
RND_SEED_WORDS          equ     ($ - rnd_seed_start) / 2
rnd_car_id              dw      11h

TSR_DATA                ends


TSR_CODE                segment

hook_load_track         proc    far
                        push    ds
                        push    es
                        push    si
                        push    di
                        mov     ax, cs
                        mov     cx, ds
                        mov     ds, ax
                        mov     game_ds, cx
                        mov     al, rng_mode
                        cmp     al, 2
                        jne     __end

                        mov     si, OFFSET TSR:rnd_seed_start
                        les     di, gp_rnd1
                        mov     cx, RND_SEED_WORDS
__loop1:
                        movsw
                        loop    __loop1

                        movsw   ; random car id

                        mov     si, OFFSET TSR:rnd_seed_start
                        les     di, gp_rnd2
                        mov     cx, RND_SEED_WORDS
__loop2:
                        movsw
                        loop    __loop2

                        xor     cx, cx
                        mov     cl, rng_seed
                        or      cx, cx
                        jz      __end
                        les     di, gp_rnd_car_next
                        mov     BYTE PTR es:[di+11h], 0CBh   ; RET -> RETF
                        push    ds
                        mov     ax, game_ds
                        mov     ds, ax
__loop_seed:
                        call    cs:[gp_rnd1_next]
                        call    cs:[gp_rnd_car_next]
                        call    cs:[gp_rnd2_next]
                        loop    __loop_seed
                        pop     ds
                        les     di, gp_rnd_car_next
                        mov     BYTE PTR es:[di+11h], 0C3h
__end:
                        pop     di
                        pop     si
                        pop     es
                        pop     ds
hook_load_track_orig    db      HOOK_LOAD_TRACK_LENGTH dup(0)
                        retf
hook_load_track         endp

TSR_CODE                ends


;*****************************************************************************
;** Purpose: Called when a new session starts.
;**

TSR_DATA                segment

hook_new_session_start  db      9Ah
hook_new_session_off    dw      ?
hook_new_session_seg    dw      ?
HOOK_NEW_SESSION_LENGTH equ     $ - hook_new_session_start

TSR_DATA                ends


TSR_CODE                segment

hook_new_session        proc    far
hook_new_session_orig   db      HOOK_NEW_SESSION_LENGTH dup(0)
                        retf
hook_new_session        endp

TSR_CODE                ends


;*****************************************************************************
;** Purpose: Called when the game is pausing in single player mode.
;**

TSR_DATA                segment

hook_pausing_start      db      9Ah
hook_pausing_off        dw      ?
hook_pausing_seg        dw      ?
HOOK_PAUSING_LENGTH     equ     $ - hook_pausing_start

TSR_DATA                ends


TSR_CODE                segment

hook_pausing            proc    far
hook_pausing_orig       db      HOOK_PAUSING_LENGTH dup(0)
                        pusha

                        push    es
                        les     di, cs:gp_display_paused
                        add     di, 21h
                        mov     al, 0CBh
                        mov     es:[di], al
                        les     di, cs:gp_play_engine_sound
                        add     di, 0Bh
                        mov     al, 0EBh
                        mov     es:[di], al
                        pop     es

                        call    cs:[gp_update_keys]
                        call    cs:[gp_display_paused]
                        call    render_data
                        ;call    cs:[gp_copy_screen_buf]    ; works to show top text, but flickers

                        push    es
                        les     di, cs:gp_display_paused
                        add     di, 21h
                        mov     al, 0E8h
                        mov     es:[di], al
                        les     di, cs:gp_play_engine_sound
                        add     di, 0Bh
                        mov     al, 74h
                        mov     es:[di], al
                        pop     es

                        popa
                        retf
hook_pausing            endp

TSR_CODE                ends


;*****************************************************************************
;** Purpose: Display various values on-screen.
;**

TSR_DATA                segment

hook_frame_start:       db     9Ah
hook_frame_off          dw     ?
hook_frame_seg          dw     ?
HOOK_FRAME_LENGTH       equ    $ - hook_frame_start

TSR_DATA                ends


TSR_CODE                segment

hook_frame              proc    far
                        call    render_data
hook_frame_orig         db      HOOK_FRAME_LENGTH dup(0)
                        retf
hook_frame              endp


render_data             proc    near
                        pusha
                        push    es
                        call    save_led_render_state

                        call    render_session_timer

                        les     di, cs:gp_sel_car_ptr
                        mov     ax, es:[di]
                        or      ax, ax
                        jz      __end

                        mov     si, ax  ; es:si points to selected car

                        ; top left
                        call    render_power
                        call    render_cs_factor
                        call    render_tyre_wear
                        call    render_actual_tyre_grip
                        call    render_speed
                        call    render_unk

                        ; top right
                        call    render_pitseq
                        call    render_lap_dist
                        call    render_driven_segs
                        call    render_seg_nr
                        call    render_best_line
                        call    render_segdist

                        ; bottom left
                        call    render_fuel_load
                        call    render_actual_wings

                        ; bottom right
                        call    render_actual_tyre_compound
__end:
                        call    restore_led_render_state
                        pop     es
                        popa
                        ret
render_data             endp


render_session_timer    proc    near
                        mov     ax, 0
                        mov     cx, 0
                        mov     bl, 80h
                        mov     dx, COLOR_BLACK
                        call    lcd_init
                        les     di, cs:gp_sestime_left  ; minutes remaining encoded as BCD
                        mov     bx, es:[di]
                        call    render_hex_word
                        ret
render_session_timer    endp


render_power            proc    near
                        mov     ax, 3
                        mov     cx, 0
                        mov     bl, 80h
                        mov     dx, COLOR_BLACK
                        call    lcd_init
                        mov     al, 'H'
                        call    cs:[gp_render_char]
                        mov     al, 'P'
                        call    cs:[gp_render_char]
                        mov     al, '='
                        call    cs:[gp_render_char]
                        mov     bx, es:[si+Car.wPower]
                        call    render_hex_word
                        ret
render_power            endp


render_cs_factor        proc    near
                        mov     ax, 4
                        mov     cx, 0
                        mov     bl, 80h
                        mov     dx, COLOR_BLACK
                        call    lcd_init
                        mov     al, 'C'
                        call    cs:[gp_render_char]
                        mov     al, 'S'
                        call    cs:[gp_render_char]
                        mov     al, '='
                        call    cs:[gp_render_char]
                        mov     bx, es:[si+Car.wCornerSpeedFactor]
                        call    render_hex_word
                        ret
render_cs_factor        endp


render_tyre_wear        proc    near
                        mov     ax, 5
                        mov     cx, 0
                        mov     bl, 80h
                        mov     dx, COLOR_BLACK
                        call    lcd_init
                        mov     al, 'T'
                        call    cs:[gp_render_char]
                        mov     al, 'W'
                        call    cs:[gp_render_char]
                        mov     al, '='
                        call    cs:[gp_render_char]
                        mov     bx, WORD PTR es:[si+Car.dTyreWear+2]
                        call    render_hex_word
                        ; mov     bl, BYTE PTR es:[si+Car.dTyreWear+1]
                        ; call    render_hex_byte
                        ret
render_tyre_wear        endp


render_actual_tyre_grip proc    near
                        mov     ax, 6
                        mov     cx, 0
                        mov     bl, 80h
                        mov     dx, COLOR_BLACK
                        call    lcd_init
                        mov     al, 'T'
                        call    cs:[gp_render_char]
                        mov     al, 'G'
                        call    cs:[gp_render_char]
                        mov     al, '='
                        call    cs:[gp_render_char]
                        mov     bx, es:[si+Car.tyreGrip]
                        call    render_hex_word
                        ret
render_actual_tyre_grip endp


render_speed            proc    near
                        mov     ax, es:[si+Car.wSpeedNextCorner]
                        cmp     ax, 7700h
                        je      __skip
                        mov     dx, es:[si+Car.wSpeed]
                        cmp     ax, dx
                        jb      __red
                        mov     dx, COLOR_BLACK
                        jmp     __cont
__red:
                        mov     dx, COLOR_RED
__cont:
                        push    ax
                        mov     ax, 7
                        mov     cx, 0
                        mov     bl, 80h
                        call    lcd_init
                        mov     al, 'S'
                        call    cs:[gp_render_char]
                        mov     al, 'I'
                        call    cs:[gp_render_char]
                        mov     al, '='
                        call    cs:[gp_render_char]
                        pop     ax
                        call    speed_to_kph
                        mov     bx, ax
                        call    render_dec_word
__skip:
                        ; mov     ax, 8
                        ; mov     cx, 0
                        ; mov     bl, 80h
                        ; mov     dx, COLOR_BLACK
                        ; call    lcd_init
                        ; mov     al, 'S'
                        ; call    cs:[gp_render_char]
                        ; mov     al, 'P'
                        ; call    cs:[gp_render_char]
                        ; mov     al, '='
                        ; call    cs:[gp_render_char]
                        ; mov     ax, es:[si+Car.wSpeed]
                        ; call    speed_to_kph
                        ; mov     bx, ax
                        ; call    render_dec_word
                        ret
render_speed            endp


render_unk              proc    near
                        mov     ax, 9
                        mov     cx, 0
                        mov     bl, 80h
                        mov     dx, COLOR_BLACK
                        call    lcd_init
                        mov     al, 'b'
                        call    cs:[gp_render_char]
                        mov     al, '8'
                        call    cs:[gp_render_char]
                        mov     al, '='
                        call    cs:[gp_render_char]
                        mov     bx, es:[si+Car.field_B8]
                        call    render_hex_word

                        mov     ax, 10
                        mov     cx, 0
                        mov     bl, 80h
                        mov     dx, COLOR_BLACK
                        call    lcd_init
                        mov     al, 'b'
                        call    cs:[gp_render_char]
                        mov     al, 'a'
                        call    cs:[gp_render_char]
                        mov     al, '='
                        call    cs:[gp_render_char]
                        mov     bx, es:[si+Car.field_BA]
                        call    render_hex_word
                        ret
render_unk              endp


render_pitseq           proc    near
                        mov     ax, 3
                        mov     ch, 4
                        mov     cl, -4
                        mov     bl, 80h
                        mov     dx, COLOR_BLACK
                        call    lcd_init
                        mov     al, 'P'
                        call    cs:[gp_render_char]
                        mov     al, 'S'
                        call    cs:[gp_render_char]
                        mov     al, '='
                        call    cs:[gp_render_char]
                        mov     al, es:[si+Car.bPitSeq]
                        call    render_nibble
                        ret
render_pitseq           endp


render_lap_dist         proc    near
                        mov     ax, 4
                        mov     ch, 4
                        mov     cl, -7
                        mov     bl, 80h
                        mov     dx, COLOR_BLACK
                        call    lcd_init
                        mov     al, '2'
                        call    cs:[gp_render_char]
                        mov     al, '6'
                        call    cs:[gp_render_char]
                        mov     al, '='
                        call    cs:[gp_render_char]
                        mov     bx, es:[si+Car.wSegNr?]
                        call    render_hex_word

                        mov     ax, 5
                        mov     ch, 4
                        mov     cl, -7
                        mov     bl, 80h
                        mov     dx, COLOR_BLACK
                        call    lcd_init
                        mov     al, '8'
                        call    cs:[gp_render_char]
                        mov     al, '0'
                        call    cs:[gp_render_char]
                        mov     al, '='
                        call    cs:[gp_render_char]
                        mov     bx, es:[si+Car.wNumFeetIntoLap?]
                        call    render_hex_word
                        ret
render_lap_dist         endp


render_driven_segs      proc    near
                        mov     ax, 6
                        mov     ch, 4
                        mov     cl, -7
                        mov     bl, 80h
                        mov     dx, COLOR_BLACK
                        call    lcd_init
                        mov     al, 'D'
                        call    cs:[gp_render_char]
                        mov     al, 'D'
                        call    cs:[gp_render_char]
                        mov     al, '='
                        call    cs:[gp_render_char]
                        mov     bx, WORD PTR es:[si+Car.wDrivenDistInSegs]
                        call    render_hex_word
                        ret
render_driven_segs      endp


render_seg_nr           proc    near
                        mov     ax, 7
                        mov     ch, 4
                        mov     cl, -7
                        mov     bl, 80h
                        mov     dx, COLOR_BLACK
                        call    lcd_init
                        mov     al, 'S'
                        call    cs:[gp_render_char]
                        mov     al, 'N'
                        call    cs:[gp_render_char]
                        mov     al, '='
                        call    cs:[gp_render_char]
                        push    es
                        push    di
                        les     di, es:[si+Car.pSeg]
                        mov     bx, es:[di+Segment.wNr]
                        pop     di
                        pop     es
                        call    render_hex_word
                        ret
render_seg_nr           endp


render_best_line        proc    near
                        mov     ax, 8
                        mov     ch, 4
                        mov     cl, -7
                        mov     bl, 80h
                        mov     dx, COLOR_BLACK
                        call    lcd_init
                        mov     al, 'B'
                        call    cs:[gp_render_char]
                        mov     al, 'L'
                        call    cs:[gp_render_char]
                        mov     al, '='
                        call    cs:[gp_render_char]
                        mov     bx, es:[si+Car.wSegPosCCLine]
                        call    render_hex_word
                        ret
render_best_line        endp


render_segdist          proc    near
                        mov     ax, 9
                        mov     ch, 4
                        mov     cl, -7
                        mov     bl, 80h
                        mov     dx, COLOR_BLACK
                        call    lcd_init
                        mov     al, 'S'
                        call    cs:[gp_render_char]
                        mov     al, 'D'
                        call    cs:[gp_render_char]
                        mov     al, '='
                        call    cs:[gp_render_char]
                        mov     bx, es:[si+Car.wSegDist]
                        call    render_hex_word

                        mov     ax, 10
                        mov     ch, 4
                        mov     cl, -7
                        mov     bl, 80h
                        mov     dx, COLOR_BLACK
                        call    lcd_init
                        mov     al, 'S'
                        call    cs:[gp_render_char]
                        mov     al, 'L'
                        call    cs:[gp_render_char]
                        mov     al, '='
                        call    cs:[gp_render_char]
                        mov     bx, es:[si+Car.wSegLength]
                        call    render_hex_word

                        mov     ax, 11
                        mov     ch, 4
                        mov     cl, -7
                        mov     bl, 80h
                        mov     dx, COLOR_BLACK
                        call    lcd_init
                        mov     al, 'S'
                        call    cs:[gp_render_char]
                        mov     al, 'C'
                        call    cs:[gp_render_char]
                        mov     al, '='
                        call    cs:[gp_render_char]
                        mov     bx, es:[si+Car.wSegDistCompleted]
                        call    render_hex_word

                        mov     ax, 12
                        mov     ch, 4
                        mov     cl, -7
                        mov     bl, 80h
                        mov     dx, COLOR_BLACK
                        call    lcd_init
                        mov     al, 'A'
                        call    cs:[gp_render_char]
                        mov     al, 'X'
                        call    cs:[gp_render_char]
                        mov     al, '='
                        call    cs:[gp_render_char]
                        mov     bx, es:[si+Car.wCarAngleX]
                        call    render_hex_word

                        ret
render_segdist          endp


render_fuel_load        proc    near
                        mov     cl, 0
                        mov     al, 0
                        mov     bl, 'F'
                        call    render_cockpit_char
                        mov     al, 'L'
                        call    cs:[gp_render_char]
                        mov     al, '='
                        call    cs:[gp_render_char]
                        mov     bx, es:[si+Car.weight]
                        push    es
                        les     di, cs:gp_car_base_weight
                        mov     ax, es:[di]
                        pop     es
                        sub     bx, ax
                        call    render_dec_word
                        ret
render_fuel_load        endp


render_actual_tyre_compound proc    near
                        xor     ax, ax
                        mov     al, es:[si+Car.tyreCompound]
                        cmp     al, 6
                        jb      __compound_ok
                        mov     al, 6
__compound_ok:
                        mov     bx, OFFSET TSR:tyre_compound_chars
                        add     bx, ax
                        mov     bl, cs:[bx]
                        mov     cl, 0
                        mov     al, 128 + 35
                        call    render_cockpit_char
                        ret
render_actual_tyre_compound endp


render_actual_wings     proc    near
                        ; render actual front wing
                        mov     cl, 4
                        mov     al, 7
                        mov     bl, 'F'
                        call    render_cockpit_char
                        mov     al, 'W'
                        call    cs:[gp_render_char]
                        mov     al, '='
                        call    cs:[gp_render_char]
                        mov     bl, es:[si+Car.bFrontWingSetup]
                        call    render_dec_byte

                        ; render actual rear wing
                        mov     cl, 4
                        mov     al, 128 + 7
                        mov     bl, 'R'
                        call    render_cockpit_char
                        mov     al, 'W'
                        call    cs:[gp_render_char]
                        mov     al, '='
                        call    cs:[gp_render_char]
                        mov     bl, es:[si+Car.bRearWingSetup]
                        call    render_dec_byte
                        ret
render_actual_wings     endp

TSR_CODE                ends


;****************************************************************************

                        end
