

static int
Ttk_BufRLEncEx(void *in, void *out, int length)
{
  uint8_t *src = (uint8_t*)in;
  uint8_t *dst = (uint8_t*)out;

  uint8_t *start = src;
  uint8_t *end = src + length;

  uint8_t *next;

  uint8_t x;
  int32_t c;
  int32_t r;

  while (src < end)
    {
      next = src;

      x = *next++;
      while (next < end && (next - start) < 0x7f && *next == x)
        next++;

      c = next - src;

      if (c > 1)
        {
          *dst++ = c;
          *dst++ = *src;
          src += c;
        }
      else
        {
          next = src;
          r = 0;

          while (next < end && r < 0x7f)
            {
              r = next++ - src;

              if (r > 1 && *next == *(next-1) && *next == *(next-2))
                break;
            }

          while (next < end && *next == *(next-1))
            next--;

          c = next - src;

          *dst++ = c + 0x7f;
          memcpy(dst, src, c);
          dst += c;
          src += c;
        }
    }
  return dst - (uint8_t*)out;
}

static void
Ttk_BufRLDecEx(void *in, void *out, int length)
{
  uint8_t *src = (uint8_t*)in;
  uint8_t *dst = (uint8_t*)out;
  uint8_t *end = src + length;
  uint8_t c;

  while (src < end)
    {
      c = *src++;
      if (c > 0x7f)
        {
          c -= 0x7f;
          memcpy(dst, src, c);
          src += c;
          dst += c;
        }
      else
        {
          memset(dst, *src++, c);
          dst += c;
        }
    }
}

TtkBuffer*
Ttk_BufRLEnc(const TtkBuffer* buf)
{
}

TtkBuffer*
Ttk_BufRLDec(const TtkBuffer* buf, uint64_t decode_size)
{
}
