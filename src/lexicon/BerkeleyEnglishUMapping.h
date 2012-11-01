// -*- mode: c++ -*-
#ifndef BERKELEYENGLISHUMAPPING_H_
#define BERKELEYENGLISHUMAPPING_H_

#include "WordSignature.h"

class BerkeleyEnglishUMapping : public WordSignature
{
public:
  std::string get_unknown_mapping(const std::string& word, unsigned position) const;
};


#endif /*BERKELEYENGLISHUMAPPING_H_*/
