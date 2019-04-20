/*
** File   : parse.c
** Author : TK
** Date   :  9/04/94
**
** $Header:   F:/TK/GPLAPTIM/GPLAPTIM/VCS/PARSE.C__   1.12   07 Apr 1996 12:37:46   tk  $
**
** Parse the command line for GP Lap Time.
**
** V2.0 23/04/94    Added command line parsing.
** V2.1 14/05/94    Updated version string to keep in step with other utils.
** V2.2 21/05/94    Steering assistance changed from +388 to +374.
** V3.0 25/07/94    Added car setup support.
**                  Turned INT 11h around so two TSRs will work together.
**                  Added pit stop support.
**                  First lap is now lap 0.
** V3.1 13/08/94    Fixed bug where MS-Mouse driver V9.01 calls Get DOS Ver
**                  when mouse is initialised. This causes the game detection
**                  to fail and the US offsets are left by default.
** V4.0 30/09/94    Added support for new AIG GP.EXE offset value.
** V4.1 01/01/95    Added detection of illegal wing settings.
** V5.0 05/03/95    Added detection of game reloads and pauses.
**                  Added detection of illegal BHP.
**                  Players car always gets AIG of 0 now.
**                  Integrated Split Timer.
**                  Removed players BHP randomisation.
**                  Replays now ignored.
**                  Fixed bug where -p of pit stops may log CC by mistake.
**                  Added grass detection code.
** V5.1 02/07/95    Added support for Italian GP.EXE.
** V6.0 07/10/95    Added code to fix split timer bug in the game.
**                  Authentication records not logged for linked player.
**                  Added new 50% and 100% race time and FL authentication.
** V6.1 04/03/96	Added support for Spanish GP.EXE.
*/

#define VERSION     "V6.1 4th Apr 1996"

/*---------------------------------------------------------------------------
** Includes
*/

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define TRUE        1
#define FALSE       0

#define WHAT_OFFSET 4

/*---------------------------------------------------------------------------
** Typedefs
*/

/*---------------------------------------------------------------------------
** Local function prototypes
*/

void wrt_msg(void);

static void  Usage(void);
static void  display_msg(char near *msg);
static void  display_chr(char c);
static short atoi(register const char far *p);

/*---------------------------------------------------------------------------
** Data
*/

extern char player_only_flag;
extern char unload_flag;

extern char far *cmd_line;
extern char cmd_line_len;
extern char near *msg_text;
extern char near *fname_ptr;

extern short   split1;
extern short   split2;
extern short   split3;
extern short   freeze_time;

static char    title_msg[] = "@(#)"
                             "GpLapTim " VERSION " - Grand Prix/World Circuit Lap Time Logger.\n"
                             "Copyright (c) Trevor Kellaway (CIS:100331,2330) 1995 - All Rights Reserved.\n\n";

/*---------------------------------------------------------------------------
** Functions
*/

int
parse(
     void
) {
     int  option_next;
     int  i;

     display_msg(&title_msg[WHAT_OFFSET]);

     option_next = FALSE;

     for (i = 0; i <= cmd_line_len; i++) {
          if ( cmd_line[i] == '\r' || cmd_line[i] == '\n' ||
               cmd_line[i] == ' '  || cmd_line[i] == '\t'
          ) {
               cmd_line[i] = '\0';
          }
     }

     while (cmd_line_len--) {
          if (option_next) {
               if (*cmd_line == 'p') {
                    player_only_flag = TRUE;
               }
               else if (*cmd_line == 'f') {
                    /*lint -e619 Ignore loss of precision */
                    fname_ptr = cmd_line + 1;
                    /*lint +e619 Ignore loss of precision */
                    if (cmd_line_len == 0 || *fname_ptr == '\0') {
                         display_msg("GpLapTim: missing log filename.\n");
                         Usage();
                         return FALSE;
                    }
               }
               else if (*cmd_line == 'u') {
                    unload_flag = TRUE;
               }
               else if (*cmd_line == 'a') {
                    split1 = atoi(cmd_line + 1);
                    if (split1 < 1 || split1 > 99) {
                         display_msg("GpLapTim: Invalid 1st split percentage (-a)\n");
                         return FALSE;
                    }
               }
               else if (*cmd_line == 'b') {
                    split2 = atoi(cmd_line + 1);
                    if (split2 < 1 || split2 > 99) {
                         display_msg("GpLapTim: Invalid 2nd split percentage (-b)\n");
                         return FALSE;
                    }
               }
               else if (*cmd_line == 'c') {
                    split3 = atoi(cmd_line + 1);
                    if (split3 < 1 || split3 > 99) {
                         display_msg("GpLapTim: Invalid 3rd split percentage (-c)\n");
                         return FALSE;
                    }
               }
               else if (*cmd_line == 's') {
                    freeze_time = atoi(cmd_line + 1);
                    if (freeze_time < 1 || freeze_time > 15) {
                         display_msg("GpLapTim: -s Stop time must be between 1s and 15s\n");
                         return FALSE;
                    }
                    else {
                         freeze_time *= 1000;
                    }
               }
               else if (*cmd_line == '?' || *cmd_line == 'h') {
                    Usage();
                    return FALSE;
               }

               option_next = FALSE;
          }
          else if (*cmd_line == '-' || *cmd_line == '/') {
               option_next = TRUE;
          }
          ++cmd_line;
     }

     return TRUE;
}

static void
Usage(
     void
) {
     display_msg(   "\n"
                    "Usage: gplaptim [-h?]  [-aN -bN -cN -s] [-p] [-u] [-f(filename)]\n"
                    "       -f(name)  Specify log filename's location.\n"
                    "       -p        Log player's times only.\n"
                    "\n"
                    "       -aN       1st split time percentage (default 25%).\n"
                    "       -bN       2nd split time percentage (default 50%).\n"
                    "       -cN       3rd split time percentage (default 75%).\n"
                    "\n"
                    "       -sN       Stop (freeze) delay in seconds (default 10s).\n"
                    "\n"
                    "       -h,-?     This help message.\n"
                    "       -u        Unload TSR.\n"
               );
}

static void
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

static void
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

static short
atoi(
     register const char far  *p        /* In  Pointer to ASCII string     */
) {
     register int   n;
     register int   f;

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