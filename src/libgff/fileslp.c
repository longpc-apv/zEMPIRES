
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


GSLPFile*
GSLP_DecodeFile(TtkBuffer *mem_buf)
{
  GSLPFile *slp_file;
  GSLPFrame *slp_frame;

  if (Ttk_BufSizeLeft(mem_buf) < sizeof(GSLPHeader))
    return NULL;

  slp_file = (GSLPFile*) malloc(sizeof(GSLPFile));

  if (!slp_file)
    return NULL;



  return slp_file;
}

void
GSLP_DestroyFile(GSLPFile *slp_file)
{
}
