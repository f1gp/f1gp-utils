/*
** File   : crypt.c
** Author : TK 
** Date   : 11/04/94
**
** $Header:   F:/tk/gplaptim/gplaptim/vcs/crypt.c__   1.2   02 Jul 1995 16:44:18   tk  $
**
** 
*/

/*---------------------------------------------------------------------------
** Includes
*/

/*lint -elib(???) */
#include <stdio.h>
/*lint +elib(???) */

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define VERSION     "V1.0 11/04/94"

#define TRUE        1
#define FALSE       0

#define KEY_INDEX   (0x3577UL + 0x0066UL)

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef int bool;

/*---------------------------------------------------------------------------
** Local function prototypes
*/

/*---------------------------------------------------------------------------
** Data
*/

/*lint -esym(528,sccsid) */
static const char sccsid[] = "@(#)" __FILE__ "\t" VERSION;

/*
** Pattern must not contain a replication of the first character.
*/
static unsigned long pattern = 0x12345678U;

/*---------------------------------------------------------------------------
** Functions
*/

int
main(
     int  argc,
     char **argv
) {
     FILE           *fp_dst;
     FILE           *fp_key;
     char           *key_filename  = argv[1];
     char           *dst_filename  = argv[2];
     unsigned char  *p_pattern     = (unsigned char *) &pattern;
     long           pos_start      = 0;
     long           pos_end        = 0;
     unsigned int   match_index    = 0;
     bool           matched1       = FALSE;
     bool           matched2       = FALSE;
     int            ch;
     unsigned char  c;

     if (argc != 3) {
          (void) fprintf(stderr, "Usage: crypt (key filename) (target filename)\n");
          (void) fprintf(stderr, "       File must contain the pattern: %08lxH\n", pattern);
          return -1;
     }

     if ((fp_key = fopen(key_filename, "rb")) == NULL) {
          (void) fprintf(stderr, "crypt: unable to open file '%s'\n", key_filename);
          return -1;
     }

     if ((fp_dst = fopen(dst_filename, "rb+")) == NULL) {
          (void) fprintf(stderr, "crypt: unable to open file '%s'\n", dst_filename);
          return -1;
     }

     while ((ch = fgetc(fp_dst)) != EOF) {
          c = (unsigned char) ch;

          if (!matched1) {
               if (c == p_pattern[0]) {
                    match_index = 1;
               }
               else if (c == p_pattern[match_index]) {
                    if (++match_index >= sizeof(pattern)) {
                         matched1       = TRUE;
                         pos_start      = ftell(fp_dst) - sizeof(pattern);
                    }
               }
               else {
                    match_index = 0;
               }
          }

          if (matched1 && !matched2) {
               if (c == p_pattern[0]) {
                    match_index = 1;
               }
               else if (c == p_pattern[match_index]) {
                    if (++match_index >= sizeof(pattern)) {
                         matched2       = TRUE;
                         pos_end        = ftell(fp_dst);
                         break;
                    }
               }
               else {
                    match_index = 0;
               }
          }
     }

     if (matched1 && matched2) {
          long           pos_key;
          unsigned char  k;
          unsigned char  v;

          for (pos_key = KEY_INDEX; pos_start < pos_end; pos_start++) {
               (void) fseek(fp_key, pos_key, SEEK_SET);
               (void) fseek(fp_dst, pos_start, SEEK_SET);
               
               (void) fread(&v, 1, 1, fp_dst);
               (void) fread(&k, 1, 1, fp_key);

               (void) printf("Key %06lxH=%02xH  Data %06lxH=%02xH\n",
                              pos_key, k, pos_start, v
                         );
               pos_key += (k & 0x03) + 1;

               v ^= k;
               (void) fseek(fp_dst, pos_start, SEEK_SET);
               (void) fwrite(&v, 1, 1, fp_dst);
          }
     }
     else {
          (void) fprintf(stderr, "crypt: unable to locate pattern(s).\n");
     }

     (void) fclose(fp_dst);
     return 0;
}

/*---------------------------------------------------------------------------
** End of File
*/

