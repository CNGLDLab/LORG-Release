// -*- mode: c++ -*-

#ifndef _LEXICALRULEC2F_H_
#define _LEXICALRULEC2F_H_

#include "LexicalRule.h"
#include "C2f.h"
//#include <cmath>

typedef C2f<LexicalRule> LexicalRuleC2f;

// class LexicalRuleC2f : public C2f<LexicalRule>
// {
// public:
//   LexicalRuleC2f(const LexicalRule& r) : C2f<LexicalRule>(r) {}

//   void set_logmode()
//   {
//     for(unsigned i = 0; i < probabilities.size(); ++i)
//       probabilities[i] = std::log(probabilities[i]);
//     logmode = true;
//   }
// };

#endif /* _LEXICALRULEC2F_H_ */
