
/*
 *    libGFF: Genie File Format
 *    By Andre Caceres Carrilho
 *
 *      This code is part of the zEMPIRES project, an open source
 *      reimplementation of Ensemble Studios Genie engine.
 *
 *      Genie engine sprites (SLP) file format
 *      Credits: Format description found on the web (unknown authors)
 *
 *      See license.md for copyright information.
 */

#include "fileslp.h"

static int32_t
GSLP_DecodeScanLine(uint8_t *src, uint8_t *dst)
{
  const uint8_t *start = dst;
  uint8_t top4b, bot4b;
  uint8_t curr_byte, next_byte, block_size;

  do
    {
      curr_byte = *src++;
      top4b = data & 0xf0;
      bot4b = data & 0x0f;

      switch (bot4b)
        {
        case 0x00:
        case 0x04:
        case 0x08:
        case 0x0c:
          block_size = (curr_byte & 0xfc) >> 2;
          while (block_size--)
            *dst++ = *src++;
        break;


        case 0x01:
        case 0x05:
        case 0x09:
        case 0x0d:
          block_size = (curr_byte & 0xfc) >> 2;
          dst += block_size;
        break;


        case 0x02:
          block_size = (top4b << 4) + *src++;
          while (block_size--)
            *dst++ = *src++;
        break;


        case 0x03:
          block_size = (top4b << 4) + *src++;
          dst += block_size;
        break;


        case 0x06:
          block_size = top4b >> 4;
          if (!block_size)
            block_size = *src++;
          while (block_size--)
            *dst++ = *src++;
        /*! Todo: need to mark this pixels to player palette translation */
        break;


        case 0x07:
          block_size = top4b >> 4;
          if (!block_size)
            block_size = *src++;
          next_byte = *src++;
          while (block_size--)
            *dst++ = next_byte;
        break;


        case 0x0a:
          block_size = top4b >> 4;
          if (!block_size)
            block_size = *src++;
          next_byte = *src++;
          while (block_size--)
            *dst++ = next_byte;
        /*! Todo: need to mark this pixels to player palette translation */
        break;


        case 0x0b:
          block_size = top4b >> 4;
          if (!block_size)
            block_size = *src++;
          while (block_size--)
            *dst++ = 0; /*! fix this */
        break;


        case 0x0e:/*! fix this */
          switch (curr_byte)
            {
            case 0x0e:
            case 0x1e:
              return -1;
            break;

            case 0x2e:
            case 0x3e:
              return -1;
            break;

            case 0x4e:
            case 0x5e:
              *dst++ = 0;
            break;

            case 0x6e:
            case 0x7e:
              block_size = *src++;
              while (block_size--)
                *dst++ = 0;
            break;
            }
        break;
        }
    }
  while (bot4b != 0x0f);
  return dst - start;
}

static void
GSLP_NeedPlayerIndex(GSLPFrame *slp_frame)
{
  int32_t i, pixel_count;

  pixel_count = slp_frame->header.size_w * slp_frame->header.size_h;

  for (i = 0; i < pixel_count; ++i)
    if (slp_frame->civcol[i])
      {
        slp_frame->need_player = TTK_TRUE;
        return;
      }

  slp_frame->need_player = TTK_FALSE;
}

static int32_t
GSLP_DecodeFrameData(GSLPFrame *slp_frame, TtkBuffer *mem_buf)
{
  int32_t i, decoded_bytes;
  uint8_t *src, *dst, *msk;

  src = mem_buf->data + mem_buf->offset;
  dst = slp_frame->dec_data;
  msk = slp_frame->civcol;

  for (i = 0; i < slp_frame->header.size_h; ++i)
    {
      /* If the right or left offsets are 0x8000 the row is completely
       * transparent. Note that there are no command bytes for these
       * rows, so will have to be skipped "manually". */
      if (slp_frame->outline[i].dist_left == 0x8000 ||
          slp_frame->outline[i].dist_right == 0x8000)
        {
          /* I think we should read one byte of the source buffer if this
           * happens, however, as we are using the command offsets to seek
           * in the memory buffer, we can neglect this issue. */
          dst += slp_frame->header.size_w;
          msk += slp_frame->header.size_w;
          continue;
        }

      Ttk_BufSeek(mem_buf, slp_frame->command[i], TTK_SEEK_SET);
      src = mem_buf->data + mem_buf->offset;

      dst += slp_frame->outline[i].dist_left;
      msk += slp_frame->outline[i].dist_left;

      decoded_bytes = GSLP_DecodeScanLine(src, dst, msk);

      if (decoded_bytes < 0)
        return -1;

      dst += slp_frame->outline[i].dist_right + decoded_bytes;
      msk += slp_frame->outline[i].dist_right + decoded_bytes;
    }
  slp_frame->decoded = TTK_TRUE;
  GSLP_NeedPlayerIndex(slp_frame);
  return 0;
}

/*
 * GSLP_GetEncodedSize()
 *
 * Dumb workaround to get the encoded pixel data size from the SLP file.
 * We just seek for the last scanline command offset and search the end byte
 * flag (0x0f) in the data. The encoded size is given by the distance to
 * the first command offset.
 *
 * Note 1: To run this code we first need to parse the frame header and read
 * the scanline command offsets and outlines.
 *
 * Note 2: This function may seem unecessary for the game code, but is useful
 * for the engine editor (SLP file encoding and saving).
 */
