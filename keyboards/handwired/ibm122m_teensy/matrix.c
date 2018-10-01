/* Copyright 2018 Ashton Snelgrove
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <string.h>
#include "hal.h"
#include "timer.h"
#include "wait.h"
#include "print.h"
#include "matrix.h"
#include "debug.h"
#include "board.h"

/* matrix state(1:on, 0:off) */
static matrix_row_t matrix[MATRIX_ROWS];
static matrix_row_t matrix_debouncing[MATRIX_ROWS];
static bool debouncing = false;
static uint16_t debouncing_time = 0;


#define palStrobe(p) { \
    palSetLineMode(p, PAL_MODE_OUTPUT_PUSHPULL); \
    palSetLine(p);                               \
}

void strobeRow(int row)
{
    switch (row) {
    case 0: palStrobe(LINE_PIN20); break;
    case 1: palStrobe(LINE_PIN21); break;
    case 2: palStrobe(LINE_PIN22); break;
    case 3: palStrobe(LINE_PIN23); break;
    case 4: palStrobe(LINE_PIN24); break;
    case 5: palStrobe(LINE_PIN25); break;
    case 6: palStrobe(LINE_PIN26); break;
    case 7: palStrobe(LINE_PIN27); break;
    }
}

#define palUnstrobe(p) { \
    palClearLine(p);                    \
    palSetLineMode(p, PAL_MODE_RESET);  \
}

void unstrobeRow(int row)
{
    switch (row) {
    case 0: palUnstrobe(LINE_PIN20); break;
    case 1: palUnstrobe(LINE_PIN21); break;
    case 2: palUnstrobe(LINE_PIN22); break;
    case 3: palUnstrobe(LINE_PIN23); break;
    case 4: palUnstrobe(LINE_PIN24); break;
    case 5: palUnstrobe(LINE_PIN25); break;
    case 6: palUnstrobe(LINE_PIN26); break;
    case 7: palUnstrobe(LINE_PIN27); break;
    }
}

void matrix_init(void)
{
    debug_enable = true;
    debug_matrix = true;

    /* Row(strobe) set in Hi-Z mode */
    palSetLineMode(LINE_PIN20,  PAL_MODE_RESET);
    palSetLineMode(LINE_PIN21,  PAL_MODE_RESET);
    palSetLineMode(LINE_PIN22,  PAL_MODE_RESET);
    palSetLineMode(LINE_PIN23,  PAL_MODE_RESET);
    palSetLineMode(LINE_PIN24,  PAL_MODE_RESET);
    palSetLineMode(LINE_PIN25,  PAL_MODE_RESET);
    palSetLineMode(LINE_PIN26,  PAL_MODE_RESET);
    palSetLineMode(LINE_PIN27,  PAL_MODE_RESET);

    /* Column(sense) */
    palSetLineMode(LINE_PIN0,  PAL_MODE_INPUT_PULLDOWN);
    palSetLineMode(LINE_PIN1,  PAL_MODE_INPUT_PULLDOWN);
    palSetLineMode(LINE_PIN2,  PAL_MODE_INPUT_PULLDOWN);
    palSetLineMode(LINE_PIN3,  PAL_MODE_INPUT_PULLDOWN);
    palSetLineMode(LINE_PIN4,  PAL_MODE_INPUT_PULLDOWN);
    palSetLineMode(LINE_PIN5,  PAL_MODE_INPUT_PULLDOWN);
    palSetLineMode(LINE_PIN6,  PAL_MODE_INPUT_PULLDOWN);
    palSetLineMode(LINE_PIN7,  PAL_MODE_INPUT_PULLDOWN);
    palSetLineMode(LINE_PIN8,  PAL_MODE_INPUT_PULLDOWN);
    palSetLineMode(LINE_PIN9,  PAL_MODE_INPUT_PULLDOWN);
    palSetLineMode(LINE_PIN10,  PAL_MODE_INPUT_PULLDOWN);
    palSetLineMode(LINE_PIN11,  PAL_MODE_INPUT_PULLDOWN);
    palSetLineMode(LINE_PIN12,  PAL_MODE_INPUT_PULLDOWN);
    palSetLineMode(LINE_PIN13,  PAL_MODE_INPUT_PULLDOWN);
    palSetLineMode(LINE_PIN14,  PAL_MODE_INPUT_PULLDOWN);
    palSetLineMode(LINE_PIN15,  PAL_MODE_INPUT_PULLDOWN);
    palSetLineMode(LINE_PIN16,  PAL_MODE_INPUT_PULLDOWN);
    palSetLineMode(LINE_PIN17,  PAL_MODE_INPUT_PULLDOWN);
    palSetLineMode(LINE_PIN18,  PAL_MODE_INPUT_PULLDOWN);
    palSetLineMode(LINE_PIN19,  PAL_MODE_INPUT_PULLDOWN);

    memset(matrix, 0, MATRIX_ROWS * sizeof(matrix_row_t));
    memset(matrix_debouncing, 0, MATRIX_ROWS * sizeof(matrix_row_t));

    matrix_init_quantum();
}

matrix_row_t scan_columns(void) {
    return
        (palReadLine(LINE_PIN0)) |
        (palReadLine(LINE_PIN1) << 1) |
        (palReadLine(LINE_PIN2) << 2) |
        (palReadLine(LINE_PIN3) << 3) |
        (palReadLine(LINE_PIN4) << 4) |
        (palReadLine(LINE_PIN5) << 5) |
        (palReadLine(LINE_PIN6) << 6) |
        (palReadLine(LINE_PIN7) << 7) |
        (palReadLine(LINE_PIN8) << 8) |
        (palReadLine(LINE_PIN9) << 9) |
        (palReadLine(LINE_PIN10) << 10) |
        (palReadLine(LINE_PIN11) << 11) |
        (palReadLine(LINE_PIN12) << 12) |
        (palReadLine(LINE_PIN13) << 13) |
        (palReadLine(LINE_PIN14) << 14) |
        (palReadLine(LINE_PIN15) << 15) |
        (palReadLine(LINE_PIN16) << 16) |
        (palReadLine(LINE_PIN17) << 17) |
        (palReadLine(LINE_PIN18) << 18) |
        (palReadLine(LINE_PIN19) << 19);
}

uint8_t matrix_scan(void)
{
    for (int row = 0; row < MATRIX_ROWS; row++) {
        matrix_row_t data = 0;
        strobeRow(row);

        // wait to settle pin state
        wait_us(20);

        data = scan_columns();

        unstrobeRow(row);
        wait_us(20);

        if (matrix_debouncing[row] != data) {
            matrix_debouncing[row] = data;
            debouncing = true;
            debouncing_time = timer_read();
        }
    }

    if (debouncing && timer_elapsed(debouncing_time) > DEBOUNCE) {
        for (int r = 0; r < MATRIX_ROWS; r++) {
            matrix[r] = matrix_debouncing[r];
        }
        debouncing = false;
    }

    matrix_scan_quantum();
    return 1;
}

bool matrix_is_on(uint8_t row, uint8_t col)
{
    return (matrix[row] & (1 << col));
}

matrix_row_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

void matrix_print(void)
{
    xprintf("\nr/c 0123456789ABCDEFGHIJ\n");
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        xprintf("%02X: ", row);
        matrix_row_t data = matrix_get_row(row);
        for (int col = 0; col < MATRIX_COLS; col++) {
            if (data & (1<<col))
                xprintf("1");
            else
                xprintf("0");
        }
        xprintf("\n");
    }

}
