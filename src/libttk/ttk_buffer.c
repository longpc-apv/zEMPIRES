
#include "ttk.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

static int
Ttk_BufIsWorkSafe (const TtkBuffer* buf)
{
  if (!buf)
    return TTK_FALSE;

  if (!buf->length || (buf->offset > buf->length))
    return TTK_FALSE;

  return TTK_TRUE;
}

static int
Ttk_BufIsStringSafe (const TtkBuffer* buf)
{
  uint8_t* ptr;
  uint64_t itr;

  if (!Ttk_BufIsWorkSafe(buf))
    return TTK_FALSE;

  ptr = (uint8_t*)buf->data + buf->offset;
  itr = buf->length - buf->offset;

  while (itr--)
    if (!(*ptr++))
      return TTK_TRUE;

  return TTK_FALSE;
}

TtkBuffer*
Ttk_BufAlloc (uint64_t buffer_size)
{
  TtkBuffer* buf = (TtkBuffer*) malloc (sizeof (TtkBuffer));

  if (!buf)
    return NULL;

  buf->data = (void*) malloc ((size_t) buffer_size);

  if (!buf->data)
    {
      free (buf);
      return NULL;
    }

  buf->offset = 0;
  buf->length = buffer_size;
  return buf;
}

void
Ttk_BufFree (TtkBuffer* buf)
{
  if (!buf)
    return;

  if (buf->data)
    free (buf->data);

  free (buf);
  buf = NULL;
}

TtkBuffer*
Ttk_BufDiskLoad (const char *sz_path)
{
  TtkBuffer* fb;
  uint64_t fs;
  FILE *fp;

  if (!sz_path)
    return NULL;

  fs = Ttk_FileGetSize(sz_path);
  fb = Ttk_BufAlloc(fs);
  fp = fopen(sz_path, "rb");

  if (!fs || !fb)
    return NULL;

  if (!fp)
    {
      free(fb);
      return NULL;
    }

  fread(fb->data, 1, (size_t)fs, fp);
  fclose(fp);
  return fb;
}

int
Ttk_BufDiskSave (const char *sz_path, const TtkBuffer *buf)
{
  FILE *fp;

  if (!sz_path || !Ttk_BufIsWorkSafe(buf))
    return TTK_FAILURE;

  fp = fopen(sz_path, "wb");

  if (!fp)
    return TTK_FAILURE;

  fwrite(buf->data, 1, buf->length, fp);

  fclose(fp);
  return TTK_SUCCESS;
}

int
Ttk_BufReSizeEx (TtkBuffer* buf, uint64_t new_size, int keep_data)
{
  uint64_t min_size;
  void* new_ptr;

  if (!buf)
    return TTK_FAILURE;

  if (keep_data == TTK_TRUE)
    {
      if (buf->data)
        free (buf->data);

      buf->data = (void*) malloc (new_size);

      if (!buf->data)
        return TTK_FAILURE;
    }
  else if (keep_data == TTK_FALSE)
    {
      buf->data = (void*) realloc (buf->data, new_size);

      if (!buf->data)
        {
          new_ptr = (void*) malloc (new_size);

          if (!new_ptr)
            return TTK_FAILURE;

          min_size = ((new_size < buf->length) ? (new_size) : (buf->length));
          memcpy (new_ptr, buf->data, min_size);

          free (buf->data);
          buf->data = new_ptr;
        }
    }
  else
    return TTK_FAILURE;

  buf->length = new_size;
  if (buf->offset > buf->length)
    buf->offset = buf->length;

  return TTK_SUCCESS;
}

int
Ttk_BufReSize (TtkBuffer* buf, uint64_t new_size)
{
  return Ttk_BufReSizeEx (buf, new_size, TTK_TRUE);
}

int
Ttk_BufMemCmp (const TtkBuffer *lhs, const TtkBuffer *rhs)
{
  if (!lhs || !rhs)
    return 0;

  if (lhs->length != rhs->length)
    return lhs->length - rhs->length;

  return memcmp (lhs->data, rhs->data, (size_t) lhs->length);
}

int
Ttk_BufMemSet (TtkBuffer* buf, int val)
{
  if (!Ttk_BufIsWorkSafe(buf))
    return TTK_SUCCESS;

  memset (buf->data, val, buf->length);
  return TTK_FAILURE;
}

TtkBuffer*
Ttk_BufCropSelect (const TtkBuffer* buf, uint64_t start, uint64_t length)
{
  TtkBuffer* ret;

  if (!Ttk_BufIsWorkSafe(buf) || !length)
    return NULL;

  if (buf->length < (start + length))
    return NULL;

  ret = Ttk_BufAlloc(length);
  memcpy(ret->data, buf->data + start, length);
  return ret;
}

