// -*- mode: c++ -*-
#ifndef NODE_H
#define NODE_H

#include "TreeIterator.h"
#include "DepthFirstTreeIterator.h"
#include "BreadthFirstTreeIterator.h"
#include "LeafTreeIterator.h"

#include "ConstTreeIterator.h"
#include "ConstDepthFirstTreeIterator.h"

class DoubleRootException{};

template< class Content >
class Tree;

template< class Content >
class Node
{
public:
  friend class TreeIterator<Content>;
  friend class DepthFirstTreeIterator<Content>;
  friend class BreadthFirstTreeIterator<Content>;
  friend class LeafTreeIterator<Content>;
  
  friend class ConstTreeIterator<Content>;
  friend class ConstDepthFirstTreeIterator<Content>;

  friend class Tree<Content>;

public:
  Node();
  Node(const Content& content, 
            Node<Content>* const mother = 0, 
            Node<Content>* const left_sister = 0, 
            Node<Content>* const right_sister = 0,
            unsigned int height = 1);
  ~Node();

  unsigned int height() const;

  bool leaf() const;
  bool has_mother() const;
  bool has_left_sister() const;
  bool has_right_sister() const;

private:
  Node<Content>* copy( unsigned int height = 1 ) const;
  void copy_daughters( Node<Content>* const mother, const Node<Content>* const first_daughter) const;

  Node(const Node&);
  Node& operator=(const Node&);

private:
  unsigned int m_height;
  Content m_content;
  Node<Content>* m_mother;
  Node<Content>* m_first_daughter;
  Node<Content>* m_last_daughter;
  Node<Content>* m_left_sister;
  Node<Content>* m_right_sister;
};

template<class Content>
Node<Content>::Node()
: m_height(1),
  m_content(Content()),
  m_mother(0),
  m_first_daughter(0),
  m_last_daughter(0),
  m_left_sister(0),
  m_right_sister(0)
{
}

template<class Content>
Node<Content>::Node( const Content& content, 
                                        Node<Content>* const mother, 
                                        Node<Content>* const left_sister, 
                                        Node<Content>* const right_sister,
                                        unsigned int height )
: m_height(height),
  m_content(content),
  m_mother(mother),
  m_first_daughter(0),
  m_last_daughter(0),
  m_left_sister(left_sister),
  m_right_sister(right_sister)
{
  if(mother)
  {
    m_height += mother->m_height;
    if(right_sister)
      right_sister->m_left_sister = this;
    else
      mother->m_last_daughter = this;
    if(left_sister)
      left_sister->m_right_sister = this;
    else
      mother->m_first_daughter = this;
  }
  else if(!mother && (right_sister || left_sister))
    throw DoubleRootException();
}

template<class Content>
Node<Content>::~Node()
{
  // separate from sisters and mother node
  if(m_mother)
  {
    if(m_left_sister && !m_right_sister)
    {
      m_left_sister->m_right_sister = 0;
      m_mother->m_last_daughter = m_left_sister;
    }
    else if(m_right_sister && !m_left_sister)
    {
      m_right_sister->m_left_sister = 0;
      m_mother->m_first_daughter = m_right_sister;
    }
    else if(m_left_sister && m_right_sister)
    {
      m_left_sister->m_right_sister = m_right_sister;
      m_right_sister->m_left_sister = m_left_sister;
    }
    else
    {
      m_mother->m_first_daughter = 0;
      m_mother->m_last_daughter = 0;
    }
  }
  // delete daughters
  Node<Content>* next = m_first_daughter;
  while(next)
  {
    Node<Content>* to_delete = next;
    next = next->m_right_sister;
    delete to_delete;
  }
}

template<class Content>
inline
unsigned int Node<Content>::height() const
{
  return m_height;
}

template<class Content>
inline
bool Node<Content>::leaf() const
{
  return m_first_daughter == 0;
}

template<class Content>
inline
bool Node<Content>::has_mother() const
{
  return m_mother != 0;
}

template<class Content>
inline
bool Node<Content>::has_left_sister() const
{
  return m_left_sister != 0;
}

template<class Content>
inline
bool Node<Content>::has_right_sister() const
{
  return m_right_sister != 0;
}

template<class Content>
Node<Content>* Node<Content>::copy( unsigned int height ) const
{
  Node<Content>* new_node = new Node<Content>(m_content,0,0,0,height);
  copy_daughters(new_node,m_first_daughter);
  return new_node;
}

template<class Content>
void Node<Content>::copy_daughters( Node<Content>* const mother, const Node<Content>* const first_daughter) const
{
  const Node<Content>* next = first_daughter;
  Node<Content>* last = 0;
  while(next)
  {
    Node<Content>* new_node = new Node<Content>(next->m_content, mother, last);
    if(last)
      last->m_right_sister = new_node;
    else
      mother->m_first_daughter = new_node;
    if(!next->m_right_sister)
      mother->m_last_daughter = new_node;
    Node<Content>* next_daughter = next->m_first_daughter;
    last = new_node;
    next = next->m_right_sister;
    if(next_daughter)
      copy_daughters(new_node, next_daughter);
  }
}


#endif // NODE_H
