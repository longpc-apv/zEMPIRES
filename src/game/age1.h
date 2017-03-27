
#ifndef GN_AGE1_H
#define GN_AGE1_H

#include "common.h"
#include "api.h"

typedef enum
  {
    GN_ACTOR_VILLAGER,
    GN_ACTOR_WARRIOR_MELEE,
    GN_ACTOR_WARRIOR_RANGE,
    GN_ACTOR_WARRIOR_MOUNT,
    GN_ACTOR_WARRIOR_SIEGE,   /* catapult */
    GN_ACTOR_ANIMAL,
    GN_ACTOR_PRIEST,
    GN_ACTOR_COUNT
  }
Gn_AoeActorType;

typedef enum
  {
    GN_ANIMAL_ALLIGATOR   = 0,
    GN_ANIMAL_LION        = 1,
    GN_ANIMAL_ELEPHANT    = 2,
    GN_ANIMAL_GAZELLE     = 3,
    GN_ANIMAL_COUNT       = 4
  }
Gn_AoeAnimalType;

typedef enum
  {
    /* Basic resources */
    GN_RSRC_FOOD          = 0,
    GN_RSRC_WOOD          = 1,
    GN_RSRC_GOLD          = 2,
    GN_RSRC_ROCK          = 3,
    /* Food sources */
    GN_RSRC_FISH          = 4,
    GN_RSRC_HUNT          = 5,
    GN_RSRC_FRUIT         = 6,
    /* Resrouce count */
    GN_RSRC_COUNT         = 7
  }
Gn_AoeResourceType;

typedef enum
  {
    GN_AGE_STONE          = 0,
    GN_AGE_TOOL           = 1,
    GN_AGE_BRONZE         = 2,
    GN_AGE_IRON           = 3,
    GN_AGE_COUNT          = 4
  }
Gn_AoeTechAge;

typedef enum
  {
    /* Stone age */
    GN_BLDNG_TOWNC, /* town center */
    GN_BLDNG_BRRCK, /* barracks */
    GN_BLDNG_STPIT, /* storage pit */
    GN_BLDNG_GRNRY, /* granary */
    GN_BLDNG_DOCKS,
    GN_BLDNG_HOUSE,
    /* Tool age */
    GN_BLDNG_ARCHR, /* archery range */
    GN_BLDNG_STABL, /* stable */
    GN_BLDNG_MARKT, /* market */
    GN_BLDNG_SWALL, /* small wall */
    GN_BLDNG_WTTWR, /* watch tower */
    GN_BLDNG_FARMS,
    /* Bronze age */
    GN_BLDNG_GMTCT, /* government center */
    GN_BLDNG_TEMPL, /* temple */
    GN_BLDNG_SIEGE, /* siege workshop */
    GN_BLDNG_ACDMY, /* academy */
    GN_BLDNG_MWALL, /* medium wall */
    GN_BLDNG_STTWR, /* sentry tower */
    /* Iron age */
    GN_BLDNG_WONDR, /* wonder */
    GN_BLDNG_FWALL, /* wall fortification */
    GN_BLDNG_GDTWR, /* guard tower */
    GN_BLDNG_BLTWR, /* ballista tower */
    /* Count */
    GN_BLDNG_COUNT
  }
Gn_AoeBuildingType;

typedef enum
  {
    GN_REGION_ASIAN_EAST,     /* Yamato, Shang, Choson */
    GN_REGION_ASIAN_CENTRAL,  /* Babylonians, Persians, Hittite */
    GN_REGION_EASTERN_MIDDLE, /* Egyptians, Assyrians, Sumerians */
    GN_REGION_MEDITERRANEAN,  /* Greeks, Minoans, Phoenicians */
    GN_REGION_ROMAN,          /* Romans, Macedonians, Carthaginians, Palmyrans */
    GN_REGION_COUNT
  }
Gn_AoeRegion;

typedef enum
  {
    GN_CIV_ASSYRIAN,
    GN_CIV_BYZANTINES,
    GN_CIV_BABYLONIANS,
    GN_CIV_CHOSON,
    GN_CIV_EGYPTIANS,
    GN_CIV_GREEKS,
    GN_CIV_HITTITES,
    GN_CIV_MINOANS,
    GN_CIV_PERSIANS,
    GN_CIV_PHOENICIANS,
    GN_CIV_SHANG,
    GN_CIV_SUMERIANS,
    GN_CIV_YAMATO,
    GN_CIV_CARTHAGINIANS,
    GN_CIV_MACEDONIANS,
    GN_CIV_PALMYRANS,
    GN_CIV_ROMANS,
    GN_CIV_COUNT
  }
Gn_AoeCivilization;

BITMAP* Gn_FixTopHUD (BITMAP *src);

#endif /* !GN_AGE1_H */
