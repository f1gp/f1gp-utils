/*
** File   : pit.c
** Author : TK
** Date   :  8/06/94
**
** $Header:   L:/ccpit/vcs/pit.c__   1.3   31 Oct 1994 22:30:26   tk  $
**
** Computer Car Pit Strategy.
*/

/*
Unscheduled pit stops
---------------------
Example strategy: stop in lap 5 and 10.
If at least half of the stint has been completed, an unscheduled pit stop will
be converted to an early regular pit stop.
An unscheduled pit stop won't change tyre compound.

p = regular
x = early regular
u = unscheduled

        01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
normal              p              p
dmg1    u           p              p
dmg2          x                    p
dmg3                p     u        p
dmg4                p        x
dmg5       u        p     u        p           u

*/

/*---------------------------------------------------------------------------
** Includes
*/

#include "ccpit.h"

extern void display_msg(char near *msg);

/*---------------------------------------------------------------------------
** Defines and Macros
*/

/*---------------------------------------------------------------------------
** Typedefs
*/

/*---------------------------------------------------------------------------
** Local function prototypes
*/

void fmemcpy(void far *dst, void far *src, unsigned short len);
byte get_car_index(CAR far *pCar);
PIT_GROUP far *get_group(unsigned short group_index);
void seed_group(unsigned short group_index, PIT_GROUP far *pg);
byte get_seed_group_value(unsigned short index);
void put_seed_group_value(unsigned short index, byte value);
byte get_last_pitstop_value(unsigned short index);
void put_last_pitstop_value(unsigned short index, byte value);
byte get_group_tyre(PIT_GROUP far *pg, byte stop, byte default_tyre);

/*---------------------------------------------------------------------------
** Data
*/

PIT_GROUP tmp_pit_groups[MAX_GROUPS];
byte      tmp_num_groups = 0;
byte      tmp_max_cars_in_pit;
bool      tmp_randomise;
byte      tmp_tyres;
bool      tmp_multiplayer;

/*---------------------------------------------------------------------------
** Functions
*/

/*---------------------------------------------------------------------------
** Purpose:    Set all indexes to zero at start of race.
**
** Returns:    Nothing.
**
** Notes  :    Called for every car on the grid.
*/
void
SeedGrid(
    void
) {
    register unsigned short  i;
    SAVE_GAME1 far           *sg1;
    long                     pg_magic1;

    /*
    ** Save max num of cars in pit in save game area & patch code.
    ** Save game area is at driver name #40.
    */
    pSaveGame2->num_groups      = tmp_num_groups;
    pSaveGame2->max_cars_in_pit = tmp_max_cars_in_pit;
    pSaveGame2->randomise       = tmp_randomise;
    pSaveGame2->tyres           = tmp_tyres;
    pSaveGame2->multiplayer     = tmp_multiplayer;
    *hook_nc_value              = tmp_max_cars_in_pit;

    /*
    ** Ensure all cars are marked as not stopping.
    */
    pSaveGame2->magic1 = MAGIC1SG2;
    pSaveGame2->magic2 = MAGIC2;
    for (i = 0; i < sizeof(pSaveGame2->seed_group); i++) {
         pSaveGame2->seed_group[i] = 0;
    }

    /*
    ** Init pit groups.
    ** Groups are stored at driver name #39, #38, etc.
    */
    sg1 = pSaveGame1;
    pg_magic1 = MAGIC1SG1;
    for (i = 0; i < tmp_num_groups; i++) {
        /* Copy command line structure to saved game area */
        fmemcpy(&sg1->pit_group, tmp_pit_groups + i, sizeof(PIT_GROUP));
        sg1->magic1 = pg_magic1;
        sg1->magic2 = MAGIC2;

        seed_group(i, &sg1->pit_group);

        sg1 = (SAVE_GAME1 far *) ((byte far *) sg1 - 24);
        pg_magic1 -= 0x10000L;
    }

    /*
    ** Init replay state.
    */
    for (i = 0; i < sizeof(pReplayState->last_pitstop); i++) {
        pReplayState->last_pitstop[i] = 0;
    }
    for (i = 0; i < sizeof(pReplayState->pit_groups); i++) {
        pReplayState->pit_groups[i].current_stop = 0;
    }
    pReplayState->magic = MAGICRS;

}

