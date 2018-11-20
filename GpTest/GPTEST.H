/*
** Test.
*/

#ifndef   gptest_h
#define   gptest_h

/*---------------------------------------------------------------------------
** Includes
*/

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define TRUE                  1
#define FALSE                 0

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef unsigned char    _byte;
typedef unsigned short   _word;
typedef unsigned long    _dword;

/*---------------------------------------------------------------------------
** Data
*/

extern char         unload_flag;
extern char    far  *cmd_line;
extern char         cmd_line_len;
extern char    near *msg_text;
extern char         beep_count;
extern char    far  *fname_ptr;
extern char         limit_cars_race;

/*---------------------------------------------------------------------------
** Functions
*/

#endif    /* gptest_h */
/*---------------------------------------------------------------------------
** End of File
*/
