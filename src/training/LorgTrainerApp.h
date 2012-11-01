// -*- mode: c++ -*-
#ifndef LORGTRAINERAPP_H
#define LORGTRAINERAPP_H

#include "LorgApp.h"

#include "Treebank.h"
#include "TrainingGrammar.h"
#include "lexicon/LexiconFactory.h"

class Lexicon;

class LorgTrainerApp : public LorgApp
{
public:
  LorgTrainerApp();
  ~LorgTrainerApp();
  int run();

private:
  LorgOptions get_options() const;
  bool read_config(ConfigTable& configuration);

  // create a zero-length file to test output
  // param suffix the suffix of the grammar, after base outputname
  // param num additional suffix (nb of iterations)
  void test_grammar(const std::string & suffix, int num = -1);
  void write_grammar(const TrainingGrammar& grammar, const std::string & suffix, int num = -1);
  void create_terminal_grammar(const Treebank<PtbPsTree>& original_tb);

  Lexicon * lexicon;
  LexiconFactory::lex_type lexicon_type;


  Treebank<PtbPsTree> * tb;

  int filter_level;
  std::string baseoutputname;
  unsigned split_size;
  unsigned n_iterations;

  unsigned merge_em;
  unsigned split_em;

  bool turn_off_merge;
  bool base_grammar_only;

  double smooth_grammar;
  double smooth_lexicon;
  bool hard_smoothing;
  TrainingGrammar::SmoothType smooth_method;


  unsigned split_randomness;
  int merge_percentage;
  long double prob_threshold;
  bool lexical_smoothing;

  std::string modified_treebank_name;

  bool final_lex_em;

  unsigned nbthreads;

  // prevent copying
  LorgTrainerApp(const LorgTrainerApp&);
  LorgTrainerApp& operator=(const LorgTrainerApp&);
};

#endif // LORGTRAINERAPP_H
