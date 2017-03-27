
#include "engine.h"
#include "sprite.h"

static void
Gn_LoadPalettes (Gn_FileSystem *ffx, Gn_AssetSystem *gfx)
{
  int32_t i;
  Gn_MemoryBuffer *fp;

  if (!ffx || !gfx)
    return;

  for (i = 0; i < GN_AGEBG_COUNT; ++i)
    {
      fp = Gn_FileSysFind (ffx, 50500 + i);
      gfx->palette[i] = Gn_OpenFilePAL (fp);
      Gn_MemBufFree (fp);
    }
}

Gn_EngineContext*
Gn_EngineInit (void)
{
  Gn_EngineContext *ctx = (Gn_EngineContext *) malloc (sizeof(Gn_EngineContext));

  if (!ctx)
    return GN_NULL;

  ctx->api = Gn_AllegInit ();
  ctx->ffx = Gn_FileSysInit ();
  ctx->gfx = Gn_AssetsInit ();
  ctx->sfx = Gn_SoundSysInit ();

  if (!ctx->api || !ctx->ffx || !ctx->gfx || !ctx->sfx)
    {
      free (ctx);
      return GN_NULL;
    }

  /* Age of Empires DRS */
  Gn_FileSysPush (ctx->ffx, "data/border.drs");
  Gn_FileSysPush (ctx->ffx, "data/graphics.drs");
  Gn_FileSysPush (ctx->ffx, "data/interfac.drs");
  Gn_FileSysPush (ctx->ffx, "data/sounds.drs");
  Gn_FileSysPush (ctx->ffx, "data/terrain.drs");

  /* Rise of Rome DRS */
//  Gn_FileSysPush (ctx->ffx, "data2/graphics.drs");
//  Gn_FileSysPush (ctx->ffx, "data2/interfac.drs");
//  Gn_FileSysPush (ctx->ffx, "data2/sounds.drs");
//  Gn_DebugFileSystem (ctx->ffx);

  Gn_LoadPalettes (ctx->ffx, ctx->gfx);
  Gn_SwitchBackGround(ctx, 50100);

  return ctx;
}

void
Gn_EngineExit (Gn_EngineContext *ctx)
{
  if (!ctx)
    return;

  Gn_SoundSysExit (ctx->sfx);
  Gn_AssetsExit (ctx->gfx);
  Gn_FileSysExit (ctx->ffx);
  Gn_AllegExit (ctx->api);

  free (ctx);
}

void
Gn_SwitchBackGround (Gn_EngineContext *ctx, int32_t file_index)
{
  Gn_MemoryBuffer *fp = GN_NULL;
  Gn_FileSLP *slp = GN_NULL;
  Gn_Byte *rgb = GN_NULL;

  if (ctx->gfx->background)
    destroy_bitmap (ctx->gfx->background);
  ctx->gfx->background = GN_NULL;

  fp = Gn_FileSysFind (ctx->ffx, file_index);
  slp = Gn_OpenFileSLP (fp);

  switch (file_index)
    {
      case 50100:
      case 50103:
      case 50105:
        rgb = Gn_ConvertSpriteFrameRGB (& slp->frames[0],
          ctx->gfx->palette[1]);
      break;

      case 50107:
      case 50109:
      case 50111:
        rgb = Gn_ConvertSpriteFrameRGB (& slp->frames[0],
          ctx->gfx->palette[2]);
      break;

      case 50113:
      case 50115:
      case 50117:
        rgb = Gn_ConvertSpriteFrameRGB (& slp->frames[0],
          ctx->gfx->palette[3]);
      break;

      case 50119:
      case 50121:
      case 50123:
        rgb = Gn_ConvertSpriteFrameRGB (& slp->frames[0],
          ctx->gfx->palette[4]);
      break;

      case 50131:
      case 50133:
      case 50135:
        rgb = Gn_ConvertSpriteFrameRGB (& slp->frames[0],
          ctx->gfx->palette[5]);
      break;

      case 50137:
      case 50139:
      case 50141:
        rgb = Gn_ConvertSpriteFrameRGB (& slp->frames[0],
          ctx->gfx->palette[6]);
      break;

      case 50143:
      case 50145:
      case 50147:
        rgb = Gn_ConvertSpriteFrameRGB (& slp->frames[0],
          ctx->gfx->palette[7]);
      break;

      case 50149:
      case 50151:
      case 50153:
        rgb = Gn_ConvertSpriteFrameRGB (& slp->frames[0],
          ctx->gfx->palette[8]);
      break;
    }

  ctx->gfx->background = Gn_BitmapFromRGB(rgb,
    slp->frames[0].header.size_w, slp->frames[0].header.size_h);

  Gn_MemBufFree (fp);
  Gn_CloseFileSLP (slp);
  free (rgb);
}
