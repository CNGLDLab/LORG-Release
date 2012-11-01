// -*- mode: c++ -*-
#ifndef GRAMMAR_H_
#define GRAMMAR_H_

#include <vector>
#include <string>
#include <algorithm>



template <typename Bin, typename Un, typename Lex>
class Grammar
{
public:
  typedef Bin MyBinaryRule;
  typedef Un  MyUnaryRule;
  typedef Lex MyLexicalRule;

  std::vector<Bin> binary_rules;
  std::vector<Un>  unary_rules;
  std::vector<Lex> lexical_rules;

public:
  Grammar();
  virtual ~Grammar();

  /**
     \brief set rules
     \param binary_rules binary rules
     \param unary_rules unary rules
     \param lexical_rules lexical rules
  */
  void set_rules(std::vector<Bin>&  binary_rules,
		 std::vector<Un>&   unary_rules,
		 std::vector<Lex>& lexical_rules);

  void init();

  Grammar(const std::string& filename);


protected:
};

#include <algorithm>


template<class Bin, class Un, class Lex>
Grammar<Bin, Un, Lex>::Grammar() :
  binary_rules(), unary_rules(), lexical_rules()
{}

template<class Bin, class Un, class Lex>
Grammar<Bin, Un, Lex>::~Grammar() {}


template<class Bin, class Un, class Lex>
void Grammar<Bin, Un, Lex>::init()
{
  // remove rules "overly low" probabilities
  binary_rules.erase(std::remove_if(binary_rules.begin(), binary_rules.end(),
                                    std::mem_fun_ref(&Bin::is_invalid)),
                     binary_rules.end());

  unary_rules.erase(std::remove_if(unary_rules.begin(), unary_rules.end(),
                                   std::mem_fun_ref(&Un::is_invalid)),
                    unary_rules.end());

  std::sort(binary_rules.begin(),binary_rules.end());
  std::sort(unary_rules.begin(),unary_rules.end());
  std::sort(lexical_rules.begin(),lexical_rules.end());
}


template<class Bin, class Un, class Lex>
inline
void Grammar<Bin, Un, Lex>::set_rules(std::vector<Bin>& binary_rules_,
				      std::vector<Un>&  unary_rules_,
				      std::vector<Lex>& lexical_rules_)
{
  binary_rules_.swap(binary_rules);
  unary_rules_.swap(unary_rules);
  lexical_rules_.swap(lexical_rules);
}


#endif /*GRAMMAR_H_*/
