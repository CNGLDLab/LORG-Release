// -*- mode: c++ -*-
#ifndef PCKYALLCELL_H
#define PCKYALLCELL_H

#include "Word.h"

#include "rules/BRuleC2f.h"
#include "rules/URuleC2f.h"
#include "rules/LexicalRuleC2f.h"
#include "grammars/AnnotatedLabelsInfo.h"


#include <cassert>
#include <cstring>

#include "edges/AnnotationInfo.h"

#include <numeric>
#include <algorithm>





/**
   \class PCKYAllCell
   \brief represents a cell in the chart for the 2 stage parser
   \note maybe fusion with PCKYBestCell
 */
template<class MyEdge>
class PCKYAllCell {
public:
  typedef  MyEdge CellEdge;
private:
  typedef void (PCKYAllCell::CellEdge::*cell_edge_create_func)() ;
public:
  /**
     \brief Simple constructor
     \note this cell is not initialised: trying
     to use the created cell will result in segfault !
     You have to call init first
  */
  PCKYAllCell() : real_cell(NULL), closed(true) {};

  /**
     \brief Constructor
     \param cl true if closed
  */
  PCKYAllCell(bool cl);

  /**
     \brief destructor
   */
  ~PCKYAllCell();


  /**
     \brief initialise the cell
     \param cl true if closed
   */
  void init(bool cl);


  /**
     \brief insert a candidate edge in the cell from application of a binary rule
     \param
  */
  void process_candidate(PCKYAllCell* left, PCKYAllCell* right, const BRuleC2f*, double LR_inside);


  /**
     \brief insert a candidate edge in the cell from application of a unary rule
     \param
  */
  void process_candidate(const URuleC2f *, double);

  /**
     \brief test if there's an edge of given lhs in the cell
     \param label a lhs label
     \return true if label is in the cell
  */
  bool exists_edge(int label) const;


  /**
     \brief test if there's an edge in the cell
     \return fasle if there exists at least an edge in the cell
  */
  bool is_empty() const;


/**
     \brief access an edge by its lhs
     \param i the label of the edge
     \return the edge with i as lhs
   */
  CellEdge * operator[](unsigned i);


  /**
     \brief access an edge by its lhs
     \param i the label of the edge
     \return the edge with i as lhs
  */
  const CellEdge& at(int i) const;
  CellEdge& at(int i);

  /**
     \brief access the mostprobable edge by its lhs
     \param i the label of the edge
     \return the best edge with i as lhs
  */
  const CellEdge& best_at(int i) const;

  /**
     \brief access
     \return true if the cell is closed
  */
  bool is_closed() const;


  /**
     \brief calculate the chart specific rule probabilities for all packed edges in this cell and
     set the best daughter edge (max-rule parsing)
  */
  void calculate_maxrule_probabilities();


  void calculate_best_edge_multiple_grammars();

  /**
     \brief Output operator
     \param out the ostream to write on
     \param cell the cell object to write
     \return the used ostream
  */
  template<class O>
  friend std::ostream& operator<<(std::ostream& out, const PCKYAllCell<O>& cell);


  /**
    \brief reset probabilities of all the edges in the cell to 0.0
    \note reset to a value passed as a parameter
  */
  void reset_probabilities();

  void compute_inside_probabilities();
  void compute_outside_probabilities();
  void adjust_inside_probability();

  void backup_annotations();

  /**
     \brief compute the best viterbi derivations for a cell
  */
  void compute_best_viterbi_derivation(const AnnotatedLabelsInfo& symbol_map);

  void add_word(const Word & word);

  void beam(const std::vector<double>& priors, double threshold);
  void beam(double threshold);
  void beam(double threshold, double sent_prob);
  void beam_huang(double threshold, double sent_prob);

  static void set_max_size(unsigned size);

  void clean();
  void clean_binary_daughters();

  void clear();

  void change_rules_resize(const AnnotatedLabelsInfo& next_annotations, const std::vector<std::vector<std::vector<unsigned> > >& annot_descendants_current);
  void change_rules_resize(unsigned new_size, unsigned fienr_idx);
  void change_rules_backup(unsigned backup_idx, unsigned  size_grammars);
  void modify_backup(unsigned backup_idx);

private:
  CellEdge ** real_cell;
  bool closed;

  static unsigned max_size;

};


