#include "Treebank.h"


#include <boost/spirit/home/phoenix/bind/bind_member_function.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/home/phoenix/bind/bind_function.hpp>
#include <boost/spirit/include/phoenix_container.hpp>
#include <boost/spirit/home/phoenix/scope/local_variable.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/home/phoenix/scope/let.hpp>

namespace px = boost::phoenix;
namespace px_lvar = px::local_names;

template <class T>
std::vector<T>&  Treebank<T>::get_trees()
{
  return trees;
}

template <class T>
const std::vector<T>&  Treebank<T>::get_trees() const
{
  return trees;
}


template < class T >
Treebank<T>::~Treebank()
{
}

template < class T >
void Treebank<T>::add_tree(T& tree)
{
  if(options.max_size == 0 ||  tree.number_of_leaves() < options.max_size) {
    if(!options.labels_to_remove.empty()) {
      tree.clean(options.labels_to_remove);    
    }

    if( options.func)
      tree.remove_function();
    
    if(options.num)
      tree.remove_numbers(options.num_regex);
    
    if(options.pannotate > 0)
      tree.parent_annotate(options.pannotate,options.pannotate_extra);
    
    
    if(options.remove_same_unary)
      tree.remove_useless_unary_chains();
    
    //  std::cout << tree << std::endl;
    
    if(options.dir != NONE)
      tree.binarise(options.dir,options.mark);
    
    
    //  std::cout << tree << std::endl;
    
    
    trees.push_back(tree);
  }
}

template < class T >
unsigned Treebank<T>::get_size()
{
  return trees.size();
}

template<class T>
void Treebank<T>::productions(std::vector<Production>& internals, std::vector<Production>& lexicals) const
{
  typedef typename std::vector<T>::const_iterator iterator;

  for(iterator t_iter(trees.begin()); t_iter != trees.end(); ++t_iter)
    t_iter->productions(internals,lexicals);
}

//assumes a grammar with binary and unary rules only
template<class T>
void Treebank<T>::collect_internal_counts(std::map<Production, double> & binary_counts,
					  std::map<Production, double> & unary_counts,
					  std::map< int, double> & LHS_counts) const
{
  typedef typename std::vector<T>::const_iterator iterator;

  for(iterator t_iter(trees.begin()); t_iter != trees.end(); ++t_iter)
    t_iter->collect_internal_counts(binary_counts,unary_counts,LHS_counts);
}


template<class T>
inline
void Treebank<T>::clear()
{
  trees.clear();
}


template <>
void Treebank<PtbPsTree>::add_tree_from_files(const std::vector<std::string>& filenames)
{  
  for(unsigned int i = 0; i< filenames.size();++i) {
    if (verbose) std::clog << "Reading " << filenames[i] << "\r";
    std::vector<PtbPsTree> trees;
    PTBInputParser::from_file(filenames[i].c_str(),trees);
    std::for_each(trees.begin(),trees.end(),
     		  px::bind(&Treebank<PtbPsTree>::add_tree,*this,px::arg_names::arg1)
     		  );
  }
  if(verbose)
    std::clog << std::endl;
}


template<class T>
std::ostream& operator<<(std::ostream& os, const Treebank<T>& treebank)
{
  std::for_each(treebank.trees.begin(),treebank.trees.end(),
		os << px::arg_names::arg1 << "\n");
  return os;
}


template <class T>
void Treebank<T>::output_unbinarised(std::ostream& out) const
{
  for(typename std::vector<T>::const_iterator i(trees.begin()) ; i != trees.end(); ++i) {
    T copy = *i;
    copy.unbinarise();
    out <<  copy << std::endl;
  }
}


template class Treebank<PtbPsTree>;
