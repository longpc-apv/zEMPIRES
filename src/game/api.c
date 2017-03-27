/* ALLEGRO (Atari Low-Level Game Routines) */

#include "api.h"
#include "config.h"
#include "common.h"

#ifdef GN_DEBUG
# include <stdio.h>
#endif

volatile int32_t close_button;
volatile int32_t speed_counter;
volatile int32_t n_fps, c_fps;

void
Gn_AllegFrameRate (void)
{
 n_fps = c_fps;
 c_fps = 0;
}
END_OF_FUNCTION(Gn_AllegFrameRate);

void
Gn_AllegCloseButtonHandler (void)
{
  close_button = 1;
}
END_OF_FUNCTION(Gn_AllegCloseButtonHandler)

Gn_ContextAPI*
Gn_AllegInit (void)
{
  Gn_ContextAPI* ctx = (Gn_ContextAPI*) malloc (sizeof (Gn_ContextAPI));
  if (!ctx)
    return 0;

  if (allegro_init ())
    goto allg4_fail;

  set_color_depth (GN_BPP);

  if (set_gfx_mode (GFX_SAFE, GN_RESW, GN_RESH, 0, 0) )
    goto allg4_fail;

  if (install_keyboard () || install_timer () || (install_mouse () < 0))
    goto allg4_fail;

  if (install_sound (DIGI_AUTODETECT, MIDI_AUTODETECT, 0) )
    goto allg4_fail;

  ctx->buffer = create_bitmap (GN_RESW, GN_RESH);

  if (!ctx->buffer)
    goto allg4_fail;

  close_button = 0;
  speed_counter = 0;

  LOCK_FUNCTION (Gn_AllegFrameRate);
  LOCK_FUNCTION (Gn_AllegCloseButtonHandler);

	LOCK_VARIABLE (c_fps);
	LOCK_VARIABLE (n_fps);

	install_int_ex (Gn_AllegFrameRate, BPS_TO_TIMER(1));
  set_close_button_callback (Gn_AllegCloseButtonHandler);

  set_window_title (GN_APPTITLE);
  return ctx;

allg4_fail:
#ifdef GN_DEBUG
  printf ("\n Allegro4 has encountered a problem: \n\t%s\n", allegro_error);
#endif
  allegro_exit ();
  return GN_NULL;
}

void
Gn_AllegExit (Gn_ContextAPI* ctx)
{
  if (ctx)
    {
      if (ctx->buffer)
        {
          destroy_bitmap (ctx->buffer);
        }
      free (ctx);
      ctx = GN_NULL;
    }
  allegro_exit ();
}

BITMAP*
Gn_BitmapFromRGB (Gn_Byte *rgb, int32_t w, int32_t h)
{
  int32_t i, j, c;
  Gn_Byte r, g, b;

  BITMAP *dst = create_bitmap(w, h);

  if (!dst)
    return GN_NULL;

  for (i = 0; i < h; ++i)
    for (j = 0; j < w; ++j)
  {
    r = *rgb++;
    g = *rgb++;
    b = *rgb++;
      c = makecol(r,g,b);
      putpixel(dst, j, i, c);
  }

  return dst;
}
