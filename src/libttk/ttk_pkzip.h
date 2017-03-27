
#ifndef TTK_PKZIP_H
#define TTK_PKZIP_H

#ifndef TTK_ENCODE_H
#error Yout must include 'ttk_encode.h' before 'ttk_pkzip.h'
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t crc32;
    uint32_t enc_size;
    uint32_t dec_size;
} PkZipDesc;

/* Local file header */
typedef struct {
    uint32_t  signature;    /* 0x04034b50 */
    uint16_t  required_version;
    uint16_t  gen_purp_flag;
    uint16_t  encode_method;
    uint16_t  last_mod_time;
    uint16_t  last_mod_date;
    PkZipDesc descriptor;
    uint16_t  file_name_length;
    uint16_t  extra_field_length;
    char*   file_name;
    char*   extra_field;
} PkZipLocal;

typedef struct {
    PkZipLocal header;
    uint8_t* data;
    PkZipDesc descriptor;
} PkZipEntry;

/* File header */
typedef struct {
    uint32_t  signature;  /* 0x02014b50 */
    uint16_t  version_madeby;
    uint16_t  required_version;
    uint16_t  gen_purp_flag;
    uint16_t  encode_method;
    uint16_t  last_mod_time;
    uint16_t  last_mod_date;
    PkZipDesc descriptor;
    uint16_t  file_name_length;
    uint16_t  extra_field_length;
    uint16_t  file_comment_length;
    uint16_t  disk_number_start;
    uint16_t  intern_attr;
    uint32_t  extern_attr;
    uint32_t  rel_offset_lochdr;
    char* file_name;
    char* extra_field;
    char* comment;
} PkZipHeader;

typedef struct {
    uint32_t  signature;  /* 0x05054b50 */
    uint16_t  data_size;
    uint8_t*  data;
} PkZipSign;

typedef struct {
  PkZipHeader* headers;
  PkZipSign signature;
} PkZipDir;

typedef struct {
  PkZipLocal* files;
  PkZipDir central_dir;
} TtkPkZip;

#ifdef __cplusplus
}
#endif
#endif /* !TTK_PKZIP_H */
