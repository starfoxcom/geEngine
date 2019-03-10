#include "RTSNode.h"

bool RTSNode::operator==(const RTSNode& _node)
{
  return this->m_position == _node.m_position;
}

RTSNode::~RTSNode()
{
}
