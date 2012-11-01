// -*- mode: c++ -*-
#ifndef CONSTDEPTHFIRSTTREEITERATOR_H
#define CONSTDEPTHFIRSTTREEITERATOR_H

#include "ConstTreeIterator.h"

template< class Content >
class ConstDepthFirstTreeIterator : public ConstTreeIterator<Content>
{
public:
  ConstDepthFirstTreeIterator();
  ConstDepthFirstTreeIterator( const Tree<Content>* const tree, const Node<Content>* const node );
  virtual ~ConstDepthFirstTreeIterator();

  ConstDepthFirstTreeIterator<Content>& operator++();
  ConstDepthFirstTreeIterator<Content>& operator--();

  ConstDepthFirstTreeIterator<Content>& up();
  ConstDepthFirstTreeIterator<Content>& left();
  ConstDepthFirstTreeIterator<Content>& right();
  ConstDepthFirstTreeIterator<Content>& down_first();
  ConstDepthFirstTreeIterator<Content>& down_last();

private:
  typedef ConstTreeIterator<Content> CTI;
};

template<class Content>
ConstDepthFirstTreeIterator<Content>::ConstDepthFirstTreeIterator()
: CTI::ConstTreeIterator()
{
}

template<class Content>
ConstDepthFirstTreeIterator<Content>::ConstDepthFirstTreeIterator( const Tree<Content>* const tree, const Node<Content>* const node )
: CTI::ConstTreeIterator(tree,node)
{
}

template<class Content>
ConstDepthFirstTreeIterator<Content>::~ConstDepthFirstTreeIterator()
{
}

template<class Content>
ConstDepthFirstTreeIterator<Content>& ConstDepthFirstTreeIterator<Content>::operator++ ()
{
  if(!CTI::m_node) // is end iterator
    CTI::m_node = CTI::m_tree->m_root;
  else // is not end iterator
  {
    // go to first daughter
    Node<Content>* next = CTI::m_node->m_first_daughter;
    if(!next) // if no first daughter
    {
      // go to next right sister of yourself or your ancestors
      while(!CTI::m_node->m_right_sister)
      {
        CTI::m_node = CTI::m_node->m_mother;
        if(!CTI::m_node) // return if at end
          return *this;
      }
      CTI::m_node = CTI::m_node->m_right_sister;
    }
    else CTI::m_node = next;
  }
  return *this;
}

template<class Content>
ConstDepthFirstTreeIterator<Content>& ConstDepthFirstTreeIterator<Content>::operator-- ()
{
  // next is root if at end or the left sister
  Node<Content>* next = CTI::m_node ? CTI::m_node->m_left_sister : CTI::m_tree->m_root;
  if(next)
  {
    while(next)
    {
      CTI::m_node = next;
      next = next->m_last_daughter;
    }
  }
  else
    CTI::m_node = CTI::m_node ? CTI::m_node->m_mother : 0;
  return *this;
}

template<class Content>
inline
ConstDepthFirstTreeIterator<Content>& ConstDepthFirstTreeIterator<Content>::up()
{
  CTI::up();
  return *this;   
}

template<class Content>
inline
ConstDepthFirstTreeIterator<Content>& ConstDepthFirstTreeIterator<Content>::left()
{
  CTI::left();
  return *this;
}

template<class Content>
inline
ConstDepthFirstTreeIterator<Content>& ConstDepthFirstTreeIterator<Content>::right()
{
  CTI::right();
  return *this;
}

template<class Content>
inline
ConstDepthFirstTreeIterator<Content>& ConstDepthFirstTreeIterator<Content>::down_first()
{
  CTI::down_first();
  return *this;
}

template<class Content>
inline
ConstDepthFirstTreeIterator<Content>& ConstDepthFirstTreeIterator<Content>::down_last()
{
  CTI::down_last();
  return *this;
}



#endif // DEPTHFIRSTTREEITERATOR_H
