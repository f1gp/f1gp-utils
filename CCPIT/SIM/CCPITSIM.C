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
#include <stdlib.h>
#include <string.h>
/*lint +elib(???) */

#include "ccpit.h"

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define SIM_VERSION     "V1.3"
#define WHAT_OFFSET     4

#define MAX_EXTRA_STOPS 100

/*---------------------------------------------------------------------------
** Defines and Macros
*/

typedef struct {
    byte car_index;
    byte lap;
} ExtraStop;

/*---------------------------------------------------------------------------
** External function prototypes
*/

extern int parse(char far *cmd_line, word cmd_line_len);
extern void dump_config(void);
extern void SeedGrid(CAR far *pCar);
extern void StartFinishLineHook(CAR far *pCar, word leaders_lap, word total_laps);
extern void OntoJacks(unsigned short total_laps, CAR far *pCar);
extern word TyreChange(word tyre, CAR far *pCar, CAR_SETUP far *pCarSetup);

/*---------------------------------------------------------------------------
** Simulation Data
*/

char driver_names[40][DRIVER_NAME_SIZE];
byte replay_state[128];
byte max_cars_in_pit;
CAR cars[MAX_CARS];
CAR_SETUP car_setup;

SAVE_GAME1   far *pSaveGame1 = (SAVE_GAME1 far *) (driver_names + 38);
SAVE_GAME2   far *pSaveGame2 = (SAVE_GAME2 far *) (driver_names + 39);
REPLAY_STATE far *pReplayState = (REPLAY_STATE far *) replay_state;
byte         far *hook_nc_value = (byte far *) &max_cars_in_pit;
CAR          far *pFirstCar = cars;
char         unload_flag = FALSE;
char         near *msg_text = 0;
char         cfg_filename[80];
char         cfg_data[MAX_CFG_SIZE];


/*---------------------------------------------------------------------------
** Fixture Data
*/

word total_laps;
word run_count;
word default_tyre;
ExtraStop extra_stops[MAX_EXTRA_STOPS];
word num_extra_stops;

/*---------------------------------------------------------------------------
** Other Data
*/

char sim_title_msg[] =
    "@(#)"
    "CCPITSIM " SIM_VERSION " - GP Simulator For Computer Cars Pit Strategy.\n"
    "Copyright (c) Rene Smit 2020 - All Rights Reserved.\n\n";

/*---------------------------------------------------------------------------
** Mock Functions
*/

static byte rndnr = 0;

byte rnd(void) {
    rndnr += 37;
    return rndnr;
}

void wrt_msg(void) {
    char near *p = msg_text;
    char c;
    while ((c = *p) != '$') {
        if (c != '\r')  /* \n already translates to \r\n with printf */
            printf("%c", c);
        p++;
    }
}

word read_cfg_file(void) {
    FILE *fp;
    word bytes_read;

    /* read cfg_filename to cfg_data */
    fp = fopen(cfg_filename, "rt");
    if (fp) {
        bytes_read = fread(cfg_data, 1, 1023, fp);
        cfg_data[bytes_read] = 0;
        fclose(fp);
        return bytes_read;
    }
    return 0;
}


/*---------------------------------------------------------------------------
** Functions
*/