template<class PEProbability>
inline
bool PCKYAllCell<PEProbability>::exists_edge(int label) const
{
  assert(label >= 0);
  assert(label < (int) max_size);
  return (real_cell[label] != NULL);
}


template<class PEProbability>
inline
bool PCKYAllCell<PEProbability>::is_empty() const
{
  if(closed)
    return true;

  for (unsigned i = 0; i < max_size; ++i)
    {
      if(real_cell[i])
        return false;
    }
  return true;
}




template<class PEProbability>
inline
void PCKYAllCell<PEProbability>::init(bool cl)
{
  if(!(closed = cl)) {
    real_cell =  new CellEdge * [max_size];
    memset(real_cell, 0, max_size * sizeof(CellEdge*));
    //   for(unsigned i = 0; i < max_size;++i)
    //     real_cell[i]=NULL;
  }
}

template<class PEProbability>
inline
bool PCKYAllCell<PEProbability>::is_closed() const
{ return closed; }

template<class MyEdge>
inline
typename PCKYAllCell<MyEdge>::CellEdge * PCKYAllCell<MyEdge>::operator[](unsigned i)
{
  assert(!closed);
  return real_cell[i];
}

template<class MyEdge>
inline
const typename PCKYAllCell<MyEdge>::CellEdge& PCKYAllCell<MyEdge>::at(int i) const
{
  //assert(i>=0);
  //assert( i < (int) max_size);
  assert(i>=0 && i < (int) max_size);

  return *real_cell[i];
}

template<class MyEdge>
inline
typename PCKYAllCell<MyEdge>::CellEdge& PCKYAllCell<MyEdge>::at(int i)
{
  //assert(i>=0);
  //assert( i < (int) max_size);
  assert(i>=0 && i < (int) max_size);

  return *real_cell[i];
}

template<class MyEdge>
inline
const typename PCKYAllCell<MyEdge>::CellEdge& PCKYAllCell<MyEdge>::best_at(int i) const
{
  return at(i);
}

template<class MyEdge>
inline
void PCKYAllCell<MyEdge>::add_word(const Word & word)
{
  typedef typename MyEdge::LexicalDaughters LDaughters;

  for(std::vector<const MetaProduction*>::const_iterator it(word.get_rules().begin());
      it != word.get_rules().end(); ++it) {
    // std::cout <<*(static_cast<const LexicalRule*>(*it)) << std::endl;

    int tag = (*it)->get_lhs();

    CellEdge ** e = &real_cell[tag];

    if(*e) {
      (*e)->add_daughters(static_cast<const LexicalRuleC2f*>(*it), &word);
    }
    else {
      *e = new CellEdge(LDaughters(static_cast<const LexicalRuleC2f*>(*it), &word));
    }

    (*e)->get_annotations().inside_probabilities.array[0] += static_cast<const LexicalRuleC2f*>(*it)->get_probability()[0];
  }
}



template<class MyEdge>
inline
void PCKYAllCell<MyEdge>::set_max_size(unsigned size)
{
  max_size =  size;
}

template<class MyEdge>
unsigned PCKYAllCell<MyEdge>::max_size = 0;


template<class MyEdge>
PCKYAllCell<MyEdge>::PCKYAllCell(bool cl):
  real_cell(NULL), closed(cl)
{
  if(!closed) {
    real_cell =  new CellEdge * [max_size];
    memset(real_cell, 0, max_size * sizeof(CellEdge*));
  }
}

template<class MyEdge>
PCKYAllCell<MyEdge>::~PCKYAllCell()
{
  if(!closed) {
    for(unsigned i = 0; i < max_size;++i) {
      delete real_cell[i];
      //      real_cell[i] = NULL;
    }
  }
  delete[] real_cell;
  //  real_cell = NULL;
}

template<class MyEdge>
void PCKYAllCell<MyEdge>::process_candidate(PCKYAllCell<MyEdge>* left,
                                            PCKYAllCell<MyEdge>* right,
                                            const BRuleC2f* rule,
                                            double LR_inside)
{
  MyEdge ** e = &real_cell[rule->get_lhs()];

  if(*e)
    (*e)->add_daughters(left,right,rule);
  else {
    *e = new MyEdge(typename MyEdge::BinaryDaughters(left,right,rule));
  }


  (*e)->get_annotations().inside_probabilities.array[0] += LR_inside * rule->get_probability()[0][0][0];
}

