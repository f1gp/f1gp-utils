;**
;** File   : install.asm
;** Author : TK
;** Date   :  8/06/94
;**
;**

;*****************************************************************************
;**                                                                         **
;**  This program is the confidential and proprietary product of Airtech    **
;**  Computer Security Limited. Any unauthorised use, reproduction or       **
;**  transfer of this program is strictly prohibited. Copyright 1994        **
;**  Airtech Computer Security Limited. (Subject to limited distribution    **
;**  and restricted disclosure only.) All rights reserved.                  **
;**                                                                         **
;*****************************************************************************

                        name    install
                        title   'Install TSR'

;*****************************************************************************
;**
;** Include files
;**

include segments.inc
include gptest.inc

;*****************************************************************************
;**
;** Externals
;**

TSR_CODE                segment

                        extrn   hook_frame
                        extrn   hook_load_track
                        extrn   hook_new_session
                        extrn   hook_pausing

                        extrn   new08_hndlr
                        extrn   new11_hndlr
                        extrn   new21_hndlr

                        extrn   end_resident

                        extrn   parse:near

TSR_CODE                ends


;*****************************************************************************
;**
;** Segments
;**

TSR_TMP                 segment

;*****************************************************************************
;**
;** Equates & Macros
;**

STACK_SIZE              equ     128


;*****************************************************************************
;**
;** Exports
;**

                        public  fname_ptr
                        public  cmd_line
                        public  cmd_line_len
                        public  msg_text
                        public  stack_top
                        public  unload_flag

                        public  install
                        public  tsr_unload
                        public  tsr_vectors_install
                        public  wrt_msg
                        public  terminate


;*****************************************************************************
;**
;** Data
;**

load_msg                db      "Installed OK", CR, LF, '$'
already_msg             db      "Already installed !", CR, LF, "$"
parse_msg               db      "Failed to determine absolute pathname.", BELL, CR, LF, "$"
cant_unload_msg         db      "Unable to unload as another TSR was loaded after GpTest.", BELL, CR, LF, "$"
no_unload_msg           db      "Can't unload as GpTest isn't loaded!", BELL, CR, LF, "$"
unloaded_msg            db      "GpTest unloaded OK.", CR, LF, "$"

fname_ptr               label   dword
                        dw      OFFSET TSR:filename
                        dw      SEG TSR
filename                db      "GPTEST.LOG", 0

cmd_line                label   dword
cmd_line_off            dw      0081h           ; Point to PSP command line.
cmd_line_seg            dw      ?
cmd_line_len            db      ?

msg_text                dw      ?

unload_flag             db      0

tsrs_cs                 dw      ?

TSR_TMP                 ends


STACK_TMP               segment
                        db      STACK_SIZE dup('#')
stack_top:
STACK_TMP               ends

TSR_TMP                 segment


;*****************************************************************************
;** Purpose: Install TSR.
;**
;** Returns: Nothing.
;**
;** Notes  :
;**
install                 proc    near

                        ;**
                        ;** Switch to our stack.
                        ;**
                        cli
                        mov     ax, cs
                        mov     ss, ax
                        mov     sp, OFFSET TSR:stack_top
                        sti

                        ;***
                        ;*** Get Environment variable info.
                        ;***
                        mov     ah, 62h
                        int     DOS                             ; Get PSP segment address
                        mov     cs:psp_seg, bx
                        mov     cs:cmd_line_seg, bx
                        mov     bx, 0080h                       ; PSP Command line length
                        mov     al, [bx]
                        mov     cs:cmd_line_len, al

                        ;**
                        ;** Point DS at our common segment before calling C code.
                        ;**
                        mov     ax, cs
                        mov     ds, ax
                        call    parse
                        or      al, al
                        jnz     parse_ok
                        jmp     terminate

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
                        mov     al, unload_flag
                        or      al, al
                        jz      not_installed1

nothing_to_unload:
                        ;**
                        ;** User is trying to unload non-existant resident portion!
                        ;**
                        push    cs
                        pop     ds
                        mov     dx, OFFSET TSR:no_unload_msg
                        mov     ah, 09h
                        int     DOS
                        jmp     terminate

installed:
                        mov     al, unload_flag
                        or      al, al
                        jz      no_unload

                        ;**
                        ;** TSR's CS will be 100h bigger, i.e. 10h for segment register for an EXE.
                        ;**
                        add     bx, 10h
                        call    tsr_unload

                        ;**
                        ;** Can't unload as someone else has hooked one of our vectors.
                        ;**
                        push    cs
                        pop     ds
                        mov     dx, OFFSET TSR:cant_unload_msg
                        mov     ah, 09h
                        int     DOS
                        jmp     terminate

                        ;**
                        ;** Already installed !
                        ;**
no_unload:
                        mov     dx, OFFSET TSR:already_msg
                        mov     ah, 09h
                        int     DOS
                        jmp     terminate

not_installed1:
                        ;**
                        ;** Convert current filename to fully qualified name.
                        ;**
                        push    ds
                        mov     ax, ds
                        mov     es, ax
                        lds     si, fname_ptr
                        mov     di, OFFSET TSR:q_filename
                        mov     ah, 60h
                        int     DOS
                        pop     ds
                        jnc     name_q_ok
                        mov     dx, OFFSET TSR:parse_msg
                        mov     ah, 09h
                        int     DOS
                        int     DOS_TERMINATE

                        ;**
                        ;** Install our vectors
                        ;**
