#include "Word.h"

#include <boost/foreach.hpp>

WordSignature* Word::wordsignature = NULL;

Word::Word( const std::string& str, int string_start, int string_end, const std::vector<std::string>& strtags)
  : form(str), start(string_start), end(string_end), id(-1), sigid(-1), tags(), rules()
{
  //  std::cout << "word is " << form << " and position is " << string_start << std::endl;

  if (end == -1)
    {
      end = start + 1;
    }


  for(std::vector<std::string>::const_iterator i(strtags.begin()); i != strtags.end(); ++i) {
    // fix "unknown tag" bug
    // if tag doesn't exist, then do tagging
    try {
      tags.push_back(SymbolTable::instance_nt().get(*i));
    }
    catch(Miss& m){
      //      std::cerr << "Here 1" << std::endl;
      std::cerr << m.what() << std::endl;
    }
  }

  if(is_tagged()) {
    try {
      sigid = SymbolTable::instance_word().get(wordsignature->get_unknown_mapping(form,start));
    }
    catch(Miss& m) {
      //   std::cerr << "Here 2" << std::endl;
      //      std::cerr << m.what() << std::endl;
      sigid = -1;
    }
  }

  try
  {
    //look up the word in the symbol table
    id = SymbolTable::instance_word().get(str);
  }
  catch(Miss&)
  {
    //see what the id corresponding to the unknown token is
    std::string signature = wordsignature->get_unknown_mapping(form,start);
    try
    {
      id = SymbolTable::instance_word().get(signature);
    }
    catch(Miss& m) {
      std::cerr << m.what() << std::endl;
      // try {
      //   id = SymbolTable::instance_word().get(signature.substr(0, signature.find_last_of("-")));
      // }
      // catch(Miss& m) {
      //   std::cerr << m.what() << std::endl;
        id = -1;
        //      }
    }
  }
}


std::ostream& operator<<(std::ostream& out, const Word& word)
{
  out << word.form ;

  return out ;
}
