#pragma once
#include "RTSPathfinding.h"


using namespace geEngineSDK;

class RTSNode;

class RTSAStarPathfinding : public RTSPathfinding
{

public:

  RTSAStarPathfinding(RTSTiledMap* _pTiledMap);

  ~RTSAStarPathfinding();

  void init();

  bool startSearch();

  SEARCH_STATE updateSearch();

  bool addConnection(Vector2I _neighbor);
};