name_q_ok:
                        call    tsr_vectors_install

                        ;**
                        ;** Patch our entry points patches!
                        ;**
                        call    install_hooks

                        ;**
                        ;** Display message
                        ;**
                        mov     dx, OFFSET TSR:load_msg
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
                        mov     ax, TSR_TMP
                        inc     ax
                        sub     ax, psp_seg
                        mov     dx, ax
                        mov     ax, 3100h
                        int     DOS

                        ;***
                        ;*** Not reached
                        ;***
quit:                   ret

install                 endp


;*****************************************************************************
;** Purpose: Uninstall (if possible). Check vectors still point to us.
;**
;** Returns: If uninstall fails, terminates if successful.
;**
;** Notes  :
;**
tsr_unload              proc    near

                        mov     tsrs_cs, bx

                        mov     ax, 3508h
                        int     DOS
                        mov     ax, es
                        cmp     ax, tsrs_cs
                        jnz     cant_unload
                        cmp     bx, OFFSET TSR:new08_hndlr
                        jnz     cant_unload

                        mov     ax, 3511h
                        int     DOS
                        mov     ax, es
                        cmp     ax, tsrs_cs
                        jnz     cant_unload
                        cmp     bx, OFFSET TSR:new11_hndlr
                        jnz     cant_unload

                        mov     ax, 3521h
                        int     DOS
                        mov     ax, es
                        cmp     ax, tsrs_cs
                        jnz     cant_unload
                        cmp     bx, OFFSET TSR:new21_hndlr
                        jnz     cant_unload

                        ;**
                        ;** Restore original vectors, point DS at currently installed TSR.
                        ;**
                        mov     es, tsrs_cs
                        lds     dx, es:old08_hndlr
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
                        mov     dx, OFFSET TSR:unloaded_msg
                        mov     ah, 09h
                        int     DOS

                        ;**
                        ;** Recover PSP value by knocking TSR's CS down by 10:0h
                        ;**
                        mov     ax, tsrs_cs
                        sub     ax, 10h
                        mov     es, ax
                        mov     ah, 49h
                        INT     DOS
                        jmp     terminate
cant_unload:
                        ret

tsr_unload              endp

;*****************************************************************************
;** Purpose: Install our interrupt vectors.
;**
;** Returns: Nothing.
;**
;** Notes  : Hooked interrupts, INT 08H, INT 11H, INT 21H.
;**
tsr_vectors_install     proc    near

                        ;***
                        ;*** Install new timer tick vector
                        ;***
                        mov     ax, 3508h
                        int     DOS
                        mov     old08_off, bx
                        mov     old08_seg, es
                        mov     dx, OFFSET TSR:new08_hndlr
                        mov     ax, 2508h
                        int     DOS

                        ;***
                        ;*** Install new equipment list vector
                        ;***
                        mov     ax, 3511h
                        int     DOS
                        mov     old11_off, bx
                        mov     old11_seg, es
                        mov     dx, OFFSET TSR:new11_hndlr
                        mov     ax, 2511h
                        int     DOS

                        ;***
                        ;*** Install new DOS Int 21 handler
                        ;***
                        mov     ax, 3521h
                        int     DOS
                        mov     old21_off, bx
                        mov     old21_seg, es
                        mov     dx, OFFSET TSR:new21_hndlr
                        mov     ax, 2521h
                        int     DOS
                        ret

tsr_vectors_install     endp


;*****************************************************************************
;** Purpose: Install our hooks.
;**
;** Returns: Nothing.
;**
install_hooks           proc    near
                        mov     ax, cs
                        mov     bx, OFFSET cs:hook_frame_off
                        mov     WORD PTR [bx], OFFSET cs:hook_frame
                        mov     [bx+2], ax

                        mov     bx, OFFSET cs:hook_new_session_off
                        mov     WORD PTR [bx], OFFSET cs:hook_new_session
                        mov     ax, cs
                        mov     [bx+2], ax

                        mov     bx, OFFSET cs:hook_pausing_off
                        mov     WORD PTR [bx], OFFSET cs:hook_pausing
                        mov     ax, cs
                        mov     [bx+2], ax

                        mov     bx, OFFSET cs:hook_load_track_off
                        mov     WORD PTR [bx], OFFSET cs:hook_load_track
                        mov     ax, cs
                        mov     [bx+2], ax

                        ret

install_hooks           endp


;*****************************************************************************
;** Purpose: Write message string to display.
;**
;** Returns: Nothing.
;**
;** Notes  : Message should be terminated with a '$', not '\0'!
;**
wrt_msg                 proc    near

                        mov     dx, msg_text
                        mov     ah, 09h
                        int     DOS
                        ret

wrt_msg                 endp


;*****************************************************************************

terminate:
                        mov     al, 5   ; abnormal termination, use non-zero exit code
                        mov     ah, 4ch
                        int     DOS

TSR_TMP                 ends

;*****************************************************************************

                        end     install
