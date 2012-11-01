// -*- mode: c++ -*-
#ifndef BERKELEYLEXICON_H_
#define BERKELEYLEXICON_H_

#include "Lexicon.h"
#include <cctype>

class BerkeleyLexicon : public Lexicon
{
private:

  const WordSignature& unknown_word_map;

  double unknown_treebank_proportion;
  unsigned smoothing_threshold;
  double smooth_parameter_0;
  double smooth_parameter_1;

  //data structures for storing corpus counts
  ////////////////////////////////////////////////////////////////////////////
  double all_tokens_total;
  double unseen_tokens_total;

  std::map<int,double> word_counts;

  //for quick lookup to see if a word is known
  std::set<int> known_words;  //all the original unmapped words (no signatures)
  std::set<int> known_signatures;  //all the signatures corresponding to words in the training corpus
  //
  std::map<int, std::vector<double> > all_annotated_tag_counts;
  std::map<int, std::vector<double> > unseen_annotated_tag_counts;
  typedef std::map<int, std::map<int, std::vector<double> > > id1_id2_annotated_counts_map;
  id1_id2_annotated_counts_map annotated_tag_word_counts;
  /////////////////////////////////////////////////////////////////////////////

  //data structures for storing probabilities
  //  std::map<int,double> p_word;
  //  std::map<int,std::vector<double> > p_annotated_tag;
  std::map<int,std::vector<double> > p_annotated_tag_given_unknown;
  // //id1_id2_counts_map p_word_given_tag;

  // //these should probably be put somewhere else, higher up the hierarchy or in utils
  void collect_counts_from_treebank(std::vector<PtbPsTree>& treebanktrees);
  void calculate_probabilities();

  void initialise_probability_vectors();

  void traverse_leaf_nodes(const BinaryTrainingTree& tree, bool second_half, bool last_iteration);
  std::vector<double> compute_weight(const LexicalRuleTraining& rule,
				     const AnnotationInfo& IO_scores,
				     const scaled_array& tree_probability) const;

  void update_annotated_counts_from_node(int lhs, int word,
					 const std::vector<double>& weight);
  void update_annotated_counts_from_node_4unknown(int lhs, int word,
						  int word_signature,
						  const std::vector<double>& weight);
  void create_additional_rules();

  //debug method
  void output_counts();
  void print_rule_probabilities();
  void print_leaf_node_probs(BinaryTrainingTree* /*tree*/);


public:
  ~BerkeleyLexicon();

  BerkeleyLexicon(const WordSignature& unknown_word_mapping_,double unknown_treebank_proportion, int smoothing_threshold);

  void read_lexicon_from_Treebank(std::vector<PtbPsTree>& treebanktrees);

  void update_annotated_counts_from_trees(const std::vector<BinaryTrainingTree> & trees,
					  bool last_iteration,
                                          std::vector< std::pair<LexicalRuleTraining*, std::vector<lrule_occurrence> > >& lex_occurrences,
                                          unsigned nbthreads);

  std::string get_word_class(const std::string& word, int position) const;

  void maximisation();

  void add_lexicon_annotated_node_counts(std::vector< std::vector<double> >& /*AnnotatedNodeCountMap*/);

  void reset_counts();
  Lexicon * copy() const;
  void copy(Lexicon*& dest) const;

  void resize_structures(AnnotatedLabelsInfo& annotated_labels_info);

  void lexical_smoothing();

};


#endif /*BERKELEYLEXICON_H_*/
