// -*- mode: c++ -*-
#ifndef _URULEC2F_H_
#define _URULEC2F_H_

#include "URule.h"
#include "C2f.h"
//#include <cmath>

typedef C2f<URule> URuleC2f;

// class URuleC2f : public C2f<URule> 
// {
// public:
//   URuleC2f(const URule& r) : C2f<URule>(r) {}

//   void set_logmode()
//   {
//     for(unsigned i = 0; i < probabilities.size(); ++i)
//       for(unsigned j = 0; j < probabilities[i].size(); ++j)
//       probabilities[i][j] = std::log(probabilities[i][j]);
//     logmode = true;
//   }
// };


#endif /* _URULEC2F_H_ */
