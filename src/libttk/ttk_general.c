
#include "ttk.h"

#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static uint32_t crc_tbl[256];
static int crc_gen = TTK_FALSE;

static void
Ttk_GenCRC32Table (void)
{
  uint32_t rem;
  int i, j;

  for (i = 0; i < 256; ++i)
    {
      rem = i;/* remainder from polynomial division */
      for (j = 0; j < 8; ++j)
        {
          if (rem & 1)
            {
              rem >>= 1;
              rem ^= 0xedb88320;
            }
          else
            rem >>= 1;
        }
      crc_tbl[i] = rem;
    }

  crc_gen = TTK_TRUE;
}

uint32_t
Ttk_UpdateCRC32 (uint32_t crc, const void* src, uint64_t length)
{
  uint8_t octet, *p, *q;

  if (!crc_gen)
    Ttk_GenCRC32Table();

  crc = ~crc;

  q = (uint8_t*) src + length;
  p = (uint8_t*) src;

  do
    {
      octet = *p;
      crc = (crc >> 8) ^ crc_tbl[(crc & 0xff) ^ octet];
    }
  while (++p < q);
  return ~crc;
}

/*!
 * @brief Get the file size in bytes
 * @param String with the file path
 * @return File size in bytes
 */
uint64_t
Ttk_FileGetSize (const char* sz_path)
{
  struct stat file_details;
  if (!stat(sz_path, & file_details))
    return (uint64_t) file_details.st_size;
  return 0;
}

uint32_t
Ttk_FileGetCRC32 (const char* sz_path)
{
  uint32_t crc;
  TtkBuffer* buf;

  buf = Ttk_BufDiskLoad(sz_path);
  crc = Ttk_UpdateCRC32(0, buf->data, buf->length);
  Ttk_BufFree(buf);

  return crc;
}

char*
Ttk_FileGetExt (const char* sz_path)
{
  size_t length, start;
  char *str, *ret, *ref;

  length = strlen(sz_path);
  start = length;
  str = (char*)sz_path + length;

  while (start--)
  {
    if (*str == '.')
      break;
    str--;
  }
  *str++;

  ref = (char*)calloc(length - start + 1, sizeof(char));
  ret = ref;

  while (*str)
    *ref++ = *str++;

  return ret;
}

static int
Ttk_FileCheckExt (const char* sz_path, const char* sz_ext)
{
  int cmpres;
  char* cmpext;

  cmpext = Ttk_FileGetExt(sz_path);
  cmpres = Ttk_StrCmpI(cmpext, sz_ext);
  free(cmpext);

  return cmpres;
}

char*
Ttk_FileFixExt (const char* sz_path, const char* sz_ext)
{
  char* resptr;
  size_t n, n1, n2;

  if (!Ttk_FileCheckExt(sz_path, sz_ext))
    {
      n = strlen(sz_path);
      resptr = (char*) calloc(1, n+1);
      memcpy(resptr, sz_path, n);
      return resptr;
    }
  else
    {
      n1 = strlen (sz_path);
      n2 = strlen (sz_ext);
      n = n1 + n2;
      resptr = (char *) calloc (1, n+2);
      memcpy (resptr, sz_path, n1);
      resptr[n1] = '.';
      memcpy (resptr + n1 + 1, sz_ext, n2);
      return resptr;
    }
  return NULL;
}

/*!
 * @brief String comparison case insensitive
 * @param lhs First string to compare
 * @param rhs Seccond string to compare
 * @return 0 if the strings are equal or -1 if not
 * @see Ttk_StrCmpIn
 */
int
Ttk_StrCmpI (const char* sz_lhs, const char* sz_rhs)
{
  size_t n1 = strlen(sz_lhs);
  size_t n2 = strlen(sz_rhs);

  char* p1 = (char*)sz_lhs;
  char* p2 = (char*)sz_rhs;

  if (n1 != n2)
    return -1;

  while (*p1 && *p2)
    if (toupper(*p1++) - toupper(*p2++))
      return -1;

  return 0;
}

int
Ttk_StrCmpIn (const char* sz_lhs, const char* sz_rhs, uint64_t n)
{
  char* p1 = (char*)sz_lhs;
  char* p2 = (char*)sz_rhs;

  while (n--)
    if (toupper(*p1++) - toupper(*p2++))
      return -1;

  return 0;
}
