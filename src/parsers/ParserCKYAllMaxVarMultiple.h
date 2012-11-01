// -*- mode: c++ -*-
#ifndef _PARSERCKYALLMAXVARMULTIPLE_H_
#define _PARSERCKYALLMAXVARMULTIPLE_H_

#include "ParserCKYAll.h"
#include "edges/MaxRuleProbabilityMultiple.h"


class ParserCKYAllMaxRuleMultiple : public ParserCKYAll_Impl<MaxRuleProbabilityMultiple>
{
public:
  ParserCKYAllMaxRuleMultiple(std::vector<AGrammar*>& cgs,
			      const std::vector<double>& p, double b_t,
			      const std::vector< std::vector<AGrammar*> >& fgs,
			      const std::vector< annot_descendants_type >& all_annot_descendants_,
			      bool accurate_, unsigned min_beam, int stubborn, unsigned k, unsigned cell_threads);

  ~ParserCKYAllMaxRuleMultiple();

  /**
     \brief wraps the calculation of the best derivation
  */
  void extract_solution();


  const AGrammar& get_fine_grammar(unsigned i, unsigned j) const;

protected:
  /**
     \brief compute scores with all the fine grammars and back them up in the chart
  */
  void precompute_all_backups();

  void multiple_inside_outside_specific();


  /**
     \brief replace rules with their followers according to the defined mapping
     and reset annotations to zero (and resize thme to 1)
   */
  void change_rules_reset() const;

  /**
     \brief replace rules with their followers + size_grammar (to skip intermediate grammars)
     and replace current annotations with backed up ones at position backup_idx
  */
  void change_rules_load_backup(unsigned backup_idx, unsigned size_grammar) const;


  void modify_backup(unsigned backup_idx) const;

  /**
     \brief Calculates the chart specific rule probabilities of the packed edges in the chart
  */
  void calculate_maxrule_probabilities() const;

  /**
     \brief pick up the best derivation once the edge scores have been calculated
   */
  void calculate_best_edge();

  /**
     \brief for all edges in chart, backup current annotation
   */
  void backup_annotations() const;


protected: // attributes
  std::vector< std::vector<AGrammar*> >fine_grammars; ///< the additional grammars to be used to extract the solution
  std::vector<annot_descendants_type> all_annot_descendants; ///< all the annotations mapping for the grammars (base + fine ones)


private:
  unsigned nb_grammars;
  unsigned k;
  void initialise_candidates();
  void extend_all_derivations();
};

inline
const ParserCKYAll::AGrammar& ParserCKYAllMaxRuleMultiple::get_fine_grammar(unsigned i, unsigned j) const
{
  return *fine_grammars[i][j];
}



ParserCKYAllMaxRuleMultiple::ParserCKYAllMaxRuleMultiple(std::vector<AGrammar*>& cgs,
                                                         const std::vector<double>& p, double b_t,
                                                         const std::vector< std::vector<AGrammar*> >& fgs,
                                                         const std::vector< annot_descendants_type >& all_annot_descendants_,
							 bool accurate_, unsigned min_beam, int stubborn, unsigned k_, unsigned cell_threads)
  : ParserCKYAll_Impl<MaxRuleProbabilityMultiple>(cgs, p, b_t, all_annot_descendants_[0], accurate_, min_beam, stubborn, cell_threads),
    fine_grammars(fgs), all_annot_descendants(all_annot_descendants_), nb_grammars(fgs.size() + 1), k(k_)
{
  // create a mapping of all grammars
  std::vector<AGrammar*> all_grammars(grammars);

  for (unsigned i = 0; i < fine_grammars.size(); ++i)
    all_grammars.insert(all_grammars.end(), fine_grammars[i].begin(), fine_grammars[i].end());

  //  std::cout << "HERE" << std::endl;


  create_coarse_to_fine_mapping(all_grammars);


  // create another mapping between final grammars starting with the last one
  // that's how MaxRuleProbability class knows where to find rules
  std::vector<AGrammar*>& lasts = fine_grammars.back();
  std::vector<AGrammar*> maxn_mapping(1,lasts.back());
  maxn_mapping.push_back(grammars.back());
  //  create_coarse_to_fine_mapping(maxn_mapping);
  for (unsigned i = 0; i < fine_grammars.size(); ++i) {
    maxn_mapping.push_back(fine_grammars[i][fine_grammars[i].size()-1]);
  }

  create_coarse_to_fine_mapping(maxn_mapping);

  //TODO calculate this prperly for multiple grammars
  Cell::CellEdge::set_viterbi_unary_chains(grammars.back()->get_unary_decoding_paths());
}