template<class MyEdge>
void PCKYAllCell<MyEdge>::process_candidate(const URuleC2f* rule, double L_inside)
{
  assert(rule);
  assert(rule->get_probability().size() > 0);


  MyEdge ** e = &real_cell[rule->get_lhs()];

  if(*e)  {
    (*e)->add_daughters(this,rule);
  }
  else {
    //std::cout <<" adding a new edge " << *rule << std::endl;
    *e = new MyEdge(typename MyEdge::UnaryDaughters(this,rule));
  }


  assert(rule);
  assert(rule->get_probability().size() > 0);

  assert(rule);
  assert(rule->get_probability().size() > 0);
  (*e)->get_annotations().inside_probabilities_unary_temp.array[0] += L_inside * rule->get_probability()[0][0];
}

template<class MyEdge>
void PCKYAllCell<MyEdge>::reset_probabilities()
{
  //  std::cout << max_size << std::endl;
  for(unsigned i = 0; i < max_size; ++i)
    if(real_cell[i]) {
      real_cell[i]->local_reset_probabilities(0.0);
    }
}

template<class MyEdge>
void PCKYAllCell<MyEdge>::compute_inside_probabilities()
{

  // Doesn't work in multiple grammar (see FIXME in ParserCKYAll.h)
// #ifndef NDEBUG
//   for(unsigned i = 0; i < max_size; ++i)
//     if(real_cell[i])
//       for(unsigned j = 0; j < real_cell[i]->get_annotations().get_size(); ++j)
//         assert(real_cell[i]->get_annotations().inside_probabilities.array[j] == 0
//                || real_cell[i]->get_annotations().inside_probabilities.array[j] == LorgConstants::NullProba);
// #endif

  clean_binary_daughters();

  // for(unsigned i = 0; i < max_size; ++i) {
  //   if(exists_edge(i)) {
  //     real_cell[i]->get_annotations().reset_probabilities();
  //   }
  // }

  for(unsigned i = 0; i < max_size; ++i) {
    if(exists_edge(i)) {

      // if (real_cell[i]->get_lex())
      //   real_cell[i]->compute_inside_probability_lexicals_only();


      if (real_cell[i]->get_lex())
        real_cell[i]->compute_inside_probability_lexicals_only();

      real_cell[i]->compute_inside_probability_binaries_only();

      real_cell[i]->prepare_inside_probability();
    }
  }

  for(unsigned i = 0; i < max_size; ++i) {
    if(exists_edge(i)) {
      real_cell[i]->compute_inside_probability_unaries_only();
    }
  }

  adjust_inside_probability();
}

template <class MyEdge>
void PCKYAllCell<MyEdge>::adjust_inside_probability()
{
  for(unsigned i = 0; i < max_size; ++i) {
    if(exists_edge(i)) {
      real_cell[i]->adjust_inside_probability();
    }
  }
}





template<class MyEdge>
void PCKYAllCell<MyEdge>::compute_outside_probabilities()
{
  for(unsigned i = 0; i < max_size; ++i) {
    if(exists_edge(i)) {
      real_cell[i]->prepare_outside_probability();
    }
  }

  for(unsigned i = 0; i < max_size; ++i) {
    if(exists_edge(i)) {
      real_cell[i]->compute_outside_probability_unaries_only();
    }
  }

  for(unsigned i = 0; i < max_size; ++i) {
    if(exists_edge(i)) {
      real_cell[i]->adjust_outside_probability();
    }
  }


  for(unsigned i = 0; i < max_size; ++i) {
    if(exists_edge(i)) {
      // if(real_cell[i]->get_lex())
      //   real_cell[i]->compute_outside_probability_lexicals_only();

        real_cell[i]->compute_outside_probability_binaries_only();
    }
  }
}

template<class MyEdge>
void PCKYAllCell<MyEdge>::calculate_maxrule_probabilities()
{
  assert(!closed);

  for(unsigned i = 0; i < max_size; ++i) {
    if(exists_edge(i)) {
      if(real_cell[i]->get_lex())
	real_cell[i]->compute_best_lexical();
      //else TODO ?
      real_cell[i]->compute_best_binary();
    }
  }
  for(unsigned i = 0; i < max_size; ++i) {
    if(exists_edge(i)) {
      real_cell[i]->compute_best_unary();
    }
  }
}

