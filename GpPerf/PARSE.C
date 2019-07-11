/*
** File   : parse.c
** Author : TK
** Date   :  9/04/94
**
** $Header:   F:/TK/GPPERF/VCS/PARSE.C__   1.4   25 Oct 1995 23:55:14   tk  $
**
** Parse the command line.
*/

/*---------------------------------------------------------------------------
** Includes
*/

#include "gpperf.h"
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
short atoi(register const char far *p);

/*---------------------------------------------------------------------------
** Data
*/

char title_msg[] =
     "@(#)"
     "GpPerf " VERSION " - Grand Prix/World Circuit Car Performance.\n"
     "Copyright (c) Trevor Kellaway (CIS:100331,2330) 1995 - All Rights Reserved.\n"
     "Copyright (c) Rene Smit 2019 - All Rights Reserved.\n\n";

/*---------------------------------------------------------------------------
** Functions
*/

int
parse(
     void
) {
     register int   i;
     int            option_next;
     int            n;

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
               if (*cmd_line == 'f') {
                    fname_ptr = cmd_line + 1;
                    if (cmd_line_len == 0 || *fname_ptr == '\0') {
                         display_msg("GpPerf: missing log filename.\n");
                         Usage();
                         return FALSE;
                    }
               }
               else if (*cmd_line == 'l') {
                    n = atoi(&cmd_line[1]);
                    if (n >= 1 && n <= 9999) {
                         log_laps = n;
                    }
                    else {
                         display_msg("GpPerf: -l value should be between 1 and 9999 laps.\n");
                         return FALSE;
                    }
               }
               else if (*cmd_line == 'a') {
                    n = atoi(&cmd_line[1]);
                    if (n >= 8000 && n <= 14000) {
                         rev_limit1 = n;
                    }
                    else {
                         display_msg("GpPerf: -a value should be between 8000 and 14000 rpm.\n");
                         return FALSE;
                    }
               }
               else if (*cmd_line == 'b') {
                    n = atoi(&cmd_line[1]);
                    if (n >= 8000 && n <= 14000) {
                         rev_limit2 = n;
                    }
                    else {
                         display_msg("GpPerf: -b value should be between 8000 and 14000 rpm.\n");
                         return FALSE;
                    }
               }
               else if (*cmd_line == 'c') {
                    n = atoi(&cmd_line[1]);
                    if (n >= 8000 && n <= 14000) {
                         rev_limit3 = n;
                    }
                    else {
                         display_msg("GpPerf: -c value should be between 8000 and 14000 rpm.\n");
                         return FALSE;
                    }
               }
               else if (*cmd_line == 'n') {
                    n = atoi(&cmd_line[1]);
                    if (n >= 1 && n <= 40) {
                         log_car_num = n;
                    }
                    else {
                         display_msg("GpPerf: -n value should be between 1 and 40.\n");
                         return FALSE;
                    }
               }
               else if (*cmd_line == 'd') {
                    pending_enable = FALSE;
               }
               else if (*cmd_line == 'e') {
                    always_enabled = TRUE;
               }
               else if (*cmd_line == 't') {
                    show_tyres = FALSE;
               }
               else if (*cmd_line == 'u') {
                    unload_flag = TRUE;
                    return TRUE;
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

     if (!(rev_limit1 <= rev_limit2 && rev_limit2 <= rev_limit3)) {
          display_msg("GpPerf: -a/-b/-c rev limits must increase in value.\n");
          return FALSE;
     }

     return TRUE;
}

void
Usage(
     void
) {
     display_msg(   "Usage: GpPerf [-lN] [-aN -bN -cN] [-nN] [-t] [-h?] [-u] [-f(filename)]\n"
                    "       -l(N)     Auto-log for N laps (default 1 lap).\n"
                    "       -f(name)  Specify log filename's location.\n"
                    "       -a(N)     Specify rev limit #1 (default 10000).\n"
                    "       -b(N)     Specify rev limit #2 (default 11500).\n"
                    "       -c(N)     Specify rev limit #3 (default 13000).\n"
                    "       -d        Startup with logging disabled.\n"
                    "       -e        Startup with logging enabled.\n"
                    "       -t        Startup with tyre wear display disabled.\n"
                    "       -n(N)     Log for computer car N only.\n"
                    "       -h,-?     This help message.\n"
                    "       -u        Unload TSR.\n"
                    "\n"
                    "In F1GP:\n"
                    "         <F7>    Turn logging on/off, when enabled continuously logs.\n"
                    "         <F8>    Auto start/end logging (triggered at start/finish line)\n"
                    "         <F9>    Toggle speed or tracks segment number display.\n"
                    "         <F10>   Toggle tyre wear display on/off.\n"
                    "\n"
                    "Opposition standard LED (left-hand side of cockpit):\n"
                    "         Green   Logging disabled.\n"
                    "         Yellow  Logging will be automatically enabled at start/finish line.\n"
                    "         Red     Automatic logging in progress, will be disabled after N laps.\n"
                    "         Blue    Continuous logging in progress.\n"
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

/*lint +e789 Ignore assigning auto to static */

short
atoi(
     register const char far *p         /* In  Pointer to ASCII string     */
) {
     register int n;
     register int f;

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
     return (f ? -n : n);
}

/*---------------------------------------------------------------------------
** End of File
*/


