/*
** File   : splittim.h
** Author : TK
** Date   :  9/06/94
**
** $Header:   L:/splittim/vcs/splittim.h__   1.1   09 Oct 1994 16:40:50   tk  $
**
** Car Performance.
*/

#ifndef   splittim_h
#define   splittim_h

/*---------------------------------------------------------------------------
** Includes
*/

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define TRUE             1
#define FALSE            0

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef unsigned char    byte;
typedef unsigned short   word;
typedef unsigned long    dword;

typedef struct {
     unsigned char si[0xc0];
} CAR;

/*---------------------------------------------------------------------------
** Data
*/

extern char         unload_flag;
extern char         far *cmd_line;
extern char         cmd_line_len;
extern char         near *msg_text;
extern char         beep_count;
extern short        split1;
extern short        split2;
extern short        split3;
extern short        freeze_time;

/*---------------------------------------------------------------------------
** Functions
*/

#endif    /* splittim_h */
/*---------------------------------------------------------------------------
** End of File
*/

