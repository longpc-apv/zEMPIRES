
#ifndef TTK_ENCODE_H
#define TTK_ENCODE_H

#ifndef TTK_H
#error Yout must include 'ttk.h' before 'ttk_encode.h'
#endif

#ifdef __cplusplus
extern "C" {
#endif

TtkBuffer*  Ttk_BufRLEnc(const TtkBuffer* buf);
TtkBuffer*  Ttk_BufRLDec(const TtkBuffer* buf, uint64_t decode_size);

TtkBuffer*   Ttk_BufEncZlibEx(const TtkBuffer* buf,
  int encode_level, int encode_strategy);

TtkBuffer*   Ttk_BufDecZlibEx(const TtkBuffer* buf,
  int decode_level, int decode_strategy, uint64_t decode_size);

TtkBuffer*   Ttk_BufEncLZMA(const TtkBuffer* buf);
TtkBuffer*   Ttk_BufDecLZMA(const TtkBuffer* buf);

TtkBuffer*   Ttk_BufEncBzip2(const TtkBuffer* buf);
TtkBuffer*   Ttk_BufDecBzip2(const TtkBuffer* buf);

#define TTK_ENCALGO_NONE     0
#define TTK_ENCALGO_RLE      1
#define TTK_ENCALGO_ZLIB     2
#define TTK_ENCALGO_LZMA     3
#define TTK_ENCALGO_BZIP2    4

typedef struct {
  TtkBuffer *enc_buf;
  TtkBuffer *dec_buf;
  uint8_t   encoded;    /* Encoded = 1 or Decoded = 0 */
  uint8_t   algorithm;  /* Unknown, none, zlib, QLZ, bzip2, LZMA, ... */
  uint8_t   level;
  uint8_t   strategy;
} TtkEncoder;

TtkEncoder* Ttk_CreateEncoder(void);
void Ttk_DestroyEncoder(TtkEncoder* ctx);

int Ttk_EncZlibDeflate(TtkEncoder* ctx);
int Ttk_EncZlibInflate(TtkEncoder* ctx);

#ifdef __cplusplus
}
#endif
#endif /* !TTK_ENCODE_H */
