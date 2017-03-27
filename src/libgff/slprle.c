
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
 *      Genie engine sprites (SLP) decoding (run-length)
 *      Credits: Format description found on the web (unknown authors)
 *
 *      See license.md for copyright information.
 */

#include "sprite.h"

/*
 * Gn_ParseScanLineSLP ()
 * Parses a scan line from a SLP frame. Returns the amout of decoded bytes.
 * See: https://github.com/Tapsa/genieutils
 */
int32_t
Gn_ParseScanLineSLP (uint8_t *src, uint8_t *dst)
{
  uint8_t *beg = dst;
  uint8_t dat, cmd, sub, nxt, len, player;

  player = 1; /** FIXME!! */

  do
    {
      dat = *src++;
      cmd = dat & 0x0f;
      sub = dat & 0xf0;

      switch (cmd)
        {
          case 0x00: /* Lesser block copy */
          case 0x04: /* Up to 64 pixels */
          case 0x08:
          case 0x0c:
            len = (dat & 0xfc) >> 2;
            while (len--) { *dst++ = *src++; }
          break;

          case 0x01: /* Lesser skip (making pixels transparent) */
          case 0x05: /* Up to 64 pixels */
          case 0x09:
          case 0x0d:
            len = (dat & 0xfc) >> 2;
            dst += len;
          break;

          case 0x02: /* Greater block copy */
            len = (sub << 4) + *src++;
            while (len--) { *dst++ = *src++; }
          break;

          case 0x03: /* Greater skip */
            len = (sub << 4) + *src++;
            dst += len;
          break;

          case 0x06:
            len = (dat & 0xf0) >> 4;
            if (!len) len = *src++;
            while (len--)
               *dst++ = *src++ + (player << 4);
          break;

          case 0x07:
            len = (dat & 0xf0) >> 4;
            if (!len) len = *src++;
            nxt = *src++;
            while (len--) { *dst++ = nxt; }
          break;

          case 0x0a:
            len = (dat & 0xf0) >> 4;
            if (!len) len = *src++;
            nxt = *src++ + (player << 4);
            while (len--) { *dst++ = nxt; }
          break;

          case 0x0b:
            len = (dat & 0xf0) >> 4;
            if (!len) len = *src++;
            while (len--) { *dst++ = 0; }
          break;

          /* Extended commands */
          case 0x0e:
            switch (dat)
              {
                case 0x0e:
                case 0x1e:
                  printf("command not fully implemented\n");
                break;

                case 0x2e:
                case 0x3e:
                  printf("command not fully implemented\n");
                break;

                case 0x4e:
                case 0x5e:
                  *dst++ = 0;
                break;

                case 0x6e:
                case 0x7e:
                  len = *src++;
                  while (len--) *dst++ = 0;
                break;
              }
          break;
        }
    }
  while (cmd != 0x0f);
  return dst - beg;
}

void
Gn_ParseFrameSLP (Gn_FrameSLP * pSlpFrame, Gn_MemoryBuffer * pMemBuf)
{
  int32_t i;
  uint8_t *src, *dst;

  dst = pSlpFrame->decoded;
  src = pMemBuf->p_data + pMemBuf->n_curr;

  /* For each scanline */
  for (i = 0; i < pSlpFrame->header.size_h; ++i)
    {
      /*
       * If the right or left value is 0x8000, the row is completely
       * transparent. Note that there are no command bytes for these
       * rows, so will have to be skipped "manually".
       */
      /**
        * FIXME: Acho que tem que ler 1 byte mesmo se for linha transparente
        */
      if (pSlpFrame->outline[i].dL == 0x8000
       || pSlpFrame->outline[i].dR == 0x8000)
        {
          dst += pSlpFrame->header.size_w;
          continue;
        }

      Gn_MemBufSeek (pMemBuf, pSlpFrame->command[i], SEEK_SET);
      src = pMemBuf->p_data + pMemBuf->n_curr;

      dst += pSlpFrame->outline[i].dL;
      dst += Gn_ParseScanLineSLP (src, dst);
      dst += pSlpFrame->outline[i].dR;
    }
}

