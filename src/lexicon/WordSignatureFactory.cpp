#include "WordSignatureFactory.h"
#include "BerkeleyEnglishUMapping.h"
#include "BaselineFrenchUMapping.h"

#include "EnglishIGMapping.h"
#include "FrenchIGMapping.h"

#include "ArabicUMapping.h"
#include "ArabicIGMapping.h"

#include "ItalianIGMapping.h"


WordSignatureFactory::unknown_word_map
WordSignatureFactory::string_2_lex_unknown_map(const std::string& s)
{
  if(s == "BerkeleyEnglish")
    return BerkeleyEnglish;
  if(s == "BaselineFrench")
    return BaselineFrench;
  if(s == "EnglishIG")
    return EnglishIG;
  if(s == "FrenchIG")
    return FrenchIG;
  if(s == "Arabic")
    return Arabic;
  if(s == "ArabicIG")
    return ArabicIG;

  if(s == "ItalianIG")
    return ItalianIG;

  //if(s == "generic")
  return Generic;
}

WordSignature *  WordSignatureFactory::create_wordsignature(unknown_word_map unknown_map, bool verbose)
{
  switch(unknown_map){

  case BerkeleyEnglish:
    if(verbose) std::clog << "Setting unknown mapping type to BerkeleyEnglish mapping " << std::endl;
    return new BerkeleyEnglishUMapping();
    break; //useless

  case BaselineFrench:
    if(verbose) std::clog << "Setting unknown mapping type to BaselineFrench mapping " << std::endl;
    return new BaselineFrenchUMapping();
    break; //useless

  case EnglishIG:
    if(verbose) std::clog << "Setting unknown mapping type to English IG tests mapping " << std::endl;
    return new EnglishIGMapping();
    break; //useless

  case FrenchIG:
    if(verbose) std::clog << "Setting unknown mapping type to French IG tests mapping " << std::endl;
    return new FrenchIGMapping();
    break; //useless


  case Arabic:
    if(verbose) std::clog << "Setting unknown mapping type to Arabic mapping " << std::endl;
    return new ArabicUMapping();
    break; //useless

  case ArabicIG:
    if(verbose) std::clog << "Setting unknown mapping type to Arabic IG tests mapping " << std::endl;
    return new ArabicIGMapping();
    break; //useless



  case ItalianIG:
    if(verbose) std::clog << "Setting unknown mapping type to Italian IG  mapping " << std::endl;
    return new ItalianIGMapping();
    break; //useless



  default:
    if(verbose) std::clog << "Setting unknown mapping type to trivial mapping (one unknown type only)"
			  << std::endl;
    return new TrivialWordSignature();
  }
}

WordSignature * WordSignatureFactory::create_wordsignature(ConfigTable& conf)
{
  bool verbose = conf.exists("verbose");

  unknown_word_map unknown_word_mapping = string_2_lex_unknown_map(conf.get_value<std::string>("unknown-word-mapping"));

  return create_wordsignature(unknown_word_mapping, verbose);
}
