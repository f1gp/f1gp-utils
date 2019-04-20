/*
** File   : savinf.c
** Author : TK 
** Date   : 29/10/94
**
** $Header$
**
** 
*/

#define VERSION     "V1.0 29/10/94"

/*---------------------------------------------------------------------------
** Includes
*/

/*lint -elib(???) */
#include <stdio.h>
#include <assert.h>
/*lint +elib(???) */

#include "savinf.h"

/*---------------------------------------------------------------------------
** Defines and Macros
*/

/*---------------------------------------------------------------------------
** Typedefs
*/

/*---------------------------------------------------------------------------
** Local function prototypes
*/

/*---------------------------------------------------------------------------
** Data
*/

/*lint -esym(528,sccsid) */
static const char sccsid[] = "@(#)" __FILE__ "\t" VERSION;

static SAVED_GAME   sg;

static const char *op_std_text[]  = { "Rookie", "Amateur", "Semi-Pro", "Pro", "Ace" };
static const char *op_sprd_text[] = { "Same", "1991", "Random" };
static const float fps[] = {
     25.0, 23.0, 21.4, 20.0, 18.7, 17.6, 16.6, 15.7, 15.0, 14.2, 13.6, 13.0, 
     12.5, 12.0, 11.5, 11.1, 10.7, 10.3, 10.0,  9.6,  9.3,  9.0,  8.8,  8.5, 
      8.3,  8.1
};

static const char *circuit[] = {
     "Phoenix, United States",
     "Interlagos, Brazil",
     "Imola, San Marino",
     "Monte-Carlo, Monaco",
     "Montreal, Canada",
     "Mexico City, Mexico",
     "Magny-Cours, France",
     "Silverstone, Great Britain",
     "Hockenheim, Germany",
     "Hungaroring, Hungary",
     "Spa-Francorchamps, Belgium",
     "Monza, Italy",
     "Estoril, Portugal",
     "Barcelona, Spain",
     "Suzuka, Japan",
     "Adelaide, Australia",
     "?"
};

/*---------------------------------------------------------------------------
** Functions
*/

