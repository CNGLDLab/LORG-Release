// -*- mode: c++ -*-

#ifndef URULE_H
#define URULE_H

#include "AnnotatedRule.h"
#include "Types4Rules.h"


#include "utils/LorgConstants.h"

#include <cassert>

class URule : public AnnotatedRule
{
public:
  URule() : rhs0(-1), probabilities() {};
  /**
     \brief constructor for creating unary rules from the grammar file
     \param l the lhs of the object
     \param rhs0  the leftmost symbol of rhs
     \param probs a vector of unary_proba_info triples (i,j,p)
  */
  URule(short l, short rhs0, const std::vector<unary_proba_info>& probs);

  /**
     \brief constructor for creating unary rules for the base grammar (annotation sizes always 1)
     \param l the lhs of the object
     \param rhs0  the leftmost symbol of rhs
     \param proba_type probability
  */
  URule(short l, short rhs0, double prob);

  /**
     \brief constructor
     \param l the lhs of the object
     \param rhs0  the  symbol of rhs
     \param probs a 2d-vector of probabilities
  */
  URule(short l, short rhs0, const std::vector< std::vector<double> > & probs);


  bool is_lexical() const {return false;}

  /**
     \brief returns attribute rhs0
  */
  short get_rhs0() const;

  /**
     \brief read access to annotated probabilities
     \param a annotation for lefthandside symbol
     \param b annotation for leftmost righthandside symbol
  */
  const double& get_probability(unsigned short a, unsigned short b) const;

  /**
     \brief write access to annotated probabilities
     \param a annotation for lefthandside symbol
     \param b annotation for leftmost righthandside symbol
     \param value the new probability value
  */
  void   set_probability(unsigned short a, unsigned short b, const double& value);
  void set_probability(const double& value);

  /**
     \brief read/write access to the 2d-vector of probabilities
     \todo should be private ?
  */
  const std::vector< std::vector<double> >& get_probability() const;
  std::vector< std::vector<double> >& get_probability();

  /**
     \brief -> always true
   */
  bool is_unary() const;

  /**
     \brief smooth probabilities *over all* lhs annotations
     \param alpha the small constant
  */
  void linear_smooth(const double& alpha);
  void weighted_smooth(const double& alpha, const std::vector<std::vector<double> > & weights);
  void generation_smooth(const std::vector<std::vector<std::vector<double> > > & weights);


  /**
     \brief Output operator
     \param out the ostream to write on
     \param rule the urule object to write
     \return the used ostream
  */
  friend std::ostream& operator<<(std::ostream& out, const URule& rule);


  void update_inside_annotations(std::vector<double>& up,
				 const std::vector<double>& left) const;

  void update_outside_annotations(const std::vector<double>& up,
				  std::vector<double>& left) const;

  /**
     \brief removes useless zeros from probability vector
  */
  void compact();

  /**
     \brief resize the probability vector by adding zeros
     \param rhs_size new size for the the right-hand side symbol
  */
  void uncompact(unsigned rhs_size);

  /**
     \brief replace annotations with 0 if they're bel;ow threshold
     \param threshold the threshold
  */
  void remove_unlikely_annotations(const double& threshold);

  bool is_empty() const {return probabilities.empty();}

  bool operator==(const URule&) const;
  bool operator<(const URule&) const;

  bool is_invalid() const;

protected:
  short rhs0;
  std::vector< std::vector<double> > probabilities ; ///< probabilities for a CFG rule  with annotation

private:
};

inline
short URule::get_rhs0() const
{
  return rhs0;
}

inline
const double& URule::get_probability(unsigned short a, unsigned short b) const
{
  return probabilities[a][b];
}

inline
void URule::set_probability(unsigned short a, unsigned short b, const double& value)
{
  if(a>=probabilities.size()) {probabilities.resize(a+1);}
  if(b>=probabilities[a].size()) {probabilities[a].resize(b+1,0);}
  probabilities[a][b]=value;
}

inline
bool URule::is_unary() const
{
  return true;
}


inline
const std::vector< std::vector<double> >& URule::get_probability() const
{
  return probabilities;
}

inline
std::vector< std::vector<double> >& URule::get_probability()
{
  return probabilities;
}

inline
bool URule::is_invalid() const
{
  return (probabilities.size() == 0 || (probabilities.size() == 1 && probabilities[0].size() == 0));
}


#endif //URULE_H
