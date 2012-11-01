// -*- mode: c++ -*-

#ifndef TREE_H
#define TREE_H

#include "tree_detail/Node.h"
#include "tree_detail/TreeIterator.h"
#include "tree_detail/DepthFirstTreeIterator.h"
#include "tree_detail/BreadthFirstTreeIterator.h"
#include "tree_detail/LeafTreeIterator.h"

#include "tree_detail/ConstDepthFirstTreeIterator.h"
#include "tree_detail/ConstLeafTreeIterator.h"

#include <iostream>
#include <vector>

/**
  \class AddNodeToEndException
  \brief Exception class, used to notify an invalid attempt to add a node to end
*/
class AddNodeToEndException{};

/**
  \class Tree
  \brief A container class that stores its content in a tree structure
*/
template< class Content >
class Tree
{
public:
  friend class DepthFirstTreeIterator<Content>;
  friend class BreadthFirstTreeIterator<Content>;
  friend class LeafTreeIterator<Content>;  

  friend class ConstDepthFirstTreeIterator<Content>;

  typedef DepthFirstTreeIterator<Content> depth_first_iterator;
  typedef BreadthFirstTreeIterator<Content> breadth_first_iterator;
  typedef LeafTreeIterator<Content> leaf_iterator;

  typedef ConstDepthFirstTreeIterator<Content> const_depth_first_iterator;
  typedef ConstLeafTreeIterator<Content> const_leaf_iterator;

public:
  /**
    \brief Standard constructor, creates an empty tree
  */
  Tree();
  /**
    \brief Constructor that creates a one-node tree with the specified root content
    \param content The content of the root node
  */
  Tree( const Content& content );

  /**
     \brief Constructor: creates a tree from a value and a collection of subtrees
  */
  Tree(const Content& content, const std::vector<Tree>& daughters);


  /**
    \brief Copy constructor
  */
  Tree( const Tree<Content>& rhs );
  virtual ~Tree();

  Tree<Content>& operator= ( const Tree<Content>& rhs );

  /**
    \brief Returns an iterator pointing to the first node of a depth-first-order traversal (root node)
    \return a depth_first_iterator pointing to the root node
  */
  depth_first_iterator dfbegin();
  const_depth_first_iterator dfbegin() const;
  /**
    \brief Returns an iterator pointing to the last node of a depth-first-order traversal (rightmost leaf)
    \return a depth_first_iterator pointing to the rightmost leaf of the tree
  */
  depth_first_iterator dflast();
  const_depth_first_iterator dflast() const;
  /**
    \brief Returns an end iterator
    \return a depth_first_iterator pointing to end
  */
  depth_first_iterator dfend();
  const_depth_first_iterator dfend() const;

  breadth_first_iterator bfbegin();
  breadth_first_iterator bflast();
  breadth_first_iterator bfend();

  /**
    \brief Returns an iterator to the first leaf of the tree
    \return a leaf_iterator pointing to the leftmost leaf of the tree
  */
  leaf_iterator lbegin();
  const_leaf_iterator lbegin() const;

  /**
    \brief Returns an iterator to the last leaf of the tree
    \return a leaf_iterator pointing to the rightmost leaf of the tree
  */
  leaf_iterator llast();
  const_leaf_iterator llast() const;
  /**
    \brief Returns an end iterator
    \return a leaf_iterator pointing to end
  */
  leaf_iterator lend();
  const_leaf_iterator lend() const;
  /**
    \brief States the emptiness of the tree
    \return true, if tree is empty, false otherwise
  */
  bool empty() const;
  /**
    \brief Erases all content making the tree empty
  */
  void clear();
  /**
    \brief States the size of the tree, i.e. the number of nodes it contains
    \return the size of the tree
  */
  unsigned int size() const;
  /**
    \brief States the height of the tree, i.e. the size of the longest path from the root node to a leaf
    \return the height of the tree
  */
  unsigned int height() const;

  /**
    \brief Returns a new Tree object which is a copy of the subtree at the specified position
    \param pos a depth_first_iterator pointing to the root node of the desired subtree
    \return a new Tree object (it's a copy of the subtree)
  */
  Tree<Content> subtree( const depth_first_iterator& pos ) const;
  Tree<Content> subtree( const const_depth_first_iterator& pos ) const;

