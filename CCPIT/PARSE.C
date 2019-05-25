/*
** File   : parse.c
** Author : TK
** Date   :  9/04/94
**
** $Header:   L:/ccpit/vcs/parse.c__   1.4   31 Oct 1994 22:30:24   tk  $
**
** Parse the command line for CCPIT.
*/

/*---------------------------------------------------------------------------
** Includes
*/

#include "ccpit.h"
#include "version.i"

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

void DumpConfig(void);
void Usage(void);
void display_msg(char near *msg);
void display_int(int v);
void display_chr(char c);
short atoi(register const char far *p);

/*---------------------------------------------------------------------------
** Data
*/

char title_msg[] =
    "@(#)"
    "CCPIT " VERSION " - Grand Prix/World Circuit Computer Cars Pit Strategy.\n"
    "Copyright (c) Trevor Kellaway (CIS:100331,2330) 1994 - All Rights Reserved.\n"
    "Copyright (c) Rene Smit 2019 - All Rights Reserved.\n\n";

/*---------------------------------------------------------------------------
** Functions
*/

int
parse(
    void
) {
    register PIT_GROUP  *pg = tmp_pit_groups;
    register int        i;
    int                 n;
    int                 option_next;
    int                 total_cars     = 0;
    int                 pit_group      = 0;

    display_msg(&title_msg[WHAT_OFFSET]);

    tmp_num_groups      = 0;
    tmp_max_cars_in_pit = DEFAULT_MAX_CARS_IN_PIT;
    tmp_randomise       = FALSE;
    tmp_tyres           = 0;
    tmp_multiplayer     = FALSE;

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
            /*
            ** Don't use switch as indirect table gets it wrong for a COM file.
            */
            if (*cmd_line == 'p') {
                n = atoi(&cmd_line[1]);
                if (n >= 5 && n <= 26) {
                    tmp_max_cars_in_pit = (byte) n;
                }
                else {
                    display_msg("ccpit: -p value should be between 5 and 26.\n");
                    return FALSE;
                }
            }
            else if (*cmd_line == 'r') {
                tmp_randomise = TRUE;
            }
            else if (*cmd_line == 'g') {
                if (pit_group < MAX_GROUPS) {
                    pg = &tmp_pit_groups[pit_group++];
                    pg->num_cars = 0;
                    pg->tyres = 0;
                }
                else {
                    display_msg("ccpit: Too many -g pit groups defined.\n");
                    return FALSE;
                }
            }
            else if (*cmd_line == 't') {
                ++cmd_line;
                if (*cmd_line >= 'A' && *cmd_line <= 'D') {
                    if (pit_group == 0) {
                       tmp_tyres = *cmd_line;
                    }
                    else if (pg->max_index == 0) {
                        pg->tyres = *cmd_line;
                        for (i = 0; i < MAX_PITS_PER_GROUP; i++) {
                            set_group_pit_tyre(pg, i, pg->tyres - 'A');
                        }
                    }
                    else if (pg->tyres) {
                        set_group_pit_tyre(pg, pg->max_index - 1, *cmd_line - 'A');
                    }
                    else {
                        display_msg("ccpit: Pit stop tyres require default group tyres.\n");
                        return FALSE;
                    }
                }
                else {
                    display_msg("ccpit: Tyres must be one 'A', 'B', 'C', or 'D'.\n");
                    return FALSE;
                }
            }
            else if (*cmd_line == 'c') {
                if (pit_group == 0) {
                    display_msg("ccpit: You must use -g before a -c.\n");
                    return FALSE;
                }

                n = atoi(&cmd_line[1]);
                if (n >= 1 && n <= 26) {
                    total_cars += n;
                    if (total_cars > 26) {
                        display_msg("ccpit: Total number of cars specified with all -c can't exceed 26.\n");
                        return FALSE;
                    }
                    pg->num_cars = (byte) n;
                }
                else {
                    display_msg("ccpit: -c value should be between 1 and 26.\n");
                    return FALSE;
                }
            }
            else if (*cmd_line == 'l') {
                if (pit_group == 0) {
                    display_msg("ccpit: You must use -g before a -l.\n");
                    return FALSE;
                }

                if (pg->num_cars == 0) {
                    display_msg("ccpit: You must use -c before the first -l in a group.\n");
                    return FALSE;
                }

                if (pg->max_index < MAX_PITS_PER_GROUP) {
                    n = atoi(&cmd_line[1]);
                    if (n >= 5 && n <= 95) {
                        pg->percent[pg->max_index++] = (byte) n;
                    }
                    else {
                        display_msg("ccpit: -l value should be between 5% and 95%.\n");
                        return FALSE;
                    }
                }
                else {
                    display_msg("ccpit: Too many -l points for a group.\n");
                    return FALSE;
                }
            }
            else if (*cmd_line == 'u') {
                unload_flag = TRUE;
                return TRUE;

            }
            else if (*cmd_line == 'm') {
                tmp_multiplayer = TRUE;
            }
            else {
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

    /*
    ** Nothing defined so use defaults.
    */
    if (pit_group == 0) {
        display_msg("Using defaults: (13 @ 25% & 55%) (13 @ 35% & 65%)  Max In Pit:10\n");
        pg = tmp_pit_groups;

        pg->num_cars        = 13;
        pg->max_index       = 2;
        pg->percent[0]      = 25;
        pg->percent[1]      = 55;

        ++pg;

        pg->num_cars        = 13;
        pg->max_index       = 2;
        pg->percent[0]      = 35;
        pg->percent[1]      = 65;

        tmp_num_groups = 2;
    }
    else {
        tmp_num_groups = pit_group;
    }

    DumpConfig();

    return TRUE;
}

void
dump_count(
    int count,
    char near *name,
    char near *plural
) {
    display_int(count);
    display_chr(' ');
    display_msg(count == 1 ? name : plural);
}

void
dump_num_cars(
    int num_cars
) {
    dump_count(num_cars, "car", "cars");
}

void
dump_tyre_compound(
    char tyre_compound
) {
    display_chr(tyre_compound);
    display_msg("'s");
}

void
dump_percentage(
    int percentage
) {
    display_int(percentage);
    display_chr('%');
}

void
DumpConfig(
    void
) {
    register PIT_GROUP *pg;
    int                 i;
    int                 j;
    int                 remaining_cars;
    int                 g;

    g = 0;
    remaining_cars = 26;
    for (i = 0, pg = tmp_pit_groups; i < tmp_num_groups; i++, pg++) {
        if (pg->num_cars > 0) {
            ++g;
            remaining_cars -= pg->num_cars;
        }
    }
    if (remaining_cars > 0) {
        ++g;
    }

    display_msg("Using ");
    dump_count(g, "pit group", "pit groups");
    display_msg(":\n");
    for (i = 0, pg = tmp_pit_groups; i < tmp_num_groups; i++, pg++) {
        if (pg->num_cars > 0) {
            display_msg(" -> ");
            dump_num_cars(pg->num_cars);
            display_msg("\n");
            if (pg->tyres != 0 || tmp_tyres != 0) {
                display_msg("    - starting on ");
                dump_tyre_compound(pg->tyres ? pg->tyres : tmp_tyres);
                display_msg("\n");
            }
            if (pg->percent[0] == 0) {
                display_msg("    - making no stops\n");
            }
            for (j = 0; j < MAX_PITS_PER_GROUP; j++) {
                if (pg->percent[j] == 0) {
                    break;
                }
                display_msg("    - stopping @ ");
                dump_percentage(pg->percent[j]);
                if (pg->tyres != 0 || tmp_tyres != 0) {
                    display_msg(" for ");
                    if (pg->tyres != 0) {
                        dump_tyre_compound('A' + get_group_pit_tyre(pg, j));
                    }
                    else {
                        dump_tyre_compound(tmp_tyres);
                    }
                }
                display_msg("\n");
            }
        }
    }
    if (remaining_cars > 0) {
        display_msg(" -> ");
        dump_num_cars(remaining_cars);
        display_msg("\n");
        if (tmp_tyres != 0) {
            display_msg("    - starting on ");
            dump_tyre_compound(tmp_tyres);
            display_msg("\n");
        }
        display_msg("    - making no stops\n");
    }
    display_msg("\n");
}

void
Usage(
    void
) {
    display_msg("Usage: ccpit [-h] [-u] [-pN] [-r] [-m] [-t?] {-g -cN [-t?] (-lN [-t?]) ...} ...\n"
                "\n"
                "       -h,-?     This help message.\n"
                "       -u        Unload TSR.\n"
                "\n"
                "       -pN       Max. number cars in pits at one time (default 10).\n"
                "       -r        Randomise group allocation on grid (default grid order).\n"
                "\n"
                "       -m        Enable local multi-player mode (player's car called to pit).\n"
                "       -t?       Specify tyres for all computer cars where ? is one of ABCD.\n"
                "\n"
                "       -g        Pit group.\n"
                "        -cN      Stop N cars (for this group).\n"
                "        -t?      Specify tyres where ? is one of ABCD (for this group).\n"
                "        -lN      Trigger cars to stop at race percentage N (for this group).\n"
                "         -t?     Specify tyres where ? is one of ABCD (for this pit stop).\n"
                "\n"
                "For the top 15 cars to pit at 30% and 60%, next 8 cars at 45% & the rest 0:\n"
                "\n"
                "       ccpit -g -c15 -l30 -l60  -g -c8 -l45\n"
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

void
display_int(
    int v
) {
    int d;
    int i;
    int b = 0;
    if (v < 0) {
        display_chr('-');
        v = -v;
    }
    for (i = 0, d = 10000; i < 5; i++, d /= 10) {
        if (b || v / d) {
            display_chr('0' + v / d);
            b = 1;
        }
        v = v % d;
    }
}

/*lint -e789 Ignore assigning auto to static */

void
display_chr(
    char c                             /* In Character to display         */
) {
    char cs[2];

    cs[0]    = c;
    cs[1]    = '$';
    msg_text = cs;
    wrt_msg();
}

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
