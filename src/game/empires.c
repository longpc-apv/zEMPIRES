
#include "empires.h"

int
main(int argc, char **argv)
{
  Gn_EngineContext *ctx = Gn_EngineInit ();

  if (!ctx)
    return 0;

  Gn_MemoryBuffer *fp;

  fp = Gn_FileSysFind (ctx->ffx, 50500);
  Gn_Palette *pal = Gn_OpenFilePAL (fp);
  Gn_MemBufFree (fp);

  fp = Gn_FileSysFind (ctx->ffx, 15000);
  Gn_FileSLP *slp = Gn_OpenFileSLP (fp);
  Gn_MemBufFree (fp);

/*  Gn_SaveSpriteFrameBMP (&slp->frames[0], pal, "frame.bmp"); */
/*  Gn_SavePaletteBMP (pal, "palette.bmp"); */

  Gn_CloseFileSLP (slp);
  Gn_CloseFilePAL (pal);

  play_midi (ctx->sfx->musics[GN_AGEMUSIC_OPEN].midi_music, 0);

  while (!close_button)
    {
      if (key[KEY_ESC])
        close_button = 1;

      clear_bitmap(screen);

      while (speed_counter > 0)
        {
          speed_counter--;
        }

      c_fps++;

      blit (ctx->gfx->background, screen, 0, 0, 0, 0,
        ctx->gfx->background->w, ctx->gfx->background->h);

      textprintf_ex(screen, font, 3,5, 0xffffff, -1, "FPS: %i", n_fps);
      vsync();
    }

  Gn_EngineExit (ctx);
  return 0;
}
END_OF_MAIN()
