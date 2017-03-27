
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

/*
 * History
 *
 * ??-???-99
 *    Created: Initial code by Derek Liauw Kie Fa.
 *
 * ??-???-99
 *    Modifications for Allegro 3.9+ compatibility by Robert J Ohannessian.
 *
 * 18-mar-17
 *    Code cleanup and adapted for zEMPIRES project by Andre Caceres Carrilho.
 */

#include "2xsai.h"

static uint32_t mask_col = 0xF7DEF7DE;
static uint32_t mask_low = 0x08210821;

static uint32_t mask_qcol = 0xE79CE79C;
static uint32_t mask_qlow = 0x18631863;

static uint32_t mask_rb = 0xF81F;   /* red & blue masks */
static uint32_t mask_g = 0x7E0;     /* green mask */

static int32_t mask_pixels = 2;
static int32_t xsai_depth = 0;

#define GET_RESULT(A, B, C, D)                                                \
  ((A != C || A != D) - (B != C || B != D))

#define INTERPOLATE(A, B)                                                     \
  (((A & mask_col) >> 1) + ((B & mask_col) >> 1) + (A & B & mask_low))

#define Q_INTERPOLATE(A, B, C, D)                                             \
  ((A & mask_qcol) >> 2) + ((B & mask_qcol) >> 2) +                           \
  ((C & mask_qcol) >> 2) + ((D & mask_qcol) >> 2) +                           \
  ((((A & mask_qlow) + (B & mask_qlow) +                                      \
     (C & mask_qlow) + (D & mask_qlow)) >> 2) & mask_qlow)

