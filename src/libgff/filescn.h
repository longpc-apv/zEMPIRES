
/*
 *    libGFF: Genie File Format
 *    By Andre Caceres Carrilho
 *
 *      This code is part of the zEMPIRES project, an open source
 *      reimplementation of Ensemble Studios Genie engine.
 *
 *      Genie engine scenario (SCN/SCX) file format
 *      Credits: David Tombs (aka DiGiT), Daniel Derevjanik and Yvan Burrie
 *
 *      See license.md for copyright information.
 */

#ifndef GFF_FILESCN_H
#define GFF_FILESCN_H


#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint16_t length;
  char *data;
} GSCNStr16;

typedef struct {
  uint32_t length;
  char *data;
} GSCNStr32;

typedef struct {
  char      version[4];
  uint32_t  header_size;      /* excluding version and self */
  int32_t   savable;
  uint32_t  last_save;        /* timestamp (savable >= 2) */
  GSCNStr32 instructions;
  uint32_t  individual_victories;
  uint32_t  player_count;
} GSCNHeader;

typedef struct {
  uint16_t  major_version;
  uint16_t  minor_version;

  GSCNHeader header;
} GSCNFile;

typedef struct {
  uint32_t separator; /* 0xffffff9d */
  int32_t player1_cam_y;
  int32_t player1_cam_x;
  int32_t AI_type;
} GSCNMap;

GSCNFile* GSCN_DecodeFile(const TtkBuffer *mem_buf);
void GSCN_DestroyFile(GSCNFile *scn_file);

#ifdef __cplusplus
}
#endif
#endif /* !GFF_FILESCN_H */
