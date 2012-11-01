// -*- mode: c++ -*-
#ifndef RULE_H_
#define RULE_H_

#include "Production.h"

/**
  \class Rule
  \brief A class to represent the rule of a context free grammar within the parser.
*/
class Rule : public Production
{

private:
  int count;			///< current count
  double probability;	///< probability associated with the rule
  bool log_mode; ///< true if the probability is in logarithm mode
public:
  Rule(): count(0), probability(1.0), log_mode(true) {};

  /**
    \brief Constructor, initializes the rule
    \param llhs given left hand side
    \param rrhs given right hand side
    \param llexical given lexical status
    \param ccount given count
    \param pproba given probability
    \param mode log_mode
  */
  Rule(const int& llhs,const std::vector<int>& rrhs,const bool& llexical,const int& ccount, const double& pproba, bool mode);


  /**
     \brief Constructor, initializes the rule
     \param pprod given CFG rule (lhs,rhs,lexical)
     \param ccount given count
     \param pproba given probability
     \param mode log_mode
  */

  Rule(const Production& pprod,const int& ccount,const double& pproba, bool mode);

  /**
    \brief Destructor
  */
  virtual ~Rule();

  /**
    \brief Gets the current count
    \return count
  */
  int get_count() const;
  /**
    \brief Sets current count of the rule
    \param c New count value
  */
  void set_count(const int& c);

  /**
    \brief Gets the current probability
    \return Probability
  */
  double get_probability() const;
  /**
    \brief Sets probability
    \param p New probability value
  */
  void set_probability(const double& p);

  /**
     \brief Sets log_mode
     \param m true for log_mode
  */
  void set_log_mode(bool m);


  /**
     \brief Output operator
     \param out the ostream to write on
     \param rule the rule object to write
     \return the used ostream
  */
  friend std::ostream& operator<<(std::ostream& out, const Rule& rule);

  bool is_invalid() const;
};


inline
int Rule::get_count() const { return count; }

inline
void Rule::set_count(const int& c) { count = c; }

inline
double Rule::get_probability() const { return probability; }


inline
void Rule::set_probability(const double& p) { probability = p; }

inline
bool Rule::is_invalid() const {return false;}

#endif // RULE_H_
