// -*- mode: c++ -*-

#ifndef _VITERBIPROBABILITY_H_
#define _VITERBIPROBABILITY_H_

#include "PackedEdgeProbability.h"
#include "PackedEdge.h"


class ViterbiProbability //: public PackedEdgeProbability
{
private:
  std::vector<packed_edge_probability> best;
public:
  ViterbiProbability() {};
  ViterbiProbability(unsigned size) : best(size) {};

  void set_size(unsigned size_) {best.resize(size_);}


  const packed_edge_probability& get(unsigned index) const
  {return best[index];}

  packed_edge_probability& get(unsigned index)
  {return best[index];}

  void update(const AnnotationInfo& a, const LexicalPackedEdgeDaughters& dtr);

  void update(const AnnotationInfo& a, const UnaryPackedEdgeDaughters<PCKYAllCell<PackedEdge<ViterbiProbability> > >& dtr);

  void update(const AnnotationInfo& a, const BinaryPackedEdgeDaughters<PCKYAllCell<PackedEdge<ViterbiProbability> > >& dtr);

  //  void extend_derivation(PackedEdge*, unsigned, bool) {};
  void finalize() {};
  bool has_solution(unsigned i) const {return i == 0;} ;
};


//#include "ViterbiProbability.h"
//#include "PackedEdge.h"

#include "PCKYAllCell.h"

void ViterbiProbability::update(const AnnotationInfo& a, const LexicalPackedEdgeDaughters& dtr)
{
  const LexicalRuleC2f* rule = dtr.get_rule();

  for (unsigned i = 0; i < rule->get_probability().size(); ++i) {
    if(a.valid_prob_at(i, LorgConstants::NullProba)) {
      double probability = rule->get_probability(i);
      packed_edge_probability& current_best = best[i];
      if (probability > current_best.probability) {
	best[i].probability = probability;
	current_best.dtrs = &dtr;
      }
    }
  }
}

void ViterbiProbability::update(const AnnotationInfo& a, const UnaryPackedEdgeDaughters<PCKYAllCell<PackedEdge<ViterbiProbability> > >& dtr)
{
  const std::vector<std::vector<double> >& rule_probs = dtr.get_rule()->get_probability();

  const PackedEdge<ViterbiProbability>& left = (dtr.left_daughter())->at(dtr.get_rule()->get_rhs0());

  for (unsigned i = 0; i < rule_probs.size(); ++i) {
    if(!a.valid_prob_at(i, LorgConstants::NullProba)) continue;
    packed_edge_probability& current_best = best[i];
    const std::vector<double>& rule_probs_i = rule_probs[i];
    for (unsigned j = 0; j < rule_probs_i.size(); ++j) {
      if(!left.valid_prob_at(j)) continue;
      double probability = rule_probs_i[j] + left.get_best().get(j).probability; // log-mode
      if (probability > current_best.probability) {
	current_best.probability = probability;
	current_best.dtrs = &dtr;
	current_best.left_index = j;
      }
    }
  }
}

void ViterbiProbability::update(const AnnotationInfo& a, const BinaryPackedEdgeDaughters<PCKYAllCell<PackedEdge<ViterbiProbability> > >& dtr)
{
  const std::vector<std::vector<std::vector<double> > >& rule_probs = dtr.get_rule()->get_probability();

  const PackedEdge<ViterbiProbability>& left  = dtr.left_daughter()->at(dtr.get_rule()->get_rhs0());
  const PackedEdge<ViterbiProbability>& right = dtr.right_daughter()->at(dtr.get_rule()->get_rhs1());

  for (unsigned i = 0; i < rule_probs.size(); ++i) {
    if(!a.valid_prob_at(i, LorgConstants::NullProba)) continue;
    packed_edge_probability& current_best = best[i];
    const std::vector<std::vector<double> >& rule_probs_i = rule_probs[i];
    for (unsigned j=0; j < rule_probs_i.size(); ++j) {
      if(!left.valid_prob_at(j)) continue;
      const std::vector<double>& rule_probs_ij = rule_probs_i[j];
      const double& left_best = left.get_best().get(j).probability;
      for (unsigned k = 0; k < rule_probs_ij.size(); ++k) {
	if(!right.valid_prob_at(k)) continue;
	double probability = rule_probs_ij[k] + left_best + right.get_best().get(k).probability; //log
	if (probability > current_best.probability) {
	  //std::cout << " best so far " << std::endl;
	  current_best.probability = probability;
	  current_best.dtrs = &dtr;
	  current_best.left_index = j;
	  current_best.right_index= k;
	}
      }
    }
  }
}



#endif /* _VITERBIPROBABILITY_H_ */
