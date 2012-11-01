// -*- mode: c++ -*-
#ifndef _ARABICIGMAPPING_H_
#define _ARABICIGMAPPING_H_

#include "WordSignature.h"
#include <boost/regex.hpp>

class ArabicIGMapping : public WordSignature
{
public:
  std::string get_unknown_mapping(const std::string& word, unsigned position) const;
};

#endif /* _ARABICIGMAPPING_H_ */
