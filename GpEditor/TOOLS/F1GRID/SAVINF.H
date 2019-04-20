/*
** File   : savinf.h
** Author : TK
** Date   : 24/10/94
**
** $Header$
**
** MPS Grand Prix Saved game file info.
*/

#ifndef   savinf_h
#define   savinf_h

/*---------------------------------------------------------------------------
** Includes
*/

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define ST_PRACTICE           0x00           /* Practice menu option       */
#define ST_FREE_PRACTICE      0x01           /* Race menu - Free Practice  */
#define ST_QUALIFYING         0x40           /* Race menu - Qualifying     */
#define ST_PRE_RACE_PRACTICE  0x02           /* Race menu - Pre-Race Pract */
#define ST_RACE               0x80           /* Race or Quick Race         */
#define ST_MASK               (ST_PRACTICE | ST_FREE_PRACTICE | ST_QUALIFYING | ST_PRE_RACE_PRACTICE | ST_RACE)

#define SAVED_GAME_SIZE       26736

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef unsigned char    byte;
typedef unsigned short   word;
typedef unsigned long    dword;

typedef struct {
     unsigned char  front_wing;
     unsigned char  rear_wing;
     unsigned char  gear1;
     unsigned char  gear2;
     unsigned char  gear3;
     unsigned char  gear4;
     unsigned char  gear5;
     unsigned char  gear6;
     unsigned char  race_tyres;
     unsigned char  brake_bias;
} CAR_SETUP;

