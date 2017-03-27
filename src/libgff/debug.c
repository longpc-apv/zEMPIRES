
/*
 * debug.c
 * Description: most of debugging related code
 *
 * This file is part of zEMPIRES, an open source reimplementation
 * of Ensemble Studios Genie Engine.
 *
 * Copyright (C) Andre Caceres Carrilho, 2015-2017.
 * Contact: carrilho.acc@gmail.com
 *
 * Microsoft (MS) and Age of Empires (AoE) are trademarks
 * of Microsoft Corporation. All other trademarks are property
 * of their respective owners.
 */

/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "debug.h"
#include "api.h"

#ifdef GN_DEBUG

/* TODO: check for d or d-1 ? Verify */
#define GN_CLIP(x, d)         \
  x = (x <= 0) ? (0) : (x);   \
  x = (x >= d) ? (d) : (x);

/*
 * Gn_RectFill ()
 * Draws a filled rectangle in the RGB pointer with color "col"
 * Coordinates (x1, y1) from (x2, y2) and checked against (w, h)
 * Note: Only used on Gn_ConvertPaletteRGB ()
 */
static void
Gn_RectFill (Gn_Byte *rgb, int32_t w, int32_t h, int32_t x1, int32_t y1,
  int32_t x2, int32_t y2, Gn_Color *col)
{
  int32_t X1, Y1, X2, Y2, x, y;
  Gn_Byte *ptr;

  if (!rgb || !col)
    return;

  /* In the case the calling function misplace the coordinates order */
  X1 = GN_MIN(x1, x2);
  Y1 = GN_MIN(y1, y2);

  X2 = GN_MAX(x1, x2);
  Y2 = GN_MAX(y1, y2);

  /* Avoiding segmentation fault by clipping the rectangle */
  GN_CLIP(X1, w)
  GN_CLIP(X2, w)

  GN_CLIP(Y1, h)
  GN_CLIP(Y2, h)

  for (x = X1; x < X2; ++x)
    {
      ptr = rgb + (x * w * 3) + (Y1 * 3);

      for (y = Y1; y < Y2; ++y)
        {
          *ptr++ = col->b_col[0];
          *ptr++ = col->b_col[1];
          *ptr++ = col->b_col[2];
        }
    }
}

/*
 * Gn_ConvertPaletteRGB ()
 * Convert a 8-bit palette into a 256 x 256 bitmap with 16 x 16 squares
 */
static Gn_Byte*
Gn_ConvertPaletteRGB (const Gn_Palette *pal)
{
  int32_t x, y, x1, y1, x2, y2;

  Gn_Color *col = pal->p_pixel;
  Gn_Byte *rgb = (Gn_Byte *) malloc (0x030000); /* 256x256x3 */

  if (!rgb)
    return GN_NULL;

  for (x = 0, x2 = 0; x < 16; ) {
    x1 = x2;
    x2 = (++x) << 4;
  for (y = 0, y2 = 0; y < 16; ) {
    y1 = y2;
    y2 = (++y) << 4;
    Gn_RectFill(rgb, 256, 256, x1, y1, x2, y2, col++);
  } }

  return rgb;
}

int32_t Gn_SaveSpriteFrameBMP (const Gn_FrameSLP *frame,
  const Gn_Palette *pal, const char *filename)
{
  Gn_Byte *rgb;

  rgb = Gn_ConvertSpriteFrameRGB (frame, pal);

  if (!rgb)
    return GN_FAILURE;

  BITMAP *bmp =
    Gn_BitmapFromRGB(rgb, frame->header.size_w, frame->header.size_h);
  free (rgb);

  save_bitmap(filename, bmp, 0);
  destroy_bitmap(bmp);

  return GN_SUCESS;
}

int32_t
Gn_SavePaletteBMP (const Gn_Palette *pal, const char *filename)
{
  Gn_Byte *rgb;

  rgb = Gn_ConvertPaletteRGB (pal);

  if (!rgb)
    return GN_FAILURE;

  BITMAP *bmp =
    Gn_BitmapFromRGB(rgb, 256, 256);
  free (rgb);

  save_bitmap(filename, bmp, 0);
  destroy_bitmap(bmp);

  return GN_SUCESS;
}

void
Gn_DebugFileSystem (const Gn_FileSystem* pFileSys)
{
  if (!pFileSys)
    return;

  Gn_FileDRS *pCurStk = pFileSys->drs_stack;

  while (pCurStk)
    {
      Gn_DebugFileDRS (pCurStk);
      pCurStk = pCurStk->next;
    }
}

void
Gn_DebugFileDRS (const Gn_FileDRS *pDrsFile)
{
  int32_t i, j, k;
  if (!pDrsFile)    return;

  printf("\n COPYRIGHT: ");
  Gn_PrintStr_n(pDrsFile->header.copyright, GN_DRS_COPYRIGHT);
  printf("\n DRS VERSION: ");
  Gn_PrintStr_n(pDrsFile->header.file_version, 4);
  printf("\n DRS TYPE: ");
  Gn_PrintStr_n(pDrsFile->header.file_type, 12);
  printf("\n TABLE COUNT: %d", pDrsFile->header.table_header_count);
  printf("\n TABLE OFFSET: %d", pDrsFile->header.table_header_offset);

  for (i = 0; i < pDrsFile->header.table_header_count; ++i)
    {
      printf("\n  TABLE [%d]", i+1);
      printf("\n   - FILE EXT: ");
      Gn_PrintStr_n(pDrsFile->tables[i].header.extension, 3);
      printf("\n   - FILE COUNT: %d",
        pDrsFile->tables[i].header.file_header_count);

      printf("\n\t");

      for (j = 0, k = 0; j < pDrsFile->tables[i].header.file_header_count; ++j, ++k)
        {
          if (k > 10) { k = 0; printf("\n\t"); }
          printf("%04d ", pDrsFile->tables[i].entries[j].index);
        }
    }
  printf("\n\n");
}

void
Gn_DebugFileSLP (const Gn_FileSLP *pSlpFile)
{
  /* Finish this function */
  int32_t i;
  Gn_FrameSLP *pSlpFrame;

  printf("Frame count %d\n", pSlpFile->header.frame_count);

  for (i = 0; i < pSlpFile->header.frame_count; ++i)
    {
      pSlpFrame = & pSlpFile->frames[i];

      printf("Frame [%d]\n", i);
      printf(" Offset Cmd: %d \tOut: %d\n",
        pSlpFrame->header.command_offset,
        pSlpFrame->header.outline_offset);
      printf(" Size: %dx%d \t\tCenter: (%d, %d)\n",
        pSlpFrame->header.size_w,
        pSlpFrame->header.size_h,
        pSlpFrame->header.center_x,
        pSlpFrame->header.center_y);
    }
}

#endif /* !GN_DEBUG */
