
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
 *      Genie engine file system (Data resource files .DRS)
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
 */

#include "filesys.h"

/*
 * Gn_OpenFileDRS ()
 * Returns null if fails to open, read or parse the DRS file
 * Todo: add endianess swaps to support other platforms
 */
Gn_FileDRS *
Gn_OpenFileDRS (const char *szFilePath)
{
  int32_t i;  char j;

#ifdef GN_FILECHECK
  if (Gn_StrCmpCI (szFilePath, ".drs") )
    return GN_NULL;
#endif /* !filecheck */

  /*
   * First step: allocating the Gn_FileDRS structure
   * and opening the file stream for binary reading
   */
  Gn_FileDRS* pDrsFile = (Gn_FileDRS*) malloc (sizeof (Gn_FileDRS));
  if (!szFilePath || !pDrsFile)
    return GN_NULL;

  pDrsFile->disk_file = fopen (szFilePath, "rb");
  if (!pDrsFile->disk_file)
    return GN_NULL;

  /*
   * Second step: reading the file header which
   * is basically the first 64 bytes of the file
   */
  fread (& pDrsFile->header, sizeof (Gn_HeaderDRS), 1, pDrsFile->disk_file);

  /*
   * Third step: following the file header there are the table headers
   * here we allocate the structures based on their number readed from
   * the file header
   */
  pDrsFile->tables = (Gn_TableDRS *) malloc (sizeof (Gn_TableDRS) *
    pDrsFile->header.table_header_count );

  if (!pDrsFile->tables)
    return GN_NULL;

  /*
   * Fourth step: Read the table headers from the disk, and allocating the
   * file entry header array for each table on the DRS file.
   */
  for (i = 0; i < pDrsFile->header.table_header_count; ++i)
    {
      fread (& pDrsFile->tables[i].header, sizeof (Gn_TableHeaderDRS), 1,
        pDrsFile->disk_file);

      pDrsFile->tables[i].entries =
        (Gn_FileEntryDRS *) malloc(sizeof (Gn_FileEntryDRS) *
          pDrsFile->tables[i].header.file_header_count);

      if (!pDrsFile->tables[i].entries)
        return GN_NULL;
    }

  /*
   * Final step: Swaping the file extension in order to "unreverse".
   * Seek and read the file entry headers in the disk.
   * NOTE: the actual files data are not readed in this function.
   */
  for (i = 0; i < pDrsFile->header.table_header_count; ++i)
    {
      j = pDrsFile->tables[i].header.extension[0];
      pDrsFile->tables[i].header.extension[0] =
        pDrsFile->tables[i].header.extension[2];
      pDrsFile->tables[i].header.extension[2] = j;

      fseek (pDrsFile->disk_file,
        pDrsFile->tables[i].header.file_headers_offset, SEEK_SET);

      fread (pDrsFile->tables[i].entries, sizeof (Gn_FileEntryDRS),
        pDrsFile->tables[i].header.file_header_count, pDrsFile->disk_file);
    }

  pDrsFile->next = GN_NULL;
  return pDrsFile;
}

/*
 * Gn_FindEntryDRS ()
 * Returns null if the entry wasnt found on the DRS specified or it wasnt
 * possible to read the entry
*/
Gn_MemoryBuffer *
Gn_FindEntryDRS (const Gn_FileDRS *pDrsFile, int32_t file_index)
{
  int32_t i, j; /* Table "i", Entry "j" */

  Gn_FileEntryDRS * pFileEntry = GN_NULL;
  Gn_MemoryBuffer * pMemBuf = GN_NULL;

  if (!pDrsFile)
    return GN_NULL;

  /*
   * First step: Search for the entry that matches the file ID
   * The search is performed on every DRS table available
   */
  for (i = 0; i < pDrsFile->header.table_header_count; ++i)
    for (j = 0; j < pDrsFile->tables[i].header.file_header_count; ++j)
      if (file_index == pDrsFile->tables[i].entries[j].index)
        {
          pFileEntry = &pDrsFile->tables[i].entries[j];
          break;
        }

  if (!pFileEntry)
    return GN_NULL;

  /*
   * Final step: Allocate the memory buffer to hold
   * the entry and read its raw data from the disk i.e.
   * additional parsing must be performed afterwards
   */
  pMemBuf = Gn_MemBufAlloc (pFileEntry->size);

  if (!pMemBuf)
    return GN_NULL;

  fseek (pDrsFile->disk_file, pFileEntry->offset, SEEK_SET);
  fread (pMemBuf->p_data, sizeof (Gn_Byte), pFileEntry->size,
    pDrsFile->disk_file);

  return pMemBuf;
}

