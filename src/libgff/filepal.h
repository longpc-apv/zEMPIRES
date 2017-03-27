
#ifndef GN_PALETTE_H
#define GN_PALETTE_H

#include "common.h"
#include "membuf.h"

typedef struct {
  union {
    int32_t n_col;
    uint8_t b_col[4];
  };
} Gn_Color;

typedef struct {
  Gn_Color *p_pixel;
  int32_t n_size;
} Gn_Palette;

/* Parse JASC-PAL files */
Gn_Palette * Gn_OpenFilePAL (Gn_MemoryBuffer * pMemBuf);
void Gn_CloseFilePAL (Gn_Palette * pal);

#endif // GN_PALETTE_H
