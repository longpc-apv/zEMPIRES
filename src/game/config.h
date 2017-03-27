
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
 *      Engine build configuration macros
 *
 *      See license.md for copyright information.
 */

#ifndef GN_CONFIG_H
#define GN_CONFIG_H
#pragma once

#define GN_BUILD_DEBUG              0x01
#define GN_BUILD_RELEASE            0x02
#define GN_BUILD_TARGET             GN_BUILD_DEBUG

/*
 * Enable file checks on filesystem routines. If it is possible, the code will
 * verify file name extensions, perform the Cyclic Redundancy Check (CRC-32)
 * on memory buffers, validate offsets for reading and other stuff. Can slow-
 * down the loading times (although negligible on modern hardware).
 */
#define GN_FILECHECK

/*
 * Vanilla build stands for the "original" game, i.e. without any modifications
 * While the "improved" version has all of the project newer features.
 */
#define GN_BUILD_VANILLA            0x01
#define GN_BUILD_IMPROVE            0x02
#define GN_BUILD                    GN_BUILD_VANILLA

/*
 * Pathfinding heuristics. It uses euclidian distance sqrt(dx^2 + dy^2) by
 * default. The manhattan distance compute "L" shaped paths while the
 * euclidian has more straight paths.
 */
#define GN_HEURISTIC_EUCLIDIAN      0x01
#define GN_HEURISTIC_MANHATTAN      0x02
#define GN_HEURISTIC                GN_HEURISTIC_EUCLIDIAN

/*
 * Which version of the engine should we build? The data resource files for the
 * Star Wars: Galactic Battlegrounds have different headers sizes. These macros
 * provide the interoperability between the AoE and SW games.
 */
#define GN_GAMEBUILD_AOEX           0x01
#define GN_GAMEBUILD_SWGB           0x02
#define GN_GAMEBUILD                GN_GAMEBUILD_AOEX

/*
 * If enabled and the compiler (currently only for GCC) and current platform
 * supports the assembly opcodes, the code will automaticaly switch to the SSE
 * version on some critical functions
 */
#define GN_ENABLE_SSE

/*
 * This flag is used to configure the path-finding system at compiler time.
 * If limited, the stacks used for open and closed lists will have a maximum
 * number of elements configured to avoid "hiccups" at the frame-rate.
 */
#define GN_PATH_LIMIT

/*
 * Maxmimum number of selectable units (to assign a target position or task).
 * This directly impacts the game performance, as the amout of queued path-
 * finding jobs are proportional to this value
 */
#define GN_SELECT_LIMIT             16

/* Default application window parameters */
#define GN_RESW                     640
#define GN_RESH                     480
#define GN_BPP                      16

/*
 * Macro configuration post-handling (to ensure that the selected values are
 * not stupid enough to break the compilation process).
 */

#if GN_BUILD_TARGET == GN_BUILD_DEBUG
# define GN_DEBUG
#endif

#if defined(__GNUC__) && defined(GN_ENABLE_SSE)
/* Inline SSE code only available for GCC and MinGW */
# define GN_HAVE_SSE
/* 4 bytes on x86 and 8 bytes on x86_64, although
 * this is not true for every machine */
# define GN_POINTER_SIZE            4
#endif

#if GN_GAMEBUILD == GN_GAMEBUILD_AOEX
# define GN_DRS_COPYRIGHT             0x28
# define GN_APPTITLE                  "Age of Empires"
#elif GN_GAMEBUILD == GN_GAMEBUILD_SWGB
# define GN_DRS_COPYRIGHT             0x3c
# define GN_APPTITLE                  "Star Wars"
#else
# error "Invalid GN_GAMEBUILD macro"
#endif

#endif /* !GN_CONFIG_H */
