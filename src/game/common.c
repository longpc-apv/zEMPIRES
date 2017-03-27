
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
 *      Engine general code and functionality
 *
 *      See license.md for copyright information.
 */

#include "common.h"

/*
 * Gn_StrCmpCI ()
 * File extension check (comparison) case insensitive
 * Returns zero if the file extension is verified
 */
int32_t
Gn_StrCmpCI (const char *str, const char *ext)
{
    size_t n1 = strlen (str);
    size_t n2 = strlen (ext);
    if (n1 < n2)  return -1;
    char *s1 = (char*) str + n1 - n2;
    char *s2 = (char*) ext;
    int32_t ds = 0; /* difference sum */
    while (*s1 && *s2)
      ds += toupper (*s1++) - toupper (*s2++);
    return ds;
}

/*
 * Gn_StrCmpCI_n ()
 * String comparison case insensitive, but only for "n" chars
 * Returns the char-wise difference sum among the "n" chars
 */
int32_t
Gn_StrCmpCI_n (const char *str, const char *cmp, int32_t n)
{
    char *s1 = (char*) str;
    char *s2 = (char*) cmp;
    int32_t ds = 0; /* difference sum */
    while (n--)
      ds += toupper (*s1++) - toupper (*s2++);
    return ds;
}

/*
 * Gn_PrintStr_n ()
 * Print a non-zero terminated string. Prints "n" characters.
 * This is used to print the header strings of DRS and SLP files.
 */
void
Gn_PrintStr_n (const char * nz_str, int32_t n)
{
  char * str = (char *)nz_str;
  while (n--)
    printf("%c", *str++);
}

/*
 * Gn_FixFileExt ()
 * Compares the last chars from filename to see if matches the desired
 * extension. If it does, just copy the filename to a new string, if not,
 * returns the new string with the correct file extension.
 */
char*
Gn_FixFileExt (const char *sz_filename, const char *sz_ext)
{
  char* ptr;
  size_t n, n1, n2;

  if (!Gn_StrCmpCI (sz_filename, sz_ext))
    {
      n = strlen (sz_filename);
      ptr = (char *) calloc (sizeof (char), n+1);
      memcpy (ptr, sz_filename, n);
      return ptr;
    }
  else
    {
      n1 = strlen (sz_filename);
      n2 = strlen (sz_ext);
      n = n1 + n2;
      ptr = (char *) calloc (sizeof (char), n+1);
      memcpy (ptr, sz_filename, n1);
      memcpy (ptr+n1, sz_ext, n2);
      return ptr;
    }

  return GN_NULL;
}

#ifdef __WIN32
static int32_t
Gn_GetFileSize_Win32 (const char * sz_filepath)
{
    WIN32_FILE_ATTRIBUTE_DATA fad;
    if (!GetFileAttributesEx(sz_filepath, GetFileExInfoStandard, &fad))
        return -1; /* error condition, could call GetLastError to find out more */
    LARGE_INTEGER size;
    size.HighPart = fad.nFileSizeHigh;
    size.LowPart = fad.nFileSizeLow;
    return (int32_t) size.QuadPart;
}
#endif /* !__WIN32 */

#ifdef __linux__
static int32_t
Gn_GetFileSize_Linux (const char * sz_filepath)
{
  struct stat file_details;
  if (!stat(sz_filepath, & file_details))
    return (int32_t) file_details.st_size;
  return 0;
}
#endif /* !__linux__ */

#ifdef GN_UNSAFE
static int32_t
Gn_GetFileSize_Unsafe (const char * sz_filepath)
{
  Gn_DiskFile *fp;
  int32_t fs;

  fp = fopen (sz_filepath, "rb");

  fseek (fp, 0, SEEK_END);
  fs = ftell (fp);
  fclose (fp);

  return fs;
}
#endif /* !GN_UNSAFE */

/*
 * Gn_GetFileSize_Naive ()
 * Not an optmial solution for this task, but is a portable implementation.
 * Return is an signed integer to accomodate the failure sign (-1).
 */
static int32_t
Gn_GetFileSize_Naive (const char * sz_filepath)
{
  Gn_DiskFile *fp;
  int32_t fs = 0, rc;

  if (!sz_filepath)
    return GN_FAILURE;

  fp = fopen (sz_filepath, "rb");

  if (!fp)
    return GN_FAILURE;

  do
    {
      /*
       * The return type is int to accommodate for the special value EOF,
       * which indicates failure: #define EOF (-1) in most cases...
       * http://www.cplusplus.com/reference/cstdio/EOF/
       */
      rc = fgetc (fp);
      fs++;
    }
  while (rc != EOF);

  fclose (fp);
  return fs;
}

int32_t
Gn_GetFileSize (const char * sz_filepath)
{
#if defined __WIN32
  return Gn_GetFileSize_Win32 (sz_filepath);
#elif defined __linux__
  return Gn_GetFileSize_Linux (sz_filepath);
#elif defined GN_UNSAFE
  return Gn_GetFileSize_Unsafe (sz_filepath);
#endif

  return Gn_GetFileSize_Naive (sz_filepath);
}