static int32_t
GSLP_GetEncodedSize(const GSLPFrame *slp_frame, const TtkBuffer *mem_buf)
{
  int32_t offset;
  uint8_t *run;

  if (!slp_frame || !mem_buf)
    return -1;

  offset = slp_frame->command[slp_frame->header.size_h-1];
  run = mem_buf->data + offset;

  while ((*run++ & 0x0f) != 0x0f)
    offset++;

  return offset - slp_frame->command[0];
}

static int
GSLP_ConstructFrame(GSLPFrame *slp_frame)
{
  int32_t pixel_count;

  if (!slp_frame)
    return;

  pixel_count = slp_frame->header.size_w * slp_frame->header.size_h;

  slp_frame->command = (uint32_t*)
    malloc(slp_frame->header.size_h * sizeof(uint32_t));

  slp_frame->outline = (GSLPRowEdge*)
    malloc(slp_frame->header.size_h * sizeof(GSLPRowEdge));

  if (!slp_frame->command || !slp_frame->outline)
    return -1

  slp_frame->dec_data = (uint8_t*) malloc(pixel_count);
  slp_frame->civcol = (uint8_t*) malloc(pixel_count);

  if (!slp_frame->dec_data || !slp_frame->civcol)
    return -1;

  memset(slp_frame->dec_data, 0, pixel_count);
  memset(slp_frame->civcol, 0, pixel_count);

  slp_frame->encoded = TTK_FALSE;
  slp_frame->decoded = TTK_FALSE;
  slp_frame->need_player = TTK_FALSE;

  return 0;
}

static void
GSLP_DestroyFrame(GSLPFrame *slp_frame)
{
  if (!slp_frame)
    return;

#define GSLP_FREE(x) if (x) free(x)
  GSLP_FREE(slp_frame->outline);
  GSLP_FREE(slp_frame->command);
  GSLP_FREE(slp_frame->dec_data);
  GSLP_FREE(slp_frame->enc_data);
  GSLP_FREE(slp_frame->civcol);
#undef GSLP_FREE
}

GSLPFile*
GSLP_DecodeFile(TtkBuffer *mem_buf)
{
  int32_t i, j, pixel_count;

  GSLPFile *slp_file;
  GSLPFrame *slp_frame;

  if (Ttk_BufSizeLeft(mem_buf) < sizeof(GSLPHeader))
    return NULL;

  slp_file = (GSLPFile*) malloc(sizeof(GSLPFile));

  if (!slp_file)
    return NULL;

  Ttk_BufSeek(mem_buf, 0, TTK_SEEK_SET);
  Ttk_BufRead(&slp_file->header, sizeof(GSLPHeader), 1, mem_buf);

  if (Ttk_BufSizeLeft(mem_buf) <
    sizeof(GSLPFrameHeader) * slp_file->header.frame_count)
    goto slp_fail;

  slp_file->frames = (GSLPFrame*)
    malloc(sizeof(GSLPFrame) * slp_file->header.frame_count);

  if (!slp_file->frames)
    goto slp_fail;

  for (i = 0; i < slp_file->header.frame_count; ++i)
    {
      slp_frame = &slp_file->frames[i];
      Ttk_BufRead(&slp_frame->header, sizeof(GSLPFrameHeader), 1, mem_buf);

      slp_frame->command = (uint32_t*)
        malloc(slp_frame->header.size_h * sizeof(uint32_t));

      slp_frame->outline = (GSLPRowEdge*)
        malloc(slp_frame->header.size_h * sizeof(GSLPRowEdge));

      if (!slp_frame->command || !slp_frame->outline)
        goto slp_fail_alloc;

      pixel_count = slp_frame->header.size_w * slp_frame->header.size_h;

      slp_frame->dec_data = (uint8_t*) malloc(pixel_count);
      slp_frame->civcol = (uint8_t*) malloc(pixel_count);

      if (!slp_frame->dec_data || !slp_frame->civcol)
        goto slp_fail_alloc;

      memset(slp_frame->dec_data, 0, pixel_count);
      memset(slp_frame->civcol, 0, pixel_count);
    }

  for (i = 0; i < slp_file->header.frame_count; ++i)
    {
      slp_frame = &slp_file->frames[i];

      Ttk_BufSeek(mem_buf, slp_frame->header.outline_offset, TTK_SEEK_SET);
      Ttk_BufRead(slp_frame->outline, sizeof(GSLPRowEdge),
        slp_frame->header.size_h, mem_buf);

      Ttk_BufSeek(mem_buf, slp_frame->header.command_offset, TTK_SEEK_SET);
      Ttk_BufRead(slp_frame->command, sizeof(uint32_t),
        slp_frame->header.size_h, mem_buf);

      if (GSLP_DecodeFrameData(slp_frame, mem_buf) < 0)
        goto slp_fail_decode;
    }

  return slp_file;

slp_fail_alloc:
  for (j = 0; j <= i; ++j)
    GSLP_DestroyFrame(&slp_file->frames[j]);

  free(slp_file->frames);
  goto slp_fail;

slp_fail_decode:
  GSLP_DestroyFile(slp_file);
  goto slp_fail;

slp_fail:
  free(slp_file);
  return NULL;
}

void
GSLP_DestroyFile(GSLPFile *slp_file)
{
  int32_t i;

  if (!slp_file)
    return;

  if (!slp_file->frames)
    {
      free(slp_file);
      return;
    }

  for (i = 0; i < slp_file->header.frame_count; ++i)
    GSLP_DestroyFrame(&slp_file->frames[i]);

  free(slp_file->frames);
  free(slp_file);
}
