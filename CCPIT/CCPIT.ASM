;**
;** File   : ccpit.asm
;** Author : TK
;** Date   :  6/06/94
;**
        name    ccpit
        title   'Computer Car Pit Strategy'

;*****************************************************************************
;**
;** Include files
;**

;*****************************************************************************
;**
;** Segments
;**

include segments.inc
include ccpit.inc

;*****************************************************************************
;**
;** Equates & Macros
;**

TICK_TIME_MS    equ     18                      ; Approx ms of a tick
DELAY_TIME      equ     (500 / TICK_TIME_MS)    ; Time to display hook msg

EUR_SAVGAM_OFF  equ     0030h
US_SAVGAM_OFF   equ     0000h

;*****************************************************************************
;**
;** Externals.
;**

TSR_CODE        segment

                extrn   install:far
                extrn   SeedGrid:near
                extrn   StartFinishLineHook:near
                extrn   OntoJacks:near
                extrn   TyreChange:near

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

game_ds         dw      ?

                public  pFirstCar
pFirstCar       label   dword
pFirstCarOff    dw      0d1bh
pFirstCarSeg    dw      ?

SG_DRIVER_NAM   equ     0c57ah  ; start of driver names
SG_REPLAY_STOR  equ     0c9bh   ; start of 128 unused bytes before car structs (included in replay and save game)

                public  pSaveGame1
pSaveGame1      label   dword
pSaveGame1Off   dw      SG_DRIVER_NAM + (38 * 24)
pSaveGame1Seg   dw      ?

                public  pSaveGame2
pSaveGame2      label   dword
pSaveGame2Off   dw      SG_DRIVER_NAM + (39 * 24)
pSaveGame2Seg   dw      ?

                public  pReplayState
pReplayState    label   dword
pReplayStateOff dw      SG_REPLAY_STOR
pReplayStateSeg dw      ?



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

game_sg_off     dw      ?
game_eur        db      ?

hooked_msg      db      CR, LF, "CCPIT activated OK", CR, LF, '$'

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

        ;**
        ;** Get return address
        ;**
        mov     di, sp
        les     di, ss:[di + ((PUSHA_SIZE + push_end - push_start) * 2)]

        ;**
        ;** Is it GP.EXE calling us?
        ;**
        mov     game_eur, 1
        mov     game_sg_off, EUR_SAVGAM_OFF
        cmp     di, GP_EUR_RET_ADDR
        jz      check_match
        mov     game_eur, 0
        mov     game_sg_off, US_SAVGAM_OFF
        cmp     di, GP_US_RET_ADDR
        jnz     no_match
check_match:
        sub     di, MATCH_OFFSET
        mov     si, OFFSET TSR:gp_code
        mov     cx, MATCH_BYTES
        cld
        repe    cmpsb
        jnz     no_match

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

        mov     cx, HOOK_EUR_SG_MSW
        mov     ax, HOOK_EUR_SG_LSW
        mov     dx, HOOK_US_SG_MSW
        mov     bx, HOOK_US_SG_LSW
        call    get_hook_offset
        mov     si, OFFSET cs:hook_sg_code_start
        mov     cx, HOOK_SG_LENGTH
        call    copy_patch_block

        mov     cx, HOOK_EUR_ST_MSW
        mov     ax, HOOK_EUR_ST_LSW
        mov     dx, HOOK_US_ST_MSW
        mov     bx, HOOK_US_ST_LSW
        call    get_hook_offset
        mov     si, OFFSET cs:hook_st_code_start
        mov     cx, HOOK_ST_LENGTH
        call    copy_patch_block

        mov     cx, HOOK_EUR_PT_MSW
        mov     ax, HOOK_EUR_PT_LSW
        mov     dx, HOOK_US_PT_MSW
        mov     bx, HOOK_US_PT_LSW
        call    get_hook_offset
        mov     si, OFFSET cs:hook_pt_code_start
        mov     cx, HOOK_PT_LENGTH
        call    copy_patch_block

        mov     cx, HOOK_EUR_SF_MSW
        mov     ax, HOOK_EUR_SF_LSW
        mov     dx, HOOK_US_SF_MSW
        mov     bx, HOOK_US_SF_LSW
        call    get_hook_offset
        mov     si, OFFSET cs:hook_sf_code_start
        mov     cx, HOOK_SF_LENGTH
        call    copy_patch_block

        mov     cx, HOOK_EUR_NC_MSW
        mov     ax, HOOK_EUR_NC_LSW
        mov     dx, HOOK_US_NC_MSW
        mov     bx, HOOK_US_NC_LSW
        call    get_hook_offset
        mov     di, sp
        les     di, ss:[di + ((PUSHA_SIZE + push_end - push_start) * 2) + 2]
        add     bx, 2
        sub     di, bx
        mov     cs:hook_nc_value_off, di
        mov     ax, es
        sub     ax, dx
        mov     cs:hook_nc_value_seg, ax

        mov     cx, HOOK_EUR_OJ_MSW
        mov     ax, HOOK_EUR_OJ_LSW
        mov     dx, HOOK_US_OJ_MSW
        mov     bx, HOOK_US_OJ_LSW
        call    get_hook_offset
        mov     si, OFFSET cs:hook_oj_code_start
        mov     cx, HOOK_OJ_LENGTH
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
;** Purpose:    Code executed at start of race to seed grid.
;**
;** Returns:    Nothing.
;**
;** Notes  :
;**
;** GPU.EXE:
;**
;** 006298  E83CB8         call   001AD7
;** 00629B  25FF00         and    ax,0FF
;** 00629E �C1E006         shl    ax,6
;** 0062A1  3B065202       cmp    ax,[252]
;** 0062A5  7F0E           jg     0062B5                        <----
;** 0062A7  36F7062E12FFFF test   word ptr ss:[122E],0FFFF
;** 0062AE  7505           jnz    0062B5
;** 0062B0  808C970001     or     byte ptr [si+97],1
;** 0062B5  8A84AC00       mov    al,[si+0AC]
;** 0062B9  E892FF         call   00624E
;** 0062BC �C1E004         shl    ax,4
;** 0062BF  F7D8           neg    ax
;**
;** (DOS VER hook point = 06d2eaH in GPU.EXE)
;** (DOS VER hook point = 06d295H in GPU.EXE)

