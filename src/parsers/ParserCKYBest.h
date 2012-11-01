// -*- mode: c++ -*-
#ifndef PARSERCKYBEST_H
#define PARSERCKYBEST_H


#include "grammars/Grammar.h"
#include "ChartCKY.h"
#include "edges/Edge.h"
#include "PCKYBestCell.h"

#include "ParserCKY.h"

/**
  \class ParserCKYBest
  \brief represents a parsing device for probabilistic cfgs using the cky algorithm
*/
class ParserCKYBest : public ParserCKY< Grammar<Rule,Rule,Rule> >
{
public:
  typedef PCKYBestCell Cell;
  typedef ChartCKY<Cell, Word> Chart;
  typedef Grammar<Rule,Rule,Rule> SimpleGrammar;
  typedef ParserCKY<SimpleGrammar> Parser;


  ~ParserCKYBest();

  /**
     \brief ParserCKYBest constructor
     \param g the grammar to be used when parsing sentences
  */
  ParserCKYBest(Grammar<Rule,Rule,Rule>* g);

  /**
      \brief parses the sentence using the grammar
      \param chart the chart to fill
  */
  void parse(Chart& chart) const;


private:

  /** \brief Add unary rules at this position in the chart
      (only consider non-terminals created from binary rules)
      \param cell the cell to fill
      \param isroot true if cell is root
  */
  void add_unary(Cell& cell, bool isroot) const;

  /** \brief Add unary rules at this position in the chart
      (only consider non-terminals created from pos)
      \param cell the cell to fill
      \param isroot true if cell is root
  */
  void add_unary_init(Cell&cell, bool isroot) const;

  /**
     \brief finds all extensions of an edge using unary rules
     \param cell the cell to fill
     \param edge_ptr the address of the edge to extend
     \param isroot true if cell is root
   */
  void follow_unary_chain(Cell& cell, const Edge * edge_ptr, bool isroot) const;


  /**
     \brief processes the internal rules, adding edges to the chart where appropriate
     \param chart the chart to fill
  */
  void process_internal_rules(Chart& chart) const;


  /**
     \brief fill the result cell with the most probable edges for each lhs,
     created from edges contained in left_cell and right_cell
     \param left_cell the  leftmost cell to combine
     \param right_cell the rightmost cell to combine
     \param result_cell the cell to store new edges
  */
  void get_candidates(const Cell& left_cell, const Cell& right_cell, Cell& result_cell) const;

};


#endif /*PARSERCKYBEST_H*/
