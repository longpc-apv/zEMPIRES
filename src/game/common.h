
/*                                          __
 *         ____      __    ___ ___   _____ /\_\  _ __    __    ____
 *        /\_ ,`\  /'__`\/' __` __`\/\ '__`\/\ \/\`'__\/'__`\ /',__\
 *        \/_/  /_/\  __//\ \/\ \/\ \ \ \L\ \ \ \ \ \//\  __//\__, `\
 *          /\____\ \____\ \_\ \_\ \_\ \ ,__/\ \_\ \_\\ \____\/\____/
 *          \/____/\/____/\/_/\/_/\/_/\ \ \/  \/_/\/_/ \/____/\/___/
 *                                     \ \_\
 *                                      \/_/
 *
 *      Open source reimplementation of the Genie engine.
 *      By Andre Caceres Carrilho
 *
 *      Engine general code and functionality
 *
 *      See license.md for copyright information.
 */

#ifndef GN_COMMON_H
#define GN_COMMON_H
#pragma once

#include "config.h"

#define GN_MAX(x, y)        ((x > y) ? (x) : (y))
#define GN_MIN(x, y)        ((x < y) ? (x) : (y))

#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t Gn_Byte;

typedef struct
  {
    int16_t x, y;
  }
Gn_Point;

#endif /* !GN_COMMON_H */
