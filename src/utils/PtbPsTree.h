// -*- mode: c++ -*-
#ifndef PTBPSTREE_H
#define PTBPSTREE_H

#include "PsTree.h"

#include <boost/unordered_map.hpp>
#include <boost/regex.hpp>
#include <iostream>

enum Bin_Direction {LEFT, RIGHT, NONE};
typedef int HorizMarkov; // negative value for infinite horizontal markovisation


/**
  \class PtbPsTree
  \brief This represents a penn treebank tree
**/

typedef std::string Content;

class PtbPsTree : public PsTree<Content>
{
public:
  /**
     \brief Default Constructor
   **/
  PtbPsTree();
  PtbPsTree( const Content& content );
  PtbPsTree( const Content& content, const std::vector<PtbPsTree>& daughters);
  ~PtbPsTree();

  /**
     \brief removes traces from a tree
     \param labels_to_remove labels to remove from the tree
   **/
  void clean(const boost::unordered_set<std::string>& labels_to_remove);

  /**
     \brief remove chains X -> X from a tree
   **/
  void remove_useless_unary_chains();


  /**
     \brief removes function from internal nodes
  **/
  void remove_function();

  /**
     \brief removes numbers from leaves
     \param num_regex a regex describing numbers
  **/
  void remove_numbers( const boost::regex& num_regex );

  /**
     \brief un-binarise a tree
   **/
  void unbinarise();

  
  /**
     \brief binarise a tree
   **/
  void binarise(Bin_Direction, HorizMarkov);


  void parent_annotate(unsigned level, bool extra);
  void productions(std::vector<Production>& internals,std::vector<Production>& lexicals) const;

  void collect_internal_counts( std::map<Production, double> & binary_counts,
				std::map<Production, double> & unary_counts,
				std::map<int, double> & LHS_counts) const;

  void coarse_terminal(bool prime,
		       boost::unordered_map< std::string, boost::unordered_set<std::string> >& coarse_to_fine_map);

};

#endif // PTBPSTREE_H
