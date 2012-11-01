// -*- mode: c++ -*-
#ifndef _URULETRAINING_H_
#define _URULETRAINING_H_

#include "URule.h"

#include <set>
#include <boost/unordered_map.hpp>

#include "grammars/AnnotatedLabelsInfo.h"

#include "edges/AnnotationInfo.h"

typedef  std::map<int,std::vector<int> > Merge_map;
typedef std::map<int,std::set<int> > Merge_lookup;
typedef std::vector< std::vector<double> > AnnotatedNodeCountMap;
typedef AnnotatedNodeCountMap ProportionsMap;


class URuleTraining : public URule
{
public:
  URuleTraining() : URule(), counts() {};
  ~URuleTraining() {};

  URuleTraining(int lhs, int rhs, double& proba) :
    URule(lhs, rhs, proba), counts() {};
  
  URuleTraining(const URule& r) : URule(r), counts() {};


  void update_inside_annotations(std::vector<double>& up,
				 const std::vector<double>& left) const;
  
  void update_outside_annotations(const std::vector<double>& up,
				  std::vector<double>& left) const;

  /**
     \brief split each annotation and recompute probabilities
     \param n the number of sub-divisions
     \param randomness deviation percentage
  */
  void split(unsigned n, unsigned randomness = 1);
  
  
  /**
     \brief merge each annotation set specified in annotation_sets_to_merge and recompute probabilities
     \param  annotation_sets_to_merge data structure created in em trainer which contains a map with key=unannotatedLabel and value= vector of 
     \ start indices.   map: label -> [vector of start indices of annotation sets that will be merged]
     \the start index of an annotation set is the first annotataion of the annotation set.
     \eg an annotation with start index 3 and split size 4, will contain annotations: {3,4,5,6}
     \param split_size a label is split into split_size new annotated labels 
     \param proportions data structure containing the proportions calculation (calculated originally in EMTrainer) for each annotated label
     \param annotations_info info on the number of annotations per non-terminal label
  */
  void merge(const Merge_map& annotation_sets_to_merge, 
	     int split_size,
	     const ProportionsMap& proportions,
	     const AnnotatedLabelsInfo& annotations,
	     const std::vector<std::map<int,int> >& annot_reorder);
  
 
  void test_annotations(const std::map<int,unsigned> & lookup) const;
  
  const std::vector<std::vector<double> >& get_counts() const;
  std::vector<std::vector<double> >&  get_counts();

  void reset_counts();
  void reset_counts_soft();

  void update_probability(const AnnotatedNodeCountMap&);
  void update_rule_frequencies(const scaled_array& left_in,
			       const scaled_array& up_out,
			       const scaled_array& root_insides);
private:

  std::vector<std::vector<double> > counts;

};

inline
const std::vector<std::vector<double> >& URuleTraining::get_counts() const
{
  return counts;
}

inline
std::vector<std::vector<double> >&  URuleTraining::get_counts()
{
  return counts;
}


#endif /* _URULETRAINING_H_ */