///////////


template<class MyEdge>
void PCKYAllCell<MyEdge>::calculate_best_edge_multiple_grammars()
{
  assert(!closed);

  for(unsigned i = 0; i < max_size; ++i) {
    if(exists_edge(i)) {
      if(real_cell[i]->get_lex())
	real_cell[i]->find_best_multiple_grammars_lexical();

      real_cell[i]->find_best_multiple_grammars_binary();
    }
  }
  for(unsigned i = 0; i < max_size; ++i) {
    if(exists_edge(i)) {
      real_cell[i]->find_best_multiple_grammars_unary();
    }
  }

}

template<class MyEdge>
void PCKYAllCell<MyEdge>::compute_best_viterbi_derivation(const AnnotatedLabelsInfo& symbol_map)
{
  //iterate through all the packed edges in this cell, processing the lexical daughters first
  for(unsigned i = 0; i < max_size; ++i) {
    if(exists_edge(i))  {
      real_cell[i]->create_viterbi(symbol_map.get_number_of_annotations(i));
      real_cell[i]->replace_rule_probabilities(0);
      real_cell[i]->compute_best_lexical();
      real_cell[i]->compute_best_binary();
    }
  }
  //now process the unary daughters
  for(unsigned i = 0; i < max_size; ++i) {
    if(exists_edge(i)) {
      real_cell[i]->compute_best_unary();
    }
  }
}

template<class MyEdge>
void PCKYAllCell<MyEdge>::backup_annotations()
{
  for(unsigned i = 0; i < max_size; ++i)
    if(exists_edge(i)) {
      //      std::cout << i << std::endl;
      real_cell[i]->backup_annotations();
    }
}

// these 2 predicates returns true if the edge can be removed
// ie, if it's pointing to invalid edges
template <typename Cell>
struct pred_beam_clean_bin : public std::unary_function<typename Cell::CellEdge::BinaryDaughters, bool>
{
  bool operator()(const typename Cell::CellEdge::BinaryDaughters& packededgedaughter) const
  {

    Cell * cell0 = packededgedaughter.left_daughter();
    if(cell0->is_closed()) return true;
    typename Cell::CellEdge * lefty = (*cell0)[packededgedaughter.get_rule()->get_rhs0()];
    if (lefty == NULL) return true;


    Cell * cell1 = packededgedaughter.right_daughter();
    if(cell1->is_closed()) return true;
    typename Cell::CellEdge * righty = (*cell1)[packededgedaughter.get_rule()->get_rhs1()];
    return righty == NULL;
  }
};


template <typename Cell>
struct pred_beam_clean_un : public std::unary_function<typename Cell::CellEdge::UnaryDaughters, bool>
{
  bool operator()(const typename Cell::CellEdge::UnaryDaughters& packededgedaughter) const
  {
    Cell * cell = packededgedaughter.left_daughter();
    // cell should be equal to the current cell so this test is useless
    //    if(cell->is_closed()) return true;
    return (*cell)[packededgedaughter.get_rule()->get_rhs0()] == NULL;
  }
};

template<class MyEdge>
void PCKYAllCell<MyEdge>::clean()
{

  bool changed;
  do {
    changed =  false;

    // go through all the lists of unary daughters and remove the ones pointing on removed edges
    for(unsigned i = 0; i < max_size; ++i)
      if(real_cell[i]) {
	std::vector<typename MyEdge::UnaryDaughters >& udaughters = real_cell[i]->get_unary_daughters();
	udaughters.erase(std::remove_if(udaughters.begin(), udaughters.end(),
                                        pred_beam_clean_un<PCKYAllCell<MyEdge> >()),
			 udaughters.end());

	if(real_cell[i]->get_binary_daughters().empty() &&
           real_cell[i]->get_lexical_daughters().empty() &&
           real_cell[i]->get_unary_daughters().empty()) {
	  //	std::cout << "I shall be removed!" << std::endl;
	  delete real_cell[i];
	  real_cell[i]=NULL;
	  changed =  true;
	}
      }
  }
  while(changed);

  // final memory reclaim
  // TODO: benchmark this carefully
  for(unsigned i = 0; i < max_size; ++i)
    if(real_cell[i]) {
      std::vector<typename MyEdge::UnaryDaughters >& udaughters = real_cell[i]->get_unary_daughters();
      if(udaughters.capacity() != udaughters.size()) {
        std::vector<typename MyEdge::UnaryDaughters > tmp;
        tmp.swap(udaughters);
        udaughters.insert(udaughters.begin(), tmp.begin(), tmp.end());
      }
    }

  // //should be a proper method
  // //if all edge pointers are NULL, close the cell
  //an overcomplicated way to do the same as below
  if(std::find_if(real_cell,real_cell+max_size,std::bind2nd(std::not_equal_to<MyEdge*>(), (MyEdge*)NULL)) == real_cell+max_size)
    {
      closed = true;
      delete[] real_cell;
      real_cell = NULL;
    }

  // bool all_null = true;
  // for(unsigned i = 0; i < max_size; ++i) {
  //   if (real_cell[i]) {
  //     all_null = false;
  //     break;
  //   }
  // }
  // if(all_null) {
  //   //  std::cout << "ALL NULL" << std::endl;
  //   closed = true;
  //   delete real_cell;
  //   real_cell = NULL;
  // }
}




