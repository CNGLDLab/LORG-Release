// -*- mode: c++ -*-
#ifndef CHARTCKY_H_
#define CHARTCKY_H_

#include "utils/PtbPsTree.h"
#include "utils/LorgConstants.h"
#include "utils/SymbolTable.h"

#include <vector>
#include <Bracketing.h>


/**
  \class ChartCKY
  \brief represents a chart of cells
*/
template<class Cell, class MyWord>
class ChartCKY
{
private:
  Cell ** chart; ///< the chart itself
  unsigned size;     ///< the size of the chart
  const std::vector< MyWord >& sentence;
  const std::vector<bracketing>& brackets;


  // prevents unwanted conversions
  ChartCKY(const ChartCKY&);
  ChartCKY& operator=(const ChartCKY&);

public:
  ~ChartCKY();

  /**
     \brief constructor with initialisation
     \param sentence the sentence to create the chart
     \param grammar_size  the number non-terminals in the grammar
     \param brackets chunks
  */
  ChartCKY(const std::vector< MyWord >& sentence, unsigned grammar_size, const std::vector<bracketing>& brackets);

  /**
     \brief get the size of the chart
     \return the size of the chart
  */
  unsigned get_size() const;


  /**
     \brief access a cell of the chart by its coordinates
     \param start starting point of the cell's span
     \param end   ending point of the cell's span
     \return a cell (may segfault if coordinates are out of bounds)
  */

  Cell& access(unsigned start, unsigned end) const;

  Cell& get_root() const;

  PtbPsTree* get_best_tree(int start_symbol, unsigned k, bool always_output_forms, bool output_annotations) const;

  double get_score(int start_symbol, unsigned k) const;


  void init(const std::vector< MyWord >& sentence);

  void reset_probabilities();

  bool has_solution(int symb, unsigned i) const;

  void clear();

  void prepare_retry();

  bool is_valid(int start_symbol) const;

};


//#include "utils/SymbolTable.h"

template<class Cell, class MyWord>
inline
unsigned ChartCKY<Cell, MyWord>::get_size() const
{
  return size;
}

template<class Cell, class MyWord>
inline
Cell& ChartCKY<Cell, MyWord>::access(unsigned start, unsigned end) const
{
//   assert(start <= end);
//   assert(end < size);
  return chart[start][end-start];
}

template<class Cell, class MyWord>
ChartCKY<Cell, MyWord>::~ChartCKY()
{
  for(unsigned i = 0; i < size; ++i)
    delete[] chart[i];
  delete[] chart;
}

struct cell_close_helper
{
  const bracketing& brackets;
  cell_close_helper(const bracketing& b) : brackets(b) {}
  bool operator()(const bracketing& other) const
  {
    return brackets.overlap(other);
  }
};


template<class Cell, class MyWord>
Cell& ChartCKY<Cell, MyWord>::get_root() const
{
  return access(0,size-1);
}

template<class Cell, class MyWord>
PtbPsTree* ChartCKY<Cell, MyWord>::get_best_tree(int start_symbol, unsigned k, bool output_forms, bool output_annotations) const
{
  PtbPsTree* tree = NULL;

  const Cell & root_cell = this->get_root();

  if (!root_cell.is_closed() && root_cell.exists_edge(start_symbol)) {
    tree = root_cell.best_at(start_symbol).to_ptbpstree(start_symbol, k, output_annotations, output_forms);
  }

  return tree;
}

//score at root
template<class Cell, class MyWord>
double ChartCKY<Cell, MyWord>::get_score(int symbol, unsigned k) const
{
  return get_root().at(symbol).get_best().get(k).probability;
}


template<class Cell, class MyWord>
void ChartCKY<Cell, MyWord>::init(const std::vector< MyWord >& sentence)
{
  //iterate through all the words in the sentence
  for(typename std::vector<MyWord>::const_iterator w_itr(sentence.begin());
      w_itr != sentence.end(); ++w_itr)  {
    Cell& cell = this->access(w_itr->get_start(), w_itr->get_end() -1);
    cell.add_word(*w_itr);
  }
}

template<class MyWord>
unsigned find_last_in_sentence(const std::vector< MyWord >& s)
{
  unsigned res = 0;
  for (typename std::vector< MyWord >::const_iterator i(s.begin()); i != s.end(); ++i)
    {
      if ((unsigned) i->get_end() > res) res = (unsigned) i->get_end();
    }
  return res;
}



// assume that words in sentence are in left to right direction (start in ascending direction)
template<class Cell, class MyWord>
ChartCKY<Cell, MyWord>::ChartCKY(const std::vector< MyWord >& s, unsigned grammar_size, const std::vector<bracketing>& bs) :
  chart(NULL),
  size(find_last_in_sentence(s)),
  sentence(s),
  brackets(bs)
{
  Cell::set_max_size(grammar_size);

  chart = new Cell * [size];

  for(unsigned i = 0; i < size; ++i) {

    //    std::cout << "i: " << i << std::endl;

    chart[i] = new Cell[size-i];

    for(unsigned j = i; j < size;++j) {
      Cell& cell = access(i,j);
      bool close = std::find_if(brackets.begin(),brackets.end(), cell_close_helper(bracketing(i,i+j))) != brackets.end() ;
      cell.init(close);
    }
  }

  for (unsigned i = 0; i < sentence.size(); ++i)
    {
      // todo: proper error handling
      if(access(sentence[i].get_start(), sentence[i].get_end()-1).is_closed())
        std::clog << "Problem in chart initialisation: brackets and tokenization are insconsistent." << std::endl;

      access(sentence[i].get_start(), sentence[i].get_end()-1).add_word(sentence[i]);
    }


  //  std::cout << "Chart is built and intialised" << std::endl;
}




template<class Cell, class MyWord>
void ChartCKY<Cell, MyWord>::reset_probabilities()
{
  for(unsigned i = 0; i < size; ++i)
    for(unsigned j = i; j < size; ++j) {
      //      std::cout << "(" << i << "," << j << ")" << std::endl;
      access(i,j).reset_probabilities();
    }
}


template<class Cell, class MyWord>
bool ChartCKY<Cell, MyWord>::has_solution(int symb, unsigned i) const
{
  //  std::cout << SymbolTable::instance_nt().translate(symb) << std::endl;
  return get_root()[symb]->has_solution(i);
}

template<class Cell, class MyWord>
void ChartCKY<Cell, MyWord>::clear()
{
  for(unsigned i = 0; i < size; ++i)
    for(unsigned j = i; j < size; ++j) {
      access(i,j).clear();
    }
}

template<class Cell, class MyWord>
void ChartCKY<Cell, MyWord>::prepare_retry()
{
  this->clear();
  this->reset_probabilities();

  for(unsigned i = 0; i < size; ++i) {
    // j == 0 -> word position
    Cell& cell = chart[i][0];
    cell.init(false);
    cell.add_word(sentence[i]);

    // regular chart cells
    for(unsigned j = 1; j < size-i;++j) {
      bool close = std::find_if(brackets.begin(),brackets.end(), cell_close_helper(bracketing(i,i+j))) != brackets.end() ;
      chart[i][j].init(close);
    }
  }
}

template <class Cell, class MyWord>
bool ChartCKY<Cell, MyWord>::is_valid(int start_symbol) const
{
  return !get_root().is_closed() && get_root().exists_edge(start_symbol);
}

#endif /*CHARTCKY_H_*/
