
/*
 *    libGFF: Genie File Format
 *    By Andre Caceres Carrilho
 *
 *      This code is part of the zEMPIRES project, an open source
 *      reimplementation of Ensemble Studios Genie engine.
 *
 *      Genie engine data resource (.DRS) file format
 *      Credits: Cyrille Artho, Steve Chen, and Stoyan Ratchev.
 *
 *      See license.md for copyright information.
 */

/*
 * History
 *
 * 31-jan-17 Created: Initial code, DRS structures and file reading.
 * 06-mar-17 Feature: Initial filesystem code design and implementation
 * 09-mar-17 Cleanup: Moved SLP and PAL code for "sprite .h & .c"
 * 09-mar-17 Cleanup: DRS file structure rework. Code adapted and corrected.
 * 28-mar-17 Rework: Now under libgff and using libttk for memory buffers.
 */

#include "filedrs.h"

/*
 * GDRS_DecodeFile ()
 * Returns null if fails to open, read or parse the DRS file
 * Todo: add endianess swaps to support other platforms
 */
GDRSFile*
GDRS_DecodeFile(TtkBuffer *mem_buf)
{
  int32_t i, j;
  size_t read_size;

  GDRSFile *drs_file;
  GDRSTable *drs_table;

  if (!mem_buf)
    return NULL;

  if ((mem_buf->length - mem_buf->offset) < sizeof(GDRSHeader))
    return NULL;

  /*
   * First step: allocating the GDRSFile structure
   * and opening the file stream for binary reading
   */
  drs_file = (GDRSFile*) malloc(sizeof(GDRSFile));

  if (!drs_file)
    return NULL;

  /*
   * Second step: reading the file header which
   * is basically the first 64 bytes of the file
   */
  Ttk_BufRead(& drs_file->header, sizeof(GDRSHeader), 1, mem_buf);
  read_size = drs_file->header.table_header_count * sizeof(GDRSTable);

  if ((mem_buf->length - mem_buf->offset) < read_size)
    goto drs_fail;

  /*
   * Third step: following the file header there are the table headers
   * here we allocate the structures based on their number readed from
   * the file header
   */
  drs_file->tables = (GDRSTable*) malloc(read_size);

  if (!drs_file->tables)
    goto drs_fail;

  /*
   * Fourth step: Read the table headers from the disk, and allocating the
   * file entry header array for each table on the DRS file.
   */
  for (i = 0; i < drs_file->header.table_header_count; ++i)
    {
      drs_table = &drs_file->tables[i];

      if ((mem_buf->length - mem_buf->offset) < sizeof(GDRSTableHeader))
        goto drs_entry_alloc_fail;

      Ttk_BufRead(& drs_table->header, sizeof(GDRSTableHeader), 1, mem_buf);

      drs_table->entries = (GDRSEntry*)
        malloc(sizeof(GDRSEntry) * drs_table->header.file_header_count);

      if (!drs_table->entries)
        goto drs_entry_alloc_fail;
    }

  /*
   * Final step: Seek and read the file entry headers in the disk.
   * Note: the actual file data are not readed in this function.
   */
  for (i = 0; i < drs_file->header.table_header_count; ++i)
    {
      drs_table = &drs_file->tables[i];

      Ttk_BufSeek(mem_buf, drs_table->header.file_headers_offset, SEEK_SET);
      read_size = drs_table->header.file_header_count * sizeof(GDRSEntry);

      if ((mem_buf->length - mem_buf->offset) < read_size)
        goto drs_entry_fail;

      Ttk_BufRead(drs_table->entries, sizeof(GDRSEntry),
        drs_table->header.file_header_count, mem_buf);
    }

  return drs_file;

drs_entry_alloc_fail:
  for (j = 0; j < i; j++)
    free(drs_file->tables[j].entries);

  free(drs_file->tables);
  goto drs_fail;

drs_entry_fail:
  GDRS_DestroyFile(drs_file);
  return NULL;

drs_fail:
  free(drs_file);
  return NULL;
}

/*
 * GDRS_DestroyFile ()
 * Closes the disk file handler and deallocates the memory.
 */
void
GDRS_DestroyFile(GDRSFile *drs_file)
{
  int32_t i;

  /* Check pointer and close the disk file if it was opened */
  if (!drs_file)
    return;

  /* Deallocate all file entries and table headers on this DRS file */
  if (drs_file->tables)
    {
      for (i = 0; i < drs_file->header.table_header_count; ++i)
        if (drs_file->tables[i].entries)
          free(drs_file->tables[i].entries);

      free(drs_file->tables);
    }

  /* This is necessary for the filesystem linked list */
  free(drs_file);
}
