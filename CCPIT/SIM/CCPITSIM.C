/*
** File   : ccpitsim.c
** Author : SDI
** Date   : 27/05/2019
**
** GP Simulator for testing CCPIT.
*/

/*---------------------------------------------------------------------------
** Includes
*/

/*lint -elib(???) */
#include <stdio.h>
#include <string.h>
/*lint +elib(???) */

#include "ccpit.h"
#include "version.i"

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define TRUE        1
#define FALSE       0

#define WHAT_OFFSET 4

/*---------------------------------------------------------------------------
** External function prototypes
*/

extern void SeedGrid(void);
extern void StartFinishLineHook(word leaders_lap, word total_laps);
extern void OntoJacks(unsigned short total_laps, CAR far *pCar);
extern word TyreChange(word tyre, CAR far *pCar, CAR_SETUP far *pCarSetup);

/*---------------------------------------------------------------------------
** Simulation Data
*/

char driver_names[40][24];
byte replay_state[128];
byte nc_value;
CAR cars[MAX_CARS];
CAR_SETUP car_setup;

SAVE_GAME1   far *pSaveGame1 = (SAVE_GAME1 far *) (driver_names + 38);
SAVE_GAME2   far *pSaveGame2 = (SAVE_GAME2 far *) (driver_names + 39);
REPLAY_STATE far *pReplayState = (REPLAY_STATE far *) replay_state;
byte         far *hook_nc_value = (byte far *) &hook_nc_value;
CAR          far *pFirstCar = cars;


/*---------------------------------------------------------------------------
** Fixture Data
*/

word total_laps;
word default_tyre;
word extra_stop_car;
word extra_stop_lap;


/*---------------------------------------------------------------------------
** Other Data
*/

char title_msg[] =
    "@(#)"
    "CCPITSIM " VERSION " - GP Simulator For Computer Cars Pit Strategy.\n"
    "Copyright (c) Rene Smit 2019 - All Rights Reserved.\n\n";

/*---------------------------------------------------------------------------
** Functions
*/

byte rnd(void) {
    /* not used */
    return 0;
}

void init_data(void) {
    word car_index;

    memset(driver_names, 0, sizeof(driver_names));
    memset(replay_state, 0, sizeof(replay_state));
    nc_value = 0;
    memset(&car_setup, 0, sizeof(car_setup));

    memset(cars, 0, sizeof(cars));
    for (car_index = 0; car_index < MAX_CARS; car_index++) {
        cars[car_index].si[CAR_DATA_SI_ID] = car_index + 1;
    }
}

PIT_GROUP *fixture_add_group(byte num_cars, byte tyres) {
    PIT_GROUP *pg = tmp_pit_groups + tmp_num_groups++;
    pg->num_cars = num_cars;
    pg->tyres = 0;
    if (tyres) {
        init_group_tyre(pg, tyres - 'A');
    }
    pg->num_stops = 0;
    return pg;
}

void fixture_add_stop(PIT_GROUP *pg, byte percent, byte tyres) {
    pg->percent[pg->num_stops] = percent;
    if (tyres) {
        set_group_pit_tyre(pg, pg->num_stops, tyres - 'A');
    }
    ++pg->num_stops;
}

void setup_fixture(void) {
    PIT_GROUP *pg;

    tmp_max_cars_in_pit = 13;
    tmp_randomise       = FALSE;
    tmp_multiplayer     = FALSE;
    tmp_tyres           = 'A';          // the default tyre for pit groups
    default_tyre        = COMPOUND_C;   // what the game provides

    total_laps          = 69;

    extra_stop_car      = 6;
    extra_stop_lap      = 11;

    tmp_num_groups = 0;

    // -tA   -g -c6 -tB -l60 -tC  -g -c8 -tC -l30 -tD -l50 -tB  -g -tD -c10 -l30 -tC -l70
    pg = fixture_add_group(6, 'B');
    fixture_add_stop(pg, 60, 'C');

    pg = fixture_add_group(8, 'C');
    fixture_add_stop(pg, 30, 'D');
    fixture_add_stop(pg, 50, 'B');

    pg = fixture_add_group(10, 'D');
    fixture_add_stop(pg, 30, 'C');
    fixture_add_stop(pg, 70, 'D');
}

void pit_car(CAR far *pCar, word car_index) {
    word tyre = default_tyre;

    OntoJacks(total_laps, pCar);
    tyre = TyreChange(tyre, pCar, &car_setup);
    pCar->si[0xb2] = tyre;

    printf("Pitted car %d on end of lap %d for %c's\n",
        car_index, pCar->si[CAR_DATA_SI_LAP_NUMBER], 'A' + tyre);
}

void simulate_race(void) {
    word lap;
    word car_index;
    CAR far *pCar;

    printf("Starting race of %d laps\n", total_laps);
    SeedGrid();
    for (lap = 1; lap <= total_laps; lap++) {
        StartFinishLineHook(lap, total_laps);
        for (car_index = 0; car_index < MAX_CARS; car_index++) {
            pCar = cars + car_index;
            cars[car_index].si[CAR_DATA_SI_LAP_NUMBER] = lap;

            if (lap == extra_stop_lap && car_index == extra_stop_car) {
                pit_car(pCar, car_index);
            }
            if (pCar->si[0x97] & 0x01) {
                pit_car(pCar, car_index);
            }
        }
    }
    printf("Race over\n");
}


short main(short argc, char *argv[]) {
    (void) argc;
    (void) argv;

    printf("%s", &title_msg[WHAT_OFFSET]);

    init_data();
    setup_fixture();
    simulate_race();

    return 0;
}