static void
Gn_2xSaI_ex(uint8_t *src, uint32_t src_pitch,
  BITMAP *dst, uint32_t width, uint32_t height)
{
    uint32_t product1a, product1b, product2a, product2b;

    uint32_t x = 0, y = 0;
    uint32_t col[16];

    uint8_t *src_line[4];
    uint8_t *dst_line[2];

    uint16_t *sbp;
    uint32_t *lbp;

    /* Point to the first 3 lines. */
    src_line[0] = src;
    src_line[1] = src;
    src_line[2] = src + src_pitch;
    src_line[3] = src + (src_pitch << 1);

    /* Can we write the results directly? */
    dst_line[0] = dst->line[0];
    dst_line[1] = dst->line[1];

    if (mask_pixels == 2)
      {
        uint16_t *sbp;

        sbp = (uint16_t*)src_line[0];
        col[0] = *sbp;
        col[1] = col[0];
        col[2] = col[0];
        col[3] = col[0];
        col[4] = col[0];
        col[5] = col[0];
        col[6] = *(sbp + 1);
        col[7] = *(sbp + 2);

        sbp = (uint16_t*)src_line[2];
        col[8] = *sbp;
        col[9] = col[8];
        col[10] = *(sbp + 1);
        col[11] = *(sbp + 2);

        sbp = (uint16_t*)src_line[3];
        col[12] = *sbp;
        col[13] = col[12];
        col[14] = *(sbp + 1);
        col[15] = *(sbp + 2);
      }
    else
      {
        uint32_t *lbp;
        lbp = (uint32_t*)src_line[0];
        col[0] = *lbp;
        col[1] = col[0];
        col[2] = col[0];
        col[3] = col[0];
        col[4] = col[0];
        col[5] = col[0];
        col[6] = *(lbp + 1);
        col[7] = *(lbp + 2);

        lbp = (uint32_t*)src_line[2];
        col[8] = *lbp;
        col[9] = col[8];
        col[10] = *(lbp + 1);
        col[11] = *(lbp + 2);

        lbp = (uint32_t*)src_line[3];
        col[12] = *lbp;
        col[13] = col[12];
        col[14] = *(lbp + 1);
        col[15] = *(lbp + 2);
      }

    for (y = 0; y < height; y++)
      {
        for (x = 0; x < width; x++)
          {

            /*
             * --------------------------
             * B0 B1 B2 B3    0  1  2  3
             * 4  5* 6  S2 -> 4  5* 6  7
             * 1  2  3  S1    8  9 10 11
             * A0 A1 A2 A3   12 13 14 15
             * --------------------------
             */
            if (col[9] == col[6] && col[5] != col[10])
              {
                product2b = col[9];
                product1b = product2b;
              }
            else if (col[5] == col[10] && col[9] != col[6])
              {
                product2b = col[5];
                product1b = product2b;
              }
            else if (col[5] == col[10] && col[9] == col[6])
              {
                int r = 0;

                r += GET_RESULT(col[6], col[5], col[8], col[13]);
                r += GET_RESULT(col[6], col[5], col[4], col[1]);
                r += GET_RESULT(col[6], col[5], col[14], col[11]);
                r += GET_RESULT(col[6], col[5], col[2], col[7]);

                if (r > 0)
                    product1b = col[6];
                else if (r < 0)
                    product1b = col[5];
                else
                    product1b = INTERPOLATE(col[5], col[6]);

                product2b = product1b;
              }
            else
              {
                if (col[6] == col[10] && col[10] == col[13]
                 && col[9] != col[14] && col[10] != col[12])
                  product2b = Q_INTERPOLATE(col[10], col[10], col[10], col[9]);
                else if (col[5] == col[9] && col[9] == col[14]
                      && col[13] != col[10] && col[9] != col[15])
                  product2b = Q_INTERPOLATE(col[9], col[9], col[9], col[10]);
                else
                  product2b = INTERPOLATE(col[9], col[10]);

                if (col[6] == col[10] && col[6] == col[1]
                 && col[5] != col[2] && col[6] != col[0])
                  product1b = Q_INTERPOLATE(col[6], col[6], col[6], col[5]);
                else if (col[5] == col[9] && col[5] == col[2]
                      && col[1] != col[6] && col[5] != col[3])
                  product1b = Q_INTERPOLATE(col[6], col[5], col[5], col[5]);
                else
                  product1b = INTERPOLATE(col[5], col[6]);
              }

            if (col[5] == col[10] && col[9] != col[6]
             && col[4] == col[5] && col[5] != col[14])
                product2a = INTERPOLATE(col[9], col[5]);
            else if (col[5] == col[8] && col[6] == col[5]
                  && col[4] != col[9] && col[5] != col[12])
                product2a = INTERPOLATE(col[9], col[5]);
            else
                product2a = col[9];

            if (col[9] == col[6] && col[5] != col[10]
             && col[8] == col[9] && col[9] != col[2])
                product1a = INTERPOLATE(col[9], col[5]);
            else if (col[4] == col[9] && col[10] == col[9]
                  && col[8] != col[5] && col[9] != col[0])
                product1a = INTERPOLATE(col[9], col[5]);
            else
                product1a = col[5];

            if (mask_pixels == 2)
              {
                *((uint32_t *) (&dst_line[0][x * 4])) =
                  product1a | (product1b << 16);
                *((uint32_t *) (&dst_line[1][x * 4])) =
                  product2a | (product2b << 16);
              }
            else
              {
                *((uint32_t *) (&dst_line[0][x * 8])) = product1a;
                *((uint32_t *) (&dst_line[0][x * 8 + 4])) = product1b;
                *((uint32_t *) (&dst_line[1][x * 8])) = product2a;
                *((uint32_t *) (&dst_line[1][x * 8 + 4])) = product2b;
              }

            /* Move col matrix forward */
            col[0] = col[1];
            col[4] = col[5];
            col[8] = col[9];
            col[12] = col[13];

            col[1] = col[2];
            col[5] = col[6];
            col[9] = col[10];
            col[13] = col[14];

            col[2] = col[3];
            col[6] = col[7];
            col[10] = col[11];
            col[14] = col[15];

            if (x < width - 3)
              {
                x += 3;

                if (mask_pixels == 2)
                  {
                    col[3] = *(((uint16_t*)src_line[0]) + x);
                    col[7] = *(((uint16_t*)src_line[1]) + x);
                    col[11] = *(((uint16_t*)src_line[2]) + x);
                    col[15] = *(((uint16_t*)src_line[3]) + x);
                  }
                else
                  {
                    col[3] = *(((uint32_t*)src_line[0]) + x);
                    col[7] = *(((uint32_t*)src_line[1]) + x);
                    col[11] = *(((uint32_t*)src_line[2]) + x);
                    col[15] = *(((uint32_t*)src_line[3]) + x);
                  }
                x -= 3;
              }
          }

        /* We're done with one line, so we shift the source lines up */
        src_line[0] = src_line[1];
        src_line[1] = src_line[2];
        src_line[2] = src_line[3];

        /* Read next line */
        if (y + 3 >= height)
            src_line[3] = src_line[2];
        else
            src_line[3] = src_line[2] + src_pitch;

        /* Then shift the col matrix up */
        if (mask_pixels == 2)
          {
            sbp = (uint16_t*)src_line[0];
            col[0] = *sbp;
            col[1] = col[0];
            col[2] = *(sbp + 1);
            col[3] = *(sbp + 2);

            sbp = (uint16_t*)src_line[1];
            col[4] = *sbp;
            col[5] = col[4];
            col[6] = *(sbp + 1);
            col[7] = *(sbp + 2);

            sbp = (uint16_t*)src_line[2];
            col[8] = *sbp;
            col[9] = col[9];
            col[10] = *(sbp + 1);
            col[11] = *(sbp + 2);

            sbp = (uint16_t*)src_line[3];
            col[12] = *sbp;
            col[13] = col[12];
            col[14] = *(sbp + 1);
            col[15] = *(sbp + 2);
          }
        else
          {
            lbp = (uint32_t*)src_line[0];
            col[0] = *lbp;
            col[1] = col[0];
            col[2] = *(lbp + 1);
            col[3] = *(lbp + 2);

            lbp = (uint32_t*)src_line[1];
            col[4] = *lbp;
            col[5] = col[4];
            col[6] = *(lbp + 1);
            col[7] = *(lbp + 2);

            lbp = (uint32_t*)src_line[2];
            col[8] = *lbp;
            col[9] = col[9];
            col[10] = *(lbp + 1);
            col[11] = *(lbp + 2);

            lbp = (uint32_t*)src_line[3];
            col[12] = *lbp;
            col[13] = col[12];
            col[14] = *(lbp + 1);
            col[15] = *(lbp + 2);
          }

        if (y < height - 1)
          {
            dst_line[0] = dst->line[y * 2 + 2];
            dst_line[1] = dst->line[y * 2 + 3];
          }
    }
}

