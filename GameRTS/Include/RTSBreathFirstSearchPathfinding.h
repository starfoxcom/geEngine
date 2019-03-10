#pragma once
#include "RTSPathfinding.h"


using namespace geEngineSDK;

class RTSNode;

class RTSBreathFirstSearchPathfinding : public RTSPathfinding
{

public:

  RTSBreathFirstSearchPathfinding(RTSTiledMap* _pTiledMap);

  ~RTSBreathFirstSearchPathfinding();

  void init();

  bool startSearch();

  SEARCH_STATE updateSearch();

  bool resetSearch();

  bool addConnection(Vector2I _neighbor);

};