  /**
    \brief Deletes the subtree at the specified position
    \param pos a depth_first_iterator pointing to the root of the subtree
    \return a depth_first_iterator pointing to the node preceding the specified one in a depth-first traversal of the tree
  */
  depth_first_iterator& erase( depth_first_iterator& pos );

  /**
    \brief Adds a root node to an empty tree
    \param content the content of the root node
    \return a depth_first_iterator pointing to the new node
  */
  depth_first_iterator add_root( const Content& content );
  /**
    \brief Adds a new node as the first daughter of the specified node
    \param pos the node which gets the daughter
    \param content the content of the new node
    \return a depth_first_iterator pointing to the new node
  */
  depth_first_iterator add_first_daughter( const depth_first_iterator& pos, const Content& content );
  /**
    \brief Adds a new node as the last daughter of the specified node
    \param pos the node which gets the daughter
    \param content the content of the new node
    \return a depth_first_iterator pointing to the new node
  */
  depth_first_iterator add_last_daughter( const depth_first_iterator& pos, const Content& content );
  /**
    \brief Adds a new node as the left sister of the specified node
    \param pos the node which gets the sister
    \param content the content of the new node
    \return a depth_first_iterator pointing to the new node
  */
  depth_first_iterator add_left_sister( const depth_first_iterator& pos, const Content& content );
  /**
    \brief Adds a new node as the right sister of the specified node
    \param pos the node which gets the sister
    \param content the content of the new node
    \return a depth_first_iterator pointing to the new node
  */  
  depth_first_iterator add_right_sister( const depth_first_iterator& pos, const Content& content );

  /**
    \brief Adds a subtree to an empty tree
    \param subtree the subtree (it's a copy)
    \return a depth_first_iterator pointing to root node of the subtree
  */
  depth_first_iterator add_root( const Tree<Content>& subtree );
  /**
    \brief Adds a subtree as the first daughter of the specified node
    \param pos the node which gets the daughter
    \param subtree the subtree (it's a copy)
    \return a depth_first_iterator pointing to the root node of the subtree
  */
  depth_first_iterator add_first_daughter( const depth_first_iterator& pos, const Tree<Content>& subtree );
  /**
    \brief Adds a subtree as the last daughter of the specified node
    \param pos the node which gets the daughter
    \param subtree the subtree (it's a copy)
    \return a depth_first_iterator pointing to the root node of the subtree
  */
  depth_first_iterator add_last_daughter( const depth_first_iterator& pos, const Tree<Content>& subtree );
  /**
    \brief Adds a subtree as the left sister of the specified node
    \param pos the node which gets the sister
    \param subtree the subtree (it's a copy)
    \return a depth_first_iterator pointing to the root node of the subtree
  */
  depth_first_iterator add_left_sister( const depth_first_iterator& pos, const Tree<Content>& subtree );
  /**
    \brief Adds a subtree as the right sister of the specified node
    \param pos the node which gets the sister
    \param subtree the subtree (it's a copy)
    \return a depth_first_iterator pointing to the root node of the subtree
  */
  depth_first_iterator add_right_sister( const depth_first_iterator& pos, const Tree<Content>& subtree );

  ////////////////////////
  /////// some other functions
  /////// should be in another file

  std::vector<Content> yield() const;
  
  unsigned number_of_leaves() const;

  std::vector< const_depth_first_iterator > get_at_depth(unsigned depth) const;


  template<class C>
  friend std::ostream& operator<<(std::ostream& out, const Tree<C>& tree);

  ///////////////////////



private:
  Tree( Node<Content>* const root );

private:
  Node<Content>* m_root;     ///< the root node
  unsigned int m_node_cnt;  ///< counts the number of nodes in the tree
  unsigned int m_height;      ///< holds the height of the tree
};




template<class Content>
Tree<Content>::Tree()
: m_root(0),
  m_node_cnt(0),
  m_height(0)
{}

template<class Content>
Tree<Content>::Tree( const Content& content )
  : m_root(new Node<Content>(content)),
    m_node_cnt(1),
    m_height(1)
{}

