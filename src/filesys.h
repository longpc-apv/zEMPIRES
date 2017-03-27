
typedef struct GDiskFile_s {
  char *file_name; /* sz_??? */
  FILE *disk_file;

  struct GDiskFile_s *next;
} GDiskFile;

#define GFF_FORMAT_UNK
#define GFF_FORMAT_BIN
#define GFF_FORMAT_SLP
#define GFF_FORMAT_WAV

typedef struct GRsrcEntry_s {
  int32_t index;
  int32_t format;

  /*
   * Although not a problem on modern hardware, we dont load all DRS files to
   * memory RAM, instead, we keep the files opened and just seek the desired
   * entry and load from the disk at runtime.
   */
  FILE    *disk_file;

  int32_t offset;
  int32_t length;

  struct GRsrcEntry_s *prev;
  struct GRsrcEntry_s *next;
} GRSRCEntry;

typedef struct {
  GDiskFile *disk_file;
  GRsrcEntry *rsrc_entry;
} GResourceSystem;
