// -*- mode: c++ -*-
#ifndef _PARSERCKYALLMAXVARKB_H_
#define _PARSERCKYALLMAXVARKB_H_

#include "ParserCKYAll.h"
#include "edges/MaxRuleProbabilityKB.h"


class ParserCKYAllMaxRuleKB : public ParserCKYAll_Impl<MaxRuleProbabilityKB>
{
private:
  unsigned k;
public:
  ParserCKYAllMaxRuleKB(std::vector<AGrammar*>& cgs,
                        const std::vector<double>& p, double b_t,
                        const std::vector< std::vector<std::vector< std::vector<unsigned> > > >& annot_descendants_,
                        bool accurate_, unsigned min_beam, int stubborn, unsigned k_, unsigned cell_threads)
    : ParserCKYAll_Impl<MaxRuleProbabilityKB>(cgs, p, b_t, annot_descendants_, accurate_, min_beam, stubborn, cell_threads) , k(k_)
  {
    //TODO maybe make this a parser option?
    //create the coarse-to-fine map
    create_coarse_to_fine_mapping(grammars);

    Cell::CellEdge::set_viterbi_unary_chains(grammars[grammars.size() - 1]->get_unary_decoding_paths());
  };
  ~ParserCKYAllMaxRuleKB() {};

  void extract_solution();
private:
  void initialise_candidates() const;
  void extend_all_derivations();
};


void ParserCKYAllMaxRuleKB::extend_all_derivations()
{
  static int start_symbol = SymbolTable::instance_nt().get(LorgConstants::tree_root_name);

  Cell& root = chart->get_root();

  if (!root.exists_edge(start_symbol))
    //   //   std::cout << "no axiom at root" << std::endl;
    return;

  for (unsigned i = 2; i <= k; ++i)
    {
      //      std::cout << "before extend" << std::endl;
      chart->get_root()[start_symbol]->extend_derivation(i,true);
    }
}


void ParserCKYAllMaxRuleKB::extract_solution()
{
  //  std::cout << "in extract" << std::endl;


  compute_inside_outside_probabilities();

  initialise_candidates();

  //  std::cout << "after init cand" << std::endl;

  extend_all_derivations();
}


void ParserCKYAllMaxRuleKB::initialise_candidates() const
{

  double sentence_probability = std::log(get_sentence_probability());
  unsigned sent_size = chart->get_size();

  MaxRuleProbabilityKB::set_log_normalisation_factor(sentence_probability);
  MaxRuleProbabilityKB::set_size(k);

  for (unsigned span = 0; span < sent_size; ++span) {
    unsigned end_of_begin=sent_size-span;
    for (unsigned begin=0; begin < end_of_begin; ++begin) {
      unsigned end = begin + span ;

      //      std::cout << "(" << begin << "," << end << ")" << std::endl;

      Cell& cell = chart->access(begin,end);

      if(!cell.is_closed()) {
        cell.calculate_maxrule_probabilities();
      }
    }
  }
}





#endif /* _PARSERCKYALLMAXVARKB_H_ */
