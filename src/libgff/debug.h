
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
 *      Most of debugging related code
 *
 *      See license.md for copyright information.
 */

#ifndef GN_DEBUG_H
#define GN_DEBUG_H

#include "common.h"
#include "palette.h"
#include "filesys.h"
#include "sprite.h"

#ifdef GN_DEBUG

#ifdef __cplusplus
extern "C" {
#endif

/* Debugging SLP sprite frames */
int32_t Gn_SaveSpriteFrameBMP (const Gn_FrameSLP *frame,
  const Gn_Palette *pal, const char *filename);
int32_t Gn_SavePaletteBMP (const Gn_Palette *pal, const char *filename);

/* Debugging File system (data resources) */
void Gn_DebugFileSystem (const Gn_FileSystem* pFileSys);

/* .DRS and .SLP file debugging */
void Gn_DebugFileDRS (const Gn_FileDRS *pDrsFile);
void Gn_DebugFileSLP (const Gn_FileSLP *pSlpFile);

#ifdef __cplusplus
}
#endif
#endif /* !GN_DEBUG */
#endif /* !GN_DEBUG_H */
