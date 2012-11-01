#ifndef ENGLISHIGMAPPING_H_
#define ENGLISHIGMAPPING_H_

#include "WordSignature.h"

class EnglishIGMapping : public WordSignature
{
public:
  std::string get_unknown_mapping(const std::string& word, unsigned /*position*/) const;
};


#endif /*ENGLISHIGTESTMAPPING_H_*/
