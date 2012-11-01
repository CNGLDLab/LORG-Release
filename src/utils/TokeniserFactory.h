#ifndef TOKENISERFACTORY_H
#define TOKENISERFACTORY_H

#include "utils/Tokeniser.h"

class TokeniserFactory {
 public:
  enum TokeniserType {English};

  static Tokeniser * create_tokeniser(TokeniserType type,
				      bool remove_punct = false,
				      TokMode mode = Raw,
				      const std::string& tokens_to_remove = ". ; : \" ? ! `` , '' -- - ` ...",
				      const std::string& delim = " ",
                                      char comment_char = '#');
};

#endif //TOKENISERFACTORY_H
