/*
** File   : parse.c
** Author : TK 
** Date   :  9/04/94
**
** $Header:   L:/splittim/vcs/parse.c__   1.1   09 Oct 1994 16:40:48   tk  $
**
** Parse the command line.
*/

/*---------------------------------------------------------------------------
** Includes
*/

#include "splittim.h"
#include "version.i"

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#ifndef TRUE
#define TRUE        1
#endif

#ifndef FALSE
#define FALSE       0
#endif

#define WHAT_OFFSET 4

/*---------------------------------------------------------------------------
** Typedefs
*/

/*---------------------------------------------------------------------------
** Local function prototypes
*/

void wrt_msg(void);

void Usage(void);
void display_msg(char near *msg);
void display_chr(char c);
short atoi(register const char far *p, int *nchars);

/*---------------------------------------------------------------------------
** Data
*/

char title_msg[] =
     "@(#)"
     "SplitTim " VERSION " - Grand Prix/World Circuit Split Section Timer.\n"
     "Copyright (c) Trevor Kellaway (CIS:100331,2330) 1994 - All Rights Reserved.\n\n";

/*---------------------------------------------------------------------------
** Functions
*/

int
parse(
     void
) {
     register int   i;
     int            option_next;
     int            nchars;

     display_msg(&title_msg[WHAT_OFFSET]);

     for (i = 0; i <= cmd_line_len; i++) {
          if ( cmd_line[i] == '\r' || cmd_line[i] == '\n' ||
               cmd_line[i] == ' '  || cmd_line[i] == '\t'               
          ) {
               cmd_line[i] = '\0';
          }
     }

     option_next = FALSE;
     while (cmd_line_len--) {
          if (option_next) {
               option_next = FALSE;

               /*
               ** Don't use switch as indirect table gets it wrong for a COM file.
               */
               if (*cmd_line == 'u') {
                    unload_flag = TRUE;
                    return TRUE;
               }
               else if (*cmd_line == 'a') {
                    split1 = atoi(++cmd_line, &nchars);
                    if (split1 < 1 || split1 > 99) {
                         display_msg("SplitTim: Invalid 1st split percentage (-a)\n");
                         return FALSE;
                    }

                    cmd_line += nchars;
                    continue;
               }
               else if (*cmd_line == 'b') {
                    split2 = atoi(++cmd_line, &nchars);
                    if (split2 < 1 || split2 > 99) {
                         display_msg("SplitTim: Invalid 2nd split percentage (-b)\n");
                         return FALSE;
                    }

                    cmd_line += nchars;
                    continue;
               }
               else if (*cmd_line == 'c') {
                    split3 = atoi(++cmd_line, &nchars);
                    if (split3 < 1 || split3 > 99) {
                         display_msg("SplitTim: Invalid 3rd split percentage (-c)\n");
                         return FALSE;
                    }

                    cmd_line += nchars;
                    continue;
               }
               else if (*cmd_line == 'f') {
                    freeze_time = atoi(++cmd_line, &nchars);
                    if (freeze_time < 1 || freeze_time > 15) {
                         display_msg("SplitTim: -f freeze time must be between 1s and 15s\n");
                         return FALSE;
                    }
                    else {
                         freeze_time *= 1000;
                    }

                    cmd_line += nchars;
                    continue;
               }
               else {
                    Usage();
                    return FALSE;
               }
          }
          else if (*cmd_line == '-' || *cmd_line == '/') {
               option_next = TRUE;
          }
          ++cmd_line;
     }

     return TRUE;
}

void
Usage(
     void
) {
     display_msg(   "Usage: SplitTim [-fN -aN -bN -cN | -u]\n"
                    "\n"
                    "       -h,-?     This help message.\n"
                    "\n"
                    "       -aN       1st split time percentage (default 25%).\n"
                    "       -bN       2nd split time percentage (default 50%).\n"
                    "       -cN       3rd split time percentage (default 75%).\n"
                    "\n"
                    "       -fN       Freeze delay in seconds (default 10).\n"
                    "\n"
                    "       -u        Unload TSR.\n"
               );
}

void
display_msg(
     char near *msg                     /* In Msg to display               */
) {
     while (*msg) {
          if (*msg == '\n') {
               display_chr('\r');
          }
          display_chr(*msg++);
     }
}

/*lint -e789 Ignore assigning auto to static */

void
display_chr(
     char c                             /* In Character to display         */
) {
     char cs[2];

     cs[0]     = c;
     cs[1]     = '$';
     msg_text  = cs;
     wrt_msg();
}

/*lint +e789 Ignore assigning auto to static */

short
atoi(
     register const char far  *p,       /* In  Pointer to ASCII string     */
     int                      *nchars   /* Out Number of chars processed   */
) {
     register int   n;
     register int   f;
     const char far *orig_p = p;

     n = 0;
     f = 0;
     for ( ; ; p++) {
          switch (*p) {
          case ' ':
          case '\t':
               continue;
          case '-':
               f++;
               /*lint fall throught */
          case '+':
               p++;
               break;

          default:
               break;
          }
          break;
     }
     while (*p >= '0' && *p <= '9') {
          n = n * 10 + *p++ - '0';
     }

     if (nchars != ((void *) 0)) {
          *nchars = (int) (p - orig_p);
     }
     return (f ? -n : n);
}

/*---------------------------------------------------------------------------
** End of File
*/

