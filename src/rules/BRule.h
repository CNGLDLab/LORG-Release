// -*- mode: c++ -*-
#ifndef BRULE_H
#define BRULE_H

#include <vector>
#include "AnnotatedRule.h"
#include "Types4Rules.h"

#include "utils/LorgConstants.h"

typedef std::vector< std::vector< std::vector<double> > > vector_3d;

/**
   \class BRule
   \brief a PCFG-LA binary rule
 */
class BRule : public AnnotatedRule
{

public:
  BRule() : rhs0(-1), rhs1(-1), probabilities() {};
  virtual ~BRule() {};

  /**
     \brief constructor for creating binary rules from the grammar file
     \param l the lhs of the object
     \param rhs0  the leftmost symbol of rhs
     \param rhs1  the rightmost symbol of rhs
     \param probs a vector of binary_proba_info 4-tuple (i,j,k,p)
  */
  BRule(short l, short rhs0, short rhs1, const std::vector<binary_proba_info>& probs);

  /**
     \brief constructor for the creation of a base grammar binary rule
     \param l the lhs of the object
     \param rhs0  the leftmost symbol of rhs
     \param rhs1  the rightmost symbol of rhs
     \param double probability
   */
  BRule(short l, short rhs0, short rhs1, double prob);

  /**
     \brief constructor
     \param l the lhs of the object
     \param rhs0  the leftmost symbol of rhs
     \param rhs1  the rightmost symbol of rhs
     \param probs a 3d-vector of probabilities
  */
  BRule(short l, short rhs0, short rhs1, const vector_3d& probs);


  bool is_lexical() const {return false;}

  /**
     \brief returns attribute rhs0
  */
  short get_rhs0() const;

  /**
     \brief returns attribute rhs1
  */
  short get_rhs1() const;


  /**
     \brief read access to annotated probabilities
     \param a annotation for lefthandside symbol
     \param b annotation for leftmost righthandside symbol
     \param c annotation for rightmost righthandside symbol
   */
  const double& get_probability(unsigned short a, unsigned short b, unsigned short c) const;

  /**
     \brief write access to annotated probabilities
     \param a annotation for lefthandside symbol
     \param b annotation for leftmost righthandside symbol
     \param c annotation for rightmost righthandside symbol
     \param value the new probability value
  */
  void set_probability(unsigned short a, unsigned short b, unsigned short c, const double& value);
  void set_probability(const double& value);

  /**
     \brief read/write access to the 3d-vector of probabilities
     \todo should be private ?
  */
  const vector_3d& get_probability() const;
  vector_3d& get_probability();

  /**
     \brief -> always false
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
  friend std::ostream& operator<<(std::ostream& out, const BRule& rule);


  void update_inside_annotations(std::vector<double>& up,
					const std::vector<double>& left,
					const std::vector<double>& right) const;

  void update_inside_annotations(std::vector<double>& up,
                                 const double& left_right_precomputation) const;


  void update_outside_annotations(const std::vector<double>& up_out,
				  const std::vector<double>& left_in,
				  const std::vector<double>& right_in,
				  std::vector<double>& left_out,
				  std::vector<double>& right_out) const;

  /**
     \brief removes useless zeros from probability vector
  */
  void compact();

  /**
     \brief resize the probability vector by adding zeros
     \param rhs0_size new size for the the first right-hand side symbol
     \param rhs1_size new size for the the second right-hand side symbol
  */
  void uncompact(unsigned rhs0_size, unsigned rhs1_size);

  /**
     \brief replace annotations with 0 if they're below threshold
     \param threshold the threshold
  */
  void remove_unlikely_annotations(const double& threshold);


  bool is_empty() const {return probabilities.empty();}

  bool operator==(const BRule&) const;
  bool operator<(const BRule& other) const;


  bool is_invalid() const;

protected:
  short rhs0;
  short rhs1;
  vector_3d probabilities ; ///< probabilities for a CFG rule  with annotations
private:
};

inline
short BRule::get_rhs0() const
{
  return rhs0;
}

inline
short BRule::get_rhs1() const
{
  return rhs1;
}


inline
const double& BRule::get_probability(unsigned short a, unsigned short b, unsigned short c) const
{
  return probabilities[a][b][c];
}

inline
void BRule::set_probability(unsigned short a, unsigned short b, unsigned short c, const double& value)
{
  probabilities[a][b][c]=value;
}

inline
bool BRule::is_unary() const
{
  return false;
}

inline
const vector_3d& BRule::get_probability() const
{
  return probabilities;
}

inline
vector_3d& BRule::get_probability()
{
  return probabilities;
}

inline
bool BRule::is_invalid() const
{
  return
    probabilities.empty() ||
    (probabilities.size() == 1 && probabilities[0].empty()) ||
    (probabilities.size() == 1 && probabilities[0].size() == 1 && probabilities[0][0].empty());
}



#endif //BRULE_H
