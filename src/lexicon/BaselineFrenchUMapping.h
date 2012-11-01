// -*- mode: c++ -*-
#ifndef BASELINEFRENCHUMAPPING_H_
#define BASELINEFRENCHUMAPPING_H_

#include "WordSignature.h"

class BaselineFrenchUMapping : public WordSignature
{
public:
  std::string get_unknown_mapping(const std::string& word, unsigned position) const;
};


#endif /*BASELINEFRENCHUMAPPING_H_*/