//relative prior beam
template <class MyEdge>
void PCKYAllCell<MyEdge>::beam(const std::vector<double>& priors, double threshold)
{
  double max = 0.0;
  double beam = threshold;

  std::vector<double> sums = priors;

  //computing unannotated inside probabilities
  //looking for the probablity of the most probable symbol
  for(unsigned i = 0; i < max_size; ++i)
    if(real_cell[i]) {
      sums[i] *= std::accumulate(real_cell[i]->get_annotations().inside_probabilities.array.begin(),
				 real_cell[i]->get_annotations().inside_probabilities.array.end(),0.0);
      max = std::max(max, sums[i]);
      //      if(max < sums[i]) {max = sums[i];}
    }

  //setting threshold
  beam *= max;

  //looking for edges below threshold
  for(unsigned i = 0; i < max_size; ++i)
    if(real_cell[i]) {
      if(sums[i] < beam) {
	delete real_cell[i];
  	real_cell[i]=NULL;
      }
    }

  //  clean the cell

  clean();
}



// Relative Inside/Outside beam
template<class MyEdge>
void PCKYAllCell<MyEdge>::beam(double threshold)
{
  double max = 0.0;
  double beam = threshold;

  std::vector<double> sums(max_size,0.0);

  //computing unannotated inside probabilities
  //looking for the probability of the most probable symbol
  for(unsigned i = 0; i < max_size; ++i)
    if(real_cell[i]) {
      double ins = std::accumulate(real_cell[i]->get_annotations().inside_probabilities.array.begin(),
				   real_cell[i]->get_annotations().inside_probabilities.array.end(),0.0);
      double outs = std::accumulate(real_cell[i]->get_annotations().outside_probabilities.array.begin(),
				    real_cell[i]->get_annotations().outside_probabilities.array.end(),0.0);

      sums[i] = ins * outs;
      if(max < sums[i]) {max = sums[i];}
    }

  //setting threshold
  beam *= max;

  //looking for edges below threshold
  for(unsigned i = 0; i < max_size; ++i)
    if(real_cell[i]) {
      if(sums[i] < beam) {
	delete real_cell[i];
  	real_cell[i]=NULL;
      }
    }

  //  clean the cell
  clean();
}



template<class MyEdge>
void PCKYAllCell<MyEdge>::clean_binary_daughters()
{
  for(unsigned i = 0; i < max_size; ++i)
    if(real_cell[i]) {
      MyEdge * edge = real_cell[i];

      // go through all the lists of binary daughters and remove the ones pointing on removed edges
      std::vector<typename MyEdge::BinaryDaughters >& bdaughters = edge->get_binary_daughters();

      bdaughters.erase(std::remove_if(bdaughters.begin(), bdaughters.end(), pred_beam_clean_bin<PCKYAllCell<MyEdge> >()), bdaughters.end());


      // Reclaim memory !
      if(bdaughters.capacity() != bdaughters.size()) {
        std::vector<typename MyEdge::BinaryDaughters > tmp;
        tmp.swap(bdaughters);
        bdaughters.insert(bdaughters.begin(), tmp.begin(), tmp.end());
      }
    }
}