int
main(
     int  argc,
     char **argv
) {
     FILE           *fp;
     unsigned short i;
     unsigned short n;
     char           *p;

     assert(sizeof(PER_CAR) == 0xc0);

     if (argc != 2) {
          (void) printf("Usage: savinf (saved game filename).\n");
          return 1;
     }

     if ((fp = fopen(argv[1], "rb")) == NULL) {
          (void) printf("savinf: Unable to open file '%s'\n", argv[1]);
          return 1;
     }

     if (fread(&sg, sizeof(sg), 1, fp) != 1) {
          (void) printf("savinf: Unable to read enough data from file '%s'\n", argv[1]);
          return 1;
     }
     (void) fclose(fp);

     /*<TK >*//* check game CRC */

     assert(sg.multiplayer_num_turns >= 2);
     (void) printf("Multi-player number of turns  : %u\n", sg.multiplayer_num_turns);

     assert(sg.opposition_standard <= 4);
     (void) printf("Opposition standard           : %s\n", op_std_text[sg.opposition_standard]);

     assert(sg.opposition_spread <= 2);
     (void) printf("Opposition spread             : %s\n", op_sprd_text[sg.opposition_spread]);

     assert(sg.race_distance <= 100);
     (void) printf("Race distance                 : %u%%\n", sg.race_distance);

     assert(sg.qualifying_period_conf <= 120);
     (void) printf("Qualifying period             : %um\n", sg.qualifying_period_conf);
     
     assert(sg.practice_period_conf <= 120);
     (void) printf("Practice period               : %um\n", sg.practice_period_conf);

     assert(sg.frames_per_second >= 0x0c && sg.frames_per_second <= 0x25);
     (void) printf("Frames per second (FPS)       : %2.1f\n", fps[sg.frames_per_second - 0x0c]);

     assert(sg.circuit <= 15);     
     (void) printf("Circuit                       : %s\n", circuit[sg.circuit]);

     (void) printf("Current race lap              : %u\n", sg.current_race_lap);

     (void) printf("Cars out of pit               : %u\n", sg.cars_out_of_pit);

     (void) printf("Season                        : %s\n", sg.season == 0x02 ? "Championship" : "Non-Championship");
     
#if 1 /*<TK >*/
     for (i = 0; i < 40; i++) {
          assert(sg.qualifying_setup[i].race_tyres <= 5);

          (void) printf("Qualifying setup for car #%2u  : %02uF/%02uR  %c  %2u,%2u,%2u,%2u,%2u,%2u  %u\n",
                    i + 1,
                    sg.qualifying_setup[i].front_wing,
                    sg.qualifying_setup[i].rear_wing,
                    "ABCDQW"[sg.qualifying_setup[i].race_tyres],
                    sg.qualifying_setup[i].gear1,
                    sg.qualifying_setup[i].gear2,
                    sg.qualifying_setup[i].gear3,
                    sg.qualifying_setup[i].gear4,
                    sg.qualifying_setup[i].gear5,
                    sg.qualifying_setup[i].gear6,
                    sg.qualifying_setup[i].brake_bias
               );
     }

     for (i = 0; i < 40; i++) {
          assert(sg.race_setup[i].race_tyres <= 5);

          (void) printf("Race setup for car #%2u        : %02uF/%02uR  %c  %2u,%2u,%2u,%2u,%2u,%2u  %u\n",
                    i + 1,
                    sg.race_setup[i].front_wing,
                    sg.race_setup[i].rear_wing,
                    "ABCDQW"[sg.race_setup[i].race_tyres],
                    sg.race_setup[i].gear1,
                    sg.race_setup[i].gear2,
                    sg.race_setup[i].gear3,
                    sg.race_setup[i].gear4,
                    sg.race_setup[i].gear5,
                    sg.race_setup[i].gear6,
                    sg.race_setup[i].brake_bias
               );
     }
#endif
     (void) printf("Session time remaining        : %c%c%cm\n",
                    (sg.session_time_remaining[1] & 0x0f) + '0',
                    (sg.session_time_remaining[0] >> 4)   + '0',
                    (sg.session_time_remaining[0] & 0x0f) + '0'
               );
     
     for (i = 0; i < 40; i++) {
          n = (sg.fastest_lap_position[i] & 0x3f);
          if (n) {
               (void) printf("Session position              : %2u. #%2u %-24s %6lums (%08lxH)\n",
                         i + 1,
                         n,
                         sg.driver_names[n - 1],
                         sg.fastest_lap_time[n - 1],
                         sg.fastest_lap_time[n - 1]
                    );
          }
     }

     for (i = 0; i < 40; i++) {
          n = (sg.fastest_lap_position[i] & 0x3f);
          if (n) {
               (void) printf("Qualifying grid               : %2u. #%2u %-24s %6lums (%08lxH)\n",
                         i + 1,
                         n,
                         sg.driver_names[n - 1],
                         sg.qualifying_grid_time[n - 1],
                         sg.qualifying_grid_time[n - 1]
                    );
          }
     }

     for (i = 0; i < 26; i++) {
          (void) printf("race_positions %2u : %u\n",
                    i + 1,
                    sg.race_positions[i] & 0x3f
               );
     }

     for (i = 0; i < 40; i++) {
          (void) printf("grid_position %2u : %u\n",
                    i + 1,
                    sg.grid_position[i] & 0x3f
               );
     }

     for (i = 0; i < 40; i++) {
          (void) printf("race_duration %2u : %6lu (%08lxH)\n",
                    i + 1,
                    sg.race_duration[i],
                    sg.race_duration[i]
               );
     }
     
     switch (sg.session_type & ST_MASK) {
     case ST_PRACTICE:
          p = "Practice";
          break;

     case ST_FREE_PRACTICE:
          p = "Free Practice";
          (void) printf("Session type: %s\n", p);
          for (i = 0; i < 40; i++) {
               n = (sg.fastest_lap_position[i] & 0x3f);
               if (n) {
                    (void) printf("%2u. #%2u %-24s %6lums (%08lxH)\n",
                              i + 1,
                              n,
                              sg.driver_names[n - 1],
                              sg.fastest_lap_time[n - 1],
                              sg.fastest_lap_time[n - 1]
                         );
               }
          }
          break;

     case ST_QUALIFYING:
          p = "Qualifying";
          (void) printf("Session type: %s\n", p);
          for (i = 0; i < 40; i++) {
               n = (sg.fastest_lap_position[i] & 0x3f);
               if (n) {
                    (void) printf("%2u. #%2u %-24s %6lums (%08lxH)\n",
                              i + 1,
                              n,
                              sg.driver_names[n - 1],
                              sg.fastest_lap_time[n - 1],
                              sg.fastest_lap_time[n - 1]
                         );
               }
          }
          break;

     case ST_PRE_RACE_PRACTICE:
          p = "Pre-Race Practice";
          break;

     case ST_RACE:
          p = "Race";
          (void) printf("Session type: %s\n", p);
          for (i = 0; i < 26; i++) {
               n = (sg.race_positions[i] & 0x3f);
               if (n) {
                    (void) printf("%2u. #%2u %-24s %6lums (%08lxH)\n",
                              i + 1,
                              n,
                              sg.driver_names[n - 1],
                              sg.race_duration[n - 1],
                              sg.race_duration[n - 1]
                         );
               }
          }
          break;

     default:
          p = "?";
          break;
     }

     (void) printf("Session type: %s\n", p);

     return 0;
}

/*---------------------------------------------------------------------------
** End of File
*/