typedef struct {
     byte      u0x00;
     byte      u0x01;
     byte      u0x02;
     byte      u0x03;
     byte      u0x04;
     byte      u0x05;
     byte      u0x06;
     byte      u0x07;
     byte      u0x08;
     byte      u0x09;
     byte      u0x0a;
     byte      u0x0b;
     byte      u0x0c;
     byte      u0x0d;
     byte      u0x0e;
     byte      u0x0f;
     word      speed;
     dword     track_segment_ptr;
     byte      u0x16;
     byte      u0x17;
     byte      u0x18;
     byte      u0x19;
     byte      u0x1a;
     byte      u0x1b;
     byte      u0x1c;
     byte      u0x1d;
     byte      u0x1e;
     byte      u0x1f;
     byte      u0x20;
     byte      u0x21;
     byte      lap_number;
     byte      u0x23;
     byte      gear;                         // 0=N,1-6=1st-6th,1=spin,0ffh=reverse
     byte      cars_team_index;
     byte      u0x26;
     byte      u0x27;
     byte      u0x28;
     byte      u0x29;
     byte      u0x2a;
     byte      u0x2b;
     byte      u0x2c;
     byte      u0x2d;
     byte      u0x2e;
     byte      u0x2f;
     byte      u0x30;
     byte      u0x31;
     byte      u0x32;
     byte      u0x33;
     byte      u0x34;
     byte      u0x35;
     byte      u0x36;
     byte      u0x37;
     byte      u0x38;
     byte      u0x39;
     byte      u0x3a;
     byte      u0x3b;
     byte      u0x3c;
     byte      u0x3d;
     byte      u0x3e;
     byte      u0x3f;
     dword     last_lap_time;
     byte      u0x44;
     byte      u0x45;
     word      tyre_grip;
     byte      u0x48;
     byte      u0x49;
     byte      u0x4a;
     byte      u0x4b;
     byte      u0x4c;
     byte      u0x4d;
     byte      u0x4e;
     byte      u0x4f;
     byte      u0x50;
     byte      u0x51;
     byte      u0x52;
     byte      u0x53;
     byte      u0x54;
     byte      u0x55;
     byte      u0x56;
     byte      u0x57;
     byte      u0x58;
     byte      u0x59;
     byte      u0x5a;
     byte      u0x5b;
     byte      u0x5c;
     byte      u0x5d;
     byte      u0x5e;
     byte      u0x5f;
     byte      u0x60;
     byte      u0x61;
     byte      u0x62;
     byte      u0x63;
     byte      u0x64;
     byte      u0x65;
     byte      u0x66;
     byte      u0x67;
     byte      u0x68;
     byte      u0x69;
     byte      u0x6a;
     byte      u0x6b;
     byte      u0x6c;
     byte      u0x6d;
     byte      u0x6e;
     byte      u0x6f;
     byte      u0x70;
     byte      u0x71;
     byte      u0x72;
     byte      u0x73;
     byte      u0x74;
     byte      u0x75;
     byte      u0x76;
     byte      u0x77;
     byte      u0x78;
     byte      u0x79;
     byte      u0x7a;
     byte      u0x7b;
     byte      u0x7c;
     byte      u0x7d;
     byte      u0x7e;
     byte      u0x7f;
     byte      u0x80;
     byte      u0x81;
     byte      u0x82;
     byte      u0x83;
     byte      u0x84;
     byte      u0x85;
     byte      u0x86;
     byte      u0x87;
     byte      u0x88;
     byte      u0x89;
     byte      u0x8a;
     byte      u0x8b;
     byte      u0x8c;
     byte      u0x8d;
     byte      u0x8e;
     byte      u0x8f;
     byte      u0x90;
     byte      u0x91;
     dword     session_duration_timer;
     byte      u0x96;
     byte      u0x97;
     byte      u0x98;
     byte      u0x99;
     byte      u0x9a;
     byte      u0x9b;
     byte      u0x9c;
     byte      u0x9d;
     byte      u0x9e;
     byte      u0x9f;
     byte      u0xa0;
     byte      u0xa1;
     byte      u0xa2;
     byte      u0xa3;
     byte      u0xa4;
     byte      u0xa5;
     byte      u0xa6;
     byte      u0xa7;
     word      acceleration;
     byte      u0xaa;
     byte      u0xab;
     byte      car_number;
     byte      u0xad;
     dword     best_lap_time;
     byte      current_tyres;                     // mask 07h 0=A,1=B,2=C,3=D,4=W,5=Q
     byte      u0xb3;
     byte      u0xb4;
     byte      u0xb5;
     byte      u0xb6;
     byte      u0xb7;
     byte      u0xb8;
     byte      u0xb9;
     byte      u0xba;
     byte      u0xbb;
     byte      u0xbc;
     byte      u0xbd;
     word      braking;
} PER_CAR;

