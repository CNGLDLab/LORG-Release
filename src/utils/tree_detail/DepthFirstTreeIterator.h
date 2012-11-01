#ifndef DEPTHFIRSTTREEITERATOR_H
#define DEPTHFIRSTTREEITERATOR_H

#include "TreeIterator.h"

template< class Content >
class DepthFirstTreeIterator : public TreeIterator<Content>
{
public:
  DepthFirstTreeIterator();
  DepthFirstTreeIterator( Tree<Content>* const tree, Node<Content>* const node );
  virtual ~DepthFirstTreeIterator();

  DepthFirstTreeIterator<Content>& operator++();
  DepthFirstTreeIterator<Content>& operator--();

  DepthFirstTreeIterator<Content>& up();
  DepthFirstTreeIterator<Content>& left();
  DepthFirstTreeIterator<Content>& right();
  DepthFirstTreeIterator<Content>& down_first();
  DepthFirstTreeIterator<Content>& down_last();

private:
  typedef TreeIterator<Content> TI;
};

//#include "DepthFirstTreeIterator.cpp"

template<class Content>
DepthFirstTreeIterator<Content>::DepthFirstTreeIterator()
: TI::TreeIterator()
{
}

template<class Content>
DepthFirstTreeIterator<Content>::DepthFirstTreeIterator( Tree<Content>* const tree, Node<Content>* const node )
: TI::TreeIterator(tree,node)
{
}

template<class Content>
DepthFirstTreeIterator<Content>::~DepthFirstTreeIterator()
{
}

template<class Content>
DepthFirstTreeIterator<Content>& DepthFirstTreeIterator<Content>::operator++ ()
{
  if(!TI::m_node) // is end iterator
    TI::m_node = TI::m_tree->m_root;
  else // is not end iterator
  {
    // go to first daughter
    Node<Content>* next = TI::m_node->m_first_daughter;
    if(!next) // if no first daughter
    {
      // go to next right sister of yourself or your ancestors
      while(!TI::m_node->m_right_sister)
      {
        TI::m_node = TI::m_node->m_mother;
        if(!TI::m_node) // return if at end
          return *this;
      }
      TI::m_node = TI::m_node->m_right_sister;
    }
    else TI::m_node = next;
  }
  return *this;
}

template<class Content>
DepthFirstTreeIterator<Content>& DepthFirstTreeIterator<Content>::operator-- ()
{
  // next is root if at end or the left sister
  Node<Content>* next = TI::m_node ? TI::m_node->m_left_sister : TI::m_tree->m_root;
  if(next)
  {
    while(next)
    {
      TI::m_node = next;
      next = next->m_last_daughter;
    }
  }
  else
    TI::m_node = TI::m_node ? TI::m_node->m_mother : 0;
  return *this;
}

template<class Content>
inline
DepthFirstTreeIterator<Content>& DepthFirstTreeIterator<Content>::up()
{
  TI::up();
  return *this;   
}

template<class Content>
inline
DepthFirstTreeIterator<Content>& DepthFirstTreeIterator<Content>::left()
{
  TI::left();
  return *this;
}

template<class Content>
inline
DepthFirstTreeIterator<Content>& DepthFirstTreeIterator<Content>::right()
{
  TI::right();
  return *this;
}

template<class Content>
inline
DepthFirstTreeIterator<Content>& DepthFirstTreeIterator<Content>::down_first()
{
  TI::down_first();
  return *this;
}

template<class Content>
inline
DepthFirstTreeIterator<Content>& DepthFirstTreeIterator<Content>::down_last()
{
  TI::down_last();
  return *this;
}



#endif // DEPTHFIRSTTREEITERATOR_H
