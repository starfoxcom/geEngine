#include "RTSPathfinding.h"
#include "RTSNode.h"

RTSPathfinding::RTSPathfinding()
{
  m_nextPositions.emplace_back(-1, -1);
  m_nextPositions.emplace_back(0, -1);
  m_nextPositions.emplace_back(1, -1);
  m_nextPositions.emplace_back(1, 0);
  m_nextPositions.emplace_back(1, 1);
  m_nextPositions.emplace_back(0, 1);
  m_nextPositions.emplace_back(-1, 1);
  m_nextPositions.emplace_back(-1, 0);

  m_startPos = Vector2I::ZERO;
  m_targetPos = Vector2I::ZERO;
}

RTSPathfinding::~RTSPathfinding()
{
}

Vector<Vector2I> RTSPathfinding::backTrace(RTSNode* _node)
{
  Vector<Vector2I> result;
  RTSNode* pParent;
  result.push_back(_node->m_position);
  pParent = _node->m_parent;

  while (pParent != nullptr)
  {
    result.push_back(pParent->m_position);
    pParent = pParent->m_parent;
  }
  return result;
}

Vector2I RTSPathfinding::getStartPos()
{
  return m_startPos;
}

Vector2I RTSPathfinding::getTargetPos()
{
  return m_targetPos;
}

void RTSPathfinding::setStartPos(int32 _x, int32 _y)
{
  m_startPos.x = _x;
  m_startPos.y = _y;
}

void RTSPathfinding::setTargetPos(int32 _x, int32 _y)
{
  m_targetPos.x = _x;
  m_targetPos.y = _y;
}

RTSPathfinding::SEARCH_STATE RTSPathfinding::getCurrentState()
{
  return m_currentState;
}

void RTSPathfinding::setCurrentState(SEARCH_STATE _state)
{
  m_currentState = _state;
}

RTSNode * RTSPathfinding::getCurrentNode()
{
  return m_pCurrent;
}

bool RTSPathfinding::checkList(Vector<RTSNode> _list, Vector2I _position)
{
  for (auto it = _list.begin(); it < _list.end(); ++it)
  {
    if (it->m_position == _position)
    {
      return true;
    }
  }

  return false;
}