/*---------------------------------------------------------------------------
** Purpose:    Mark all cars as having pitted.
**
** Returns:    Nothing.
**
** Notes  :    Called as every car crosses the line.
*/
void
StartFinishLineHook(
    word leaders_lap,        /* In  Leaders lap                 */
    word total_laps          /* In  Total race distance         */
) {
    PIT_GROUP far            *pg;
    PIT_GROUP_STATE far      *pgs;
    unsigned short           j;
    unsigned short           i;
    byte                     lap;

    /*
    ** Check for pit stops (catch saved reloaded saved games which don't
    ** contain our data).
    */
    if (pSaveGame2->magic1 == MAGIC1SG2 && pSaveGame2->magic2 == MAGIC2 &&
        pReplayState->magic == MAGICRS
    ) {
        /*
        ** Patch code for saved game max num of cars in pit.
        */
        *hook_nc_value = pSaveGame2->max_cars_in_pit;

        for (i = 0, pgs = pReplayState->pit_groups; i < pSaveGame2->num_groups; i++, pgs++) {
            pg = get_group(i);
            if (pg != 0) {
                if (pgs->current_stop < pg->num_stops) {
                    /*
                    ** Has leader reached current pit lap yet?
                    */
                    lap = (byte) ((total_laps * pg->percent[pgs->current_stop] + 50) / 100);
                    if (leaders_lap >= lap) {
                        /*
                        ** Mark all cars to pit.
                        */
                        for (j = 0; j < MAX_CARS; j++) {
                            /*
                            ** Only mark cars in this group, don't mark player(s).
                            */
                            if (pSaveGame2->multiplayer || CAR_IS_CC(pFirstCar + j)) {
                                if (get_seed_group_value(j) == (byte) (i + 1)) {
                                    if (get_last_pitstop_value(j) == 0) {
                                        /*
                                        ** On proper pit schedule.
                                        */
                                        pFirstCar[j].si[0x97] |= 0x01;

                                        /*
                                        ** Mark next pit as scheduled.
                                        */
                                        put_last_pitstop_value(j, 0x80);
                                    }
                                    else if (get_last_pitstop_value(j) == 1) {
                                        /*
                                        ** Skip, but continue with normal schedule.
                                        */
                                        put_last_pitstop_value(j, 0);
                                    }
                                }
                            }
                        }
                        ++pgs->current_stop;
                    }
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------
** Purpose:    Called when a car pits during a race and goes onto the jacks.
**
** Returns:    Nothing.
**
** Notes  :
*/
void
OntoJacks(
    unsigned short total_laps,         /* In  Total race distance         */
    CAR far        *pCar               /* In  Car data structure          */
) {
    PIT_GROUP far  *pg;
    unsigned char  current_lap;
    byte           group;
    byte           prev_percent;
    byte           current_stop;
    byte           pit_window_percent;
    byte           pit_window_lap;
    unsigned short car_index;

    /*
    ** Catch saved reloaded saved games which don't contain our data.
    */
    if (pSaveGame2->magic1 == MAGIC1SG2 && pSaveGame2->magic2 == MAGIC2 &&
        pReplayState->magic == MAGICRS
    ) {
        /*
        ** If computer car (not player).
        */
        if (pSaveGame2->multiplayer || CAR_IS_CC(pCar)) {
            car_index = get_car_index(pCar);

            /*
            ** Ensure "pit request" bit is reset, as GP.EXE only resets it
            ** if the current number of laps is greater than its pit stop
            ** threshold.
            */
            pCar->si[0x97] &= ~0x01;

            if ((get_last_pitstop_value(car_index) & 0x80) != 0x00) {
                /*
                ** Scheduled stop, clear flag bit.
                */
                put_last_pitstop_value(car_index, 0);
            }
            else {
                /*
                ** Unscheduled stop.
                */
                group = get_seed_group_value(car_index);
                if (group != 0) {
                    pg = get_group(group - 1);
                    if (pg != 0) {
                        current_stop = pReplayState->pit_groups[group - 1].current_stop;
                        if (current_stop <= pg->num_stops) {
                            prev_percent = 0;
                            if (current_stop > 0) {
                                prev_percent = pg->percent[current_stop - 1];
                            }
                            pit_window_percent = (prev_percent + pg->percent[current_stop] + 1) >> 1;
                            pit_window_lap = (byte) ((total_laps * pit_window_percent + 50) / 100);
                            current_lap = pCar->si[CAR_DATA_SI_LAP_NUMBER];
                            if (current_lap > pit_window_lap) {
                                /* this will skip the next regular pit stop */
                                put_last_pitstop_value(car_index, 0x01);
                            }
                        }
                    }
                }
            }
        }
    }
}


/*---------------------------------------------------------------------------
** Purpose:    Called to change the tyres on a car at the start of a race and
**             during all pit stops.
**
** Returns:    Nothing.
**
** Notes  :    Only modifies tyre compound for (dry) races.
*/
word
TyreChange(
    word           tyre,               /* In  Tyre compound               */
    CAR far        *pCar,              /* In  Car data structure          */
    CAR_SETUP far  *pCarSetup          /* In  Per car setup base          */
) {
    register int            car_number;
    register unsigned short car_index;
    PIT_GROUP far           *pg;
    byte                    group;
    byte                    stop;

    /*
    ** Don't modify use of qualifies or wet tyres (shouldn't get these)
    */
    if (tyre != COMPOUND_W && tyre != COMPOUND_Q) {
        /*
         ** If computer car (not player).
         */
        if (CAR_IS_CC(pCar)) {
            /*
            ** Check tyre in settings.
            ** Catch saved reloaded saved games which don't contain our data.
            */
            if (pSaveGame2->magic1 == MAGIC1SG2 && pSaveGame2->magic2 == MAGIC2) {
                /*
                ** Use default game behaviour if not over-ridden.
                */
                if (pSaveGame2->tyres != 0) {
                    tyre = pSaveGame2->tyres - 'A';
                }
            }
            /*
            ** Check tyre in pit group.
            ** Catch saved reloaded saved games which don't contain our data.
            */
            car_index = get_car_index(pCar);
            group = get_seed_group_value(car_index);
            if (group != 0) {
                pg = get_group(group - 1);
                if (pg != 0) {
                    stop = pReplayState->pit_groups[group - 1].current_stop;
                    tyre = get_group_tyre(pg, stop, tyre);
                }
            }
        }
        else {
            /*
            ** Use specified players car setup compound.
            */
            car_number = CAR_NUMBER(pCar);
            tyre = pCarSetup[car_number - 1].race_tyres;
         }
    }

    return tyre;
}


/*---------------------------------------------------------------------------
** Purpose:    Our own far memory copy routine.
**
** Returns:    Nothing.
**
** Notes  :
*/
void
fmemcpy(
    void far       *dst,
    void far       *src,
    unsigned short len
) {
    register unsigned char far *d = dst;
    register unsigned char far *s = src;

    while (len--) {
        *d++ = *s++;
    }
}

byte
get_car_index(
    CAR far *pCar
) {
    return (byte) ((int) ((byte far *) pCar - (byte far *) pFirstCar) / 0xc0);
}

PIT_GROUP far *
get_group(
    unsigned short group_index
) {
    register SAVE_GAME1 far *sg1;
    if (group_index < pSaveGame2->num_groups) {
        /*
        ** Groups are stored inside SAVE_GAME1 at driver name #39, #38, etc.
        */
        sg1 = (SAVE_GAME1 far *) ((byte far *) pSaveGame1 - 24 * group_index);
        if (sg1->magic2 == MAGIC2 && sg1->magic1 == MAGIC1SG1 - 0x10000L * group_index) {
            return &sg1->pit_group;
        }
    }
    return 0;
}

void
seed_group(
    unsigned short group_index,
    PIT_GROUP far *pg
) {
    register unsigned short num_cars;
    byte j;
    byte n;

    for (num_cars = 0; num_cars < pg->num_cars; ++num_cars) {
        if (pSaveGame2->randomise) {
            n = rnd() % MAX_CARS;
        }
        else {
            n = 0;
        }

        for (j = 0; j < MAX_CARS; j++) {
            /*
            ** Fill in slot if unused.
            */
            if (get_seed_group_value(n) == 0) {
                put_seed_group_value(n, (byte) (group_index + 1));
                break;
            }

            /*
            ** Update index for current group.
            */
            if (++n >= MAX_CARS) {
                n = 0;
            }
        }
    }
}

byte
get_seed_group_value(
     unsigned short index
) {
    if (index & 1) {
        return (pSaveGame2->seed_group[(index >> 1)] & 0x0f);
    }
    return ((pSaveGame2->seed_group[(index >> 1)] & 0xf0) >> 4);
}

void
put_seed_group_value(
    unsigned short index,
    byte value
) {
    if (index & 1) {
        pSaveGame2->seed_group[(index >> 1)] &= 0xf0;
        pSaveGame2->seed_group[(index >> 1)] |= value;
    }
    else {
        pSaveGame2->seed_group[(index >> 1)] &= 0x0f;
        pSaveGame2->seed_group[(index >> 1)] |= (byte) (value << 4);
    }
}

byte
get_last_pitstop_value(
     unsigned short index
) {
    return pReplayState->last_pitstop[index];
}

void
put_last_pitstop_value(
    unsigned short index,
    byte           value
) {
    pReplayState->last_pitstop[index] = value;
}

byte
get_group_tyre(
    PIT_GROUP far *pg,
    byte stop,
    byte default_tyre
) {
    byte tyre;

    tyre = default_tyre;
    if (pg->tyres != 0) {
        if (stop == 0) {
            /* tyre for start of race */
            tyre = pg->tyres - 'A';
        }
        else {
            /* tyre for current pit stop */
            if (stop > MAX_PITS_PER_GROUP) {
                stop = MAX_PITS_PER_GROUP;
            }
            tyre = get_group_pit_tyre(pg, stop - 1);
        }
    }
    return tyre;
}

void
init_group_tyre(
    PIT_GROUP far *pg,
    byte tyre
) {
    byte i;

    pg->tyres = 'A' + tyre;
    for (i = 0; i < MAX_PITS_PER_GROUP; i++) {
        set_group_pit_tyre(pg, i, tyre);
    }
}

byte
get_group_pit_tyre(
    PIT_GROUP far *pg,
    byte index
) {
    /* stored as 12 times 2 bits */
    byte far *p = pg->pit_tyres + (index >> 2);
    byte s = (index & 3) << 1;
    return (*p >> s) & 3;
}

void
set_group_pit_tyre(
    PIT_GROUP far *pg,
    byte index,
    byte tyre
) {
    /* stored as 12 times 2 bits */
    byte far *p = pg->pit_tyres + (index >> 2);
    byte s = (index & 3) << 1;
    *p &= ~(3 << s);
    *p |= (tyre & 3) << s;
}

/*---------------------------------------------------------------------------
** End of File
*/

