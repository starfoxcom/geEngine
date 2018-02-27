#pragma once

//Define only if the map should be isometric
#define MAP_IS_ISOMETRIC

#ifdef MAP_IS_ISOMETRIC
//Tiles pixel size
# define TILESIZE_X 64
# define TILESIZE_Y 32
#else
//Tiles pixel size
# define TILESIZE_X 64
# define TILESIZE_Y 64
#endif

#include <geModule.h>
#include <geVector2I.h>

using namespace geEngineSDK;

class GameOptions : public Module<GameOptions>
{
public:
  static Vector2I s_Resolution;

  static const Vector2I TILEHALFSIZE;
  static const Vector2I BITSFHT_TILEHALFSIZE;
  static const Vector2I BITSHFT_TILESIZE;
};

GameOptions&
g_gameOptions();
