/*
** Parse the command line.
*/

/*---------------------------------------------------------------------------
** Includes
*/

#include "gptest.h"
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
    "GpTest " VERSION " - Grand Prix/World Circuit Testing Support Tool.\n"
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
                     display_msg("GpTest: missing log filename.\n");
                     Usage();
                     return FALSE;
                }
            }
            else if (*cmd_line == 'r') {
                if (cmd_line[1] == 'g') {
                     rng_mode = 1;
                     rng_seed = atoi(&cmd_line[2]);
                }
                else if (cmd_line[1] == 'e') {
                     rng_mode = 2;
                     rng_seed = atoi(&cmd_line[2]);
                }
                // else if (cmd_line[1] == 'n') {
                //      rng_mode = 3;
                //      rng_value = atoi(&cmd_line[2]);
                // }
                else {
                     Usage();
                     return FALSE;
                }
            }
            else if (*cmd_line == 'l') {
                n = atoi(&cmd_line[1]);
                if (n >= 1 && n <= 26) {
                     limit_cars_race = n;
                }
                else {
                     display_msg("GpTest: -l value should be between 1 and 26 cars.\n");
                     return FALSE;
                }
            }
            else if (*cmd_line == 'w') {
                keep_fw = TRUE;
                return TRUE;
            }
            else if (*cmd_line == 'd') {
                no_drivers = TRUE;
                return TRUE;
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

    return TRUE;
}

void
Usage(
     void
) {
    display_msg("Usage: GpTest [-r] [-l] [-w] [-h?] [-u]\n"
                "       -r(mode)  Set randomization mode. One of:\n"
                "          -rg(N)    use random seed N for game instance.\n"
                "          -re(N)    use random seed N for each event.\n"
                // "          -rn(N)    random number always N (default 0).\n"
                "       -l(N)     Limit race to N cars.\n"
                "       -d        Do not select a default driver.\n"
                "       -w        Do not copy rear wing to front wing.\n"
                "       -h,-?     This help message.\n"
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