// Absolute Inside/Outside beam
template<class MyEdge>
void PCKYAllCell<MyEdge>::beam(double log_threshold, double log_sent_prob)
{
  double beam = log_threshold  + log_sent_prob;

  for(unsigned i = 0; i < max_size; ++i)
    if(real_cell[i]) {
      bool all_invalid = true;
      AnnotationInfo& ai = real_cell[i]->get_annotations();

      // calculate posterior for each annotation
      for(unsigned annot = 0 ; annot < ai.inside_probabilities.array.size(); ++annot) {
	if(ai.inside_probabilities.array[annot] != LorgConstants::NullProba
	   //|| ai.outside_probabilities.array[annot] != LorgConstants::NullProba
	   ) {

          double prob = std::log(ai.inside_probabilities.array[annot]) + std::log(ai.outside_probabilities.array[annot]);
          //          double prob = std::log(ai.inside_probabilities.array[annot] * ai.outside_probabilities.array[annot]);

	  if (prob > beam)
	    all_invalid = false;
	  else {
	    ai.inside_probabilities.array[annot] = ai.outside_probabilities.array[annot] = LorgConstants::NullProba;
	  }
	}
      }

      //remove edge if all annotations are NullProba
      if(all_invalid) {
	delete real_cell[i];
	real_cell[i]=NULL;
      }
    }
  // you must call clean after this method
}


// returns true if the branching can be removed
// in the sense of Huang, 2008
template <typename Cell>
struct pred_beam_huang
{
  double log_threshold;
  double log_outside_up;

  pred_beam_huang(double th, double se, double ou) : log_threshold(th + se), log_outside_up(ou) {}


  // assume that clean has already been called
  // and so lefty and righty are never NULL
  bool operator()(const typename Cell::CellEdge::BinaryDaughters& packededgedaughter) const
  {


    Cell * cell0 = packededgedaughter.left_daughter();
    assert(cell0 != NULL);

    typename Cell::CellEdge * lefty = (*cell0)[packededgedaughter.get_rule()->get_rhs0()];
    assert(lefty != NULL);
    const AnnotationInfo& ailefty = lefty->get_annotations();

    double total_in = 0;
    double sum = 0;
    for(unsigned annot = 0 ; annot < ailefty.inside_probabilities.array.size(); ++annot) {
      if(ailefty.inside_probabilities.array[annot] != LorgConstants::NullProba) {
	sum += ailefty.inside_probabilities.array[annot];
      }
    }
    total_in += std::log(sum);


    Cell * cell1 = packededgedaughter.right_daughter();
    assert(cell1 != NULL);
    typename Cell::CellEdge * righty = (*cell1)[packededgedaughter.get_rule()->get_rhs1()];
    assert(righty != NULL);
    const AnnotationInfo& airighty = righty->get_annotations();

    sum = 0;
    for(unsigned annot = 0 ; annot < airighty.inside_probabilities.array.size(); ++annot) {
      if(airighty.inside_probabilities.array[annot] != LorgConstants::NullProba) {
	sum += airighty.inside_probabilities.array[annot];
      }
    }

    total_in += std::log(sum);

    bool remove = log_outside_up + total_in  < log_threshold;

    return remove;
  }

  bool operator()(const typename Cell::CellEdge::UnaryDaughters& packededgedaughter) const
  {
    Cell * cell = packededgedaughter.left_daughter();
    assert(cell != NULL);
    typename Cell::CellEdge * lefty = (*cell)[packededgedaughter.get_rule()->get_rhs0()];
    assert(lefty != NULL);

    const AnnotationInfo& ailefty = lefty->get_annotations();

    double total_in = 0;

    for(unsigned annot = 0 ; annot < ailefty.inside_probabilities.array.size(); ++annot) {
      if(ailefty.inside_probabilities.array[annot] != LorgConstants::NullProba) {
	total_in += ailefty.inside_probabilities.array[annot];
      }
    }

    total_in = std::log(total_in);

    bool remove = log_outside_up + total_in  < log_threshold;

    return remove;

  }
};

