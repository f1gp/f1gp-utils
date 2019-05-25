/*
** File   : pit.c
** Author : TK
** Date   :  8/06/94
**
** $Header:   L:/ccpit/vcs/pit.c__   1.3   31 Oct 1994 22:30:26   tk  $
**
** Computer Car Pit Strategy.
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
SAVE_GAME1 far *get_group_container(unsigned short group_index);
void seed_group(unsigned short group_index, PIT_GROUP far *pg);
byte get_seed_group_value(unsigned short index);
void put_seed_group_value(unsigned short index, byte value);
byte get_last_pitstop_value(unsigned short index);
void put_last_pitstop_value(unsigned short index, byte value);
byte get_group_tyre(PIT_GROUP far *pg, int group_index, byte default_tyre);

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
    pSaveGame2->num_groups        = tmp_num_groups;
    pSaveGame2->max_cars_in_pit   = tmp_max_cars_in_pit;
    pSaveGame2->randomise         = tmp_randomise;
    pSaveGame2->tyres             = tmp_tyres;
    pSaveGame2->multiplayer       = tmp_multiplayer;
    *hook_nc_value                = tmp_max_cars_in_pit;

    /*
    ** Ensure all cars are marked as not stopping.
    */
    pSaveGame2->magic1  = MAGIC1SG2;
    pSaveGame2->magic2  = MAGIC2;
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
        fmemcpy(&sg1->pit_group, tmp_pit_groups + i, sizeof(pSaveGame1->pit_group));
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
        pReplayState->pit_groups[i].current_index = 0;
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
    word           leaders_lap,        /* In  Leaders lap                 */
    word           total_laps,         /* In  Total race distance         */
    unsigned short car_index,          /* In  Car index * 0xc0            */
    CAR far        *pCar               /* In  Car data structure          */
) {
    SAVE_GAME1 far           *sg1;
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
            sg1 = get_group_container(i);
            if (sg1 != 0) {
                pg = &sg1->pit_group;
                if (pgs->current_index < pg->max_index) {
                    /*
                    ** Has leader reached current pit lap yet?
                    */
                    lap = (byte) ((total_laps * pg->percent[pgs->current_index]) / 100) - 1;
                    if (leaders_lap >= lap) {
                        /*
                        ** Mark all cars to pit.
                        */
                        for (j = 0; j < MAX_CARS; j++) {
                            /*
                            ** Only mark cars in this group, don't mark player(s).
                            */
                            if (pSaveGame2->multiplayer || (pFirstCar[j].si[0xac] & 0x80) == 0x00) {
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
                                }
                            }
                        }
                        ++pgs->current_index;
                    }
                }
            }
        }

        /*
        ** Check for unscheduled stops.
        */
        car_index /= sizeof(pCar->si);
        if (pSaveGame2->multiplayer || (pCar->si[0xac] & 0x80) == 0x00) {
            if ((get_last_pitstop_value(car_index) & 0x7f) != 0) {
                if (pCar->si[CAR_DATA_SI_LAP_NUMBER] == get_last_pitstop_value(car_index)) {
                    pCar->si[0x97] |= 0x01;
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
    unsigned short car_index,          /* In  Car index * 0xc0            */
    CAR far        *pCar               /* In  Car data structure          */
) {
    unsigned char  current_lap;

    /*
    ** Catch saved reloaded saved games which don't contain our data.
    */
    if (pSaveGame2->magic1 == MAGIC1SG2 && pSaveGame2->magic2 == MAGIC2 &&
        pReplayState->magic == MAGICRS
    ) {
        /*
        ** If computer car (not player).
        */
        if (pSaveGame2->multiplayer || (pCar->si[0xac] & 0x80) == 0x00) {
            car_index /= sizeof(pCar->si);

            /*
            ** Ensure "pit request" bit is reset, as GP.EXE only resets it
            ** if the current number of laps is greater than its pit stop
            ** threshold.
            */
            pCar->si[0x97] &= ~0x01;

            /*
            ** Store unscheduled pit stops.
            */
            if ((get_last_pitstop_value(car_index) & 0x80) == 0x00) {
                /*
                ** If less than 40% of the race distance remains then
                ** schedule a single stop at 40% of the remaining distance.
                ** Leave value set to non-zero so we don't go back to the
                ** routine stop strategy.
                */
                current_lap = pCar->si[CAR_DATA_SI_LAP_NUMBER];
                if (current_lap <= (total_laps * 40 / 100)) {
                    put_last_pitstop_value(car_index, (byte) (((total_laps - current_lap) * 40 / 100) + current_lap));
                }
                else {
                    put_last_pitstop_value(car_index, 0x7f);
                }
            }
            else {
                /*
                ** Scheduled stop, clear flag bit.
                */
                put_last_pitstop_value(car_index, 0);
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
    register unsigned short j;
    SAVE_GAME1 far          *sg1;
    byte                    group;

    /*
    ** Don't modify use of qualifies or wet tyres (shouldn't get these)
    */
    if (tyre != COMPOUND_W && tyre != COMPOUND_Q) {
        /*
         ** If computer car (not player).
         */
        if ((pCar->si[0xac] & 0x80) == 0x00) {
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
            j = (byte) ((int) ((byte far *) pCar  - (byte far *) pFirstCar) / 0xc0);
            group = get_seed_group_value(j);
            if (group != 0) {
                sg1 = get_group_container(group - 1);
                if (sg1 != 0) {
                    tyre = get_group_tyre(&sg1->pit_group, group - 1, tyre);
                }
            }
        }
        else {
            /*
            ** Use specified players car setup compound.
            */
            car_number = pCar->si[0xac] & 0x3f;
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

SAVE_GAME1 far *
get_group_container(
    unsigned short group_index
) {
    /*
    ** Group containers are stored at driver name #39, #38, etc.
    */
    register SAVE_GAME1 far *sg1 = (SAVE_GAME1 far *) ((byte far *) pSaveGame1 - 24 * group_index);
    if (sg1->magic2 == MAGIC2 && sg1->magic1 == MAGIC1SG1 - 0x10000L * group_index) {
        return sg1;
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
        return (pSaveGame2->seed_group[(index / 2)] & 0x0f);
    }
    return ((pSaveGame2->seed_group[(index / 2)] & 0xf0) >> 4);
}

void
put_seed_group_value(
    unsigned short index,
    byte value
) {
    if (index & 1) {
        pSaveGame2->seed_group[(index / 2)] &= 0xf0;
        pSaveGame2->seed_group[(index / 2)] |= value;
    }
    else {
        pSaveGame2->seed_group[(index / 2)] &= 0x0f;
        pSaveGame2->seed_group[(index / 2)] |= (byte) (value << 4);
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
    int group_index,
    byte default_tyre
) {
    byte tyre;
    byte index;

    tyre = default_tyre;
    index = pReplayState->pit_groups[group_index].current_index;
    if (pg->tyres != 0) {
        if (index == 0) {
            /* check default pit group tyre */
            tyre = pg->tyres - 'A';
        }
        else {
            /* check tyre for current pit stop */
            tyre = get_group_pit_tyre(pg, index - 1);
        }
    }
    return tyre;
}

byte
get_group_pit_tyre(
    PIT_GROUP far *pg,
    byte index
) {
    return ((pg->pit_tyres[index / 4]) >> ((index % 4) * 2)) & 3;
}

void
set_group_pit_tyre(
    PIT_GROUP far *pg,
    byte index,
    byte tyre
) {
    byte s = (index % 4) * 2;
    byte t = (tyre & 3) << s;
    pg->pit_tyres[index / 4] &= ~(3 << s);
    pg->pit_tyres[index / 4] |= t;
}

/*---------------------------------------------------------------------------
** End of File
*/

