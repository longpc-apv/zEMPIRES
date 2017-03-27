
#include "assets.h"

Gn_AssetSystem*
Gn_AssetsInit (void)
{
  int32_t i;
  Gn_AssetSystem *gfx = (Gn_AssetSystem *) malloc (sizeof (Gn_AssetSystem));

  if (!gfx)
    return GN_NULL;

  gfx->palette = (Gn_Palette**) malloc (sizeof(Gn_Palette*) * 9);

  if (!gfx->palette)
    {
      free (gfx);
      return GN_NULL;
    }

  for (i = 0; i < GN_AGEBG_COUNT; ++i)
    gfx->palette[i] = GN_NULL;

  gfx->background = GN_NULL;
  return gfx;
}

void
Gn_AssetsExit (Gn_AssetSystem *gfx)
{
  int32_t i;

  if (!gfx)
    return;

  if (gfx->palette)
    {
      for (i = 0; i < GN_AGEBG_COUNT; ++i)
        Gn_CloseFilePAL (gfx->palette[i]);

      free (gfx->palette);
    }

  free (gfx);
  gfx = GN_NULL;
}

static int32_t
Gn_LoadMidiMusics (Gn_SoundSystem *sfx)
{
  int32_t i;
  char midi_name[32];

  if (!sfx)
    return GN_FAILURE;

  for (i = 0; i < GN_SOUNDTRACK_MATCH; ++i)
    {
      sprintf (midi_name, "sound/music%d.mid", i+1);
      sfx->musics[i].midi_music = load_midi (midi_name);

      if (!sfx->musics[i].midi_music)
        return GN_FAILURE;
    }

  sfx->musics[GN_AGEMUSIC_OPEN].midi_music = load_midi ("sound/open.mid");
  sfx->musics[GN_AGEMUSIC_LOST].midi_music = load_midi ("sound/lost.mid");
  sfx->musics[GN_AGEMUSIC_WON].midi_music = load_midi ("sound/won.mid");

  for (i = GN_AGEMUSIC_OPEN; i < GN_SOUNDTRACK_SIZE; ++i)
    if (!sfx->musics[i].midi_music)
      return GN_FAILURE;

  return GN_SUCESS;
}

Gn_SoundSystem*
Gn_SoundSysInit (void)
{
  Gn_SoundSystem *sfx = (Gn_SoundSystem *) malloc (sizeof (Gn_SoundSystem));

  if (!sfx)
    return GN_NULL;

  sfx->musics = (Gn_Music *) malloc (sizeof (Gn_Music) * GN_SOUNDTRACK_SIZE);

  if (!sfx->musics)
    {
      free (sfx);
      return GN_NULL;
    }

  if (Gn_LoadMidiMusics (sfx))
    {
      free (sfx->musics);
      free (sfx);
      return GN_NULL;
    }

  return sfx;
}

void
Gn_SoundSysExit (Gn_SoundSystem *sfx)
{
  int32_t i;

  if (!sfx)
    return;

  if (sfx->musics)
    {
      for (i = 0; i < GN_SOUNDTRACK_SIZE; ++i)
        destroy_midi (sfx->musics[i].midi_music);

      free (sfx->musics);
    }

  free (sfx);
  sfx = GN_NULL;
}
