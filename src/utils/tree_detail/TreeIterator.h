// -*- mode: c++ -*-
#ifndef TREEITERATOR_H
#define TREEITERATOR_H

class EndDereferencedException{};

template< class Content >
class Node;

template< class Content >
class Tree;

template< class Content >
class TreeIterator
{
public:
  friend class Tree<Content>;

public:
  TreeIterator();
  TreeIterator( Tree<Content>* const tree, Node<Content>* const node );
  virtual ~TreeIterator();

  virtual TreeIterator<Content>& operator++() = 0;
  virtual TreeIterator<Content>& operator--() = 0;
  Content& operator* () const;
  // TODO Perhaps the semantics should be changed. Return the content, not the node.
  Node<Content>* operator->() const;

  bool operator== ( const TreeIterator<Content>& rhs ) const;
  bool operator!= ( const TreeIterator<Content>& rhs ) const;


  Node<Content>* get_node() const;

  TreeIterator<Content>& up();
  TreeIterator<Content>& left();
  TreeIterator<Content>& right();
  TreeIterator<Content>& down_first();
  TreeIterator<Content>& down_last();

protected:
  Node<Content>* m_node;
  Tree<Content>* m_tree;
};

template<class Content>
TreeIterator<Content>::TreeIterator()
: m_node(0),
  m_tree(0)
{
}

template<class Content>
TreeIterator<Content>::TreeIterator( Tree<Content>* const tree, Node<Content>* const node )
: m_node(node),
  m_tree(tree)
{
}

template<class Content>
TreeIterator<Content>::~TreeIterator()
{
}

template<class Content>
inline
Content& TreeIterator<Content>::operator* () const
{
  if(!m_node)
    throw EndDereferencedException();
  return m_node->m_content;
}

template<class Content>
inline
Node<Content>* TreeIterator<Content>::operator->() const
{
  return m_node;
}

template<class Content>
inline
Node<Content>* TreeIterator<Content>::get_node() const
{
  return m_node;
}

template<class Content>
inline
bool TreeIterator<Content>::operator== ( const TreeIterator<Content>& rhs ) const
{
  return m_tree == rhs.m_tree && m_node == rhs.m_node;
}

template<class Content>
inline
bool TreeIterator<Content>::operator!= ( const TreeIterator<Content>& rhs ) const
{
  return m_tree != rhs.m_tree || m_node != rhs.m_node;
}

template<class Content>
inline
TreeIterator<Content>& TreeIterator<Content>::up()
{
  if(m_node)
    m_node = m_node->m_mother;
  return *this;
}

template<class Content>
inline
TreeIterator<Content>& TreeIterator<Content>::left()
{
  if(m_node)
    m_node = m_node->m_left_sister;
  return *this;
}

template<class Content>
inline
TreeIterator<Content>& TreeIterator<Content>::right()
{
  if(m_node)
    m_node = m_node->m_right_sister;
  return *this;
}

template<class Content>
inline
TreeIterator<Content>& TreeIterator<Content>::down_first()
{
  if(m_node)
    m_node = m_node->m_first_daughter;
  return *this;
}

template<class Content>
inline
TreeIterator<Content>& TreeIterator<Content>::down_last()
{
  if(m_node)
    m_node = m_node->m_last_daughter;
  return *this;
}




#endif // TREEITERATOR_H
