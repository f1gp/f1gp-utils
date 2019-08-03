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

#define WHAT_OFFSET 4

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef struct {
    int        pit_group;
    int        total_cars;
} ParseState;

/*---------------------------------------------------------------------------
** Local function prototypes
*/

void Usage(void);
void display_msg(char near *msg);
void display_int(int v);
void display_chr(char c);
short parse_short(register const char far *p);

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
do_parse(
    char far *cmd_line,
    word cmd_line_len,
    ParseState *parse_state
) {
    register PIT_GROUP *pg;
    register int       i, n;
    int                option_next, comment, in_cfg_file;

    in_cfg_file = cmd_line == cfg_data;
    comment = FALSE;
    for (i = 0; i <= cmd_line_len; i++) {
        int newline = cmd_line[i] == '\r' || cmd_line[i] == '\n';
        if (newline) {
            comment = FALSE;
        }
        else if (in_cfg_file && cmd_line[i] == ';') {
            comment = TRUE;
        }
        if (comment || newline || cmd_line[i] == ' '  || cmd_line[i] == '\t') {
            cmd_line[i] = '\0';
        }
    }

    pg = &tmp_pit_groups[parse_state->pit_group];
    option_next = FALSE;
    while (cmd_line_len--) {
        if (option_next) {
            /*
            ** Don't use switch as indirect table gets it wrong for a COM file.
            */
            if (*cmd_line == 'p') {
                n = parse_short(&cmd_line[1]);
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
                if (parse_state->pit_group < MAX_GROUPS) {
                    if (parse_state->pit_group == 0 || pg->num_cars > 0) {
                        pg = &tmp_pit_groups[parse_state->pit_group++];
                        pg->num_cars = 0;
                        pg->tyres = 0;
                    }
                }
                else {
                    display_msg("ccpit: Too many -g pit groups defined.\n");
                    return FALSE;
                }
            }
            else if (*cmd_line == 't') {
                ++cmd_line;
                if (*cmd_line >= 'A' && *cmd_line <= 'D') {
                    if (parse_state->pit_group == 0) {
                       tmp_tyres = *cmd_line;
                    }
                    else if (pg->num_stops == 0) {
                        init_group_tyre(pg, *cmd_line - 'A');
                    }
                    else if (pg->tyres) {
                        set_group_pit_tyre(pg, pg->num_stops - 1, *cmd_line - 'A');
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
                if (parse_state->pit_group == 0) {
                    display_msg("ccpit: You must use -g before a -c.\n");
                    return FALSE;
                }
                n = parse_short(&cmd_line[1]);
                if (n >= 1 && n <= 26) {
                    parse_state->total_cars += n;
                    if (parse_state->total_cars > 26) {
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
                if (parse_state->pit_group == 0) {
                    display_msg("ccpit: You must use -g before a -l.\n");
                    return FALSE;
                }

                if (pg->num_cars == 0) {
                    display_msg("ccpit: You must use -c before the first -l in a group.\n");
                    return FALSE;
                }

                if (pg->num_stops < MAX_PITS_PER_GROUP) {
                    n = parse_short(&cmd_line[1]);
                    if (n >= 5 && n <= 95) {
                        pg->percent[pg->num_stops++] = (byte) n;
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
        else if (*cmd_line == '@') {
            if (in_cfg_file) {
                display_msg("@ option inside config file is not allowed\n");
                return FALSE;
            }
            else {
                word cfg_len;
                char *pCfg = cfg_filename;
                ++cmd_line;
                n = 0;
                while (*cmd_line && n < sizeof(cfg_filename) - 1) {
                    *pCfg++ = *cmd_line++;
                    ++n;
                }
                *pCfg = 0;
                cfg_len = read_cfg_file();
                if (cfg_len > 0) {
                    if (!do_parse(cfg_data, cfg_len, parse_state)) {
                        return FALSE;
                    }
                } else {
                    display_msg("Failed to read config file: ");
                    display_msg(cfg_filename);
                    display_chr('\n');
                    return FALSE;
                }
            }
        }
        ++cmd_line;
    }

    if (parse_state->pit_group > 0 && pg->num_cars == 0) {
        /* last group is empty */
        --parse_state->pit_group;
    }

    return TRUE;
}

int
parse(
    char far *cmd_line,
    char cmd_line_len
) {
    ParseState parse_state;

    parse_state.pit_group = 0;
    parse_state.total_cars = 0;

    display_msg(&title_msg[WHAT_OFFSET]);

    tmp_num_groups      = 0;
    tmp_max_cars_in_pit = DEFAULT_MAX_CARS_IN_PIT;
    tmp_randomise       = FALSE;
    tmp_tyres           = 0;
    tmp_multiplayer     = FALSE;

    if (!do_parse(cmd_line, cmd_line_len, &parse_state)) {
        return FALSE;
    }

    /*
    ** Nothing defined so use defaults.
    */
    if (parse_state.pit_group == 0) {
        PIT_GROUP  *pg = tmp_pit_groups;
        pg->num_cars   = 13;
        pg->num_stops  = 2;
        pg->percent[0] = 25;
        pg->percent[1] = 55;
        ++pg;

        pg->num_cars   = 13;
        pg->num_stops  = 2;
        pg->percent[0] = 35;
        pg->percent[1] = 65;

        parse_state.pit_group = 2;
    }

    tmp_num_groups = parse_state.pit_group;

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
dump_group_hdr(
    int num_cars
) {
    display_msg(" -> ");
    dump_num_cars(num_cars);
    display_msg("\n");
}

void
dump_group_item_hdr(
    char near *hdr
) {
    display_msg("    - ");
    display_msg(hdr);
}

void
dump_tyres_start(
    byte tyres
) {
    dump_group_item_hdr("starting on ");
    dump_tyre_compound(tyres);
    display_msg("\n");
}

void
dump_no_stops(
    void
) {
    dump_group_item_hdr("making no pit stops\n");
}


/*
** Show parsed pit group configuration.
** Called from install.asm when parsing successful and ready to be installed.
*/
void
dump_config(
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
        else {
            display_msg("Warning: empty pit group.\n");
        }
    }
    if (remaining_cars > 0) {
        ++g;
    }

    display_msg("Maximum ");
    dump_num_cars(tmp_max_cars_in_pit);
    display_msg(" in the pits");
    display_msg("\n");
    if (tmp_randomise) {
        display_msg("Group allocation on grid will be randomised\n");
    }
    if (tmp_multiplayer) {
        display_msg("Local multi-player mode enabled\n");
    }

    display_msg("Using ");
    dump_count(g, "pit group", "pit groups");
    display_msg(":\n");
    for (i = 0, pg = tmp_pit_groups; i < tmp_num_groups; i++, pg++) {
        if (pg->num_cars > 0) {
            dump_group_hdr(pg->num_cars);
            if (pg->tyres != 0 || tmp_tyres != 0) {
                dump_tyres_start(pg->tyres ? pg->tyres : tmp_tyres);
            }
            for (j = 0; j < MAX_PITS_PER_GROUP; j++) {
                if (pg->percent[j] == 0) {
                    if (j == 0) {
                        dump_no_stops();
                    }
                    break;
                }
                dump_group_item_hdr("pitting @ ");
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
        dump_group_hdr(remaining_cars);
        if (tmp_tyres != 0) {
            dump_tyres_start(tmp_tyres);
        }
        dump_no_stops();
    }
    if (tmp_num_groups > 4) {
        display_msg("Warning: drivers of cars 3");
        display_int(10 - tmp_num_groups);
        display_msg(" through 39 will be named #3");
        display_int(10 - tmp_num_groups);
        display_msg(" through #39 resp.\n");
    }
    display_msg("\n");
}

void
Usage(
    void
) {
    display_msg("Usage: ccpit [-h] [-u] [-pN] [-r] [-m] [-t?] {-g -cN [-t?] (-lN [-t?]) ...} ...\n"
                "\n"
                "       @filename Read options from filename.\n"
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
                "         -t?     Specify tyres where ? is one of ABCD (for this stop).\n"
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
    if (v == 0) {
        display_chr('0');
    }
    else {
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
parse_short(
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
