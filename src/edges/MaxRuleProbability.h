// -*- mode: c++ -*-
#ifndef _MAXRULEPROBABILITY_H_
#define _MAXRULEPROBABILITY_H_
#include <numeric>

#include "PackedEdgeProbability.h"
#include "PackedEdge.h"
#include "PCKYAllCell.h"
#include "maxrule_functions.h"


class MaxRuleProbability
{
private:
  packed_edge_probability best;
  static double log_normalisation_factor;
public:
  MaxRuleProbability() : best() {};
  ~MaxRuleProbability() {};

  static void set_log_normalisation_factor(double lnf);

  const packed_edge_probability& get(unsigned/*ignored*/) const
  {return best;}
  packed_edge_probability& get(unsigned /*ignored*/)
  {return best;}

  void update(const AnnotationInfo& a, const LexicalPackedEdgeDaughters& dtr);

  void update(const AnnotationInfo& a, const UnaryPackedEdgeDaughters<PCKYAllCell<PackedEdge<MaxRuleProbability> > >& dtr);

  void update(const AnnotationInfo& a, const BinaryPackedEdgeDaughters<PCKYAllCell<PackedEdge<MaxRuleProbability> > >& dtr);

  void finalize();

  unsigned n_deriv() const {return 1;}

  bool has_solution(unsigned i) const {return i == 0;} ;

};

double MaxRuleProbability::log_normalisation_factor = 0;

void MaxRuleProbability::set_log_normalisation_factor(double lnf)
{
  log_normalisation_factor = lnf;
}

void MaxRuleProbability::update(const AnnotationInfo& a, const LexicalPackedEdgeDaughters& dtr)
{
  //  std::cout  << "MaxRuleProbability::update lexical" << std::endl;

  const LexicalRuleC2f* rule = dtr.get_rule();

  //std::cout << "update with " << *rule << std::endl;

  double probability = maxrule_function::update_maxrule_probability(a, rule, log_normalisation_factor);

  if (probability > best.probability) {
    best.probability = probability;
    best.dtrs = &dtr;
  }
}


void MaxRuleProbability::update(const AnnotationInfo& a, const UnaryPackedEdgeDaughters<PCKYAllCell<PackedEdge<MaxRuleProbability> > >& dtr)
{
  double probability = -std::numeric_limits<double>::infinity();

  PackedEdge<MaxRuleProbability>& left  = dtr.left_daughter()->at(dtr.get_rule()->get_rhs0());
  if(left.get_best().get(0).dtrs && (left.get_best().get(0).dtrs->is_lexical() || left.get_best().get(0).dtrs->is_binary())) {
    probability =  maxrule_function::update_maxrule_probability<PackedEdge<MaxRuleProbability> >(a, dtr, log_normalisation_factor);
  }

  if (probability > best.probability) {
    best.probability = probability;
    best.dtrs = &dtr;
  }
}

void MaxRuleProbability::update(const AnnotationInfo& a, const BinaryPackedEdgeDaughters<PCKYAllCell<PackedEdge<MaxRuleProbability> > >& dtr)
{
  //calculate the probability for this edge -
  //if it's the best probability so far, update the best edge info

double probability = maxrule_function::update_maxrule_probability<PackedEdge<MaxRuleProbability> >(a, dtr, log_normalisation_factor);

  if (probability > best.probability)
    {
      best.probability = probability;
      best.dtrs = &dtr;
    }
}


// //uncomment the function to get the best indexes
// // only useful if you want to print them out
void MaxRuleProbability::finalize() {}
// // {
// //   if(up) {
// //     //   std::cout << "here" << std::endl;
// //     if(best.dtrs !=  NULL) {
// //       if (best.dtrs->is_binary())
// // 	{
// // 	  //	  std::cout << "there" << std::endl;
// // 	  compute_best_indexes(up->get_annotations(),
// // 			       *static_cast<const BinaryPackedEdgeDaughters*>(best.dtrs),
// // 			       log_normalisation_factor,best.left_index,best.right_index);
// // 	}
// //       else { //unary
// // 	compute_best_indexes(up->get_annotations(),
// // 			     *static_cast<const UnaryPackedEdgeDaughters*>(best.dtrs),
// // 			     log_normalisation_factor,best.left_index);
// //       }
// //     }
// //   }
// // }







////////////



#endif /* _MAXRULEPROBABILITY_H_ */
