// -*- mode: c++ -*-

#ifndef CONSTLEAFTREEITERATOR_H
#define CONSTLEAFTREEITERATOR_H

#include "ConstDepthFirstTreeIterator.h"

template< class Content >
class ConstLeafTreeIterator : public ConstDepthFirstTreeIterator<Content>
{
public:
  ConstLeafTreeIterator();
  ConstLeafTreeIterator( const Tree<Content>* const tree, const Node<Content>* const node );
  virtual ~ConstLeafTreeIterator();

  ConstLeafTreeIterator<Content>& operator++();
  ConstLeafTreeIterator<Content>& operator--();

  ConstDepthFirstTreeIterator<Content>& up();
  ConstDepthFirstTreeIterator<Content>& left();
  ConstDepthFirstTreeIterator<Content>& right();
  ConstDepthFirstTreeIterator<Content>& down_first();
  ConstDepthFirstTreeIterator<Content>& down_last();

private:
 typedef ConstDepthFirstTreeIterator<Content> CDFTI;
};


template<class Content>
ConstLeafTreeIterator<Content>::ConstLeafTreeIterator()
: CDFTI::ConstDepthFirstTreeIterator()
{
}

template<class Content>
ConstLeafTreeIterator<Content>::ConstLeafTreeIterator( const Tree<Content>* const tree, const Node<Content>* const node )
: CDFTI::ConstDepthFirstTreeIterator(tree,node)
{
  if(node && !this->m_node->leaf())
    operator++();
}

template<class Content>
ConstLeafTreeIterator<Content>::~ConstLeafTreeIterator()
{
}

template<class Content>
ConstLeafTreeIterator<Content>& ConstLeafTreeIterator<Content>::operator++ ()
{
  CDFTI::operator++();
  for(;this->m_node && !this->m_node->leaf();CDFTI::operator++()){;}
  return *this;
}

template<class Content>
ConstLeafTreeIterator<Content>& ConstLeafTreeIterator<Content>::operator-- ()
{
  CDFTI::operator--();
  for(;this->m_node && !this->m_node->leaf();CDFTI::operator--()){;}
  return *this;
}

template<class Content>
inline
ConstDepthFirstTreeIterator<Content>& ConstLeafTreeIterator<Content>::up()
{
  return CDFTI::up();
}

template<class Content>
inline
ConstDepthFirstTreeIterator<Content>& ConstLeafTreeIterator<Content>::left()
{
  return CDFTI::left();
}

template<class Content>
inline
ConstDepthFirstTreeIterator<Content>& ConstLeafTreeIterator<Content>::right()
{
  return CDFTI::right();
}

template<class Content>
inline
ConstDepthFirstTreeIterator<Content>& ConstLeafTreeIterator<Content>::down_first()
{
  return CDFTI::down_first();
}

template<class Content>
inline
ConstDepthFirstTreeIterator<Content>& ConstLeafTreeIterator<Content>::down_last()
{
  return CDFTI::down_last();
}



#endif // CONSTLEAFTREEITERATOR_H
