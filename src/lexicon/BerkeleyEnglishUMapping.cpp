#include "BerkeleyEnglishUMapping.h"

std::string BerkeleyEnglishUMapping::get_unknown_mapping(const std::string& word, unsigned position) const
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
    std::string last_two = word_lower_case.substr(word_length-2,2);
    std::string last_three = word_lower_case.substr(word_length-3,3);
	  
    if ("ed" == last_two) {
      word_class.append("-ed");
    } else if ("ing" == last_three) {
      word_class.append("-ing");
    } else if ("ion" == last_three) {
      word_class.append("-ion");
    } else if ("er" == last_two) {
      word_class.append("-er");
    } else if ("est" == last_three) {
      word_class.append("-est");
    } else if ("ly" == last_two) {
      word_class.append("-ly");
    } else if ("ity" == last_three) {
      word_class.append("-ity");
    } else if ("y" == last_character) {
      word_class.append("-y");
    } else if ("al" == last_two) {
      word_class.append("-al");
    }
	  
  }
	
      
    
  //std::cout << "word class " << word_class << std::endl;
    
  return word_class;
    
}
