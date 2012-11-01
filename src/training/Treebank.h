// -*- mode: c++ -*-

#ifndef TREEBANK_H
#define TREEBANK_H

#include <vector>
#include <iostream>
#include "boost/unordered_set.hpp"

#include "boost/regex.hpp"

#include "rules/Production.h"

#include "utils/PtbPsTree.h"
#include "utils/data_parsers/PTBInputParser.h"




struct treebank_options {
  boost::unordered_set<std::string> labels_to_remove; // labels to remove
  bool func;            //remove functional annotations
  bool num;             // replace numbers by |NUMBER|
  boost::regex num_regex; // regex to recognize numbers
  unsigned pannotate;   // parent annotation
  bool pannotate_extra; // extend parent annotation to parts of speech
  bool remove_same_unary;//remove unary chains X -> X
  Bin_Direction dir;    // direction for binarisation
  HorizMarkov   mark;    // horizontal precision of the binarisation
  unsigned int max_size;// maximum size for the input sentences to be added to the treebank (0 means no limit)
  
treebank_options(const boost::unordered_set<std::string>& labels_to_remove_ = boost::unordered_set<std::string>(), 
		 bool func_ = true,
		 bool num_ =  false,
		 const boost::regex & num_regex_ = boost::regex(),
		 bool pannotate_ = 0, 
		 bool pannotate_extra_ = false, 
		 bool remove_same_unary_= false,
		 Bin_Direction dir_ = LEFT, 
		 HorizMarkov mark_ = -1, //infinite -> "exact" binarization
		 unsigned max_size_ = 0) :
  labels_to_remove(labels_to_remove_),func(func_),num(num_), num_regex(num_regex_),
    pannotate(pannotate_), pannotate_extra(pannotate_extra_),
    remove_same_unary(remove_same_unary_), dir(dir_),mark(mark_),
    max_size(max_size_)
  {}
};


/**
   \class Treebank
   \brief performs global operation on a set of trees
 **/
template<class T>
class Treebank
{
private:
  std::vector<T> trees; ///< the trees of the treebank
  treebank_options options;
  bool verbose;
  
public:
  /**
     \brief Empty Treebank
  **/
  Treebank(bool verb = false) : trees(), options(), verbose(verb) {};
  
  Treebank(const treebank_options& tb_options, bool verb = false)
    : trees(), options(tb_options) , verbose(verb)
  {};

  /**
     \brief Destructor
   **/
  ~Treebank();

  /**
     \brief add a tree  to the treebank
     \param tree a tree to add
   **/
  void add_tree( T& tree);

  /**
     \brief return the size of the treebank
   **/
  unsigned get_size();
  /**
     \brief direct access to the trees of the treebank
   **/
  std::vector<T>& get_trees();
  const std::vector<T>& get_trees() const;

  /**
     \brief erase all trees
   **/
  void clear();

  /**
     \brief adds all the production rules contained in a treebank to its parameters
     \param il the internal rules
     \param ll the lexical rules
   **/
  void productions(std::vector<Production>& il,std::vector<Production>& ll) const;

  /**
     \brief iterate through each tree in treebank, calling collect_internal_counts on each individual tree
     \	assumes max number of children = 2
     \param binary_counts counts for LHS -> RHS0 RHS1
     \param unary_counts counts for LHS -> RHS0
     \param LHS_counts wot it says on the tin
   **/
  void collect_internal_counts(  std::map<Production, double> & binary_counts,
				 std::map<Production, double> & unary_counts,
				 std::map<int, double> & LHS_counts) const;

  void add_tree_from_files(const std::vector<std::string>& filenames);

  /**
     \brief Output operator
     \param os the ostream to write on
     \param tb the treebank object to write
     \return the used ostream
  */  
  template< class U>
  friend std::ostream& operator<<(std::ostream& os, const Treebank<U>& tb);


  void output_unbinarised(std::ostream& out) const;

};



#endif //TREEBANK_H
