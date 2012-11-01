// -*- mode: c++ -*-

#ifndef LEAFTREEITERATOR_H
#define LEAFTREEITERATOR_H

#include "DepthFirstTreeIterator.h"

template< class Content >
class LeafTreeIterator : public DepthFirstTreeIterator<Content>
{
public:
  LeafTreeIterator();
  LeafTreeIterator( Tree<Content>* const tree, Node<Content>* const node );
  virtual ~LeafTreeIterator();

  LeafTreeIterator<Content>& operator++();
  LeafTreeIterator<Content>& operator--();

  DepthFirstTreeIterator<Content>& up();
  DepthFirstTreeIterator<Content>& left();
  DepthFirstTreeIterator<Content>& right();
  DepthFirstTreeIterator<Content>& down_first();
  DepthFirstTreeIterator<Content>& down_last();

private:
 typedef DepthFirstTreeIterator<Content> DFTI;
};

//#include "LeafTreeIterator.cpp"


template<class Content>
LeafTreeIterator<Content>::LeafTreeIterator()
: DFTI::DepthFirstTreeIterator()
{
}

template<class Content>
LeafTreeIterator<Content>::LeafTreeIterator( Tree<Content>* const tree, Node<Content>* const node )
: DFTI::DepthFirstTreeIterator(tree,node)
{
  if(node && !this->m_node->leaf())
    operator++();
}

template<class Content>
LeafTreeIterator<Content>::~LeafTreeIterator()
{
}

template<class Content>
LeafTreeIterator<Content>& LeafTreeIterator<Content>::operator++ ()
{
  DFTI::operator++();
  for(;this->m_node && !this->m_node->leaf();DFTI::operator++()){;}
  return *this;
}

template<class Content>
LeafTreeIterator<Content>& LeafTreeIterator<Content>::operator-- ()
{
  DFTI::operator--();
  for(;this->m_node && !this->m_node->leaf();DFTI::operator--()){;}
  return *this;
}

template<class Content>
inline
DepthFirstTreeIterator<Content>& LeafTreeIterator<Content>::up()
{
  return DFTI::up();
}

template<class Content>
inline
DepthFirstTreeIterator<Content>& LeafTreeIterator<Content>::left()
{
  return DFTI::left();
}

template<class Content>
inline
DepthFirstTreeIterator<Content>& LeafTreeIterator<Content>::right()
{
  return DFTI::right();
}

template<class Content>
inline
DepthFirstTreeIterator<Content>& LeafTreeIterator<Content>::down_first()
{
  return DFTI::down_first();
}

template<class Content>
inline
DepthFirstTreeIterator<Content>& LeafTreeIterator<Content>::down_last()
{
  return DFTI::down_last();
}



#endif // LEAFTREEITERATOR_H
