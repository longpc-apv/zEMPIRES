
#ifndef GN_ASSETS_H
#define GN_ASSETS_H

#include "filesys.h"
#include "palette.h"
#include "sprite.h"

/* Musics that keep playing (repeating) during the gameplay */
#define GN_AGEMUSIC_1         0
#define GN_AGEMUSIC_2         1
#define GN_AGEMUSIC_3         2
#define GN_AGEMUSIC_4         3
#define GN_AGEMUSIC_5         4
#define GN_AGEMUSIC_6         5
#define GN_AGEMUSIC_7         6
#define GN_AGEMUSIC_8         7
#define GN_AGEMUSIC_9         8
#define GN_SOUNDTRACK_MATCH   9

/* Start menu, won, and lost soundtracks */
#define GN_AGEMUSIC_OPEN      9
#define GN_AGEMUSIC_LOST      10
#define GN_AGEMUSIC_WON       11
#define GN_SOUNDTRACK_SIZE    12

typedef struct
  {
   /*
    * It can be a lot of sound effects going on during the match, but with
    * only one music at the time. This pointer is used just make easy the
    * management of the musics.
    */
    Gn_Music *music_current;

    Gn_Music *musics;
    Gn_Sound *sounds;
  }
Gn_SoundSystem;

Gn_SoundSystem* Gn_SoundSysInit (void);
void Gn_SoundSysExit (Gn_SoundSystem *sfx);

/* Animation data */
typedef struct
  {
    int32_t frame_count;
    Gn_Sprite *frames;
  }
Gn_Anim;

/* Animation player */
typedef struct
  {
    Gn_Anim *data;
    Gn_Byte playing;
    int32_t frame_current;
  }
Gn_AnimHandler;

#define GN_AGEBG_COUNT 9

/* Resource Manager */
typedef struct
  {
   /*
    * All interfac.drs 8 bit RGB palettes (IDs from 50500 to 50508)
    * The default game palette is the first one (index 50500).
    * The other palettes are used for background loading.
    */
    Gn_Palette **palette;   /* [GN_AGEBG_COUNT] */

   /*
    * Current background BITMAP
    * Although we could store all backgrounds in the memory RAM (is not a
    * problem on modern hardware), we opted to allocate just the current
    * background. When needed, the new BITMAP will be loaded from disk.
    * See: Gn_SwitchBackGround (), Gn_LoadPalettes ()
    */
    BITMAP *background;

 //   Gn_Sprite *sprites;

 //   Gn_Anim *anims;
 //   Gn_Music *musics;
 //   Gn_Sound *sounds;
  }
Gn_AssetSystem;

Gn_AssetSystem* Gn_AssetsInit (void);
void Gn_AssetsExit (Gn_AssetSystem *gfx);

//Gn_Anim* Gn_GetAnimFromSLP (const Gn_FileSLP *slp);
//void Gn_FreeAnim (Gn_Anim *anim);

#endif /* !GN_ASSETS_H */
