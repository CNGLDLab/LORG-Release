// -*- mode: c++ -*-
#ifndef _BRULEC2F_H_
#define _BRULEC2F_H_

#include "BRule.h"
#include "C2f.h"
//#include <cmath>

typedef C2f<BRule> BRuleC2f;


// class BRuleC2f : public C2f<BRule> 
// {
// public:
//   BRuleC2f(const BRule& r) : C2f<BRule>(r) {}

//   void set_logmode()
//   {
//     for(unsigned i = 0; i < probabilities.size(); ++i)
//       for(unsigned j = 0; j < probabilities[i].size(); ++j)
// 	for(unsigned k = 0; k < probabilities[i][j].size(); ++k)
//       probabilities[i][j][k] = std::log(probabilities[i][j][k]);
//     logmode = true;
//   }
// };

#endif /* _BRULEC2F_H_ */
