
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
Gn_ParseScanLineSLP (Gn_Byte *src, Gn_Byte *dst)
{
  Gn_Byte *beg = dst;
  Gn_Byte dat, cmd, sub, nxt, len, player;

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
  Gn_Byte *src, *dst;

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

Gn_FileSLP *
Gn_OpenFileSLP (Gn_MemoryBuffer * pMemBuf)
{
  int32_t i, pixel_count;

  Gn_FrameSLP *pSlpFrame;
  Gn_FileSLP *pSlpFile;

  pSlpFile = (Gn_FileSLP *) malloc (sizeof (Gn_FileSLP));

  if (!pMemBuf || !pSlpFile)
    return GN_NULL;

  /* Reading the SLP file header */
  Gn_MemBufSeek (pMemBuf, 0, SEEK_SET);
  Gn_MemBufRead (& pSlpFile->header, sizeof (Gn_FileHeaderSLP), 1, pMemBuf);

  pSlpFile->frames = (Gn_FrameSLP *) malloc (sizeof(Gn_FrameSLP) *
    pSlpFile->header.frame_count);

  if (!pSlpFile->frames)
    return GN_NULL;

  /*
   * For each frame: read its header (this data chunk starts right after the
   * main SLP file header)
   */
  for (i = 0; i < pSlpFile->header.frame_count; ++i)
    {
      pSlpFrame = & pSlpFile->frames[i];

      Gn_MemBufRead (& pSlpFrame->header,
        sizeof (Gn_FrameHeaderSLP), 1, pMemBuf);

      /* Allocate the command offset and outline arrays */
      pSlpFrame->command = (uint32_t *) malloc
        (pSlpFrame->header.size_h * sizeof (uint32_t));

      pSlpFrame->outline = (Gn_RowEdgeSLP *) malloc
        (pSlpFrame->header.size_h * sizeof (Gn_RowEdgeSLP));

      /* Allocate and decode frame sprite */
      pixel_count = pSlpFrame->header.size_w * pSlpFrame->header.size_h;
      pSlpFrame->decoded = (Gn_Byte*) malloc (sizeof (Gn_Byte) * pixel_count);
      memset (pSlpFrame->decoded, 0, pixel_count);

      if (!pSlpFrame->command || !pSlpFrame->outline || !pSlpFrame->decoded)
        return GN_NULL;
    }

  /* For each frame (again, but need to be after that loop) */
  for (i = 0; i < pSlpFile->header.frame_count; ++i)
    {
      pSlpFrame = & pSlpFile->frames[i];

      /* Seek and Read outlines */
      Gn_MemBufSeek (pMemBuf, pSlpFrame->header.outline_offset, SEEK_SET);
      Gn_MemBufRead (pSlpFrame->outline, sizeof (Gn_RowEdgeSLP),
        pSlpFrame->header.size_h, pMemBuf);

      /* Seek and Read command offsets */
      Gn_MemBufSeek (pMemBuf, pSlpFrame->header.command_offset, SEEK_SET);
      Gn_MemBufRead (pSlpFrame->command, sizeof (uint32_t),
        pSlpFrame->header.size_h, pMemBuf);

      Gn_ParseFrameSLP (pSlpFrame, pMemBuf);
    }

  Gn_MemBufSeek (pMemBuf, 0, SEEK_SET);
  return pSlpFile;
}

void
Gn_CloseFileSLP (Gn_FileSLP *pSlpFile)
{
  int32_t i;

  if (!pSlpFile)
    return;

  for (i = 0; i < pSlpFile->header.frame_count; ++i)
    {
      if ( pSlpFile->frames[i].command
        && pSlpFile->frames[i].outline
        && pSlpFile->frames[i].decoded)
      {
        free (pSlpFile->frames[i].command);
        free (pSlpFile->frames[i].outline);
        free (pSlpFile->frames[i].decoded);
      }
    }

  free (pSlpFile->frames);
  free (pSlpFile);
  pSlpFile = GN_NULL;
}

/*
 * Gn_ConvertSpriteFrameRGB ()
 * Convert a decoded SLP frame (palette indexes) into a RGB array.
 * NOTE: Remember to deallocate the RGB pointer after usage.
 */
Gn_Byte*
Gn_ConvertSpriteFrameRGB (const Gn_FrameSLP *frame, const Gn_Palette *pal)
{
  Gn_Byte *rgb, *ptr;
  int i, j, c, k = 0;

  if (!frame || !pal)
    return GN_NULL;

  rgb = (Gn_Byte *)
    malloc (frame->header.size_w * frame->header.size_h * 3);

  /* storing the start pointer position (which is going to be returned) */
  ptr = rgb;

  if (!rgb)
    return GN_NULL;

  for (i = 0; i < frame->header.size_h; ++i)
  {
    for (j = 0; j < frame->header.size_w; ++j)
    {
      c = frame->decoded[k++];

      if (!c)
        {
          /* Allegro 4 transparency mask (255, 0, 255) */
          *rgb++ = 0xff;
          *rgb++ = 0x00;
          *rgb++ = 0xff;
        }
      else
        {
          *rgb++ = pal->p_pixel[c].b_col[0];
          *rgb++ = pal->p_pixel[c].b_col[1];
          *rgb++ = pal->p_pixel[c].b_col[2];
        }
    }
  }

  return ptr;
}

Gn_Sprite*
Gn_GetSpriteFromSLP (const Gn_FileSLP *slp, const Gn_Palette *pal,
  int32_t frame_index)
{
  Gn_Sprite *sprite = GN_NULL;
  Gn_FrameSLP *frame = GN_NULL;
  Gn_Byte *rgb = GN_NULL;

  if (!slp || frame_index < 0)
    return GN_NULL;

  if (frame_index >= slp->header.frame_count)
    return GN_NULL;

  frame = & slp->frames[frame_index];
  sprite = (Gn_Sprite*) malloc (sizeof (Gn_Sprite));

  if (!sprite)
    return GN_NULL;

  rgb = Gn_ConvertSpriteFrameRGB (frame, pal);

  if (!rgb)
    {
      free (sprite);
      return GN_NULL;
    }

  sprite->center.x = frame->header.center_x;
  sprite->center.y = frame->header.center_y;

  sprite->bmp = Gn_BitmapFromRGB (rgb,
    frame->header.size_w, frame->header.size_h);

  free (rgb);
  return sprite;
}

void
Gn_FreeSprite (Gn_Sprite * sprite)
{
  if (!sprite)
    return;

  if (sprite->bmp)
    destroy_bitmap (sprite->bmp);

  free (sprite);
  sprite = GN_NULL;
}
