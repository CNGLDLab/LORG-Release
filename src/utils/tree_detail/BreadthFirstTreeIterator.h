#ifndef BREADTHFIRSTTREEITERATOR_H
#define BREADTHFIRSTTREEITERATOR_H

#include "TreeIterator.h"

template< class Content >
class BreadthFirstTreeIterator : public TreeIterator<Content>
{
public:
  BreadthFirstTreeIterator();
  BreadthFirstTreeIterator( Tree<Content>* const tree, Node<Content>* const node );
  virtual ~BreadthFirstTreeIterator();

  BreadthFirstTreeIterator<Content>& operator++();
  BreadthFirstTreeIterator<Content>& operator--();
private:
  typedef TreeIterator<Content> TI;
};

//#include "BreadthFirstTreeIterator.cpp"



template<class Content>
BreadthFirstTreeIterator<Content>::BreadthFirstTreeIterator()
: TreeIterator<Content>::TreeIterator()
{
}

template<class Content>
BreadthFirstTreeIterator<Content>::BreadthFirstTreeIterator( Tree<Content>* const tree, Node<Content>* const node )
: TreeIterator<Content>::TreeIterator(tree,node)
{
}

template<class Content>
BreadthFirstTreeIterator<Content>::~BreadthFirstTreeIterator()
{
}

template<class Content>
BreadthFirstTreeIterator<Content>& BreadthFirstTreeIterator<Content>::operator++ ()
{
  if(!TI::m_node)
    TI::m_node = TI::m_tree->m_root;
  else
  {
    Node<Content>* next = TI::m_node->m_right_sister;
    if(!next)
    {
      if(TI::m_node->m_mother)
      {
        next = TI::m_node->m_mother->m_first_daughter;
        // TODO
      }
      else { ;}
    }
    else TI::m_node = next;
  }
  return *this;
}

template<class Content>
BreadthFirstTreeIterator<Content>& BreadthFirstTreeIterator<Content>::operator-- ()
{
}


#endif // BREADTHFIRSTTREEITERATOR_H
