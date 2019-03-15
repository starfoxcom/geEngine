#pragma once
#include "RTSPathfinding.h"


using namespace geEngineSDK;

class RTSNode;

class RTSDepthFirstSearchPathfinding : public RTSPathfinding
{

public:

  RTSDepthFirstSearchPathfinding(RTSTiledMap* _pTiledMap);

  ~RTSDepthFirstSearchPathfinding();

  void init();

  bool startSearch();

  SEARCH_STATE updateSearch();

  bool addConnection(Vector2I _neighbor);

};
