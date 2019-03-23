#include "RTSPathfinding.h"
#include "RTSNode.h"

RTSPathfinding::RTSPathfinding()
{

  //Emplace the positions for the possible connection nodes
  m_nextPositions.emplace_back(-1, -1);
  m_nextPositions.emplace_back(0, -1);
  m_nextPositions.emplace_back(1, -1);
  m_nextPositions.emplace_back(1, 0);
  m_nextPositions.emplace_back(1, 1);
  m_nextPositions.emplace_back(0, 1);
  m_nextPositions.emplace_back(-1, 1);
  m_nextPositions.emplace_back(-1, 0);

  //Set the start and target positions to zero
  m_startPos = Vector2I::ZERO;
  m_targetPos = Vector2I::ZERO;

  //Clear the vectors
  m_nextNodes.clear();
  m_visited.clear();
}

RTSPathfinding::~RTSPathfinding()
{

  //Reset the search (in this case, free the allocated memory before deleting the object)
  resetSearch();
}

bool RTSPathfinding::resetSearch()
{

  //While the vector is not empty
  while (m_nextNodes.size() > 0)
  {
    
    //Delete the parent of the node on the vector
    ge_delete(m_nextNodes.back().m_parent);

    //Pop out the node from the vector
    m_nextNodes.pop_back();
  }

  //While the vector is not empty
  while (m_visited.size() > 0)
  {

    //On current node pointer is not the back node
    if (!(*m_pCurrent == m_visited.back()))
    {

      //Delete the parent of the node on the vector
      ge_delete(m_visited.back().m_parent);
    }

    //Pop out the node from the vector
    m_visited.pop_back();
  }

  //On current node pointer not nullptr
  if (m_pCurrent)
  {

    //Delete the current node pointer
    ge_delete(m_pCurrent);

    //Set the pointer to nullptr
    m_pCurrent = nullptr;
  }

  //Set the current state to idle
  m_currentState = SEARCH_STATE::idle;

  return true;
}

Vector<Vector2I> RTSPathfinding::backTrace(RTSNode* _node)
{

  //Create a vector to store the result
  Vector<Vector2I> result;

  //Create a node pointer for the parent
  RTSNode* pParent;

  //Push back the position of the node on the vector
  result.push_back(_node->m_position);

  //Set the parent node pointer
  pParent = _node->m_parent;

  //While the parent node pointer is not nullptr
  while (pParent != nullptr)
  {

    //Push back the position of the parent
    result.push_back(pParent->m_position);

    //Set the parent to the parent of itself
    pParent = pParent->m_parent;
  }
  return result;
}

Vector<RTSNode> RTSPathfinding::getVisitedNodes()
{
  return m_visited;
}

Vector<RTSNode> RTSPathfinding::getNextNodes()
{
  return m_nextNodes;
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

  //For the begin of the vector to the end of it, iterate
  for (auto it = _list.begin(); it < _list.end(); ++it)
  {

    //On the position of the object iterated equals to the position of the vector provided
    if (it->m_position == _position)
    {
      return true;
    }
  }

  return false;
}
