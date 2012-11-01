// -*- mode: c++ -*-
#ifndef BASICLEXICON_H_
#define BASICLEXICON_H_

#include "Lexicon.h"

class BasicLexicon : public Lexicon
{
private:
  unsigned unknown_word_cutoff;

  //TODO shouldn't this be moved to base class?
  const WordSignature& unknown_word_map;

  struct lexical_pair
  {
    int label;
    int word;
    lexical_pair(int l, int w) : label(l), word(w) {};
    bool operator<(const lexical_pair& other) const
    {
      return label < other.label || (label == other.label && word < other.word);
    }
  };
  typedef std::map<lexical_pair, unsigned> lexical_counts_map;

  /////////////////////////////////////////
  //datastructures for annotated counts
  //////////////////////////////////////

  // lhs -> vector of probs (1 per annot)
  typedef std::map<int, std::vector<double> > AnnotatedNodeCountMap;
  AnnotatedNodeCountMap annotated_node_counts;

  void update_node_counts();
  void replace_words_with_unknown(const std::set<std::string>& known_words, std::vector<PtbPsTree>& treebanktrees);
  void collect_base_lexical_counts(const std::vector<PtbPsTree>& treebanktrees,
				   std::map<int,unsigned>& LHS_counts, std::map<int,unsigned>& RHS_counts,
				   lexical_counts_map& lexical_counts);

  void update_annotated_counts_from_trees(const std::vector<BinaryTrainingTree> & trees,
					  bool last_iteration,
                                          std::vector< std::pair<LexicalRuleTraining*, std::vector<lrule_occurrence> > >& lex_occurrences,
                                          unsigned nbthreads);

  void update_annotated_rule_counts(LexicalRuleTraining& rule, const AnnotationInfo& up_annotations,
				    const scaled_array& root_insides) const;

  void traverse_leaf_nodes(const BinaryTrainingTree& tree);

  void maximisation();
  void output_counts(const std::map<int, int>& LHS_counts, const std::map<int ,int>& RHS_counts,
		     const lexical_counts_map& lexical_counts, const lexical_counts_map& lex_and_unknown_counts);

 public:
  ~BasicLexicon();

  BasicLexicon(const WordSignature& unknown_word_mapping,int unknown_word_cutoff=0);

  std::string get_word_class(const std::string& word, int position) const;

  void read_lexicon_from_Treebank(std::vector<PtbPsTree>& treebanktrees);

  void reset_counts();


  void add_lexicon_annotated_node_counts(std::vector< std::vector<double> >& /*AnnotatedNodeCountMap*/);
  Lexicon * copy() const;
  void copy(Lexicon*& other) const;

  void resize_structures(AnnotatedLabelsInfo& /*AnnotatedLabelsInfo*/);


  void lexical_smoothing();
  void create_additional_rules();

  friend class basic_lexicon_update_thread;
};

#endif /*BASICLEXICON_H_*/
