
#ifndef GN_ACTOR_H
#define GN_ACTOR_H

#include "age1.h"
#include "assets.h"

typedef struct
  {
    Gn_AnimHandler *anim_handler;

    #if 0
    int16_t hp_current;
    int16_t hp_max;

    int16_t combat_armor;
    int16_t combat_attack;
    int16_t combat_range;
    int16_t combat_shield;

    /* Fog of war */
    Gn_Byte grants_vision;
    Gn_Byte visibility_radius;

   /*
    * Villagers and fishing boats can carry resources
    * Current and maximum carriable resrouces
    */
    Gn_AoeResourceType resource_type;
    Gn_Byte resource_carrying;
    Gn_Byte resource_max;

    /*
     * Because we can update the working speed for different resources
     * on the "market" building, therefore we need to keep track of it
     */
    Gn_Byte work_speed[GN_RSRC_COUNT];

    Gn_Byte velocity;
    Gn_Point current_cell;       /* World cell position (for path finding) */
    #endif
  }
Gn_Actor;

#endif /* !GN_ACTOR_H */
