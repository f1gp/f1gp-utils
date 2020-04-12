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
CAR far *get_car(byte car_index);
byte calc_lap_from_percent(byte total_laps, byte percent);
byte calc_pit_window_lap(PIT_GROUP far *pg, byte current_stop, byte total_laps);
PIT_GROUP far *get_group(unsigned short group_index);
void seed_group(unsigned short group_index);
void assign_next_car_to_seed_group(byte n, unsigned short group_index);
bool is_seed_group_unused(unsigned short index);
byte get_seed_group(unsigned short index);
void reset_seed_group(unsigned short index);
void put_seed_group(unsigned short index, byte value);
byte get_group_tyre(PIT_GROUP far *pg, byte stop, byte default_tyre);
SAVE_GAME1 far *get_savegame1(unsigned short group_index);
long get_group_magic1(long group_index);

/*---------------------------------------------------------------------------
** Data
*/

PIT_GROUP tmp_pit_groups[MAX_GROUPS];
byte      tmp_num_groups = 0;
byte      tmp_reserved_pit_groups[40];  /* id -> group */
byte      tmp_max_cars_in_pit;
bool      tmp_randomise;
byte      tmp_tyres;
bool      tmp_multiplayer;

/*---------------------------------------------------------------------------
** Functions
*/

/*---------------------------------------------------------------------------
** Purpose:    Assign pit groups to all cars on the grid.
**
** Returns:    Nothing.
**
** Notes  :    Called for every car on the grid.
**             All car ID's have been assigned already.
**             TyreChange will be called afterwards.
*/
void
SeedGrid(
    CAR far *pCar                /* In  Car to be seeded            */
) {
    register unsigned short  i;
    SAVE_GAME1 far           *sg1;
    byte                     id;
    byte                     car_index;

    id = CAR_NUMBER(pCar);
    car_index = get_car_index(pCar);

    if (car_index == 0) {
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
        for (i = 0; i < MAX_CARS; i++) {
            reset_seed_group(i);
        }

        /*
        ** Init replay state.
        */
        pReplayState->magic = MAGICRS;
        for (i = 0; i < MAX_CARS; i++) {
            pReplayState->current_stop[i] = 0;
        }

        /*
        ** Init pit groups.
        */
        for (i = 0; i < tmp_num_groups; i++) {
            sg1 = get_savegame1(i);

            /* Copy command line structure to saved game area */
            fmemcpy(&sg1->pit_group, tmp_pit_groups + i, sizeof(PIT_GROUP));
            sg1->magic1 = get_group_magic1(i);
            sg1->magic2 = MAGIC2;

            seed_group(i);
        }
    }

    /* Check if car is explicitly assigned to a pit group */
    if (id >= 1 && id <= 40) {
        byte reserved_group = tmp_reserved_pit_groups[id - 1];
        if (reserved_group != 0) {
            put_seed_group(car_index, reserved_group);
        }
    }
}

