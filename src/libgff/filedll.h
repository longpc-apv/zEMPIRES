
/*
 *    libGFF: Genie File Format
 *    By Andre Caceres Carrilho
 *
 *      This code is part of the zEMPIRES project, an open source
 *      reimplementation of Ensemble Studios Genie engine.
 *
 *      MS-DOS/Windows NT portable executable (PE) file format
 *      Credits: Johannes Plachy, Mikko P (Tapsa) and 'apreiml'
 *
 *      See license.md for copyright information.
 */

#ifndef GFF_FILEDLL_H
#define GFF_FILEDLL_H

#define IMAGE_DOS_SIGNATURE             0x5A4D      /* MZ */
#define IMAGE_OS2_SIGNATURE             0x454E      /* NE */
#define IMAGE_OS2_SIGNATURE_LE          0x454C      /* LE */
#define IMAGE_NT_SIGNATURE              0x00004550  /* PE00 */

#ifdef __cplusplus
extern "C" {
#endif

/* DOS .EXE header */
typedef struct {
  uint16_t  magic;            /* Magic number */
  uint16_t  cblp;             /* Bytes on last page of file */
  uint16_t  cp;               /* Pages in file */
  uint16_t  crlc;             /* Relocations */
  uint16_t  cparhdr;          /* Size of header in paragraphs */
  uint16_t  minalloc;         /* Minimum extra paragraphs needed */
  uint16_t  maxalloc;         /* Maximum extra paragraphs needed */
  uint16_t  ss;               /* Initial (relative) SS value */
  uint16_t  sp;               /* Initial SP value */
  uint16_t  csum;             /* Checksum */
  uint16_t  ip;               /* Initial IP value */
  uint16_t  cs;               /* Initial (relative) CS value */
  uint16_t  lfarlc;           /* File address of relocation table */
  uint16_t  ovno;             /* Overlay number */
  uint16_t  res[4];           /* Reserved words */
  uint16_t  oemid;            /* OEM identifier (for e_oeminfo) */
  uint16_t  oeminfo;          /* OEM information; e_oemid specific */
  uint16_t  res2[10];         /* Reserved words */
  int32_t   lfanew;           /* File address of new EXE header */
} DOS_header_t;

typedef struct {
  uint16_t  machine;
  uint16_t  number_sections;
  uint32_t  time_date_stamp;
  uint32_t  pointer_symbol_table;
  uint32_t  number_symbols;
  uint16_t  size_optional_header;
  uint16_t  characteristics;
} IMG_header_t;

typedef struct {
  uint32_t  rva;      /* Virtual address */
  uint32_t  size;
} IMG_directory_t;

#define DATA_DIRECTORY_COUNT 16
typedef struct {
  /* Standard fields */
  uint16_t  magic;
  uint8_t   major_linker_version;
  uint8_t   minor_linker_version;
  uint32_t  size_of_code;
  uint32_t  size_of_initialized_data;
  uint32_t  size_of_uninitialized_data;
  uint32_t  adress_entry_point;
  uint32_t  base_of_code;
  uint32_t  base_of_data;
  /* NT additional fields */
  uint32_t  image_base;
  uint32_t  section_alignment;
  uint32_t  file_alignment;
  uint16_t  major_os_version;
  uint16_t  minor_os_version;
  uint16_t  major_img_version;
  uint16_t  minor_img_version;
  uint32_t  reserved1;
  uint32_t  size_img;
  uint32_t  size_headers;
  uint32_t  checksum;
  uint16_t  subsystem;
  uint16_t  dll_characteristics;
  uint32_t  size_stack_reserve;
  uint32_t  size_stack_commit;
  uint32_t  size_heap_reserve;
  uint32_t  size_heap_commit;
  uint32_t  loader_flags;
  uint32_t  number_rva_sizes;
  IMG_directory_t data_directory[DATA_DIRECTORY_COUNT];
} IMG_optional_t;

#define IMAGE_SIZEOF_SHORT_NAME 8
typedef struct {
  char  name[IMAGE_SIZEOF_SHORT_NAME];
  union {
    uint32_t  physical_address;
    uint32_t  virtual_size;
  } misc;
  uint32_t  virtual_address;
  uint32_t  size_raw_data;
  uint32_t  pointer_raw_data;
  uint32_t  pointer_relocations;
  uint32_t  pointer_linenumbers;
  uint16_t  number_relocations;
  uint16_t  number_linenumbers;
  uint32_t  characteristics;
} IMG_section_t;

typedef struct {
  uint32_t offset;
  uint32_t size;
  uint32_t codepage;
  uint32_t reserved;
} IMG_entry_t;

typedef struct {
  DOS_header_t    dos_header;
  IMG_header_t    file_header;
  IMG_optional_t  *optional_header;
  IMG_section_t   *section_table;
} GDLLFile;

GDLLFile* GDLL_DecodeFile(const TtkBuffer *mem_buf);
void GDLL_DestroyFile(GDLLFile *dll_file);

#ifdef __cplusplus
}
#endif
#endif /* !GFF_FILEDLL_H */