ParserCKYAllMaxRuleMultiple::~ParserCKYAllMaxRuleMultiple()
{
  for(unsigned i = 0; i < fine_grammars.size(); ++i)
    for(unsigned j = 0; j < fine_grammars[i].size(); ++j)
      delete fine_grammars[i][j];
}


void ParserCKYAllMaxRuleMultiple::change_rules_reset() const
{
  unsigned sent_size=chart->get_size();
  for (unsigned span = 1; span <= sent_size; ++span) {
    unsigned end_of_begin=sent_size-span;
    for (unsigned begin=0; begin <= end_of_begin; ++begin) {
      unsigned end = begin + span -1;

      Cell& cell = chart->access(begin,end);
      //std::cout << "crr: (" << begin << "," << end << ")" << std::endl;

      if(!cell.is_closed()) {
        // 0 means c2f
        // 1 means multiple grammar decoding
	cell.change_rules_resize(1,0);
      }
    }
  }
}


void ParserCKYAllMaxRuleMultiple::change_rules_load_backup(unsigned backup_idx, unsigned size) const
{
  //  std::cout << "change_rules_load_backup" << std::endl;

  unsigned sent_size=chart->get_size();
  for (unsigned span = 1; span <= sent_size; ++span) {
    unsigned end_of_begin=sent_size-span;
    for (unsigned begin=0; begin <= end_of_begin; ++begin) {
      unsigned end = begin + span -1;

      Cell& cell = chart->access(begin,end);
      //std::cout << "crr: (" << begin << "," << end << ")" << std::endl;

      if(!cell.is_closed()) {
        //        std::cout << backup_idx << " " << size << std::endl;
	cell.change_rules_backup(backup_idx, size);
      }
    }
  }
}

void ParserCKYAllMaxRuleMultiple::modify_backup(unsigned backup_idx) const
{
  unsigned sent_size=chart->get_size();
  for (unsigned span = 1; span <= sent_size; ++span) {
    unsigned end_of_begin=sent_size-span;
    for (unsigned begin=0; begin <= end_of_begin; ++begin) {
      unsigned end = begin + span -1;

      Cell& cell = chart->access(begin,end);
      //std::cout << "crr: (" << begin << "," << end << ")" << std::endl;

      if(!cell.is_closed()) {
	cell.modify_backup(backup_idx);
      }
    }
  }
}



void ParserCKYAllMaxRuleMultiple::precompute_all_backups()
{

  backup_annotations();

  for(unsigned i = 0; i < fine_grammars.size(); ++i) {

    //    std::cout << "fine grammar " << i << std::endl;

    //    std::cout << "changing rules" << std::endl;
    // change grammar rules in the chart and  pick the new baseline
    change_rules_reset();
    //    std::cout << "changed rules" << std::endl;

    //    std::cout << "changing annotation history" << std::endl;
    // update annotation history
    annot_descendants = all_annot_descendants[i+1];
    //    std::cout << "changed annotation history" << std::endl;

    //    std::cout << "before beam_c2f " << std::endl;
    beam_c2f(fine_grammars[i], annot_descendants);
    //    std::cout << "after beam_c2f " << std::endl;

    backup_annotations();
  }
}