/*
** Size: 26736 bytes
** The start of this structure is equivalent to SS:121ch when GP.EXE is running.
*/
typedef struct {
     byte      u_0000h;                      // 0000h - Always 2ch 00h ?
     byte      u_0001h;                      // 0001h -
     byte      multiplayer_num_turns;        // 0002h - # as per menu
     byte      u_0003h;                      // 0003h - 
     byte      u_0004h;                      // 0004h -
     byte      u_0005h;                      // 0005h -
     byte      opposition_standard;          // 0006h - 0=Rookie,1=Amateur,2=Semi-Pro,3=Pro,4=Ace
     byte      u_0007h;                      // 0007h -
     byte      opposition_spread;            // 0008h - 0=Same,1=1991,2=Random
     byte      u_0009h;                      // 0009h -
     byte      race_distance;                // 000ah - # as per menu
     byte      u_000bh;                      // 000bh -
     byte      qualifying_period_conf;       // 000ch - # as per menu
     byte      u_000dh;                      // 000dh -
     byte      practice_period_conf;         // 000eh - # as per menu
     byte      u_000fh;                      // 000fh -
     byte      u_0010h;                      // 0010h -
     byte      u_0011h;                      // 0011h -
     byte      wet_race;                     // 0012h - =60h wet race (mask?)
     byte      u_0013h;                      // 0013h -
     byte      frames_per_second;            // 0014h - FPS
     byte      u_0015h;                      // 0015h -
     byte      season;                       // 0016h - 02=Season,01=Non-Championship
     byte      u_0017h;                      // 0017h -
     byte      u_0018h;                      // 0018h -
     byte      u_0019h;                      // 0019h -
     byte      circuit;                      // 001ah - # as per menu (right to left then down) 7=UK,8=Ger
     byte      u_001bh[0x07];                // 001bh -
     /*<TK >*/
     byte      current_race_lap;             // 0022h -
     byte      u_0023h;                      // 0023h -
     byte      session_time_remaining[2];    // 0024h - BCD value in word
     byte      u_0026h[0x08];                // 0026h -
     /*<TK >*/
     byte      session_type;                 // 002eh - 01h=Free Practice, 02h=Pre-Race Practice, 40h=Qualifying, 80h=Race, 00h=Practice
     byte      u_002fh[0x2d];                // 002fh -
     /*<TK >*/
     byte      mask_5ch;                     // 005ch - Mask 01h ??
     byte      u_005dh[0x0d];                // 005dh -
     /*<TK >*/
     CAR_SETUP qualifying_setup[40];         // 006ah - 40t * 10t bytes
     byte      u_01fah;                      // 01fah -
     byte      u_01fbh;                      // 01fbh -
     CAR_SETUP race_setup[40];               // 01fch - 40t * 10t bytes
     byte      u_038ch[0x230];               // 038ch -
     /*<TK >*/
     byte      fastest_lap_position[40];     // 05bch - Car Number (ordered by position).
     dword     fastest_lap_time[40];         // 05e4h - Indexed by car number (-1) above.
     dword     qualifying_grid_time[40];     // 0684h - ?
     byte      race_positions[26];           // 0724h -
     byte      grid_position[40];            // 073eh -
     dword     race_duration[40];            // 0766h -
     byte      car_numbers[40];              // 0806h -
     byte      driver_names[40][24];         // 082fh -
     byte      team_names[20][13];           // 0befh -
     byte      engine_names[20][13];         // 0cf2h -
     byte      u_0df6h[0x146];               // 0df6h -
     byte      position[26];                 // 0f3ch
     byte      u_0f56h[0x250];               // 0f56h -
     /*<TK >*/
     PER_CAR   per_car[26];                  // 11a6h -
     byte      u_2526h[0x86e];               // 2526h -
     /*<TK >*/
     byte      cars_out_of_pit;              // 2d94h - (and 2d96?)
     byte      u_2d95h[0x0045];              // 2d95h -
     /*<TK >*/
     dword     current_session_time_ms;      // 2ddah - 
     byte      u_2ddeh[0x3a8e];              // 2ddeh -
     /*<TK >*/
     word      crc1;                         // 686ch
     word      crc2;                         // 686eh
} SAVED_GAME;

/*---------------------------------------------------------------------------
** Data
*/

/*
** FPS
*/
// 25.0 fps= 0xc
// 23.0 fps= 0xd
// 21.4 fps= 0xe
// 20.0 fps= 0xf
// 18.7 fps=0x10
// 17.6 fps=0x11
// 16.6 fps=0x12
// 15.7 fps=0x13
// 15.0 fps=0x14
// 14.2 fps=0x15
// 13.6 fps=0x16
// 13.0 fps=0x17
// 12.5 fps=0x18
// 12.0 fps=0x19
// 11.5 fps=0x1a
// 11.1 fps=0x1b
// 10.7 fps=0x1c
// 10.3 fps=0x1d
// 10.0 fps=0x1e
//  9.6 fps=0x1f
//  9.3 fps=0x20
//  9.0 fps=0x21
//  8.8 fps=0x22
//  8.5 fps=0x23
//  8.3 fps=0x24
//  8.1 fps=0x25

/*---------------------------------------------------------------------------
** Functions
*/


#endif    /* savinf_h */
/*---------------------------------------------------------------------------
** End of File
*/