TtkBuffer*
Ttk_BufGenCpy (const TtkBuffer* buf)
{
  TtkBuffer *ret;

  if (!Ttk_BufIsWorkSafe(buf))
    return NULL;

  ret = Ttk_BufAlloc(buf->length);
  memcpy(ret->data, buf->data, (size_t) buf->length);
  return ret;
}

TtkBuffer*
Ttk_BufMemCpy (const void* src, uint64_t length)
{
  TtkBuffer *buf;

  if (!src || !length)
    return NULL;

  buf = Ttk_BufAlloc(length);
  memcpy(buf->data, src, (size_t)length);
  return buf;
}

uint64_t
Ttk_BufRead (void* dst, uint64_t entry_size,
  uint64_t entry_count, TtkBuffer* buf)
{
  uint64_t block_size = entry_count * entry_size;

  if (!dst || !Ttk_BufIsWorkSafe(buf) || !block_size)
    return 0;

  if ((buf->offset + block_size) > buf->length)
    return 0;

  memcpy(dst, buf->data + buf->offset, block_size);
  buf->offset += block_size;

  return entry_count;
}

uint64_t
Ttk_BufWrite (const void* src, uint64_t entry_size,
  uint64_t entry_count, TtkBuffer* dst)
{
  uint64_t block_size = entry_count * entry_size;

  if (!dst || !Ttk_BufIsWorkSafe(dst) || !block_size)
    return 0;

  if ((dst->offset + block_size) > dst->length)
    return 0;

  memcpy(dst->data + dst->offset, src, block_size);
  dst->offset += block_size;

  return entry_count;
}

uint64_t
Ttk_BufSeek (TtkBuffer* buf, uint64_t offset, uint64_t origin)
{
  if (!Ttk_BufIsWorkSafe(buf))
    return TTK_FAILURE;

  if (origin == TTK_SEEK_CUR)
    {
      buf->offset += offset;
      goto seek_finish;
    }
  else if (origin == TTK_SEEK_SET)
    {
      buf->offset = offset;
      goto seek_finish;
    }
  else if (origin == TTK_SEEK_END)
    {
      if (offset > buf->length)
        buf->offset = 0;
      else
        buf->offset = buf->length - offset;
    }

  return TTK_FAILURE;

seek_finish:

  if (buf->offset > buf->length)
    buf->offset = buf->length;

  return TTK_SUCCESS;
}

uint64_t
Ttk_BufMemMove (TtkBuffer *dst, TtkBuffer *src, uint64_t length)
{
  if (!Ttk_BufIsWorkSafe(dst) || !Ttk_BufIsWorkSafe(src) || !length)
    return 0;

  if (((dst->offset + length) > dst->length) ||
      ((src->offset + length) > src->length))
    return 0;

  memcpy(dst->data + dst->offset, src->data + src->offset, (size_t)length);
  dst->offset += length;
  src->offset += length;
  return length;
}

#if 0
char*
Ttk_BufGenStrUTF8 (const TtkBuffer* buf, uint64_t* dst_len)
{
}

wchar_t*
Ttk_BufGenStrUTF16 (const TtkBuffer* buf, uint64_t* dst_len)
{
}

float
Ttk_BufStrToFlt32 (TtkBuffer* buf)
{
  if (Ttk_BufIsStringSafe(buf) == TTK_FALSE)
    return 0.0f;
}

double
Ttk_BufStrToFlt64 (TtkBuffer* buf)
{
  if (Ttk_BufIsStringSafe(buf) == TTK_FALSE)
    return 0.0;
}
#endif

int32_t
Ttk_BufStrToInt32 (TtkBuffer* buf)
{
  int32_t res;
  char *src, *dst;

  if (Ttk_BufIsStringSafe(buf) == TTK_FALSE)
    return 0;

  src = (char*) buf->data + buf->offset;

  res = strtol(src, &dst, 10);
  buf->offset += dst - src;

  return res;
}

int64_t
Ttk_BufStrToInt64 (TtkBuffer* buf)
{
  int64_t res;
  char *src, *dst;

  if (Ttk_BufIsStringSafe(buf) == TTK_FALSE)
    return 0;

  src = (char*) buf->data + buf->offset;

  res = strtoll(src, &dst, 10);
  buf->offset += dst - src;

  return res;
}

uint32_t
Ttk_BufHashCRC32 (const TtkBuffer* buf)
{
  if (!Ttk_BufIsWorkSafe(buf))
    return 0;

  return Ttk_UpdateCRC32(0, buf->data, buf->length);
}
