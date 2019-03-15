#pragma once
#include "RTSPathfinding.h"


using namespace geEngineSDK;

class RTSNode;

class RTSBestFirstSearchPathfinding : public RTSPathfinding
{

public:

  RTSBestFirstSearchPathfinding(RTSTiledMap* _pTiledMap);

  ~RTSBestFirstSearchPathfinding();

  void init();

  bool startSearch();

  SEARCH_STATE updateSearch();

  bool addConnection(Vector2I _neighbor);
};
