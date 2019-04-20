;**
;** File   : install.asm
;** Author : TK
;** Date   :  8/06/94
;**
;** $Header:   L:/splittim/vcs/install.as_   1.1   09 Oct 1994 16:40:46   tk  $
;**
;** 
;**

                name    install
                title   'Install TSR'

;*****************************************************************************
;**
;** Include files
;**

include segments.inc
include splittim.inc

;*****************************************************************************
;**
;** Segments
;**

TSR_TMP         segment

;*****************************************************************************
;**
;** Equates & Macros
;**

STACK_SIZE      equ     512

;*****************************************************************************
;**
;** Data
;**

        extrn   hook

        extrn   new08_hndlr
        extrn   new11_hndlr
        extrn   new21_hndlr

        extrn   end_resident

load_msg        db      "Installed OK", CR, LF, '$'
already_msg     db      "Already installed !", CR, LF, "$"
cant_unload_msg db      "Unable to unload as another TSR was loaded after SplitTim.", BELL, CR, LF, "$"
no_unload_msg   db      "Can't unload as SplitTim isn't loaded!", BELL, CR, LF, "$"
unloaded_msg    db      "SplitTim unloaded OK.", CR, LF, "$"

public          cmd_line
public          cmd_line_len
cmd_line        label   dword
cmd_line_off    dw      0081h           ; Point to PSP command line.
cmd_line_seg    dw      ?
cmd_line_len    db      ?

public          msg_text
msg_text        dw      ?

public          unload_flag
unload_flag     db      0

tsrs_cs         dw      ?

TSR_TMP         ends

STACK_TMP       segment
                db      STACK_SIZE dup('#')
                public  stack_top
stack_top:
STACK_TMP       ends

TSR_TMP         segment

extrn           parse:near 

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
        ;** Install our vectors
        ;**
        call    tsr_vectors_install

        ;**
        ;** Patch our entry points patches!
        ;**
        mov     bx, OFFSET cs:hook_code_off
        mov     WORD PTR [bx], OFFSET cs:hook
        mov     ax, cs
        mov     [bx+2], ax

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

tsr_vectors_install endp

;*****************************************************************************
;** Purpose: Write message string to display.
;**
;** Returns: Nothing.
;**
;** Notes  : Message should be terminated with a '$', not '\0'!
;**
        public  wrt_msg
wrt_msg proc   near

        mov     dx, msg_text
        mov     ah, 09h
        int     DOS
        ret

wrt_msg endp

;*****************************************************************************

        public  terminate
terminate:
        mov     ah, 4ch
        int     DOS

TSR_TMP         ends

;*****************************************************************************

                end     install