HOOK_EUR_SG_MSW equ     6000h
HOOK_EUR_SG_LSW equ     (0d2eah - 62A5h)

HOOK_US_SG_MSW  equ     6000h
HOOK_US_SG_LSW  equ     (0d295h - 62A5h)

TSR_DATA                segment

hook_sg_code_start:
                        nop
                        nop
                        db      36h, 0f7h, 06h, 2eh, 12h, 0ffh, 0ffh    ; test   word ptr ss:[122E],0FFFF
                        db      75h, 05h                                ; jnz    0062B5

                        db      09ah
hook_sg_code_off        dw      ?
hook_sg_code_seg        dw      ?
hook_sg_code_end:
HOOK_SG_LENGTH          equ     (hook_sg_code_end - hook_sg_code_start)

TSR_DATA                ends

TSR_CODE                segment

        public  hook_seed_grid
hook_seed_grid:
        pushf
        pusha
        push    ds
        push    es
        cld

        call    save_games_ds

        push    cs
        pop     ds
        call    SeedGrid

        pop     es
        pop     ds
        popa
        popf
        retf

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
;** 003215  B90000         mov    cx,0
;** 003218  8A0E8B29       mov    cl,[298B]
;** 00321C  81C10100       add    cx,1
;** 003220  3B8E3C12       cmp    cx,[bp+123C]
;** 003224  7D50           jge    003276
;** 003226  F684B30004     test   byte ptr [si+0B3],4
;** 00322B  7549           jnz    003276
;** 00322D  3B0E5002       cmp    cx,[250]                          <----
;** 003231  7C43           jl     003276
;** 003233  F684970001     test   byte ptr [si+97],1
;** 003238  743C           jz     003276
;** 00323A  56             push   si
;** 00323B  BE1B0D         mov    si,0D1B
;** 00323E  B81900         mov    ax,19
;** 003241  B90000         mov    cx,0
;** 003244  F6442380       test   byte ptr [si+23],80
;** 003248  7404           jz     00324E
;** 00324A  81C10100       add    cx,1
;** 00324E  81C6C000       add    si,0C0
;** 003252  83E801         sub    ax,+1
;** 003255  79ED           jns    003244
;** 003257  5E             pop    si
;** 003258  81F90700       cmp    cx,7
;** 00325C  7D18           jge    003276
;** 00325E  E8492D         call   005FAA
;** 003261  7813           js     003276
;** 003263  804C2380       or     byte ptr [si+23],80
;** 003267  80A49700FB     and    byte ptr [si+97],0FB
;** 00326C  808CB30004     or     byte ptr [si+0B3],4
;** 003271  9ACB1BED19     call   19ED:1BCB
;** 003276 �61             popa
;** 003277  C3             ret
;**
;** (DOS VER hook point = 06d2eaH in GPU.EXE)
;** (DOS VER hook point = 06d295H in GPU.EXE)
;**

