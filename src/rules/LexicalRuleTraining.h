// -*- mode: c++ -*-
#ifndef _LEXICALRULETRAINING_H_
#define _LEXICALRULETRAINING_H_

#include "LexicalRule.h"
#include "grammars/AnnotatedLabelsInfo.h"

#include <boost/unordered_map.hpp>

typedef  std::map<int,std::vector<int> > Merge_map;
typedef std::map<int,std::set<int> > Merge_lookup;
typedef std::vector< std::vector<double> > AnnotatedNodeCountMap;
typedef AnnotatedNodeCountMap ProportionsMap;


class LexicalRuleTraining : public LexicalRule
{
public:

  // to make the compiler happy
  LexicalRuleTraining();

  /**
     \brief constructor for creating unary rules for the base grammar (annotation sizes always 1)
     \param l the lhs of the object
     \param rhs0  the leftmost symbol of rhs
     \param double probability
  */
  LexicalRuleTraining(short l, int rhs0, double prob);
  
  /**
     \brief constructor for creating unary rules for the base grammar (annotation sizes always 1)
     \param l the lhs of the object
     \param rhs0  the leftmost symbol of rhs
     \param double probability
     \param size size of annotations for lhs
  */
  LexicalRuleTraining(short l, int rhs0, double prob, unsigned size);



  ~LexicalRuleTraining() {};


  void update_inside_annotations(std::vector<double>& up) const;
  
  void update_outside_annotations(const std::vector<double>& up,
				  double& left) const;


  void reset_counts();
  void reset_counts_soft();

  /**
     \brief merge each annotation set specified in annotation_sets_to_merge and recompute probabilities
     \param  annotation_sets_to_merge data structure created in em trainer which contains a map with 
     \ key=unannotatedLabel and value= vector of start indices.   map: label -> [vector of start indices 
     \ of annotation sets that will be merged]
     \the start index of an annotation set is the first annotataion of the annotation set.
     \eg an annotation with start index 3 and split size 4, will contain annotations: {3,4,5,6}
     \param split_size a label is split into split_size new annotated labels 
     \param proportions data structure containing the proportions calculation (calculated originally in EMTrainer) for each annotated label
     \param annotations_info info on the number of annotations per non-terminal label
  */
  void merge(const Merge_map& annotation_sets_to_merge, 
	     int split_size,
	     const ProportionsMap& proportions,
	     const AnnotatedLabelsInfo& a,
	     const std::vector<std::map<int,int> >& annot_reorder);
  
  const std::vector<double>& get_counts() const;
  std::vector<double>&  get_counts();


  /**
     \brief split left handside and copy probabilities
     \param n the number of sub-divisions
  */
  void split(unsigned n, double /*randomness*/);
  

  double get_count(unsigned i);
  
private:
  std::vector<double> counts;
};


inline 
double LexicalRuleTraining::get_count(unsigned i)
{
  return counts[i];
}

inline
const std::vector<double>& LexicalRuleTraining::get_counts() const
{
  return counts;
}

inline
std::vector<double>&  LexicalRuleTraining::get_counts()
{
  return counts;
}

inline
void LexicalRuleTraining::reset_counts()
{
  counts = std::vector<double>(probabilities.size(),0.0);
}

inline
void LexicalRuleTraining::reset_counts_soft()
{
  std::fill(counts.begin(), counts.end(), 0.0);
}


#endif /* _LEXICALRULETRAINING_H_ */