template<class Content>
Tree<Content>::Tree(const Content& content, const std::vector< Tree<Content> >& daughters)
  : m_root(new Node<Content>(content)),
    m_node_cnt(1),
    m_height(1)
{
  typename Tree<Content>::depth_first_iterator current = dfbegin();
  for(typename std::vector< Tree<Content> >::const_iterator d_iter = daughters.begin();
      d_iter != daughters.end();++d_iter)
    (void) add_last_daughter(current,*d_iter);
}



template<class Content>
Tree<Content>::Tree( const Tree<Content>& rhs )
 : m_root(rhs.m_root == NULL  ? NULL : rhs.m_root->copy()),
   m_node_cnt(rhs.m_node_cnt),
   m_height(rhs.m_height)
{}

template<class Content>
Tree<Content>::~Tree()
{
  delete m_root; // Node class will make sure, that everything is deleted
}

template<class Content>
Tree<Content>& Tree<Content>::operator= ( const Tree<Content>& rhs )
{
  if(this != &rhs)
  {
    delete m_root;
    m_root = rhs.m_root == NULL ? NULL : rhs.m_root->copy();
    m_node_cnt = rhs.m_node_cnt;
    m_height = rhs.m_height;
  }
  return *this;
}

template<class Content>
inline
typename Tree<Content>::depth_first_iterator 
Tree<Content>::dfbegin()
{
  return ++dfend();
}

template<class Content>
inline
typename Tree<Content>::const_depth_first_iterator 
Tree<Content>::dfbegin() const
{
  return ++dfend();
}

template<class Content>
inline
typename Tree<Content>::depth_first_iterator 
Tree<Content>::dflast()
{
  return --dfend();
}

template<class Content>
inline
typename Tree<Content>::const_depth_first_iterator 
Tree<Content>::dflast() const
{
  return --dfend();
}


template<class Content>
inline
typename Tree<Content>::depth_first_iterator 
Tree<Content>::dfend()
{
  return depth_first_iterator(this,0);
}

template<class Content>
inline
typename Tree<Content>::const_depth_first_iterator 
Tree<Content>::dfend() const
{
  return const_depth_first_iterator(this,0);
}


template<class Content>
inline
typename Tree<Content>::breadth_first_iterator 
Tree<Content>::bfbegin()
{
  return ++bfend();
}

template<class Content>
inline
typename Tree<Content>::breadth_first_iterator 
Tree<Content>::bflast()
{
  return --bfend();
}

template<class Content>
inline
typename Tree<Content>::breadth_first_iterator 
Tree<Content>::bfend()
{
  return breadth_first_iterator(this,0);
}

template<class Content>
inline
typename Tree<Content>::leaf_iterator 
Tree<Content>::lbegin()
{
  return ++lend();
}

template<class Content>
inline
typename Tree<Content>::const_leaf_iterator 
Tree<Content>::lbegin() const
{
  return ++lend();
}


template<class Content>
inline
typename Tree<Content>::leaf_iterator 
Tree<Content>::llast()
{
  return --lend();
}

template<class Content>
inline
typename Tree<Content>::const_leaf_iterator 
Tree<Content>::llast() const
{
  return --lend();
}

template<class Content>
inline
typename Tree<Content>::leaf_iterator 
Tree<Content>::lend()
{
  return leaf_iterator(this,0);
}

template<class Content>
inline
typename Tree<Content>::const_leaf_iterator 
Tree<Content>::lend() const
{
  return const_leaf_iterator(this,0);
}


template<class Content>
inline
bool Tree<Content>::empty() const
{
  return !m_node_cnt;
}
#include <iostream>
template<class Content>
inline
void Tree<Content>::clear()
{
  delete m_root;
  m_root = 0;
  m_node_cnt = 0;
  m_height = 0;
}

template<class Content>
inline
unsigned int Tree<Content>::size() const
{
  return m_node_cnt;
}

template<class Content>
inline
unsigned int Tree<Content>::height() const
{
  return m_height;
}

template<class Content>
inline
Tree<Content> Tree<Content>::subtree( const depth_first_iterator& pos ) const
{
  return Tree<Content>(pos->copy());
}

template<class Content>
inline
Tree<Content> Tree<Content>::subtree( const const_depth_first_iterator& pos ) const
{
  return Tree<Content>(pos->copy());
}



