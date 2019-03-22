#pragma once
#include "RTSPathfinding.h"


using namespace geEngineSDK;

class RTSNode;

class RTSDijkstraPathfinding : public RTSPathfinding
{

public:

  RTSDijkstraPathfinding(RTSTiledMap* _pTiledMap);

  ~RTSDijkstraPathfinding();

  void init();

  bool startSearch();

  SEARCH_STATE updateSearch();

  bool addConnection(Vector2I _neighbor);
};
