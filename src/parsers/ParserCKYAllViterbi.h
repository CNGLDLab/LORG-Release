// -*- mode: c++ -*-
#ifndef _PARSERCKYALLVITERBI_H_
#define _PARSERCKYALLVITERBI_H_

#include "ParserCKYAll.h"
#include "edges/ViterbiProbability.h"


class ParserCKYAllViterbi : public ParserCKYAll_Impl<ViterbiProbability>
{
public:
  ParserCKYAllViterbi(std::vector<AGrammar*>& cgs,
		      const std::vector<double>& p, double b_t,
		      const std::vector< std::vector<std::vector< std::vector<unsigned> > > >& annot_descendants_, bool accurate_, unsigned min_beam, int stubborn, unsigned cell_threads);

  virtual ~ParserCKYAllViterbi() { delete fine_grammar; fine_grammar = NULL;};

  void extract_solution();
  const AGrammar& get_fine_grammar() const;

private: // attributes
  AGrammar* fine_grammar; ///< the grammar to be used to extract the solution
};

inline
const ParserCKYAll::AGrammar& ParserCKYAllViterbi::get_fine_grammar() const
{
  return *fine_grammar;
}


ParserCKYAllViterbi::ParserCKYAllViterbi(std::vector<AGrammar *>& cgs,
					 const std::vector<double>& p, double b_t,
                                         const std::vector< std::vector<std::vector< std::vector<unsigned> > > >& annot_descendants_,
					 bool accurate_, unsigned min_beam, int stubborn, unsigned cell_threads)
  : ParserCKYAll_Impl<ViterbiProbability>(cgs, p, b_t, annot_descendants_, accurate_, min_beam, stubborn, cell_threads)
{
  fine_grammar = new ParserCKYAll::AGrammar(*cgs[cgs.size()-1]);
  fine_grammar->set_logmode(); // the Viterbi algorithms assume the fine grammar rules weights are log_probs

  //TODO maybe make this a parser option?
  //create the coarse-to-fine map

  std::vector<AGrammar*> all_grammars(cgs);
  all_grammars.push_back(fine_grammar);

  create_coarse_to_fine_mapping(all_grammars);

  ParserCKYAll_Impl<ViterbiProbability>::Cell::CellEdge::set_viterbi_unary_chains(fine_grammar->get_unary_decoding_paths());
}


void ParserCKYAllViterbi::extract_solution()
{
  unsigned sent_size=chart->get_size();
  const AnnotatedLabelsInfo& annotations = fine_grammar->get_annotations_info();


  // for (unsigned i=0; i < sent_size; ++i) {
  //   //std::cout << "(" << i << "," << i << ")" << std::endl;

  //   assert(!chart->access(i,i).is_closed());

  //   chart->access(i,i).compute_best_viterbi_derivation(annotations);
  // }

  for (unsigned span = 0; span < sent_size; ++span) {
    unsigned end_of_begin=sent_size-span;
    for (unsigned begin=0; begin < end_of_begin; ++begin) {
      unsigned end = begin + span ;

      //std::cout << "(" << begin << "," << end << ")" << std::endl;

      Cell& cell = chart->access(begin,end);

      if(!cell.is_closed())
      	cell.compute_best_viterbi_derivation(annotations);
    }
  }
}



#endif /* _PARSERCKYALLVITERBI_H_ */
