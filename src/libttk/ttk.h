
/*
 * ANSI-C Tiny Toolkit
 * Copyright (c) 2017, Andre Caceres Carrilho
 */

#ifndef TTK_H
#define TTK_H

#define TTK_FAILURE     -1
#define TTK_SUCCESS     0

#define TTK_TRUE        1
#define TTK_FALSE       0

#define TTK_SEEK_SET    0
#define TTK_SEEK_CUR    1
#define TTK_SEEK_END    2

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <wchar.h>

uint32_t Ttk_UpdateCRC32 (uint32_t crc, const void* src, uint64_t length);
uint32_t Ttk_FileGetCRC32 (const char* sz_path);
uint64_t Ttk_FileGetSize (const char* sz_path);

char* Ttk_FileGetExt (const char* sz_path);
char* Ttk_FileFixExt (const char* sz_path, const char* sz_ext);

int Ttk_StrCmpI (const char* sz_lhs, const char* sz_rhs);
int Ttk_StrCmpIn (const char* sz_lhs, const char* sz_rhs, uint64_t n);

typedef struct {
    uint64_t   length;    /* Encoded and Decoded sizes */
    uint64_t   offset;    /* Current position */
    void*     data;       /* Pointer to data chunk */
} TtkBuffer;

TtkBuffer* Ttk_BufAlloc (uint64_t buffer_size);
void Ttk_BufFree (TtkBuffer* buf);

TtkBuffer* Ttk_BufDiskLoad (const char *sz_path);
int Ttk_BufDiskSave (const char *sz_path, const TtkBuffer *buf);

int Ttk_BufReSizeEx (TtkBuffer* buf, uint64_t new_size, int keep_data);
int Ttk_BufReSize (TtkBuffer* buf, uint64_t new_size);

int Ttk_BufMemCmp (const TtkBuffer *lhs, const TtkBuffer *rhs);
int Ttk_BufMemSet (TtkBuffer* buf, int val);

TtkBuffer* Ttk_BufCropSelect (const TtkBuffer* buf,
  uint64_t start, uint64_t length);

TtkBuffer* Ttk_BufGenCpy (const TtkBuffer* buf);
TtkBuffer* Ttk_BufMemCpy (const void* src, uint64_t length);

uint64_t Ttk_BufRead (void* dst, uint64_t entry_size,
  uint64_t entry_count, TtkBuffer* buf);

uint64_t Ttk_BufWrite (const void* src, uint64_t entry_size,
  uint64_t entry_count, TtkBuffer* dst);

uint64_t Ttk_BufSeek (TtkBuffer* buf,
  uint64_t offset, uint64_t origin);

uint64_t Ttk_BufMemMove (TtkBuffer *src, TtkBuffer *dst, uint64_t length);

char* Ttk_BufGenStrUTF8 (const TtkBuffer* buf, uint64_t* dst_len);
wchar_t* Ttk_BufGenStrUTF16 (const TtkBuffer* buf, uint64_t* dst_len);

float Ttk_BufStrToFlt32 (TtkBuffer* buf);
double Ttk_BufStrToFlt64 (TtkBuffer* buf);

int32_t Ttk_BufStrToInt32 (TtkBuffer* buf);
int64_t Ttk_BufStrToInt64 (TtkBuffer* buf);

uint32_t Ttk_BufHashCRC32 (const TtkBuffer* buf);

#ifdef __cplusplus
}
#endif
#endif /* !TTK_H */
