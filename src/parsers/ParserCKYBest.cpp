#include "ParserCKYBest.h"

ParserCKYBest::~ParserCKYBest()
{
}

ParserCKYBest::ParserCKYBest(Grammar<Rule,Rule,Rule>* g):
  ParserCKY< Grammar<Rule,Rule,Rule> >(g)
{}

void ParserCKYBest::parse(Chart& chart) const
{
  bool isroot = chart.get_size() == 1;
  for(unsigned i = 0; i < chart.get_size(); ++i) {
    //    std::cout << "initialising position: " << i << std::endl;
    add_unary_init(chart.access(i,i),isroot);
    //    std::cout << chart.access(i,i) << std::endl;
  }

  process_internal_rules(chart);
}

inline
void ParserCKYBest::get_candidates(const Cell& left_cell,
				   const Cell& right_cell,
				   Cell& result_cell) const
{
  Edge current_candidate;

  //iterating through all the rules P -> L R, indexed by L
  for(std::vector<Parser::vector_rhs0>::const_iterator same_rhs0_itr = brules->_begin;
      same_rhs0_itr != brules->_end; ++same_rhs0_itr) {

    // is L present in  left_cell ?
    if(left_cell.exists_edge(same_rhs0_itr->rhs0)) {
      const Edge& left_edge = left_cell.at(same_rhs0_itr->rhs0);
      current_candidate.set_left_child(&left_edge);

      //iterating through all the rules P -> L R, indexed by R, L fixed
      for (std::vector<Parser::vector_rhs1>::const_iterator same_rhs1_itr = same_rhs0_itr->_begin;
	   same_rhs1_itr != same_rhs0_itr->_end; ++same_rhs1_itr) {

	// is R present in right_cell ?
	if(right_cell.exists_edge(same_rhs1_itr->rhs1)) {

	  const Edge& right_edge = right_cell.at(same_rhs1_itr->rhs1);
	  current_candidate.set_right_child(&right_edge);

	  double prob1 = left_edge.get_probability() + right_edge.get_probability();

	  //iterating through all the rules P -> L R, indexed by P, R and L fixed
	  std::vector< const Rule * >::const_iterator bitr = same_rhs1_itr->_begin;
	  for(; bitr != same_rhs1_itr->_end; ++bitr) {
	    current_candidate.set_lhs((*bitr)->get_lhs());

	    current_candidate.set_probability(prob1 + (*bitr)->get_probability());

	    //	    std::cout << *(*bitr) << std::endl;

	    (void) result_cell.process_candidate(current_candidate);
	  }
	}
      }
    }
  }
}


void ParserCKYBest::process_internal_rules(Chart& chart) const
{
  unsigned sent_size=chart.get_size();
  for (unsigned span = 2; span <= sent_size; ++span) {
    unsigned end_of_begin=sent_size-span;
    for (unsigned begin=0; begin <= end_of_begin; ++begin) {
    	unsigned end = begin + span -1;

      //      std::cout << "begin: " << begin << ", end: " << end << std::endl;

    	Cell& result_cell = chart.access(begin,end);

    	if(!result_cell.is_closed()) {
    		// look for all possible new edges
    		for (unsigned m = begin; m < end; ++m) {
    			const Cell& left_cell = chart.access(begin,m);
    			if(!left_cell.is_closed()) {
    				const Cell& right_cell = chart.access(m+1,end);
    				if( !right_cell.is_closed())
    					get_candidates(left_cell,right_cell,result_cell);
    			}
    		}
    		// std::cout << result_cell << std::endl;

    		add_unary(result_cell, span == sent_size);

	//	result_cell.apply_beam();
      }
      // std::cout << result_cell << std::endl;
    }
  }
}


inline
void ParserCKYBest::add_unary_init(Cell& cell, bool isroot) const
{

  //for each unary rule set in the grammar [sets made up of all unary rules with a particular rhs]
  std::vector<short>::const_iterator unary_rhs_itr_end = unary_rhs_from_pos.end();
  for (std::vector<short>::const_iterator unary_rhs_itr = unary_rhs_from_pos.begin();
       unary_rhs_itr != unary_rhs_itr_end; ++unary_rhs_itr) {

    if (cell.exists_edge(*unary_rhs_itr))
      follow_unary_chain(cell,&cell[*unary_rhs_itr],isroot);
  }
}


inline
void ParserCKYBest::add_unary(Cell& cell, bool isroot) const
{

  //for each unary rule set in the grammar [sets made up of all unary rules with a particular rhs being a lhs of a binary rule]
  std::vector<short>::const_iterator unary_rhs_itr_end = unary_rhs_from_binary.end();
  for (std::vector<short>::const_iterator unary_rhs_itr = unary_rhs_from_binary.begin();
       unary_rhs_itr != unary_rhs_itr_end; ++unary_rhs_itr) {

    if (cell.exists_edge(*unary_rhs_itr))
      follow_unary_chain(cell,&cell[*unary_rhs_itr],isroot);
  }
}

//inline
void ParserCKYBest::follow_unary_chain(Cell& cell, const Edge * edge, bool isroot) const
{
  static int start_symbol = SymbolTable::instance_nt().get(LorgConstants::tree_root_name);

  std::vector<const Edge*> accumulator(1,edge);

  Edge candidate;
  candidate.set_right_child(NULL);

  do {
    const Edge * current_edge = accumulator.back();
    accumulator.pop_back();

    candidate.set_left_child(current_edge);
    const std::vector<const Rule*>& rules = isroot ?
      unary_rhs_2_rules_toponly[current_edge->get_lhs()] :
      unary_rhs_2_rules_notop[current_edge->get_lhs()];
    std::vector<const Rule*>::const_iterator rule_end = rules.end();
    for (std::vector<const Rule*>::const_iterator rule_itr = rules.begin(); rule_itr != rule_end; ++rule_itr) {

      //      std::cout << *(*rule_itr) << std::endl;

      if(isroot || (*rule_itr)->get_lhs() != start_symbol)

      candidate.set_lhs((*rule_itr)->get_lhs());
      candidate.set_probability(current_edge->get_probability() + (*rule_itr)->get_probability());

      const Edge * new_edge = cell.process_candidate(candidate);
      if(new_edge && rules_for_unary_exist(new_edge->get_lhs())) {
	accumulator.push_back(new_edge);
	//	std::cout << *(*rule_itr) << std::endl;
      }
    }
  } while(!accumulator.empty());
}
