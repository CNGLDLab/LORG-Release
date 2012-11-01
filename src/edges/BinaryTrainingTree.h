// -*- mode: c++ -*-
#ifndef _BINARYTRAININGTREE_H_
#define _BINARYTRAININGTREE_H_

#include "utils/PtbPsTree.h"

#include "TrainingNode.h"

class BinaryTrainingTree
{
public:
  BinaryTrainingTree();
  ~BinaryTrainingTree();

protected:
  TrainingNode * root;

public:
  /**
     \brief Constructor transforms a ptbpstree into a binary
     \param tree a tree assumed to be binarized
     \param brules optional rules to incorporate rules in packed_edge
     \param urules optional rules to incorporate rules in packed_edge
     \param lrules optional rules to incorporate rules in packed_edge
  */

  BinaryTrainingTree(const PtbPsTree& tree,
		     MAP<std::pair<int,std::pair<int,int> >,BRuleTraining*> * brulemap,
		     MAP<std::pair<int,int>,URuleTraining*> * urulemap,
		     MAP<std::pair<int,int>,LexicalRuleTraining*> * lrulemap);

  const TrainingNode * get_root() const {return root;}

  void update_rule_frequencies();


    /**
     \brief  traverses tree; at each node calls calculate_delta_score with same parameter list.
     See calculate_delta_score for explanation of paramaters
     \param delta_scores_map
     \param split_number size of the split
     \param proportions
     \param tree_likelihood
     \TODO write a proper independent tree traversal function that can have a pointer to a
     function passed in - function will act on each node.
     \ a visitor design pattern
  */
  void update_delta_scores(DeltaMap&  delta_scores_map,int split_number, const ProportionsMap& proportions,
			   const scaled_array& root_inside) const;


  void compute_inside_probability();
  void compute_outside_probability();

  // void get_rules(boost::unordered_set<const BRule*>& brules,
  // 		 boost::unordered_set<const URule*>& urules,
  // 		 boost::unordered_set<const URule*>& lrules) const;


  void resize_annotations(const AnnotatedLabelsInfo & lookup);

  void reset_inside_probabilities(const double& value = 0);
  void reset_outside_probabilities(const double& value = 0);
  void reset_probabilities(const double& value = 0);

  BinaryTrainingTree* get_left_daughter();
  BinaryTrainingTree* get_right_daughter();

  bool contains_empty_rules() const;

  double get_probability() const;
  double get_log_probability() const ;

  std::vector<LexicalTrainingNode*> get_lexical_nodes() const;

  static TrainingNode * create_training_node(const PtbPsTree& tree,
					     PtbPsTree::const_depth_first_iterator current,
					     MAP<std::pair<int,std::pair<int,int> >,BRuleTraining*> * brulemap,
					     MAP<std::pair<int,int>,URuleTraining*> * urulemap,
					     MAP<std::pair<int,int>,LexicalRuleTraining*> * lrulemap);

  void print_leaf_node_probs();


  // to make the compiler happy ...
  BinaryTrainingTree& operator=(const BinaryTrainingTree&);
  BinaryTrainingTree(const BinaryTrainingTree& other) : root(other.root) {}
private:



  /**
     \brief create a bttree from a ptbpstree
  */
  BinaryTrainingTree(const PtbPsTree& tree, PtbPsTree::const_depth_first_iterator current,
		     MAP<std::pair<int,std::pair<int,int> >,BRuleTraining*> * brulemap,
		     MAP<std::pair<int,int>,URuleTraining*> * urulemap,
		     MAP<std::pair<int,int>,LexicalRuleTraining*> * lrulemap);

  void resize_annotations(unsigned size);
  void calculate_delta_score(DeltaMap &  delta_scores_map, int split_size,
			     const ProportionsMap & proportions,double tree_likelihood) const;
};

// the tree probability is the inside probability of
// the top node (which is not annotated)
  inline double BinaryTrainingTree::get_probability() const
  {
    return root->annotations.inside_probabilities.get_scaled_value(0);
  }

  inline double BinaryTrainingTree::get_log_probability() const
  {
    return root->annotations.inside_probabilities.get_scaled_logvalue(0);
  }

inline
std::vector<LexicalTrainingNode*> BinaryTrainingTree::get_lexical_nodes() const
{
  std::vector<LexicalTrainingNode*> result;
  root->get_lexical_nodes(result);
  return result;
}

inline
void BinaryTrainingTree::reset_inside_probabilities(const double& value)
{
  root->reset_inside_probabilities(value);
}

inline
void BinaryTrainingTree::reset_outside_probabilities(const double& value)
{
 root->reset_outside_probabilities(value);
}

inline
void BinaryTrainingTree::reset_probabilities(const double& value)
{
  root->reset_probabilities(value);
}

inline
void BinaryTrainingTree::resize_annotations(unsigned size)
{
  // this is recursive
  root->resize_annotations(size);
  // but we don't want to resize the TOP node
  root->annotations.resize(1);
}

inline
void BinaryTrainingTree::resize_annotations(const AnnotatedLabelsInfo & lookup)
{
  root->resize_annotations(lookup);
}

inline
void BinaryTrainingTree::compute_inside_probability()
{
  root->compute_inside_probability();
}

inline
void BinaryTrainingTree::compute_outside_probability()
{
  //TOP node has an outside probability of 1
  root->annotations.reset_outside_probabilities(1);
  root->compute_outside_probability();
}

inline
void BinaryTrainingTree::update_rule_frequencies()
{
  root->update_rule_frequencies(root->get_annotations().inside_probabilities);
}

inline
void BinaryTrainingTree::update_delta_scores(DeltaMap &  delta_scores_map,int split_number,
					     const ProportionsMap & proportions,
					     const scaled_array& root_inside) const
{
  root->update_delta_scores(delta_scores_map,split_number,proportions,root_inside);
}

inline
bool BinaryTrainingTree::contains_empty_rules() const
{
  return root->contains_empty_rules();
}

inline
BinaryTrainingTree& BinaryTrainingTree::operator=(const BinaryTrainingTree& other)
{
  root= other.root;
  return *this;
}


#endif /* _BINARYTRAININGTREE_H_ */
