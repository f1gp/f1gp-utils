LED_COLIDX              equ     0
LED_ROWIDX              equ     1
LED_COLSUB              equ     2
LED_ROWSUB              equ     3
LED_TRANSPARENT         equ     10
LED_COLOR_BG            equ     11
LED_COLOR_FG            equ     12

COLOR_BLACK             equ     0
COLOR_LGRAY             equ     1
COLOR_RED               equ     2
COLOR_YELLOW            equ     3
COLOR_LGREEN            equ     4
COLOR_PURPLE            equ     5
COLOR_LBLUE             equ     6
COLOR_GRAY              equ     7
COLOR_WHITE             equ     8
COLOR_LRED              equ     9
COLOR_DGRAY             equ     10
COLOR_GREEN             equ     11
COLOR_L2GRAY            equ     12
COLOR_L3GRAY            equ     13
COLOR_GRGREEN           equ     18
COLOR_CPGREEN           equ     153

global               gp_hex2bcd:dword
global               gp_lcd_init:dword
global               gp_render_char:dword
global               gp_led_pos_ptr:dword

global               save_led_render_state:near
global               restore_led_render_state:near
global               lcd_init:near
global               render_cockpit_char:near
global               render_hex_word:near
global               render_hex_byte:near
global               render_dec_word:near
global               render_dec_byte:near
global               render_nibble:near
