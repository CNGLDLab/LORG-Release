// -*- mode: c++ -*-
#ifndef _FRENCHIGMAPPING_H_
#define _FRENCHIGMAPPING_H_

#include "WordSignature.h"


class FrenchIGMapping : public WordSignature
{
public:
  std::string get_unknown_mapping(const std::string& word, unsigned position) const;
};

#endif /* _FRENCHIGMAPPING_H_ */