template<class Content>
typename Tree<Content>::depth_first_iterator& 
Tree<Content>::erase( depth_first_iterator& pos )
{
  Node<Content>* to_delete = pos.m_node; // remember node
  --pos; // go one step back
  if(to_delete == m_root) // the node to delete is the root node
    clear();
  else delete to_delete;
  return pos;
}

template<class Content>
typename Tree<Content>::depth_first_iterator 
Tree<Content>::add_root( const Content& content )
{
  if(m_root)
    throw DoubleRootException();
  m_root = new Node<Content>(content);
  ++m_node_cnt;
  ++m_height;
  return dfbegin();
}

template<class Content>
typename Tree<Content>::depth_first_iterator 
Tree<Content>::add_first_daughter( const Tree<Content>::depth_first_iterator& pos, const Content& content )
{
  if(pos == dfend())
    throw AddNodeToEndException();
  ++m_node_cnt;
  Node<Content>* new_node = new Node<Content>(content, pos.m_node, 0, pos->m_first_daughter);
  if(pos->m_height == m_height)
    ++m_height;
  return depth_first_iterator(this,new_node);
}

template<class Content>
typename Tree<Content>::depth_first_iterator 
Tree<Content>::add_last_daughter( const Tree<Content>::depth_first_iterator& pos, const Content& content )
{
  if(pos == dfend())
    throw AddNodeToEndException();
  ++m_node_cnt;
  Node<Content>* new_node = new Node<Content>(content, pos.m_node, pos->m_last_daughter,0);
  if(pos->m_height == m_height)
    ++m_height;
  return depth_first_iterator(this,new_node);
}

template<class Content>
typename Tree<Content>::depth_first_iterator 
Tree<Content>::add_left_sister( const Tree<Content>::depth_first_iterator& pos, const Content& content )
{
  if(pos == dfend())
    throw AddNodeToEndException();
  else if(pos == dfbegin())
    throw DoubleRootException();
  Node<Content>* new_node = new Node<Content>(content, pos->m_mother, pos->m_left_sister, pos.m_node);
  ++m_node_cnt;
  return depth_first_iterator(this,new_node);
}

template<class Content>
typename Tree<Content>::depth_first_iterator 
Tree<Content>::add_right_sister( const Tree<Content>::depth_first_iterator& pos, const Content& content )
{
  if(pos == dfend())
    throw AddNodeToEndException();
  else if(pos == dfbegin())
    throw DoubleRootException();
  Node<Content>* new_node = new Node<Content>(content, pos->m_mother, pos.m_node, pos->m_right_sister);
  ++m_node_cnt;
  return depth_first_iterator(this,new_node);
}

template<class Content>
typename Tree<Content>::depth_first_iterator 
Tree<Content>::add_root( const Tree<Content>& subtree )
{
  delete m_root;
  m_root = subtree.m_root->copy();
  m_height = subtree.m_height;
  m_node_cnt = subtree.m_node_cnt;
  return dfbegin();
}

template<class Content>
typename Tree<Content>::depth_first_iterator 
Tree<Content>::add_first_daughter( const depth_first_iterator& pos, const Tree<Content>& subtree )
{
  if(pos == dfend())
    throw AddNodeToEndException();
  Node<Content>* new_node = subtree.m_root->copy(pos->height() + 1);
  new_node->m_mother = pos.m_node;
  if(pos->m_first_daughter)
  {
    new_node->m_right_sister = pos->m_first_daughter;
    pos->m_first_daughter->m_left_sister = new_node;
  }
  else
    pos->m_last_daughter = new_node;
  pos->m_first_daughter = new_node;
  // update height
  if(m_height < pos->height() + subtree.height())
    m_height = pos->height() + subtree.height();
  return depth_first_iterator(this,new_node);
}

template<class Content>
typename Tree<Content>::depth_first_iterator 
Tree<Content>::add_last_daughter( const depth_first_iterator& pos, const Tree<Content>& subtree )
{
  if(pos == dfend())
    throw AddNodeToEndException();
  Node<Content>* new_node = subtree.m_root->copy(pos->height() + 1);
  new_node->m_mother = pos.m_node;
  if(pos->m_last_daughter)
  {
    new_node->m_left_sister = pos->m_last_daughter;
    pos->m_last_daughter->m_right_sister = new_node;
  }
  else
    pos->m_first_daughter = new_node;
  pos->m_last_daughter = new_node;
  // update height
  if(m_height < pos->height() + subtree.height())
    m_height = pos->height() + subtree.height();
  return depth_first_iterator(this,new_node);
}

