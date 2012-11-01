#include "EnglishIGMapping.h"

std::string EnglishIGMapping::get_unknown_mapping(const std::string& word, unsigned position) const
{
  //	std::cout << "Berk Mapping!! new word: " << word << std::endl;
  std::string word_class("UNK");
  std::string word_lower_case=to_lower_case(word);
    
  bool hasDigit = word.find_first_of("0123456789") != std::string::npos;
  bool hasDash =  word.find_first_of("-") != std::string::npos;
  bool hasLowerCase = false;
  int upperCaseCount=0;
    
  //std::cout << "this string contains a digit, see " << word << std::endl;
  //std::cout << "this string contains a dash, see " << word << std::endl;

  for(std::string::const_iterator c = word.begin(); c != word.end(); c++){
      
    if (is_upper_case_letter(*c)){
      //Berkeley parser puts hasLower = true here - an error?
      ++upperCaseCount;
    }else if  (is_lower_case_letter(*c)){
      hasLowerCase = true;
    }
  }
    
  //	if (upperCaseCount > 0){
  //	std::cout << "original word is " << word << " lower case version is " << word_lower_case << std::endl;
  //}
  //if first character is upper case and it is the first word in sentences and the remaining characters are lower case
    
  static SymbolTable& sym_tab_word = SymbolTable::instance_word();
    
  //deal with capitalisation
  if ( is_upper_case_letter(*(word.begin()))){
    if (position==0 && upperCaseCount==1){
      word_class.append("-INITC");
      if (sym_tab_word.token_exists(word_lower_case)){
	word_class.append("-KNOWNLC");
      }
    }else {
      word_class.append("-CAPS");
    }
    //missing an else if here - basically, I'm not dealing with non-letters
  }else if (hasLowerCase){
    word_class.append("-LC");
  }
    
  if (hasDigit){
    word_class.append("-NUM");
  }
  if (hasDash){
    word_class.append("-DASH");
  }	
    
  unsigned word_length = word_lower_case.size(); 
  std::string last_character = word_lower_case.substr(word_length-1,1);
  if ( (word_length >=3) && (last_character.compare("s")==0 ) )
    {
      //	std::cout << "word is " << word_in_lc << std::endl;
      std::string secondLast = word_lower_case.substr(word_length-2,1);
      //    std::cout  << "second last is " << secondLast << std::endl;
      if ( secondLast.find_first_of("siu") == std::string::npos) {//if second last not equal to s, nor i nor u
	word_class.append("-s"); 
      }
      ///
    }
  else if (word_length >= 5 && !hasDash && !(hasDigit && upperCaseCount > 0)) {
    // don't do for very short words;
    // Implement common discriminating suffixes
    std::string last_one = word_lower_case.substr(word_length-1,1);
    std::string last_two = word_lower_case.substr(word_length-2,2);
    std::string last_three = word_lower_case.substr(word_length-3,3);
	  

    /* Gain Ratio Ones */

    if ("ing" == last_three) {
      word_class.append("-ing");
    }else if ("ted" == last_three) {
      word_class.append("-ted");
    }else if ("ika" == last_three) {
      word_class.append("-ika");
    }else if ("lly" == last_three) {
      word_class.append("-lly");
    }else if ("est" == last_three) {
      word_class.append("-est");
    }else if ("ned" == last_three) {
      word_class.append("-ned");
    }else if ("ink" == last_three) {
      word_class.append("-ink");
    }else if ("ded" == last_three) {
      word_class.append("-ded");
    }else if ("nik" == last_three) {
      word_class.append("-nik");
    }else if ("rma" == last_three) {
      word_class.append("-rma");
    }else if ("ely" == last_three) {
      word_class.append("-ely");
    }else if ("sly" == last_three) {
      word_class.append("-sly");
    }else if ("gly" == last_three) {
      word_class.append("-gly");
    }else if ("nly" == last_three) {
      word_class.append("-nly");
    }else if ("ify" == last_three) {
      word_class.append("-ify");
    }else if ("rys" == last_three) {
      word_class.append("-rys");
    }else if ("jis" == last_three) {
      word_class.append("-jis");
    }else if ("ier" == last_three) {
      word_class.append("-ier");
    }else if ("est" == last_three) {
      word_class.append("-est");
    }else if ("tly" == last_three) {
      word_class.append("-tly");
    } else if ("ly" == last_two) {
      word_class.append("-ly");
    } else if ("ed" == last_two) {
      word_class.append("-ed");
    } else if ("es" == last_two) {
      word_class.append("-es");
    } else if ("ng" == last_two) {
      word_class.append("-ng");
    }  else if ("d" == last_one) {
      word_class.append("-d");
    } else if ("g" == last_one) {
      word_class.append("-g");
    }
  }
	
      
    
  //std::cout << "word class " << word_class << std::endl;
    
  return word_class;
    
}
