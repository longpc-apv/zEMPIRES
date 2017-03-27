
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
 *      2x Scale and Interpolation (SaI) engine (magnification technique)
 *      Credits: Derek Liauw Kie Fa (aka Kreed), Robert J Ohannessian, and Peter Wang
 *
 *      See license.md for copyright information.
 */

#ifndef GN_2XSAI_H
#define GN_2XSAI_H

#include "common.h"

int32_t Gn_Init2xSaI (int32_t depth);
void Gn_2xSaI (BITMAP *src, BITMAP *dst);

#endif /* !GN_2XSAI_H */
