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
#include <geVector2.h>
#include <geColor.h>

using namespace geEngineSDK;

class GameOptions : public Module<GameOptions>
{
 public:
  //Application values
  static Vector2I s_Resolution;
  static Vector2 s_MapMovementSpeed;

  //World Values

  //Map Values
  static bool s_MapShowGrid;
  static bool s_MapShowPath;
  static Color s_MapGridColor;

  //Global Constants
  static const Vector2I TILEHALFSIZE;
  static const Vector2I BITSFHT_TILEHALFSIZE;
  static const Vector2I BITSHFT_TILESIZE;
};

class EditorOptions : public Module<EditorOptions>
{
public:
  //Radial button Id
  static int32 s_selected;

  //brush size
  static int32 s_brushSize;

  //open/close flag
  static bool s_editorIsOpen;
};

class PathfindingOptions : public Module<PathfindingOptions>
{
public:
  //Radial button Id
  static int32 s_selected;

  //open/close flag
  static bool s_editorIsOpen;

  //Radial button Id
  static int32 s_posSelected;
};

GameOptions&
g_gameOptions();

EditorOptions&
g_editorOptions();

PathfindingOptions&
g_pathfindingOptions();