void ParserCKYAllMaxRuleMultiple::multiple_inside_outside_specific()
{
  static int start_symbol = SymbolTable::instance_nt().get(LorgConstants::tree_root_name);

  for(unsigned i = 0; i < fine_grammars.size() + 1; ++i) {

    //    std::cout << "computation " << i << std::endl;

    //assume that all grammars have the same size
    unsigned distance = i==0 ? 0 : 1;
    change_rules_load_backup(i, distance);

    compute_inside_probabilities();
    //    std::cout << "sentence_prob: " << std::log(get_sentence_probability()) << std::endl;


    if(!chart->get_root().is_closed() && chart->get_root().exists_edge(start_symbol)) {
      chart->get_root()[start_symbol]->get_annotations().reset_outside_probabilities(1.0);
      compute_outside_probabilities();

      MaxRuleProbabilityMultiple::set_log_normalisation_factor(std::log(get_sentence_probability()));
      calculate_maxrule_probabilities();

    }

    modify_backup(i);

  }
}


void ParserCKYAllMaxRuleMultiple::extract_solution()
{
  static int start_symbol = SymbolTable::instance_nt().get(LorgConstants::tree_root_name);

  // first we backup all annotations for all grammars
  precompute_all_backups();

  // actual computation
  // inside/outside + max rule score
  // all in one function
  multiple_inside_outside_specific();


  if(!chart->get_root().is_closed() && chart->get_root().exists_edge(start_symbol)) {
    //    std::cerr << "calculate_best_edge" << std::endl;

    initialise_candidates();
    extend_all_derivations();
  }


  //reset to first grammar for next parse
  annot_descendants = all_annot_descendants[0];

  MaxRuleProbabilityMultiple::reset_log_normalisation_factor();

}


void ParserCKYAllMaxRuleMultiple::calculate_maxrule_probabilities() const
{

  unsigned sent_size = chart->get_size();


  for (unsigned span = 0; span < sent_size; ++span) {
    unsigned end_of_begin=sent_size-span;
    for (unsigned begin=0; begin < end_of_begin; ++begin) {
      unsigned end = begin + span ;

      //      std::cout << "(" << begin << "," << end << ")" << std::endl;

      Cell& cell = chart->access(begin,end);

      if(!cell.is_closed())
      	cell.calculate_maxrule_probabilities();
    }
  }
}



void ParserCKYAllMaxRuleMultiple::calculate_best_edge()
{

  unsigned sent_size = chart->get_size();

  for (unsigned i=0; i < sent_size; ++i) {
    //std::cout << "(" << i << "," << i << ")" << std::endl;

    Cell& cell = chart->access(i,i);
    if(!cell.is_closed())
      cell.calculate_best_edge_multiple_grammars();
  }

  for (unsigned span = 1; span < sent_size; ++span) {
    unsigned end_of_begin=sent_size-span;
    for (unsigned begin=0; begin < end_of_begin; ++begin) {
      unsigned end = begin + span ;

      //std::cout << "(" << begin << "," << end << ")" << std::endl;

      Cell& cell = chart->access(begin,end);

      if(!cell.is_closed())
      	cell.calculate_best_edge_multiple_grammars();
    }
  }
}

void ParserCKYAllMaxRuleMultiple::backup_annotations() const
{
  unsigned sent_size = chart->get_size();

  for (unsigned span = 0; span < sent_size; ++span) {
    unsigned end_of_begin=sent_size-span;
    for (unsigned begin=0; begin < end_of_begin; ++begin) {
      unsigned end = begin + span ;

      Cell& cell = chart->access(begin,end);

      if(!cell.is_closed()) {
	//	std::cout << "span(" << begin << "," << end << ")" << std::endl;
      	cell.backup_annotations();
      }

    }
  }
}


void ParserCKYAllMaxRuleMultiple::extend_all_derivations()
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


void ParserCKYAllMaxRuleMultiple::initialise_candidates()
{
  MaxRuleProbabilityMultiple::set_size(k);
  MaxRuleProbabilityMultiple::set_nbgrammars(nb_grammars);


  calculate_best_edge();
}


#endif /* _PARSERCKYALLMAXVARMULTIPLE_H_ */