template<class Content>
typename Tree<Content>::depth_first_iterator 
Tree<Content>::add_left_sister( const depth_first_iterator& pos, const Tree<Content>& subtree )
{
  if(pos == dfend())
    throw AddNodeToEndException();
  else if(pos == dfbegin())
    throw DoubleRootException();
  Node<Content>* new_node = subtree.m_root->copy(pos->height());
  new_node->m_mother = pos->m_mother;
  new_node->m_right_sister = pos.m_node;
  if(pos->m_left_sister)
  {
    new_node->m_left_sister = pos->m_left_sister;
    pos->m_left_sister->m_right_sister = new_node;
  }
  else
    new_node->m_mother->m_first_daughter = new_node;
  pos->m_left_sister = new_node;
  // update height
  if(m_height < pos->height() + subtree.height())
    m_height = pos->height() + subtree.height();
  return depth_first_iterator(this,new_node);
}

template<class Content>
typename Tree<Content>::depth_first_iterator 
Tree<Content>::add_right_sister( const depth_first_iterator& pos, const Tree<Content>& subtree )
{
  if(pos == dfend())
    throw AddNodeToEndException();
  else if(pos == dfbegin())
    throw DoubleRootException();
  Node<Content>* new_node = subtree.m_root->copy(pos->height());
  new_node->m_mother = pos->m_mother;
  new_node->m_left_sister = pos.m_node;
  if(pos->m_right_sister)
  {
    new_node->m_right_sister = pos->m_right_sister;
    pos->m_right_sister->m_left_sister = new_node;
  }
  else
    new_node->m_mother->m_last_daughter = new_node;
  pos->m_right_sister = new_node;
  // update height
  if(m_height < pos->height() + subtree.height())
    m_height = pos->height() + subtree.height();
  return depth_first_iterator(this,new_node);
}

template<class Content>
Tree<Content>::Tree( Node<Content>* const root )
: m_root(root),
  m_node_cnt(0),
  m_height(0)
{
  for(typename Tree<Content>::depth_first_iterator i = dfbegin(); i != dfend(); ++i)
  {
    ++m_node_cnt;
    if(m_height < i->m_height)
      m_height = i->m_height;
  }
}


template<class Content>
std::vector<Content> 
Tree<Content>::yield() const
{
  std::vector<Content> result;

  typename Tree<Content>::leaf_iterator l_iter = lbegin();
  for(; l_iter !=  lend(); ++l_iter)
    result.push_back(*l_iter);

  return result;
}


template<class Content>
unsigned
Tree<Content>::number_of_leaves() const
{
  unsigned result = 0;

  typename Tree<Content>::const_leaf_iterator l_iter = lbegin();
  for(; l_iter !=  lend(); ++l_iter)
    ++result;

  return result;
}


template <typename T>
std::vector< typename Tree<T>::const_depth_first_iterator > Tree<T>::get_at_depth(unsigned depth) const
{
  std::vector< typename Tree<T>::const_depth_first_iterator > result;
  unsigned target_height = height() - depth;

  for (typename Tree<T>::const_depth_first_iterator i(dfbegin()); i != dfend(); ++i) {
    
    Tree<T> t = subtree(i);
    
    if( t.height() == target_height) {
      //		 std::cout << "\t" << t.height() << " " << t << std::endl;
      result.push_back(i);
    }
  }
  
  
  return result;
}  



template <typename C>
std::ostream& operator<<(std::ostream& out, const Tree<C>& tree)
{
  out << "( ";
  unsigned int old_height = 0;
  for(typename Tree<C>::const_depth_first_iterator i = tree.dfbegin(); i != tree.dfend(); ++i )
    {
      unsigned int current_height = i->height();
    if( current_height < old_height )
      for(unsigned int j = 0; j < old_height - current_height; ++j)
        out << ")";
    if(old_height)
      out << " ";
    if(!i->leaf())
        out << "(";
    out << *i;
    old_height = current_height;
  }
  if(old_height > 1)
    for(unsigned int j = 0; j < old_height-1; ++j)
      out << ")";
  out << ")" ;

  return out;
}


#endif // TREE_H
