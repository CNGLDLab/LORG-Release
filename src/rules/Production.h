// -*- mode: c++ -*-
#ifndef PRODUCTION_H
#define PRODUCTION_H

#include <vector>
#include <string>
#include <iostream>

#include "MetaProduction.h"
/**
   \class Production
   \brief a class to represent CFG Productions
 **/

class Production : public MetaProduction
{
protected:
  std::vector<int> rhs; ///< right hand side symbols
  bool lexical;                 ///< indicates lexical vs. non-terminal rule

public:
  Production(): MetaProduction(), rhs(), lexical(false) {};

   /**
    \brief Constructor, initializes the production
    \param llhs given left hand side
    \param rrhs given right hand side
    \param llexical given lexical status
  */
  Production(int llhs, const std::vector<int>& rrhs, bool llexical);
  
  /**
    \brief Constructor, that constructs a production from a formatted string
  */
  Production(const std::string& s);

  /**
    \brief Destructor
  */
  virtual ~Production();

  /**
    \brief Gets the (integer value) child at a particular index in the vector of children
    \param index index of child to be returned
  */
  int get_rhs(int index) const;

  /**
    \brief Gets the (integer value) child at a index 0 in the vector of children
  */
  int get_rhs0() const;

  /**
     \brief Gets the (integer value) child at a index 1 in the vector of children
  */
  int get_rhs1() const;

  /**
    \brief returns true if rule is unary, false otherwise
  */
  bool is_unary() const;
  
  /**
    \brief Gets the current right hand side of the production
    \return Right hand side
  */
  std::vector<int>& get_rhs();
  const std::vector<int>& get_rhs() const;
  /**
    \brief Sets right hand side symbols of the production
    \param[in] r Vector of new left hand side symbols
  */
  void set_rhs(const std::vector<int>& r);

  /**
    \brief Gets the current lexical status of the production
    \return true, if lexical, false otherwise
  */
  bool is_lexical() const;
  
  /**
    \brief Sets the lexical status of the production
    \param[in] l New lexical status, true for lexical, false for non-terminal
  */
  void set_lexical(bool l);

  // std::list<Production> binarize_right() const;
  
  // std::list<Production> binarize_left() const;



  bool operator==(const Production& other) const;
  
  // to be able to use std::maps with Productions as keys
  bool operator<(const Production& other) const;


  
  /**
     \brief Output operator
     \param os the ostream to write on
     \param prod the production object to write
     \return the used ostream
  */
  friend std::ostream& operator<<(std::ostream& os, const Production& prod);


};

//TODO test index < vector.size()
inline
int Production::get_rhs(int index) const 
{ 
  return rhs[index]; 
}

//TODO test vector.size() > 0
inline
int Production::get_rhs0() const 
{ 
  return rhs[0]; 
}

//TODO test vector.size() > 1
inline
int Production::get_rhs1() const 
{ 
  return rhs[1]; 
}



inline
bool Production::is_unary() const 
{ 
  return (rhs.size()==1); 
}
  

inline
const std::vector<int>& Production::get_rhs() const 
{ 
  return rhs; 
}

inline
std::vector<int>& Production::get_rhs()
{ 
  return rhs; 
}


inline
void Production::set_rhs(const std::vector<int>& r) 
{ 
  rhs = r; 
}

inline
bool Production::is_lexical() const 
{ 
	return lexical; 
}

inline 
void Production::set_lexical(bool l) 
{ 
  lexical = l; 
}

inline
bool Production::operator==(const Production& other) const
{
  return lhs == other.lhs && lexical == other.lexical && rhs == other.rhs;
}


inline
bool Production::operator<(const Production& other) const
{
  return lhs < other.lhs || 
    (lhs == other.lhs && rhs < other.rhs);
}




// inline
// Production & Production::operator=(const Production& other)
// {
//   if (this != &other) // protect against invalid self-assignment
//   {
//     lhs = other.lhs;
//     rhs = other.rhs;
//     lexical = other.lexical;
//   }
  
//   // by convention, always return *this
//   return *this;
// };

#endif // PRODUCTION_H