int32_t
Gn_Init2xSaI(int32_t depth)
{
  int32_t minr = 0, ming = 0, minb = 0, i = 0;

  if (depth != 15 && depth != 16 && depth != 24 && depth != 32)
    return -1;

  /* Get lowest col bit */
  for (; i < 0xff; i++)
    {
      if (!minr)
          minr = makecol(i, 0, 0);
      if (!ming)
          ming = makecol(0, i, 0);
      if (!minb)
          minb = makecol(0, 0, i);
    }

  mask_col = (makecol_depth(depth, 0xff, 0, 0) - minr)
           | (makecol_depth(depth, 0, 0xff, 0) - ming)
           | (makecol_depth(depth, 0, 0, 0xff) - minb);

  mask_low = minr | ming | minb;

  mask_qcol = (makecol_depth(depth, 0xff, 0, 0) - 3 * minr)
            | (makecol_depth(depth, 0, 0xff, 0) - 3 * ming)
            | (makecol_depth(depth, 0, 0, 0xff) - 3 * minb);

  mask_qlow = (minr * 3) | (ming * 3) | (minb * 3);

  mask_rb = makecol_depth(depth, 0xff, 0, 0xff);
  mask_g = makecol_depth(depth, 0, 0xff, 0);

  mask_pixels = (depth <= 16) ? 2 : 1;

  if (mask_pixels == 2)
    {
      mask_col  |= (mask_col  << 16);
      mask_qcol |= (mask_qcol << 16);
      mask_low  |= (mask_low  << 16);
      mask_qlow |= (mask_qlow << 16);
    }

  xsai_depth = depth;
  return 0;
}

void
Gn_2xSaI(BITMAP *src, BITMAP *dst)
{
  int32_t sbpp, dbpp, pitch;

  if (!src || !dst)
    return;

  sbpp = bitmap_color_depth (src);
  dbpp = bitmap_color_depth (dst);

  /* Must be same col depth */
  if ((sbpp != xsai_depth) || (sbpp != dbpp))
    return;

  if (dst->w < 4 || dst->h < 4)
    {
      /* Image is too small to be 2xSaI'ed. */
      stretch_blit(src, dst, 0, 0, src->w, src->h, 0, 0, dst->w, dst->h);
      return;
    }

  pitch = (uint32_t)(src->line[1] - src->line[0]);
  Gn_2xSaI_ex(src->line[0], pitch, dst, src->w, src->h);
}
