#include "TokeniserFactory.h"
#include "utils/EnglishSpec.h"

Tokeniser * TokeniserFactory::create_tokeniser(TokeniserType type,
					       bool remove_punct,
					       TokMode mode,
					       const std::string& tokens_to_remove,
					       const std::string& delim,
                                               char comment_char)
{
  if(type == English)
    return new Tokeniser(new EnglishSpec(), remove_punct, mode, tokens_to_remove, delim, comment_char);

  //beware !!
  return NULL;
}

