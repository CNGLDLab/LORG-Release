// -*- mode: c++ -*-
#ifndef PSTREE_H
#define PSTREE_H

#include "Tree.h"

#include "rules/Production.h"
#include <vector>
#include <map>

// we should make a decision about these 2 implementations
#include <boost/unordered_set.hpp>

/**
   \class PsTree
   \brief a specialisation of trees for Phrase Structures
 **/
template< typename Content >
class PsTree : public Tree<Content>
{
public:
  PsTree();
  PsTree( const Content& content );
  PsTree( const Content& content, const std::vector<PsTree>& trees);
  virtual ~PsTree() { };

  /**
     \brief collects the productions of a phrase structure tree
     \param internals the internal rules
     \param lexicals the lexical rules
  **/
  virtual void productions(std::vector<Production>& internals,std::vector<Production>& lexicals) const = 0;

  
  /**
     \brief collects the internal counts of productions of a  phrase structure tree (binary and unary rules only)
     \param binary_counts counts for LHS -> RHS0 RHS1
     \param unary_counts counts for LHS -> RHS0
     \param LHS_counts wot it says on the tin
  **/
  virtual void collect_internal_counts( std::map<Production, double> & binary_counts,
					std::map<Production, double> & unary_counts,
					std::map< int, double> & LHS_counts) const = 0;

  


  /**
     \brief removes traces from the tree
     Should be implemented by subclasses
     \param labels_to_remove labels to remove from the tree
   **/
  virtual void clean(const boost::unordered_set<std::string>& labels_to_remove) = 0;

  /**
     \brief add parent information (ie, name) to each node
     Should be implemented by subclasses
   **/
  virtual void parent_annotate(unsigned level, bool annotate_pos) = 0;

};

template<class Content>
PsTree<Content>::PsTree()
: Tree<Content>()
{
}

template<class Content>
PsTree<Content>::PsTree( const Content& content )
: Tree<Content>(content)
{
}

template<class Content>
PsTree<Content>::PsTree( const Content& content, const std::vector<PsTree>& trees )
  : Tree<Content>(content,trees)
{
}

#endif // PSTREE_H
