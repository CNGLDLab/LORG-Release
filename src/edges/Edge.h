// -*- mode: c++ -*-
#ifndef EDGE_H_
#define EDGE_H_

#include <string>
#include "utils/PtbPsTree.h"

/**
  \class Edge
  \brief represents an edge in a chart
*/
class Edge
{
public:
  /**
     \brief Constructor for creating an empty edge
  */
  Edge();

  /**
     \brief Constructor
     \param l lhs nonterminal
     \param r rhs in a unary rule
     \param p probability
  */
  Edge(int l,const Edge * r, const double& p);

  /**
     \brief Constructor
     \param l lhs nonterminal
     \param r1 first rhs symbol in a binary rule
     \param r2 first rhs symbol in a binary rule
     \param p probability
  */

  Edge(int l,const Edge * r1,const Edge * r2, const double& p);

  /**
     \brief Constructor
     \param l lhs nonterminal
     \param p probability
  */
  Edge(int l, const double& p, bool lex =  false);

  /**
     \brief Copy Constructor
     \param e edge to copy
  */
  Edge(const Edge& e);

  /**
     \brief Constructor transform a tree in an edge with all probabilities set to log(1)=0
     \param tree a tree assumed to be binarized
   */
  Edge(PtbPsTree& tree);

  /**
     \brief reset the attribute values of an unary edge
     \param e a pointer to the child edge
     \param p probability
  */
  void replace(const Edge& e);

  /**
     \brief Destructor
  */
  ~Edge();


  /**
     \brief get the lhs of the edge
  */
  int get_lhs() const;

  /**
     \brief set the lhs of the edge
     \param l new lhs
  */
  void set_lhs(int l);

  /**
     \brief set the probability of the edge
     \param p new probability
  */
  void set_probability(const double& p);

  /**
     \brief set the left child of the edge
     \param lc new left child
  */
  void set_left_child(const Edge * lc);

  /**
     \brief set the right child of the edge
     \param rc new right child
  */
  void set_right_child(const Edge * rc);


    /**
     \brief set the left child of the edge
     \param lc new left child
  */
  const Edge * get_left_child() const;

  /**
     \brief set the right child of the edge
     \param rc new right child
  */
  const Edge * get_right_child() const;


  /**
     \brief get the probability of the edge
  */
  double get_probability() const;

  /**
     \brief return true if the edge is childless
  */
  bool is_terminal() const;

  /**
     \brief explore an edge to build a ptbpstree
  */
  PtbPsTree * to_ptbpstree(int /*start symbol, not read*/, unsigned /*not read*/,bool /*not read*/, bool /* not read*/) const;


  /**
     \brief Output operator
     \param out the ostream to write on
     \param edge the edge object to write
     \return the used ostream
  */
  friend std::ostream& operator<<(std::ostream& out, const Edge& edge);

private:
  int lhs;				///< left hand side symbol
  const Edge* left;		///< children of the edge
  const Edge* right;		///< children of the edge
  double probability;			///< probability associated with the edge
  bool lex; /// true if the lhs should be read in symboltable::instance_word
            /// should be false during CKY viterbi parsing  for all edges.

  void to_ptbpstree(PtbPsTree& tree, PtbPsTree::depth_first_iterator& pos) const;
  Edge(PtbPsTree& tree, PtbPsTree::depth_first_iterator pos);
  Edge& operator=(const Edge&);
};


inline
void
Edge::replace(const  Edge& e)
{
  //  assert(lhs==e.lhs);

  left = e.left;
  right = e.right;
  probability = e.probability;
}

inline
int Edge::get_lhs() const
{
  return lhs;
}

inline
void Edge::set_lhs(int l)
{
  lhs=l;
}

inline
void Edge::set_probability(const double& p)
{
  probability = p;
}

inline
double Edge::get_probability() const
{
  return probability;
}

inline
bool Edge::is_terminal() const
{
  return (left==NULL && right==NULL);
}

inline
void Edge::set_left_child(const Edge * lc)
{
  left=lc;
}

inline
void Edge::set_right_child(const Edge * rc)
{
  right=rc;
}

inline
const Edge * Edge::get_left_child() const
{
  return left;
}

inline
const Edge * Edge::get_right_child() const
{
return right;
}

inline
Edge& Edge::operator=(const Edge& other)
{
  lhs = other.lhs;
  left = other.left;
  right= other.right;
  probability = other.probability;
  return *this;
}

#endif /*EDGE_H_*/
