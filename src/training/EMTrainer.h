// -*- mode: c++ -*-

#ifndef EMTRAINER_H_
#define EMTRAINER_H_

#include "edges/BinaryTrainingTree.h"

#include "training/TrainingGrammar.h"
#include "grammars/AnnotatedLabelsInfo.h"


#include <boost/unordered_map.hpp>

//first int in pair is nt label id, second int in pair is start index of split in annotations
typedef boost::unordered_map< std::pair<int, int >, double> DeltaMap;
typedef std::multimap<double,std::pair<int,int> > DeltaMultiMap;

/**
  \class EMTrainer
  \brief This class performs the learning of a PCFG using the EM algorithm
*/

class EMTrainer
{

public:
  EMTrainer(int split_number_, double threshold_, bool verbose_, unsigned threads_) :
    verbose(verbose_), split_number(split_number_), log_likelihood(-1000000000),
    threshold(threshold_), annotated_node_counts(), threads(threads_)
  {};

  //where are these typedefs defined?
  /**
     \brief the EM algorithm in action - Expectation Step followed by Maximisation step for a set number of iterations
     \param trees the treebank
     \param num_iterations temp parameter - min and max ignored, num_iterations specifies exactly number of em iterations
     \param smooth_grammar smooth parameter for internal rules
     \param smooth_lexicon smooth parameter for lexical rules
     \param remove_unlikes replace low probabilities with zeros in grammar rules
     \param type the type of smoothing algorithm
  */
  void  do_em(std::vector<BinaryTrainingTree> & trees,
	      TrainingGrammar& em_grammar, unsigned num_iterations,
	      double smooth_grammar, double smooth_lexicon,
	      bool remove_unlikes, TrainingGrammar::SmoothType type);


  /**
     \brief Calculates the likelihood of a set of trees: trees
  */
  double calculate_likelihood(std::vector<BinaryTrainingTree> & trees) const;


  /**
   * 	\brief the merge step, containing four mains steps: calculate proportions; calculate delta scores;  sort delta scores and select splits that will be merged;
   * \ finally, merge the binary, unary, and lexical rules of the grammar.
   \param trees the treebank, needed to calculate proportions
   \param em_grammar The grammar whose binary, unary and lexical rules will be merged.
   \param merge_percentage The percentage of splits to merge
   \param final_lex_em do an EM iteration on the lexicon afterwards
  */
  void  merge(std::vector<BinaryTrainingTree> & trees,
	      TrainingGrammar& em_grammar, int merge_percentage,
	      bool final_lex_em);
  /**
   * 	\brief the smoothing set step
   \param em_grammar The grammar whose binary, unary and lexical rules will be smoothed.
   \param smooth_grammar smooth parameter for internal rules
   \param smooth_lexicon smooth parameter for lexical rules
  */
  void smooth_grammar_rules(TrainingGrammar& em_grammar, double smooth_grammar, double smooth_lexicon,
			    TrainingGrammar::SmoothType type);

  /**
   * 	\brief the split step.  Before splitting the rules in the grammar, the (new) number of annotations per non-terminal label is
   *  \set in the grammar, and also in the treebank
   \param trees the treebank
   \param em_grammar The grammar whose binary, unary and lexical rules will be split.
   \param randomness ?
  */
  void split(std::vector<BinaryTrainingTree> & trees, TrainingGrammar& em_grammar, unsigned randomness);

private:

  bool verbose;
  int split_number;
  double log_likelihood;
  double threshold;

  //needed for merge
  AnnotatedNodeCountMap annotated_node_counts;

  unsigned threads;


  /**
     \brief The Expectation Step of the EM algorithm - obtain new rule frequencies
     \param trees a collection of trees to process
     \param em_grammar, the grammar
     \param last_iteration true if this is the last iteration
  */
  void expectation(std::vector<BinaryTrainingTree> & trees, TrainingGrammar& em_grammar,
		   bool last_iteration,
		   std::vector< std::pair<BRuleTraining*, std::vector<brule_occurrence> > >& vbrule,
		   std::vector< std::pair<URuleTraining*, std::vector<urule_occurrence> > >& vurule,
                   std::vector< std::pair<LexicalRuleTraining*, std::vector<lrule_occurrence> > >& vlrule
		   );  

  /**
     \brief The Maximisation Step of the EM algorithm - calculate new rule probabilities using the new rule counts
     \param em_grammar the grammar containing rules
     \note the rules are modified by this method
  */
  void maximisation(TrainingGrammar& em_grammar);

  /**
     \brief Reset rule and node counts - after new rule probabilities have been calculated (in Maximisation step)
  */
  void reset_counts_soft(TrainingGrammar& grammar);
  void reset_counts(TrainingGrammar& grammar);


  /**
     \brief calculates the denominator for the maximimisation part of EM.  The node count (denominator) for each annotated label is
     \stored in the annotated_node_counts data structure.  Note lexicon node counts calculated elsewhere (in the lexicon).
     \param em_grammar the grammar containing rules
  */
  void calculate_grammar_node_counts(const TrainingGrammar& grammar);


  /**
     \brief calculates the proportion: count of annotated label a, over the count of all annotated labels that are part of the same split.
     \ Say Ap is split into Ap1 and Ap2.  The proportion for Ap1 is P(Ap1|Ap).  The proportions are stored in class member data structure: proportions
     \param annotation_info This contains information on the number annotations for each label in the grammar.
  */
  void calculate_proportions(const AnnotatedLabelsInfo& annotation_info, AnnotatedNodeCountMap& proportions);

  /**
     \brief calculates the delta score for each annotation/split set : the estimate of the loss in the log likelihood of the training set when the split is merged back
     \together.  The x% of split sets with the lowest delta scores will later be merged.
     \param trees the treebank
  */
  void calculate_delta_scores(const std::vector<BinaryTrainingTree> & trees,
			      DeltaMap& delta_scores,
			      const AnnotatedNodeCountMap& proportions);

  /**
     \brief resizes the number of annotations for each node in each tree in the treebank.  Called before grammar split and again before grammar merge
     \param trees the treebank
     \param grammar the grammar
  */
  void resize_annotations_in_treebank(std::vector<BinaryTrainingTree> & trees, const TrainingGrammar& grammar) const;

  void one_em_cycle_lexicon_only(TrainingGrammar& em_grammar,std::vector<BinaryTrainingTree>& trees, std::vector< std::pair<LexicalRuleTraining*, std::vector<lrule_occurrence> > >& vl);


};

#endif /*EMTRAINER_H_*/