HOOK_EUR_SF_MSW equ     6000h
HOOK_EUR_SF_LSW equ     (0d2eah - 322Dh)

HOOK_US_SF_MSW  equ     6000h
HOOK_US_SF_LSW  equ     (0d295h - 322Dh)

TSR_DATA                segment

hook_sf_code_start:
                        db     09ah
hook_sf_code_off        dw     ?
hook_sf_code_seg        dw     ?
                        nop
hook_sf_code_end:
HOOK_SF_LENGTH          equ     (hook_sf_code_end - hook_sf_code_start)

TSR_DATA                ends

TSR_CODE                segment

        public  hook_sf_line
hook_sf_line:
        pushf
        pusha
        push    ds
        push    es
        cld

        call    save_games_ds

        push    ds
        pop     es
        push    cs
        pop     ds

        ;**
        ;** Has leader reached current pit lap yet?
        ;**
        mov     cx, 0
        mov     cl, es:[298Bh]
        add     cx, 1

        push    [bp+123Ch]
        push    cx
        call    StartFinishLineHook
        add     sp, 4

        pop     es
        pop     ds
        popa
        popf
        retf

TSR_CODE        ends

;*****************************************************************************
;** Purpose:    Max number of cars in pit.
;**
;** Returns:    Nothing.
;**
;** Notes  :
;**
;** GPU.EXE:
;**
;** 00324E  81C6C000       add    si,0C0
;** 003252  83E801         sub    ax,+1
;** 003255  79ED           jns    003244
;** 003257  5E             pop    si
;** 003258  81F90700       cmp    cx,7                  <----
;** 00325C  7D18           jge    003276
;** 00325E  E8492D         call   005FAA
;** 003261  7813           js     003276
;**
;** (DOS VER hook point = 06d2eaH in GPU.EXE)
;** (DOS VER hook point = 06d295H in GPU.EXE)
;**

TSR_DATA                segment

HOOK_EUR_NC_MSW equ     6000h
HOOK_EUR_NC_LSW equ     (0d2eah - 325ah)

HOOK_US_NC_MSW  equ     6000h
HOOK_US_NC_LSW  equ     (0d295h - 325ah)

                        public  hook_nc_value
hook_nc_value           label   dword
hook_nc_value_off       dw      ?
hook_nc_value_seg       dw      ?

TSR_DATA                ends

;*****************************************************************************
;** Purpose: Car going onto jacks in pit during a race pit stop.
;**
;** Returns: Nothing.
;**
;** Notes  :
;**
;** GPU.EXE:
;**
;** 0056DB  E96E02         jmp    00594C
;** 0056DE  C744100000     mov    word ptr [si+10],0
;** 0056E3  C744300000     mov    word ptr [si+30],0
;** 0056E8  C744320000     mov    word ptr [si+32],0
;** 0056ED  C744340000     mov    word ptr [si+34],0
;** 0056F2  C744360000     mov    word ptr [si+36],0
;** 0056F7  C744380000     mov    word ptr [si+38],0
;** 0056FC  C7443A0000     mov    word ptr [si+3A],0
;** 005701  808C970040     or     byte ptr [si+97],40           <----
;** 005706  80A4B300DF     and    byte ptr [si+0B3],0DF
;** 00570B  E886FC         call   005394
;**
;** (DOS VER hook point = 06d2eaH in GPU.EXE)
;** (DOS VER hook point = 06d295H in GPU.EXE)
;**

TSR_DATA                segment

HOOK_EUR_OJ_MSW equ     6000h
HOOK_EUR_OJ_LSW equ     (0d2eah - 5701h)

HOOK_US_OJ_MSW  equ     6000h
HOOK_US_OJ_LSW  equ     (0d295h - 5701h)

hook_oj_code_start:
                        db     09ah
hook_oj_code_off        dw     ?
hook_oj_code_seg        dw     ?
hook_oj_code_end:
HOOK_OJ_LENGTH          equ     (hook_oj_code_end - hook_oj_code_start)

TSR_DATA                ends

TSR_CODE                segment

        public  hook_on_jacks
hook_on_jacks:
        or      byte ptr [si+97h],40h

        pushf
        pusha
        push    ds
        push    es
        cld

        call    save_games_ds

        push    ds
        push    cs
        pop     ds
        push    si
        push    [bp+123Ch]
        call    OntoJacks
        add     sp, 6

        pop     es
        pop     ds
        popa
        popf
        retf

TSR_CODE        ends

