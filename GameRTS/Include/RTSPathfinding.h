#pragma once
#include <gePrerequisitesUtil.h>
#include <geVector2I.h>

using namespace geEngineSDK;

class RTSTiledMap;
class RTSNode;

class RTSPathfinding
{
public:

  enum SEARCH_STATE
  {
    idle = 0,
    onSearch,
    goalReached,
    goalNotReached
  };

  RTSPathfinding();

  ~RTSPathfinding();

  virtual void init() = 0;

  virtual bool startSearch() = 0;

  virtual SEARCH_STATE updateSearch() = 0;

  virtual bool resetSearch() = 0;

  virtual bool addConnection(Vector2I _possibleConnection) = 0;

  Vector<Vector2I> backTrace(RTSNode* _node);

  Vector2I getStartPos();
  
  Vector2I getTargetPos();

  void setStartPos(int32 _x, int32 _y);

  void setTargetPos(int32 _x, int32 _y);

  SEARCH_STATE getCurrentState();

  void setCurrentState(SEARCH_STATE _state);

  RTSNode * getCurrentNode();

  bool checkList(Vector<RTSNode> _list, Vector2I _position);

protected:

  Vector2I m_startPos;
  Vector2I m_targetPos;

  SEARCH_STATE m_currentState;

  RTSTiledMap* m_pTiledMap;

  Vector<RTSNode> m_nextNodes;
  Vector<RTSNode> m_visited;

  Vector<Vector2I> m_nextPositions;

  RTSNode* m_pCurrent;
};