template<class MyEdge>
void PCKYAllCell<MyEdge>::beam_huang(double log_threshold, double log_sent_prob)
{
  for(unsigned i = 0; i < max_size; ++i) {
    // std::cout << real_cell << std::endl;
    // std::cout << i << std::endl;
    if(real_cell[i]) {
      MyEdge *  edge = real_cell[i];
      AnnotationInfo& ai = edge->get_annotations();

      double total_out = 0;
      for(unsigned annot = 0 ; annot < ai.outside_probabilities.array.size(); ++annot) {
	if(ai.outside_probabilities.array[annot] != LorgConstants::NullProba) {
	  total_out += ai.outside_probabilities.array[annot];
	}
      }

      total_out = std::log(total_out);

      pred_beam_huang<PCKYAllCell<MyEdge> > huang(log_threshold, log_sent_prob, total_out);


      std::vector<typename MyEdge::BinaryDaughters >& bdaughters = edge->get_binary_daughters();
      bdaughters.erase(std::remove_if(bdaughters.begin(), bdaughters.end(), huang),
		       bdaughters.end());

      std::vector<typename MyEdge::UnaryDaughters >& udaughters = edge->get_unary_daughters();

      udaughters.erase(std::remove_if(udaughters.begin(), udaughters.end(), huang),
                       udaughters.end());
    }
  }
}

template<class MyEdge>
void PCKYAllCell<MyEdge>::change_rules_resize(const AnnotatedLabelsInfo& next_annotations,
                                              const std::vector<std::vector<std::vector<unsigned> > >& annot_descendants_current)
{
  for(unsigned i = 0; i < max_size; ++i)
    if(real_cell[i]) {

      AnnotationInfo a(next_annotations.get_number_of_annotations(i), 0.0);

      //process invalid annotations
      for(unsigned annot = 0; annot < real_cell[i]->get_annotations().inside_probabilities.array.size(); ++annot) {
	if(!real_cell[i]->valid_prob_at(annot)) {

          const std::vector<unsigned>& next_invalids = annot_descendants_current[i][annot];
          for(std::vector<unsigned>::const_iterator new_annot(next_invalids.begin()); new_annot != next_invalids.end(); ++new_annot) {
            a.inside_probabilities.array[*new_annot] = LorgConstants::NullProba;
            a.outside_probabilities.array[*new_annot] = LorgConstants::NullProba;
          }


        }
      }

      //replace annot
      std::swap(a,real_cell[i]->get_annotations());

      //replace rule
      real_cell[i]->replace_rule_probabilities(0);

    }
}



template<class MyEdge>
void PCKYAllCell<MyEdge>::change_rules_resize(unsigned new_size, unsigned finer_idx)
{
  for(unsigned i = 0; i < max_size; ++i)
    {
      if(real_cell[i]) {
	//resize
	real_cell[i]->get_annotations().reset_probabilities(0.0);
	real_cell[i]->get_annotations().resize(new_size);


	//replace rule
	real_cell[i]->replace_rule_probabilities(finer_idx);
      }
    }
}


template<class MyEdge>
void PCKYAllCell<MyEdge>::change_rules_backup(unsigned backup_idx, unsigned size_grammars)
{
  for(unsigned i = 0; i < max_size; ++i)
    {
      if(real_cell[i]) {

       	//replace rule TODO : replace all at once
        //	for(unsigned s = 0; s < size_grammars; ++ s)
          // 1 means we are in multiple grammar decoding
        real_cell[i]->replace_rule_probabilities( size_grammars);

	//load backup
	real_cell[i]->get_annotations() = real_cell[i]->get_annotations_backup()[backup_idx];
      }
    }

}


template<class MyEdge>
void PCKYAllCell<MyEdge>::modify_backup(unsigned backup_idx)
{
  for(unsigned i = 0; i < max_size; ++i)
    {
      if(real_cell[i]) {
        real_cell[i]->get_annotations_backup()[backup_idx] = real_cell[i]->get_annotations();
      }
    }
}



//simple stuff
template<class MyEdge>
std::ostream& operator<<(std::ostream& out, const PCKYAllCell<MyEdge>& cell)
{
  int nb_entries = 0;
  for(unsigned i = 0; i < cell.max_size ; ++i)
    if(cell.real_cell[i]) ++nb_entries;

  return out << "filled entries: " << nb_entries;
}

template<class MyEdge>
void PCKYAllCell<MyEdge>::clear()
{
  closed = false;

  if(!real_cell)
    {
      real_cell =  new CellEdge * [max_size];
      memset(real_cell, 0, max_size * sizeof(CellEdge*));
    }
  else
    for(unsigned i = 0; i < max_size; ++i)
      {
        if(real_cell[i]) {
          delete real_cell[i];
          real_cell[i] = 0;
        }
      }
}



#endif //PCKYALLCELL_H
