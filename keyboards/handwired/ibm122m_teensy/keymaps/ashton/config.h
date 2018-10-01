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

#pragma once

// Special case for C-M-n ghosting. Mask off the ghosted h key.
#define GHOST_HANDLING() {                      \
    if ((matrix_debouncing[7] & (1 << 8)) \
        && (matrix_debouncing[0] & 1) \
        && (matrix_debouncing[7] & 1)) { \
        matrix[0] &= ~(1 << 8);          \
    } \
}
