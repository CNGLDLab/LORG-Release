// -*- mode: c++ -*-
#ifndef LEXICON_H_
#define LEXICON_H_

#include <memory>

#include "utils/PtbPsTree.h"
#include "rules/LexicalRuleTraining.h"
#include "utils/SymbolTable.h"
#include "WordSignature.h"

#include "edges/BinaryTrainingTree.h"

class Lexicon
{
protected:
  std::vector<LexicalRuleTraining> lexical_rules;
  std::vector<LexicalRuleTraining> additional_rules;
  Lexicon() : lexical_rules(), additional_rules() {}
public:
  virtual ~Lexicon();


  const std::vector<LexicalRuleTraining>& get_lexical_rules() const;
  std::vector<LexicalRuleTraining>& get_lexical_rules();

  const std::vector<LexicalRuleTraining>& get_additional_rules() const;
  std::vector<LexicalRuleTraining>& get_additional_rules();


  virtual void read_lexicon_from_Treebank(std::vector<PtbPsTree>& treebanktrees) = 0;
  virtual std::string get_word_class(const std::string& word, int position) const = 0;

  virtual void update_annotated_counts_from_trees(const std::vector<BinaryTrainingTree> & trees,
						  bool last_iteration,
                                                  std::vector< std::pair<LexicalRuleTraining*, std::vector<lrule_occurrence> > >& lex_occurrences,
                                                  unsigned nbthreads)=0;

  virtual void maximisation()=0;

  virtual void add_lexicon_annotated_node_counts(std::vector< std::vector<double> >& AnnotatedNodeCountMap)=0;

  virtual void reset_counts()=0;
  virtual void resize_structures(AnnotatedLabelsInfo& AnnotatedLabelsInfo)=0;

  virtual Lexicon * copy() const = 0;
  virtual void copy(Lexicon*& dest) const = 0;

  virtual void lexical_smoothing() = 0;

  virtual void create_additional_rules() = 0;

};

inline
const std::vector<LexicalRuleTraining>& Lexicon::get_lexical_rules() const
{
  return lexical_rules;
}


inline
std::vector<LexicalRuleTraining>& Lexicon::get_lexical_rules()
{
  return lexical_rules;
}

inline
const std::vector<LexicalRuleTraining>& Lexicon::get_additional_rules() const
{
  return additional_rules;
}


inline
std::vector<LexicalRuleTraining>& Lexicon::get_additional_rules()
{
  return additional_rules;
}


#endif /*LEXICON_H_*/
