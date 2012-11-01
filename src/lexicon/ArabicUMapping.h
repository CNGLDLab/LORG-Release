// -*- mode: c++ -*-
#ifndef ARABICUMAPPING_H
#define ARABICUMAPPING_H

#include "WordSignature.h"


class ArabicUMapping : public WordSignature
{
public:
  std::string get_unknown_mapping(const std::string& word, unsigned position) const;
};





#endif /*ARABICUMAPPING_H*/
