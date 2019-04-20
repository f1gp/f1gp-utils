;**
;** File   : vdm.asm
;** Author : TK
;** Date   : 20/09/93
;**
;** Routines to co-operate with Window or OS/2 VDMs.
;**
                name    vdm
                title   'Virtual DOS Machine'

;*****************************************************************************
;**
;** Include files
;**

;*****************************************************************************
;**
;** Segments
;**

VDM_TEXT        segment byte public 'CODE'
VDM_TEXT        ends

;*****************************************************************************
;**
;** Equates & Macros
;**

;*****************************************************************************
;**
;** Data
;**

;*****************************************************************************

        assume  CS:VDM_TEXT, DS:nothing, SS:nothing, ES:nothing


VDM_TEXT        segment

                public  _VDM_is_windows
_VDM_is_windows proc    FAR

                mov     ax, 4680h
                int     2fh
                ret

_VDM_is_windows endp


                public  _VDM_is_windows_VDM
_VDM_is_windows_VDM proc FAR

                mov     ax, 1600h
                int     2fh
                ret

_VDM_is_windows_VDM endp


                public  _VDM_release_time_slice
_VDM_release_time_slice proc FAR

                mov     ax, 1680h
                int     2fh
                xor     ah, ah
                ret

_VDM_release_time_slice endp


VDM_TEXT        ends

;*****************************************************************************

                end


