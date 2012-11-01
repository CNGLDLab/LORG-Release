// -*- mode: c++ -*-
#ifndef PCKYBESTCELL_H
#define PCKYBESTCELL_H

#include <cassert>
#include <cstring>
#include "edges/Edge.h"
#include "Word.h"
#include "rules/Rule.h"

/**
  \class PCKYBestCell
  \brief represents a cell in a chart
  that only accepts new or more probable edges
*/
class PCKYBestCell {
 private:
  Edge ** real_cell;
  bool closed;
  const Edge * word_edge;

  static unsigned max_size;

public:
  typedef Edge CellEdge;

  /**
     \brief Simple constructor
     \note this cell is not initialised: trying
     to use it will result in segfault !
     You have to call init first
   */
  PCKYBestCell() : real_cell(NULL), closed(true), word_edge(NULL) {};

  /**
     \brief Constructor
     \param cl true is closed
   */
  PCKYBestCell(bool cl);

  /**
     \brief destructor
   */
  ~PCKYBestCell();


  /**
     \brief initialise the cell
     \param cl true is closed
   */
  void init(bool cl);

  /**
     \brief insert a candidate edge in the cell
     \param e a pointer to the candidate edge
     \return a pointer to the newly created edge or NULL if no insertion took place
  */
  const Edge * process_candidate(const Edge &e);


  /**
     \brief test if there's an edge of given lhs in the cell
     \param label a lhs label
     \return true if label is in the cell
  */
  bool exists_edge(int label) const;



  /**
     \brief add an edge  to the cell
     \param edge the edge to add
     \return a pointer to the newly created edge
  */
  const Edge * add_edge(const Edge& edge);


  /**
     \brief access an edge by its lhs
     \param i the label of the edge
     \return the edge with i as lhs
   */
  Edge& operator[](unsigned i);


  /**
     \brief access an edge by its lhs
     \param i the label of the edge
     \return the edge with i as lhs
  */
  const Edge& at(int i ) const;


  /**
     \brief access the mostprobable edge by its lhs
     \param i the label of the edge
     \return the best edge with i as lhs
  */
  const Edge& best_at(int i) const;


  /**
     \brief access
     \return true if the cell is closed
  */
  bool is_closed() const;



  /**
     \brief removes edges to far from the most probable one
   */

  void apply_beam();


  void set_word_edge(const Edge * we);
  const Edge * get_word_edge() const;

  void add_word(const Word & word);


  /**
     \brief Output operator
     \param out the ostream to write on
     \param cell the cell object to write
    \return the used ostream
  */
  friend std::ostream& operator<<(std::ostream& out, const PCKYBestCell& cell);


  static void set_max_size(unsigned size);

};


inline
bool PCKYBestCell::exists_edge(int label) const
{
//   assert(label >= 0);
//   assert(label < (int) max_size);
  return (real_cell[label] != NULL);
}


inline
const Edge * PCKYBestCell::process_candidate(const Edge& candidate)
{
  //  std::cout << max_size << std::endl;
  //  std::cout << candidate.get_lhs() << std::endl;
  assert(0 <= candidate.get_lhs());
  assert(candidate.get_probability() <= 0);
  //  assert(candidate.get_lhs() <= (int) max_size);


  Edge ** current = &real_cell[candidate.get_lhs()];

  if(*current) {
    if (candidate.get_probability() > (*current)->get_probability()) {
      (*current)->replace(candidate);
    }
    else
      return NULL;
  }
  else
    *current = new Edge(candidate);

  return *current;
}

inline
const Edge * PCKYBestCell::add_edge(const Edge& edge)
{
  return real_cell[edge.get_lhs()] = new Edge(edge);
}


inline
Edge& PCKYBestCell::operator[](unsigned i)
{
  return *real_cell[i];
}

inline
const Edge& PCKYBestCell::at(int i) const
{
  //assert(i>=0);
  //assert( i < (int) max_size);
  assert(i>=0 && i < (int) max_size);

  return *real_cell[i];
}

inline
const Edge& PCKYBestCell::best_at(int i) const
{
  return at(i);
}


inline
void PCKYBestCell::init(bool cl)
{
  if(!(closed = cl)) {
    word_edge = NULL;
    real_cell =  new Edge * [max_size];
    memset(real_cell, 0, max_size * sizeof(Edge*));
    //   for(unsigned i = 0; i < max_size;++i)
    //     real_cell[i]=NULL;
  }
}

inline
bool PCKYBestCell::is_closed() const
{ return closed; }



inline
void PCKYBestCell::set_word_edge(const Edge * we)
{
  word_edge = we;
}

inline
const Edge * PCKYBestCell::get_word_edge() const
{
  return word_edge;
}

inline
void PCKYBestCell::add_word(const Word & word)
{
  if(!word_edge)
    set_word_edge(new Edge(word.get_id(),0,true));

  for(std::vector<const MetaProduction*>::const_iterator r_iter = word.get_rules().begin();
      r_iter != word.get_rules().end(); ++ r_iter) {
    //      std::cout <<*(*r_iter) << std::endl;
    (void) add_edge(Edge(static_cast<const Rule*>(*r_iter)->get_lhs(),word_edge,(static_cast<const Rule*>(*r_iter))->get_probability()));
  }

}

inline
void PCKYBestCell::apply_beam()
{
  // TODO: remove the define
#define BEAM_CONST 60

  double max_prob = - std::numeric_limits<double>::infinity();

  for(unsigned i = 0 ; i < max_size; ++i)
    if(real_cell[i] && max_prob < real_cell[i]->get_probability())
      max_prob = real_cell[i]->get_probability();


  for(unsigned i = 0 ; i < max_size; ++i)
    if(real_cell[i] && max_prob > real_cell[i]->get_probability() + BEAM_CONST) {
      delete real_cell[i];
      real_cell[i] =  NULL;
    }
}

inline
void PCKYBestCell::set_max_size(unsigned size)
{
  max_size = size;
}





#endif //PCKYBESTCELL_H
