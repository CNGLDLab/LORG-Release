#ifndef CONSTTREEITERATOR_H
#define CONSTTREEITERATOR_H

//class EndDereferencedException{};

template< class Content >
class Node;

template< class Content >
class Tree;

template< class Content >
class ConstTreeIterator
{
public:
  friend class Tree<Content>;

public:
  ConstTreeIterator();
  ConstTreeIterator( const Tree<Content>* const tree, const Node<Content>* const node );
  virtual ~ConstTreeIterator();

  virtual ConstTreeIterator<Content>& operator++() = 0;
  virtual ConstTreeIterator<Content>& operator--() = 0;
  const Content& operator* () const;
  // TODO Perhaps the semantics should be changed. Return the content, not the node.
  const Node<Content>* operator->() const;

  bool operator== ( const ConstTreeIterator<Content>& rhs ) const;
  bool operator!= ( const ConstTreeIterator<Content>& rhs ) const;

  const Node<Content>* get_node() const;

  ConstTreeIterator<Content>& up();
  ConstTreeIterator<Content>& left();
  ConstTreeIterator<Content>& right();
  ConstTreeIterator<Content>& down_first();
  ConstTreeIterator<Content>& down_last();

protected:
  const Node<Content>* m_node;
  const Tree<Content>* m_tree;
};

template<class Content>
ConstTreeIterator<Content>::ConstTreeIterator()
: m_node(0),
  m_tree(0)
{
}

template<class Content>
ConstTreeIterator<Content>::ConstTreeIterator( const Tree<Content>* const tree, const Node<Content>* const node )
: m_node(node),
  m_tree(tree)
{
}

template<class Content>
ConstTreeIterator<Content>::~ConstTreeIterator()
{
}

template<class Content>
inline
const Content& ConstTreeIterator<Content>::operator* () const
{
  if(!m_node)
    throw EndDereferencedException();
  return m_node->m_content;
}

template<class Content>
inline
const Node<Content>* ConstTreeIterator<Content>::operator->() const
{
  return m_node;
}


template<class Content>
inline
const Node<Content>* ConstTreeIterator<Content>::get_node() const
{
  return m_node;
}

template<class Content>
inline
bool ConstTreeIterator<Content>::operator== ( const ConstTreeIterator<Content>& rhs ) const
{
  return m_tree == rhs.m_tree && m_node == rhs.m_node;
}

template<class Content>
inline
bool ConstTreeIterator<Content>::operator!= ( const ConstTreeIterator<Content>& rhs ) const
{
  return m_tree != rhs.m_tree || m_node != rhs.m_node;
}

template<class Content>
inline
ConstTreeIterator<Content>& ConstTreeIterator<Content>::up()
{
  if(m_node)
    m_node = m_node->m_mother;
  return *this;
}

template<class Content>
inline
ConstTreeIterator<Content>& ConstTreeIterator<Content>::left()
{
  if(m_node)
    m_node = m_node->m_left_sister;
  return *this;
}

template<class Content>
inline
ConstTreeIterator<Content>& ConstTreeIterator<Content>::right()
{
  if(m_node)
    m_node = m_node->m_right_sister;
  return *this;
}

template<class Content>
inline
ConstTreeIterator<Content>& ConstTreeIterator<Content>::down_first()
{
  if(m_node)
    m_node = m_node->m_first_daughter;
  return *this;
}

template<class Content>
inline
ConstTreeIterator<Content>& ConstTreeIterator<Content>::down_last()
{
  if(m_node)
    m_node = m_node->m_last_daughter;
  return *this;
}


#endif // TREEITERATOR_H
