// -*- mode: c++ -*-
#ifndef _ITALIANIGMAPPING_H_
#define _ITALIANIGMAPPING_H_

#include "WordSignature.h"


class ItalianIGMapping : public WordSignature
{
public:
  std::string get_unknown_mapping(const std::string& word, unsigned position) const;
};

#endif /* _ITALIANIGMAPPING_H_ */
