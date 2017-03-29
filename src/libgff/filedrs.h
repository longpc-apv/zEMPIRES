
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

#ifndef GFF_FILEDRS_H
#define GFF_FILEDRS_H


#ifdef __cplusplus
extern "C" {
#endif

/*
 * Data resource (.DRS) file format
 *
 * The Age of Empires data resource files contain the resources (graphics,
 * sounds, other binary info) used by MS AoE. They are large files that
 * contain other smaller files (embedded files).
 */

/* Genie DRS file header */
typedef struct {
  /*
   * The version of Genie engine used in Star Wars: Galactic
   * Battlegrounds had 60 char string for the copyright.
   * NOTE: this strings are not zero terminated, be careful when using.
   */
  char      copyright[GN_DRS_COPYRIGHT];
  char      file_version[4];
  char      file_type[12];

  /*
   * Each DRS file can store various tables (folders), and each table can store
   * various files (entries), however, only one file type for table is allowed
   */
  int32_t   table_header_count;
  int32_t   table_header_offset;
} GDRSHeader;

/* Genie DRS table header */
typedef struct {
  Gn_Byte     file_type;
  char        rev_extension[3];    /* File extention (reversed) */

  /*
   * NOTE: the order of offset/count on the header is different of the other
   * structures. Maybe a standardization problem during game development.
   */
  int32_t     file_headers_offset;
  int32_t     file_header_count;
} GDRSTableHeader;

/*
 * Genie DRS file entry (header)
 * Every game file has a different index, i.e. we can use its index to serach
 * on all opened DRS files (which is what the filesystem does).
 */
typedef struct {
  int32_t     index;
  int32_t     offset;
  int32_t     length;
} GDRSEntry;

typedef struct {
  GDRSTableHeader   header;
  GDRSEntry       *entries;
} GDRSTable;

/* Genie DRS file entry (data) */
typedef struct GDRSFile {
  GDRSHeader          header;
  GDRSTable         *tables;
} GDRSFile;

GDRSFile* GDRS_DecodeFile(TtkBuffer *mem_buf);
void GDRS_DestroyFile(GDRSFile *drs_file);

#ifdef __cplusplus
}
#endif
#endif /* GFF_FILEDRS_H */
