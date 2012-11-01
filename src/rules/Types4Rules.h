// -*- mode: c++ -*-
#ifndef TYPES4RULES_H
#define TYPES4RULES_H

//typedef int rulesymbol;

struct binary_proba_info
{
  short lhs_pos; // position in the matrix
  short rhs0_pos; // position in the matrix
  short rhs1_pos; // position in the matrix
  double probability;

  binary_proba_info() : lhs_pos(), rhs0_pos(), rhs1_pos(), probability() {};

  binary_proba_info(short l, short r0, short r1, double p) :
    lhs_pos(l), rhs0_pos(r0), rhs1_pos(r1), probability(p){};
};

struct unary_proba_info
{
  short lhs_pos; // position in the matrix
  short rhs0_pos; // position in the matrix
  double probability;

  unary_proba_info() : lhs_pos(), rhs0_pos(), probability() {};

  unary_proba_info(short l, short r0, double p) :
    lhs_pos(l), rhs0_pos(r0), probability(p) {};
};

struct lexical_proba_info
{
  short lhs_pos; // position in the matrix
  double probability;

  lexical_proba_info() : lhs_pos(), probability() {};

  lexical_proba_info(short l, double p) :
    lhs_pos(l), probability(p) {};
};



#endif