void init_data(void) {
    word car_index;

    memset(driver_names, 0, sizeof(driver_names));
    memset(replay_state, 0, sizeof(replay_state));
    max_cars_in_pit = 0;
    memset(&car_setup, 0, sizeof(car_setup));

    memset(cars, 0, sizeof(cars));
    for (car_index = 0; car_index < MAX_CARS; car_index++) {
        cars[car_index].si[CAR_DATA_SI_ID] = car_index + 1;
    }

    default_tyre = COMPOUND_A;

    /* unsupported for now */
    tmp_randomise = FALSE;
    tmp_multiplayer = FALSE;

    num_extra_stops = 0;
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

void fixture_add_extra_stop(byte car_index, byte lap) {
    extra_stops[num_extra_stops].car_index = car_index;
    extra_stops[num_extra_stops].lap = lap;
    ++num_extra_stops;
}

int setup_fixture_from_args(short argc, char *argv[]) {
    PIT_GROUP *pg;
    int ret, a, i, j, k, uc, ul;
    char cmd[MAX_CFG_SIZE], *arg;

    /* look for -X args for the simulator itself */
    for (a = 1; a < argc; a++) {
        arg = argv[a];
        if (arg[0] == '-' && arg[1] == 'X') {
            switch (arg[2]) {
                case 'r':
                    run_count = atoi(arg + 3);
                    break;
                case 't':
                    total_laps = atoi(arg + 3);
                    break;
                case 'u':
                    /* unscheduled pitstop for car:lap */
                    k = 3;
                    uc = atoi(arg + k);
                    while (arg[k] && arg[k] != ':') {
                        k++;
                    }
                    ul = atoi(arg + k + 1);
                    if (ul && num_extra_stops < MAX_EXTRA_STOPS) {
                        fixture_add_extra_stop(uc, ul);
                    }
                    break;
            }
            arg[0] = 0;
        }
    }

    /* CCPIT wants a space separated string for the args */
    i = 0;
    for (a = 1; a < argc && i < sizeof(cmd) - 1; a++) {
        arg = argv[a];
        if (arg[0]) {
            if (i > 0) {
                cmd[i++] = ' ';
            }
            for (j = 0; arg[j] != 0; j++) {
                cmd[i++] = arg[j];
            }
        }
    }
    cmd[i] = 0;
    printf("Calling CCPIT with parameters: %s\n", cmd);

    ret = parse(cmd, i);
    if (!ret) {
        return FALSE;
    }

    return TRUE;
}

int setup_default_fixture(void) {
    PIT_GROUP *pg;
    int ret;

    tmp_max_cars_in_pit = 5;
    tmp_tyres           = 'A';          // the default tyre for pit groups

    /* pit groups */
    tmp_num_groups = 0;
    pg = fixture_add_group(6, 'B');
    fixture_add_stop(pg, 60, 'C');

    pg = fixture_add_group(8, 'C');
    fixture_add_stop(pg, 30, 'D');
    fixture_add_stop(pg, 50, 'B');

    pg = fixture_add_group(10, 'D');
    fixture_add_stop(pg, 30, 'C');
    fixture_add_stop(pg, 70, 'D');

    /* extra pit stops */
    fixture_add_extra_stop(0, 11);
    fixture_add_extra_stop(6, 11);
    fixture_add_extra_stop(6, 66);
    return TRUE;
}

void dump_grid(void) {
    word car_index;

    for (car_index = 0; car_index < MAX_CARS; car_index++) {
        byte group;
        if (car_index & 1) {
            group = (pSaveGame2->seed_group[(car_index >> 1)] & 0x0f);
        } else {
            group = ((pSaveGame2->seed_group[(car_index >> 1)] & 0xf0) >> 4);
        }
        printf("Car %2d assigned to group %d\n", car_index + 1, group);
    }


}

void init_race(void) {
    word car_index;
    CAR far *pCar;

    for (car_index = 0; car_index < MAX_CARS; car_index++) {
        pCar = cars + car_index;
        SeedGrid(pCar);
        pCar->si[CAR_DATA_SI_TYRE_COMPOUND] = TyreChange(default_tyre, pCar, &car_setup);
        printf("Car %2d starting on %c's\n", car_index + 1, 'A' + pCar->si[CAR_DATA_SI_TYRE_COMPOUND], 'A');
    }
    /*dump_grid();*/
}

void pit_car(CAR far *pCar, word car_index, byte extra) {
    word tyre = default_tyre;

    OntoJacks(total_laps, pCar);
    tyre = TyreChange(tyre, pCar, &car_setup);

    printf("Lap %2d: Car %2d pitted, going from %c's to %c's%s\n",
        pCar->si[CAR_DATA_SI_LAP_NUMBER], car_index + 1,
        'A' + pCar->si[CAR_DATA_SI_TYRE_COMPOUND], 'A' + tyre,
        extra ? " (unscheduled)" : "");

    pCar->si[CAR_DATA_SI_TYRE_COMPOUND] = tyre;
}

void simulate_race(void) {
    word lap;
    word car_index;
    word i;
    word nc;
    CAR far *pCar;

    printf("Starting race simulation\n");
    printf("Race distance: %d laps\n", total_laps);
    init_race();

    printf("It's 5 lights, 4 lights, 3 lights, 2 lights, 1 light... go go go!\n");
    for (lap = 1; lap <= total_laps; lap++) {
        for (car_index = 0; car_index < MAX_CARS; car_index++) {
            pCar = cars + car_index;
            pCar->si[CAR_DATA_SI_LAP_NUMBER] = lap;
            StartFinishLineHook(pCar, lap, total_laps);
        }
        nc = 0;
        for (car_index = 0; car_index < MAX_CARS; car_index++) {
            pCar = cars + car_index;
            for (i = 0; i < num_extra_stops; i++) {
                if (lap == extra_stops[i].lap && car_index == extra_stops[i].car_index) {
                    pit_car(pCar, car_index, TRUE);
                    ++nc;
                }
            }
            if ((pCar->si[CAR_DATA_SI_PITFLAGS] & 0x01) && nc < max_cars_in_pit) {
                pit_car(pCar, car_index, FALSE);
                ++nc;
            }
        }
    }
    printf("Lap %2d: Race over\n", lap);
}


short main(short argc, char *argv[]) {
    int ret, r;
    char *arg;

    printf("%s", &sim_title_msg[WHAT_OFFSET]);

    init_data();

    run_count = 1;
    total_laps = 70;
    if (argc > 1) {
        ret = setup_fixture_from_args(argc, argv);
    } else {
        ret = setup_default_fixture();
    }
    if (ret) {
        dump_config();
        for (r = 0; r < run_count; r++) {
            simulate_race();
        }
    }

    return ret ? 0 : 5;
}
