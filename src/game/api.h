
#ifndef GN_ALLEGAPI_H
#define GN_ALLEGAPI_H
#pragma once

#include "common.h"

#include <allegro.h>

/*
 * Attention: Do not include <windows.h> directly! This will cause ambiguity
 * between Allegro API and WinAPI BITMAP structures. Fortunately, Allegro 4.2
 * provide us with a workaround header <winalleg.h> to solve this issue.
 * However, we need to include <allegro.h> first.
 */
#ifdef __WIN32
#include <winalleg.h>
#endif /* !__WIN32 */

extern volatile int32_t close_button;
extern volatile int32_t speed_counter;
extern volatile int32_t n_fps, c_fps;

typedef struct
  {
    MIDI *midi_music;
    int32_t duration;
  }
Gn_Music;

typedef struct
  {
    SAMPLE *wav_sound;
  }
Gn_Sound;

typedef struct
  {
    Gn_Byte key_code;
    Gn_Byte pressed_now;
    Gn_Byte pressed_old;
  }
Gn_KeyPress;

typedef struct
  {
    Gn_KeyPress *key;
  }
Gn_KeyInput;

void Gn_UpdateKeyStates (Gn_KeyInput *keys);

typedef struct
  {
    BITMAP *buffer;
  }
Gn_ContextAPI;

Gn_ContextAPI* Gn_AllegInit (void);
void Gn_AllegExit (Gn_ContextAPI* ctx);

BITMAP* Gn_BitmapFromRGB (Gn_Byte *rgb, int32_t w, int32_t h);

#endif /* !GN_ALLEGAPI_H */
