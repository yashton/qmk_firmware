/*
Copyright 2018 Ashton Snelgrove

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CONFIG_H
#define CONFIG_H

#define PREVENT_STUCK_MODIFIERS

#define VENDOR_ID       0x4153
#define PRODUCT_ID      0x4D4D
#define DEVICE_VER      0x0001
#define MANUFACTURER IBM
#define PRODUCT IBM Model M 122
#define DESCRIPTION Custom QMK Firmware by Ashton

#define MATRIX_ROWS 8
#define MATRIX_COLS 20

#define DIODE_DIRECTION CUSTOM_MATRIX

#define DEBOUNCE    15

/* key combination for command */
#define IS_COMMAND() ( \
    keyboard_report->mods == (MOD_BIT(KC_LSHIFT) | MOD_BIT(KC_RSHIFT)) \
)

/* disable debug print */
//#define NO_DEBUG

/* disable print */
//#define NO_PRINT

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT
//#define NO_ACTION_MACRO
//#define NO_ACTION_FUNCTION

#endif
