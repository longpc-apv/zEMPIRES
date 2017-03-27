
#include "palette.h"

#ifdef GN_FILECHECK
static int32_t
Gn_CountEntriesPAL ( Gn_MemoryBuffer* pMemBuf )
{
  int32_t i, loc;
  /* Starting from 0x15 we skip the three header lines */
  for (loc = 0, i = 0x15; i < pMemBuf->n_size; ++i)
      if (pMemBuf->p_data[i] == 0x0a)
        loc++;
  return loc;
}
#endif /* !filecheck */

typedef struct
  {
    char        format[8];      /* JASC-PAL */
    int16_t     file_version;   /* 0100 (not checked though) */
    int16_t     pixel_count;    /* less than 256 (not checked though) */
  }
Gn_PaletteFileHeader;

/*
 * Gn_OpenFilePAL ()
 * Parse JASC-PAL (Paint Shop Pro) palette files
 */
Gn_Palette*
Gn_OpenFilePAL ( Gn_MemoryBuffer* pMemBuf )
{
  int32_t i;
  Gn_Palette *pPalFile;
  Gn_PaletteFileHeader *header;

  pPalFile = (Gn_Palette*) malloc (sizeof (Gn_Palette));
  header = (Gn_PaletteFileHeader*)
    malloc (sizeof (Gn_PaletteFileHeader));

  if (!pPalFile || !pMemBuf || !header)
    return GN_NULL;

  /* Parse PAL file header */
  Gn_MemBufRead (&header->format, 0x08, 0x01, pMemBuf);
  header->file_version = Gn_MemBufStrtol (pMemBuf);
  header->pixel_count = Gn_MemBufStrtol (pMemBuf);

#ifdef GN_FILECHECK
  if (header->pixel_count != Gn_CountEntriesPAL (pMemBuf)
    || Gn_StrCmpCI_n (header->format, "JASC-PAL", 0x08))
    {
      free (pPalFile);
      free (header);
      return GN_NULL;
    }
#endif /* !filecheck */

  pPalFile->n_size = header->pixel_count;
  pPalFile->p_pixel = (Gn_Color *)
    malloc (pPalFile->n_size * sizeof (Gn_Color));

  if (!pPalFile->p_pixel)
    {
      free (pPalFile);
      free (header);
      return GN_NULL;
    }

  /*
   * Workaround for the "strtol" code. The palette text files doesn't
   * end with an end line char '\n', so we have to manually allocate the
   * additional space and put one to avoid segmentation fault
   */

  Gn_MemBufResize (pMemBuf, pMemBuf->n_size+1);

  for (i = 0; i < header->pixel_count; ++i)
    {
      pPalFile->p_pixel[i].b_col[0] = Gn_MemBufStrtol (pMemBuf);
      pPalFile->p_pixel[i].b_col[1] = Gn_MemBufStrtol (pMemBuf);
      pPalFile->p_pixel[i].b_col[2] = Gn_MemBufStrtol (pMemBuf);
      pPalFile->p_pixel[i].b_col[3] = 0xff;
    }

  Gn_MemBufSeek (pMemBuf, 0, SEEK_SET);

  free (header);
  return pPalFile;
}

void Gn_CloseFilePAL (Gn_Palette * pal)
{
  if (!pal)
    return;

  if (pal->p_pixel)
    free (pal->p_pixel);

  free (pal);
  pal = GN_NULL;
}
