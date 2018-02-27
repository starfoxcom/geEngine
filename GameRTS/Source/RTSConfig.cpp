#include "RTSConfig.h"

const Vector2I
GameOptions::TILEHALFSIZE = Vector2I(TILESIZE_X >> 1, TILESIZE_Y >> 1);

const Vector2I
GameOptions::BITSHFT_TILESIZE = Vector2I(
  Math::countTrailingZeros(GameOptions::TILEHALFSIZE.x),
  Math::countTrailingZeros(GameOptions::TILEHALFSIZE.y)
);

const Vector2I
GameOptions::BITSFHT_TILEHALFSIZE = Vector2I(GameOptions::BITSHFT_TILESIZE.x - 1,
  GameOptions::BITSHFT_TILESIZE.y - 1);

GameOptions&
g_gameOptions() {
  return GameOptions::instance();
}