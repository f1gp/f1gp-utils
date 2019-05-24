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
byte get_seed_group_value(unsigned short index);
void put_seed_group_value(unsigned short index, byte value);
byte get_last_pitstop_value(unsigned short index);
void put_last_pitstop_value(unsigned short index, byte value);

/*---------------------------------------------------------------------------
** Data
*/

PIT_GROUP tmp_pit_groups[MAX_GROUPS];
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
    register unsigned short  j;
    register unsigned short  i;
    register PIT_GROUP far   *pg;
    unsigned short           num_cars;
    byte                     n;

    /*
    ** Save max num of cars in pit in save game area & patch code.
    */
    pSaveGame2->max_cars_in_pit   = tmp_max_cars_in_pit;
    pSaveGame2->randomise         = tmp_randomise;
    pSaveGame2->tyres             = tmp_tyres;
    pSaveGame2->multiplayer       = tmp_multiplayer;
    *hook_nc_value                = tmp_max_cars_in_pit;

    /*
    ** Copy command line structure to saved game area.
    */
    fmemcpy(pSaveGame1->pit_groups, tmp_pit_groups, sizeof(tmp_pit_groups));
    pSaveGame1->magic1  = MAGIC1SG1;
    pSaveGame1->magic2  = MAGIC2;

    /*
    ** Ensure all cars are marked as not stopping.
    */
    pSaveGame2->magic1  = MAGIC1SG2;
    pSaveGame2->magic2  = MAGIC2;
    for (i = 0; i < sizeof(pSaveGame2->seed_group); i++) {
         pSaveGame2->seed_group[i] = 0;
    }

    /*
    ** Clear damaged car strategy structures.
    */
    pSaveGame3->magic1  = MAGIC1SG3;
    pSaveGame3->magic2  = MAGIC2;
    pSaveGame4->magic1  = MAGIC1SG4;
    pSaveGame4->magic2  = MAGIC2;
    for (i = 0; i < sizeof(pSaveGame3->last_pitstop1); i++) {
         pSaveGame3->last_pitstop1[i] = 0;
         pSaveGame4->last_pitstop2[i] = 0;
    }

    /*
    ** Zero group indexes.
    */
    for (i = 0, pg = pSaveGame1->pit_groups; i < MAX_GROUPS; i++, pg++) {
         pg->current_index = 0;

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
                    put_seed_group_value(n, (byte) (i + 1));
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
    register PIT_GROUP far   *pg;
    register unsigned short  j;
    int                      i;
    byte                     lap;

    /*
    ** Check for pit stops (catch saved reloaded saved games which don't
    ** contain our data).
    */
    if ( pSaveGame1->magic1 == MAGIC1SG1 && pSaveGame1->magic2 == MAGIC2 &&
         pSaveGame2->magic1 == MAGIC1SG2 && pSaveGame2->magic2 == MAGIC2 &&
         pSaveGame3->magic1 == MAGIC1SG3 && pSaveGame3->magic2 == MAGIC2 &&
         pSaveGame4->magic1 == MAGIC1SG4 && pSaveGame4->magic2 == MAGIC2
    ) {
        /*
        ** Patch code for saved game max num of cars in pit.
        */
        *hook_nc_value = pSaveGame2->max_cars_in_pit;

        for (i = 0, pg = pSaveGame1->pit_groups; i < MAX_GROUPS; i++, pg++) {
            if (pg->current_index < pg->max_index) {
                /*
                ** Has leader reached current pit lap yet?
                */
                lap = (byte) ((total_laps * pg->percent[pg->current_index]) / 100) - 1;
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
                    ++pg->current_index;
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
    if ( pSaveGame3->magic1 == MAGIC1SG3 && pSaveGame3->magic2 == MAGIC2 &&
        pSaveGame4->magic1 == MAGIC1SG4 && pSaveGame4->magic2 == MAGIC2
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
    register int car_number;
    register unsigned short j;
    byte group;

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
            if (pSaveGame1->magic1 == MAGIC1SG1 && pSaveGame1->magic2 == MAGIC2) {
                j = (byte) ((int) ((byte far*) pCar  - (byte far*) pFirstCar) / 0xc0);
                group = get_seed_group_value(j);
                if (group != 0) {
                    if (pSaveGame1->pit_groups[group-1].tyres != 0) {
                        tyre = pSaveGame1->pit_groups[group-1].tyres - 'A';
                    }
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
    if (index < MAX_CARS / 2) {
        return pSaveGame3->last_pitstop1[index];
    }
    return pSaveGame4->last_pitstop2[index - (MAX_CARS / 2)];
}

void
put_last_pitstop_value(
    unsigned short index,
    byte           value
) {
    if (index < MAX_CARS / 2) {
        pSaveGame3->last_pitstop1[index] = value;
    }
    else {
        pSaveGame4->last_pitstop2[index - (MAX_CARS / 2)] = value;
    }
}

/*---------------------------------------------------------------------------
** End of File
*/

