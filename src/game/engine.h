
#ifndef GN_ENGINE_H
#define GN_ENGINE_H

#include "assets.h"
#include "pathfind.h"

typedef enum
  {
    GN_PROGRAM_MENU,    /* Initial menu */
    GN_PROGRAM_LOBBY,   /* Lobby for deathmatch */
    GN_PROGRAM_OPTIONS, /* Lobby configuration */
    GN_PROGRAM_MATCH,   /* In game */
    GN_PROGRAM_STATS    /* After game statistics */
  }
Gn_ProgramState;

/* Engine systems */
typedef struct
  {
    Gn_ContextAPI *api;
    Gn_FileSystem *ffx;
    Gn_AssetSystem *gfx;

    Gn_SoundSystem *sfx;
    Gn_KeyInput *keys;

  /*  Gn_PathSystem *pfs; */  /* Path Finding system */

    Gn_ProgramState state;
    Gn_Byte paused;
  }
Gn_EngineContext;

Gn_EngineContext* Gn_EngineInit (void);
void Gn_EngineExit (Gn_EngineContext *ctx);
void Gn_SwitchBackGround (Gn_EngineContext *ctx, int32_t file_index);

#endif /* !GN_ENGINE_H */
