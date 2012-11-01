// -*- mode: c++ -*-
#ifndef _PARSERCKYALLMAXVAR_H_
#define _PARSERCKYALLMAXVAR_H_

#include "ParserCKYAll.h"
#include "edges/MaxRuleProbability.h"


class ParserCKYAllMaxRule : public ParserCKYAll_Impl<MaxRuleProbability>
{
public:
  ParserCKYAllMaxRule(std::vector<AGrammar*>& cgs,
		      const std::vector<double>& p, double b_t,
		      const std::vector< std::vector<std::vector< std::vector<unsigned> > > >& annot_descendants_,
		      bool accurate_, unsigned min_beam, int stubborn, unsigned cell_threads)
    : ParserCKYAll_Impl<MaxRuleProbability>(cgs, p, b_t, annot_descendants_, accurate_, min_beam, stubborn, cell_threads)
  {
    //TODO maybe make this a parser option?
    //create the coarse-to-fine map
    create_coarse_to_fine_mapping(grammars);

    Cell::CellEdge::set_viterbi_unary_chains(grammars[grammars.size() - 1]->get_unary_decoding_paths());
}

  ~ParserCKYAllMaxRule() {};

  void extract_solution();
protected:

  void change_rules_reset() const;


  /**
     \brief Calculates the chart specific rule probabilities of the packed edges in the chart
     and uses this to select the best edge (max rule parsing)
   */
  void calculate_chart_specific_rule_probabilities_and_best_edge() const;
};


void ParserCKYAllMaxRule::extract_solution()
{

  compute_inside_outside_probabilities();

  calculate_chart_specific_rule_probabilities_and_best_edge();

  // PCKYAllCell& root = chart->get_root();
  // if (!root.exists_edge(SymbolTable::instance_nt()->get_label_id(LorgConstants::tree_root_name)))
  //   std::cout << "no axiom at root" << std::endl;
}

void ParserCKYAllMaxRule::calculate_chart_specific_rule_probabilities_and_best_edge() const
{
  double sentence_probability = std::log(get_sentence_probability());

  unsigned sent_size = chart->get_size();

  MaxRuleProbability::set_log_normalisation_factor(sentence_probability);

  for (unsigned span = 0; span < sent_size; ++span) {
    unsigned end_of_begin=sent_size-span;
    for (unsigned begin=0; begin < end_of_begin; ++begin) {
      unsigned end = begin + span ;

      //std::cout << "(" << begin << "," << end << ")" << std::endl;

      Cell& cell = chart->access(begin,end);

      if(!cell.is_closed())
      	cell.calculate_maxrule_probabilities();
    }
  }
}



#endif /* _PARSERCKYALLMAXVAR_H_ */