/*---------------------------------------------------------------------------
** Purpose:    Mark car as having to pit.
**
** Returns:    Nothing.
**
** Notes  :    Called as every car crosses the line.
*/
void
StartFinishLineHook(
    CAR far *pCar,               /* In  Car crossing the line       */
    word     leaders_lap,        /* In  Leaders lap                 */
    word     total_laps          /* In  Total race distance         */
) {
    unsigned short car_index;
    PIT_GROUP far  *pg;
    byte           group;
    byte           current_stop;
    byte           lap;

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

        /*
        ** Don't mark player(s).
        */
        if (pSaveGame2->multiplayer || CAR_IS_CC(pCar)) {
            car_index = get_car_index(pCar);
            group = get_seed_group(car_index);
            if (group != 0) {
                pg = get_group(group - 1);
                if (pg != 0) {
                    /* TODO: store lap number instead for more dynamic strategy in the future */
                    current_stop = pReplayState->current_stop[car_index];
                    if (current_stop < pg->num_stops) {
                        /*
                        ** Has leader reached current pit lap yet?
                        */
                        lap = calc_lap_from_percent(total_laps, pg->percent[current_stop]);
                        if (leaders_lap >= lap) {
                            /*
                            ** Mark car to pit.
                            */
                            pCar->si[CAR_DATA_SI_PITFLAGS] |= 0x01;

                            ++current_stop;
                            pReplayState->current_stop[car_index] = current_stop;
                        }
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
    byte           current_stop;
    byte           current_stop_value;
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

            current_stop_value = pReplayState->current_stop[car_index];
            current_stop = current_stop_value & 0x7f;
            if ((pCar->si[CAR_DATA_SI_PITFLAGS] & 0x01) == 0x00) {
                /*
                ** Unscheduled stop.
                */
                group = get_seed_group(car_index);
                if (group != 0) {
                    pg = get_group(group - 1);
                    if (pg != 0) {
                        if (current_stop < pg->num_stops) {
                            pit_window_lap = calc_pit_window_lap(pg, current_stop, total_laps);
                            current_lap = pCar->si[CAR_DATA_SI_LAP_NUMBER];
                            if (current_lap > pit_window_lap) {
                                /* reschedule the next regular pit stop to right now */
                                ++current_stop;
                                pReplayState->current_stop[car_index] = current_stop;
                            }
                        }
                    }
                }
            }

            /*
            ** Ensure "pit request" bit is reset, as GP.EXE only resets it
            ** if the current number of laps is greater than its pit stop
            ** threshold.
            */
            pCar->si[CAR_DATA_SI_PITFLAGS] &= ~0x01;
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
        ** Check for pit stops (catch saved reloaded saved games which don't
        ** contain our data).
        */
        if (pSaveGame2->magic1 == MAGIC1SG2 && pSaveGame2->magic2 == MAGIC2 &&
            pReplayState->magic == MAGICRS
        ) {
            /*
             ** If computer car (not player).
             */
            if (pSaveGame2->multiplayer || CAR_IS_CC(pCar)) {
                /*
                ** Check tyre in settings.
                ** Use default game behaviour if not over-ridden.
                */
                if (pSaveGame2->tyres != 0) {
                    tyre = pSaveGame2->tyres - 'A';
                }
                /*
                ** Check tyre in pit group.
                ** Catch saved reloaded saved games which don't contain our data.
                */
                car_index = get_car_index(pCar);
                group = get_seed_group(car_index);
                if (group != 0) {
                    pg = get_group(group - 1);
                    if (pg != 0) {
                        stop = pReplayState->current_stop[car_index];
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
         else if (!CAR_IS_CC(pCar)) {
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
    register byte far *d = (byte far *)dst;
    register byte far *s = (byte far *)src;

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

CAR far *
get_car(
    byte car_index
) {
    return pFirstCar + car_index;
}

byte
calc_lap_from_percent(
    byte total_laps,
    byte percent
) {
     return (byte) ((total_laps * percent + 50) / 100);
}

byte
calc_pit_window_lap(
    PIT_GROUP far *pg,
    byte current_stop,
    byte total_laps
) {
    byte prev_percent;
    byte pit_window_percent;

    prev_percent = 0;
    if (current_stop > 0) {
        prev_percent = pg->percent[current_stop - 1];
    }
    pit_window_percent = (prev_percent + pg->percent[current_stop] + 1) >> 1;
    return calc_lap_from_percent(total_laps, pit_window_percent);
}

PIT_GROUP far *
get_group(
    unsigned short group_index
) {
    register SAVE_GAME1 far *sg1;
    if (group_index < pSaveGame2->num_groups) {
        sg1 = get_savegame1(group_index);
        if (sg1->magic2 == MAGIC2 && sg1->magic1 == get_group_magic1(group_index)) {
            return &sg1->pit_group;
        }
    }
    return 0;
}

void
seed_group(
    unsigned short group_index
) {
    unsigned short num_cars;
    PIT_GROUP far *pg;
    byte           n;

    pg = &get_savegame1(group_index)->pit_group;

    for (num_cars = 0; num_cars < pg->num_cars; ++num_cars) {
        if (pSaveGame2->randomise) {
            n = rnd() % MAX_CARS;
        }
        else {
            n = 0;
        }
        assign_next_car_to_seed_group(n, group_index);
    }
}

void
assign_next_car_to_seed_group(
    byte n,
    unsigned short group_index
) {
    register byte  j;

    for (j = 0; j < MAX_CARS; j++) {
        if (n >= MAX_CARS) {
            n = 0;
        }

        /*
        ** Fill in slot if unused.
        */
        if (is_seed_group_unused(n)) {
            put_seed_group(n, (byte) (group_index + 1));
            break;
        }

        /*
        ** Try next car.
        */
        ++n;
    }
}

bool
is_seed_group_unused(
     unsigned short index
) {
    byte id;

    if (get_seed_group(index) != 0) {
        return FALSE;
    }

    id = CAR_NUMBER(get_car(index));
    if (id >= 1 && id <= 40 && tmp_reserved_pit_groups[id - 1] != 0) {
        return FALSE;
    }

    return TRUE;
}

byte
get_seed_group(
     unsigned short index
) {
    if (index & 1) {
        return (pSaveGame2->seed_group[(index >> 1)] & 0x0f);
    }
    return ((pSaveGame2->seed_group[(index >> 1)] & 0xf0) >> 4);
}

void
reset_seed_group(
    unsigned short index
) {
    put_seed_group(index, 0);
}

void
put_seed_group(
     unsigned short index,
     byte value
) {
    value &= 0x0f;
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
    set_group_pit_tyre(pg, 0, tyre);
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
    byte i;
    byte s;
    byte far *p;

    /* set it for this and all subsequent stops */
    for (i = index; i < MAX_PITS_PER_GROUP; i++) {
        /* stored as 12 times 2 bits */
        p = pg->pit_tyres + (i >> 2);
        s = (i & 3) << 1;
        *p &= ~(3 << s);
        *p |= (tyre & 3) << s;
    }
}

SAVE_GAME1 far *
get_savegame1(
    unsigned short group_index
) {
    /* Groups are stored at driver name #39, #38, etc. */
    return (SAVE_GAME1 far *) ((byte far *) pSaveGame1 - group_index * DRIVER_NAME_SIZE);
}

long
get_group_magic1(
    long group_index
) {
    return MAGIC1SG1 - 0x10000L * group_index;
}

/*---------------------------------------------------------------------------
** End of File
*/