/*
 * Gn_FileSysInit ()
 */
Gn_FileSystem *
Gn_FileSysInit (void)
{
  Gn_FileSystem * pFileSys = (Gn_FileSystem *) malloc (sizeof (Gn_FileSystem));

  if (!pFileSys)
    return GN_NULL;

  pFileSys->drs_stack = GN_NULL;
  return pFileSys;
}

/*
 * Gn_FileSysPush ()
 * Push a new DRS file (from disk) at the end of the stack
 * Similar to push_back () of a STL std::vector<>
 */
int32_t
Gn_FileSysPush (Gn_FileSystem * pFileSys, const char * szFilePath)
{
  if (!pFileSys || !szFilePath)
    return GN_FAILURE;

  /*
   * Try to read the DRS file from disk first, because
   * if we cant, there is no point on moving forward
   */
  Gn_FileDRS *pNewNod, *pCurStk;
  pNewNod = Gn_OpenFileDRS (szFilePath);

  if (!pNewNod)
    return GN_FAILURE;

  /*
   * If there was no DRS file on stack yet,
   * this is going to be the first one
   */
  if (!pFileSys->drs_stack)
    {
      pFileSys->drs_stack = pNewNod;
      return GN_SUCESS;
    }

  /*
   * If there was DRS files already, we have to iterate
   * until the back of the stack and add the DRS file to it
   */
  pCurStk = pFileSys->drs_stack;
  while (pCurStk->next)
    pCurStk = pCurStk->next;
  pCurStk->next = pNewNod;
  return GN_SUCESS;
}

/*
 * Gn_FileSysExit ()
 * Deallocate and close all DRS files used by the file system
 */
void
Gn_FileSysExit (Gn_FileSystem * pFileSys)
{
  if (!pFileSys)
    return;

  /* Pointers to iterate through the linked list */
  Gn_FileDRS *pNodStk, *pCurStk;
  pCurStk = pFileSys->drs_stack;
  while (pCurStk)
    {
      /*
       * Store the current element (node) in a temporary pointer
       * and update it to the next element in the stack
       */
      pNodStk = pCurStk;
      pCurStk = pCurStk->next;
      /* Perform the deallocation properly for the DRS file */
      Gn_CloseFileDRS (pNodStk);
    }
  pFileSys->drs_stack = GN_NULL;
  free (pFileSys);
}

/*
 * Gn_FileSysFind ()
 * Search for the file entry on all opened DRS files
 */
Gn_MemoryBuffer *
Gn_FileSysFind (const Gn_FileSystem * pDrsList, int32_t file_index)
{
  Gn_MemoryBuffer* pMemBuf = GN_NULL;
  Gn_FileDRS* pCurStk = GN_NULL;

  if (!pDrsList || file_index < 0)
    return GN_NULL;

  /* Get the first element in the stack */
  pCurStk = pDrsList->drs_stack;
  while (pCurStk)
    {
      /*
       * Iterate on all elements of the stack and look for the desired entry
       * if found we can return the pointer immidiately
       */
      pMemBuf = Gn_FindEntryDRS (pCurStk, file_index);
      if (pMemBuf)
        return pMemBuf;

      /* Otherwise, look in the next DRS file */
      pCurStk = pCurStk->next;
    }
  return GN_NULL;
}

/*
 * Gn_CloseFileDRS ()
 * Closes the disk file handler and deallocates the memory.
 */
void
Gn_CloseFileDRS (Gn_FileDRS * pDrsFile)
{
  int32_t i;

  /* Check pointer and close the disk file if it was opened */
  if (!pDrsFile)
    return;

  if (pDrsFile->disk_file)
    fclose (pDrsFile->disk_file);

  /* Deallocate all file entries and table headers on this DRS file */
  if (pDrsFile->tables)
    {
      for (i = 0; i < pDrsFile->header.table_header_count; ++i)
        if (pDrsFile->tables[i].entries)
          free (pDrsFile->tables[i].entries);
      free (pDrsFile->tables);
    }

  /* This is necessary for the filesystem linked list */
  free (pDrsFile);
  pDrsFile = GN_NULL;
}
