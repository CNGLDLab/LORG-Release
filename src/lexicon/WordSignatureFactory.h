// -*- -mode: c++ *-
#ifndef _WORDSIGNATUREFACTORY_H_
#define _WORDSIGNATUREFACTORY_H_

#include "utils/ConfigTable.h"

#include "WordSignature.h"

namespace WordSignatureFactory {

  enum unknown_word_map {Generic,
			 BerkeleyEnglish, BaselineFrench, Arabic,
			 EnglishIG, FrenchIG, ArabicIG,
  ItalianIG};

  unknown_word_map string_2_lex_unknown_map(const std::string& s);

  WordSignature *  create_wordsignature(unknown_word_map unknown_map, bool verbose);
  WordSignature *  create_wordsignature(ConfigTable& config);


}






#endif /* _WORDSIGNATUREFACTORY_H_ */