;*****************************************************************************
;** Purpose:    Code executed at start of race to seed cars tyres.
;**
;** Returns:    Nothing.
;**
;** Notes  :
;**
;** GPU.EXE:
;**
;** 00BFA3  F7862E12FFFF   test   word ptr [bp+122E],0FFFF
;** 00BFA9  7402           jz     00BFAD                        <----
;** 00BFAB  B005           mov    al,5
;** 00BFAD  8884B200       mov    [si+0B2],al
;** 00BFB1  59             pop    cx
;** 00BFB2  C3             ret
;** 00BFB3  B002           mov    al,2
;** 00BFB5  F6864A1240     test   byte ptr [bp+124A],40
;** 00BFBA  7402           jz     00BFBE
;** 00BFBC  B004           mov    al,4
;** 00BFBE  F7862E12FFFF   test   word ptr [bp+122E],0FFFF
;** 00BFC4  7402           jz     00BFC8                        <----
;** 00BFC6  B005           mov    al,5
;** 00BFC8  8884B200       mov    [si+0B2],al
;** 00BFCC  C3             ret
;**
;** (DOS VER hook point = 06d2eaH in GPU.EXE)
;** (DOS VER hook point = 06d295H in GPU.EXE)

HOOK_EUR_ST_MSW equ     6000h
HOOK_EUR_ST_LSW equ     (0d2eah - 0bfa9h)

HOOK_US_ST_MSW  equ     6000h
HOOK_US_ST_LSW  equ     (0d295h - 0bfa9h)

HOOK_EUR_PT_MSW equ     6000h
HOOK_EUR_PT_LSW equ     (0d2eah - 0bfc4h)

HOOK_US_PT_MSW  equ     6000h
HOOK_US_PT_LSW  equ     (0d295h - 0bfc4h)

TSR_DATA                segment

hook_st_code_start:     db      09ah
hook_st_code_off        dw      ?
hook_st_code_seg        dw      ?
                        nop
                        nop
                        nop
hook_st_code_end:
HOOK_ST_LENGTH          equ     (hook_st_code_end - hook_st_code_start)

hook_pt_code_start:     db      09ah
hook_pt_code_off        dw      ?
hook_pt_code_seg        dw      ?
                        nop
                        nop
                        nop
hook_pt_code_end:
HOOK_PT_LENGTH          equ     (hook_pt_code_end - hook_pt_code_start)

local_tyre              db      ?

TSR_DATA                ends

TSR_CODE                segment

        public  hook_seed_grid_tyre
hook_seed_grid_tyre:

        jz      not_wet_race
        mov     al, 05h
ignore_st:
        mov     [si+0b2h], al
        jmp     done_st

not_wet_race:
        ;**
        ;** Ignore if this is not a race.
        ;**
        test    byte ptr ss:[124ah], 80h
        jz      ignore_st

        pushf
        pusha
        push    ds
        push    es
        cld

        call    save_games_ds

        push    ss      ; car setup
        push    1286h
        push    ds      ; current car
        push    si
        xor     ah, ah
        push    ax      ; current tyres (in AL)

        push    cs
        pop     ds
        call    TyreChange
        add     sp, 10
        mov     local_tyre, al

        pop     es
        pop     ds
        popa
        mov     al, cs:local_tyre
        mov     [si+0b2h], al
        popf
done_st:
        retf

TSR_CODE        ends

;*****************************************************************************
;** Purpose:    Random number routine.
;**
;** Returns:    Random word.
;**
;** Notes  :    We must use the same method as the game for random numbers as
;**             otherwise linked player games won't stay in step.
;**

TSR_CODE        segment

        public  rnd

rnd     proc    near

        push    es
        mov     es, cs:game_ds

        push    bx
        push    cx
        mov     ax, es:[2985h]
        shr     ax, 4
        mov     bx, es:[2987h]
        shr     bx, 1
        xor     al, bl
        mov     bx, es:[2985h]
        mov     cx, es:[2987h]
        mov     ch, cl
        mov     cl, bh
        mov     bh, bl
        mov     bl, 0
        mov     bl, es:[2984h]
        mov     es:[2985h], bx
        mov     es:[2987h], cx
        pop     cx
        pop     bx
        mov     es:[2984h], al

        pop     es
        ret

rnd     endp

TSR_CODE        ends

;*****************************************************************************
;** Purpose:    Save the games DS for all our far pointers.
;**
;** Returns:    Nothing.
;**
;** Notes  :
;**

TSR_CODE        segment

        public  save_games_ds

save_games_ds proc near

        mov     cs:game_ds, ds
        mov     cs:pFirstCarSeg, ds
        mov     cs:pSaveGame1Seg, ds
        mov     cs:pSaveGame2Seg, ds
        mov     cs:pReplayStateSeg, ds
        ret

save_games_ds endp

TSR_CODE        ends

;****************************************************************************

        end



