
#include "age1.h"

/*
 * Gn_FixTopHUD ()
 * The top bar SLP whose index into Interfac.DRS is 50734 have incorrect
 * height when parsed (177 instead of 20 pixels). This function just "crops"
 * the BITMAP in order to be compatible with the other 3 top bar HUDs.
 */
BITMAP*
Gn_FixTopHUD (BITMAP *src)
{
  BITMAP *fix_bmp = GN_NULL;

  if (!src)
    return GN_NULL;

  fix_bmp = create_bitmap (640, 20);
  blit (src, fix_bmp, 0, 0, 0, 0, fix_bmp->w, fix_bmp->h);

  destroy_bitmap (src);
  src = GN_NULL;

  return fix_bmp;
}
