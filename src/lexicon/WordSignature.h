// -*- mode: c++ -*-
#ifndef WORDSIGNATURE_H
#define WORDSIGNATURE_H

#include "utils/SymbolTable.h"
#include <string>
#include <sstream>

class WordSignature{

protected:
  static bool is_upper_case_letter(char c){ return ((c >= 'A') && (c <= 'Z'));}
  
  static bool is_lower_case_letter(char c){ return ((c >= 'a') && (c <= 'z'));}
  
  static std::string to_lower_case(const std::string& word)
  {
    std::string new_word(word);
    for(std::string::iterator c = new_word.begin(); c != new_word.end(); ++c){
      *c = tolower(*c);
    }
    //std::cout << new_word << std::endl;
    return new_word;
  }
  
public: 
  virtual std::string get_unknown_mapping(const std::string& word, unsigned position) const =0;
  virtual ~WordSignature() {};
};


class TrivialWordSignature : public WordSignature 
{
public:
  std::string get_unknown_mapping(const std::string& /*word*/, unsigned /*position*/) const {
	//  std::cout << "where is this being called? " << std::endl;
	 // std::exit(1);
	  return SymbolTable::unknown_string;
  };
};

#endif // WORDSIGNATURE_H
