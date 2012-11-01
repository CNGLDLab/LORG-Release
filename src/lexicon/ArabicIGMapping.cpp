#include "ArabicIGMapping.h"

std::string get_base(const std::string& txt)
{

  std::string newtxt = "";
  boost::regex re;
  //unsigned i = 0;
  //       std::cout << "test get_base" << ++i << std::endl;
  re.assign("-");
  newtxt = boost::regex_replace(txt,re,"");
  //std::cout << "test get_base" << ++i << std::endl;
  re.assign("^Al\\+");
  newtxt = boost::regex_replace(newtxt,re, "");//noun
  //std::cout << "test get_base" << ++i << std::endl;
  re.assign("\\+AF$");
  newtxt = boost::regex_replace(newtxt,re, ""); //noun
  //std::cout << "test get_base" << ++i << std::endl;
  re.assign("\\+[uiaoNKF]$");
  newtxt = boost::regex_replace(newtxt,re, ""); //both
  //       std::cout << "test get_base" << ++i << std::endl;
  re.assign("\\+At$");
  newtxt = boost::regex_replace(newtxt,re, ""); //noun
  //std::cout << "test get_base" << ++i << std::endl;
  re.assign("\\+atayoni$");
  newtxt = boost::regex_replace(newtxt,re, ""); //noun
  //std::cout << "test get_base" << ++i << std::endl;
  re.assign("\\+atayo$");
  newtxt = boost::regex_replace(newtxt,re, ""); //noun
  //std::cout << "test get_base" << ++i << std::endl;
  re.assign("\\+ayoni$");
  newtxt = boost::regex_replace(newtxt,re, ""); //noun
  //std::cout << "test get_base" << ++i << std::endl;
  re.assign("\\+iyona$");
  newtxt = boost::regex_replace(newtxt,re, ""); //noun
  //std::cout << "test get_base" << ++i << std::endl;
  re.assign("\\+uw$");
  newtxt = boost::regex_replace(newtxt,re, ""); //noun
  //std::cout << "test get_base" << ++i << std::endl;
  re.assign("\\+ap$");
  newtxt = boost::regex_replace(newtxt,re, ""); //noun
  //std::cout << "test get_base" << ++i << std::endl;
  re.assign("\\+iyna$");
  newtxt = boost::regex_replace(newtxt,re, ""); //both
  //std::cout << "test get_base" << ++i << std::endl;
  re.assign("^sa\\+");
  newtxt = boost::regex_replace(newtxt,re, ""); //verb
  //std::cout << "test get_base" << ++i << std::endl;
  re.assign("^(ya|yu|ta|tu|>a|>u|na|nu|sa)\\+");
  newtxt = boost::regex_replace(newtxt,re, ""); //verb
  //std::cout << "test get_base" << ++i << std::endl;
  re.assign("\\+at$");
  newtxt = boost::regex_replace(newtxt,re, ""); //verb
  //std::cout << "test get_base" << ++i << std::endl;
  re.assign("\\+awona$");
  newtxt = boost::regex_replace(newtxt,re, ""); //verb
  //std::cout << "test get_base" << ++i << std::endl;
  re.assign("\\+uwA$");
  newtxt = boost::regex_replace(newtxt,re, ""); //verb
  //std::cout << "test get_base" << ++i << std::endl;
  re.assign("\\+awoA$");
  newtxt = boost::regex_replace(newtxt,re, ""); //verb
  //std::cout << "test get_base" << ++i << std::endl;
  re.assign("\\+ayo$");
  newtxt = boost::regex_replace(newtxt,re, ""); //both
  //std::cout << "test get_base" << ++i << std::endl;
  re.assign("\\+uwna$");
  newtxt = boost::regex_replace(newtxt,re, ""); //both
  //std::cout << "test get_base" << ++i << std::endl;
  re.assign("\\+Ani$");
  newtxt = boost::regex_replace(newtxt,re, ""); //both
  //std::cout << "test get_base" << ++i << std::endl;
  re.assign("\\+A$");
  newtxt = boost::regex_replace(newtxt,re, ""); //both
  //std::cout << "test get_base" << ++i << std::endl;
  re.assign("\\+iy$");
  newtxt = boost::regex_replace(newtxt,re, ""); //both
  //std::cout << "test get_base" << ++i << std::endl;
  re.assign("\\+");
  newtxt = boost::regex_replace(newtxt,re, "");
  //std::cout << "test get_base" << ++i << std::endl;
  re.assign("\\+LRBnullRRB");
  newtxt = boost::regex_replace(newtxt,re, "");
  return newtxt;
}

std::string replace(const std::string & word)
{
  std::string new_word = "";
  boost::regex re;
  //unsigned i = 0;

  //std::cout << "test replace" << ++i << std::endl;
  re.assign("Q");
  new_word = boost::regex_replace(word,re,"\\`");
  //std::cout << "test replace" << ++i << std::endl;
  re.assign("I");
  new_word = boost::regex_replace(new_word,re,"\\|");
  //std::cout << "test replace" << ++i << std::endl;
  re.assign("G");
  new_word = boost::regex_replace(new_word,re,"\\>");
  //std::cout << "test replace" << ++i << std::endl;
  re.assign("J");
  new_word = boost::regex_replace(new_word,re,"\\<");
  //std::cout << "test replace" << ++i << std::endl;
  re.assign("O");
  new_word = boost::regex_replace(new_word,re,"\\&");
  //std::cout << "test replace" << ++i << std::endl;
  re.assign("U");
  new_word = boost::regex_replace(new_word,re,"\\}");
  //std::cout << "test replace" << ++i << std::endl;
  re.assign("V");
  new_word = boost::regex_replace(new_word,re,"\\{");
  //std::cout << "test replace" << ++i << std::endl;
  re.assign("c");
  new_word = boost::regex_replace(new_word,re,"\\*");
  //std::cout << "test replace" << ++i << std::endl;
  re.assign("L");
  new_word = boost::regex_replace(new_word,re,"\\$");
  //std::cout << "test replace" << ++i << std::endl;
  re.assign("W");
  new_word = boost::regex_replace(new_word,re,"\\_");
  //std::cout << "test replace" << ++i << std::endl;
  re.assign("C");
  new_word = boost::regex_replace(new_word,re,"\\~");
  //std::cout << "test replace" << ++i << std::endl;
  re.assign("e");
  new_word = boost::regex_replace(new_word,re,"\\+");
  //std::cout << "test replace" << ++i << std::endl;
  re.assign("B");
  new_word = boost::regex_replace(new_word,re,"\\'");
  return new_word;
}



std::string back(const std::string & word)
{
  std::string new_word = "";
  boost::regex re;
  re.assign("\\`");
  new_word = boost::regex_replace(word,re,"Q");
  re.assign("\\|");
  new_word = boost::regex_replace(new_word,re,"I");
  re.assign("\\>");
  new_word = boost::regex_replace(new_word,re,"G");
  re.assign("\\<");
  new_word = boost::regex_replace(new_word,re,"J");
  re.assign("\\&");
  new_word = boost::regex_replace(new_word,re,"O");
  re.assign("\\}");
  new_word = boost::regex_replace(new_word,re,"U");
  re.assign("\\{");
  new_word = boost::regex_replace(new_word,re,"V");
  re.assign("\\*");
  new_word = boost::regex_replace(new_word,re,"c");
  re.assign("\\$");
  new_word = boost::regex_replace(new_word,re,"L");
  re.assign("\\_");
  new_word = boost::regex_replace(new_word,re,"W");
  re.assign("\\~");
  new_word = boost::regex_replace(new_word,re,"C");
  re.assign("\\+");
  new_word = boost::regex_replace(new_word,re,"e");
  re.assign("\\'");
  new_word = boost::regex_replace(new_word,re,"B");
  return new_word;
}



std::string ArabicIGMapping::get_unknown_mapping(const std::string& word, unsigned /*position*/) const
{
  //  std::cout << "Berk Mapping!! new word: " << word << std::endl;
  std::string word_class = "UNK";
  std::string word_lower_case=to_lower_case(word);
  //    std::cout << word << std::endl;
  //    std::cout << "before replace" << std::endl;
  std::string word_newchar = replace(word);
  //    std::cout << "after replace" << std::endl;
  //    std::cout << word_newchar << std::endl;

  bool hasDigit = word_newchar.find_first_of("0123456789") != std::string::npos;
  //    bool hasDash =  !hasDigit  && word_newchar.find_first_of("-") != std::string::npos;
  //    bool hasLowerCase = false;
  //    int upperCaseCount=0;

  //std::cout << "this string contains a digit, see " << word << std::endl;
  //std::cout << "this string contains a dash, see " << word << std::endl;

  //     for(std::string::const_iterator c = word.begin(); c != word.end(); c++){

  //       if (is_upper_case_letter(*c)){
  //        //Berkeley parser puts hasLower = true here - an error?
  //        ++upperCaseCount;
  //       }else if  (is_lower_case_letter(*c)){
  //        hasLowerCase = true;
  //       }
  //     }

  //  if (upperCaseCount > 0){
  //  std::cout << "original word is " << word << " lower case version is " << word_lower_case << std::endl;
  //}
  //if first character is upper case and it is the first word in sentences and the remaining characters are lower case

  //static SymbolTable* sym_tab_word = SymbolTable::instance_word();

  //deal with capitalisation
  //     if ( is_upper_case_letter(*(word.begin()))){
  //       if (position==0 && upperCaseCount==1){
  //        word_class.append("-INITC");
  //        if (sym_tab_word->token_exists(word_lower_case)){
  //                word_class.append("-KNOWNLC");
  //        }
  //       }else {
  //        word_class.append("-CAPS");
  //       }
  //       //missing an else if here - basically, I'm not dealing with non-letters

  //     }else if (hasLowerCase){
  //       word_class.append("-LC");
  //     }

  if (hasDigit){
    word_class.append("-NUM");
  }
  //     if (hasDash){
  //       word_class.append("-DASH");
  //     }

  unsigned word_length = word_newchar.size();

  if (word_length >= 5 && !hasDigit) {
    // don't do for very short words;
    // Implement common discriminating suffixes

    std::string last_character = word_newchar.substr(word_length-1,1);
    std::string last_two = word_newchar.substr(word_length-2,2);
    std::string last_three = word_newchar.substr(word_length-3,3);

    boost::regex re;
    re.assign("^Al~a\\*i.*$");
    if (boost::regex_match(get_base(word_newchar), re))
      {
        word_class.append("-pref_Al~a*i");
	//      std::cout << "word: " << word_newchar << " word class: " << word_class << std::endl;
        return word_class;
      }
    re.assign(".*a\\*iyna$");
    if (boost::regex_match(get_base(word_newchar), re))
      {
        word_class.append("-suf_a*iyna");
	//      std::cout << "word: " << word_newchar << " word class: " << word_class << std::endl;
        return word_class;
      }
    re.assign(".*niy~$");
    if (boost::regex_match(get_base(word_newchar), re))
      {
	word_class.append("-suf_niy~");
        //      std::cout << "word: " << word_newchar << " word class: " << word_class << std::endl;
	return word_class;
      }
    re.assign(".*liy~$");
    if (boost::regex_match(get_base(word_newchar), re))
      {
	word_class.append("-suf_liy~");
        //      std::cout << "word: " << word_newchar << " word class: " << word_class << std::endl;
	return word_class;
      }
    re.assign(".*biy~$");
    if (boost::regex_match(get_base(word_newchar), re))
      {
	word_class.append("-suf_biy~");
        //      std::cout << "word: " << word_newchar << " word class: " << word_class << std::endl;
	return word_class;
      }
    re.assign(".*riy~$");
    if (boost::regex_match(get_base(word_newchar), re))
      {
	word_class.append("-suf_riy~");
        //      std::cout << "word: " << word_newchar << " word class: " << word_class << std::endl;
	return word_class;
      }
    re.assign(".*miy~$");
    if (boost::regex_match(get_base(word_newchar), re))
      {
	word_class.append("-suf_miy~");
        //      std::cout << "word: " << word_newchar << " word class: " << word_class << std::endl;
	return word_class;
      }
    re.assign(".*kiy~$");
    if (boost::regex_match(get_base(word_newchar), re))
      {
	word_class.append("-suf_kiy~");
        //      std::cout << "word: " << word_newchar << " word class: " << word_class << std::endl;
	return word_class;
      }
    re.assign("^Al~a.*$");
    if (boost::regex_match(get_base(word_newchar), re))
      {
        word_class.append("-pref_Al~a");
	//      std::cout << "word: " << word_newchar << " word class: " << word_class << std::endl;
        return word_class;
      }
    re.assign(".*iyna$");
    if (boost::regex_match(get_base(word_newchar), re))
      {
	word_class.append("-suf_iyna");
        //      std::cout << "word: " << word_newchar << " word class: " << word_class << std::endl;
	return word_class;
      }
    re.assign(".*siy~$");
    if (boost::regex_match(get_base(word_newchar), re))
      {
	word_class.append("-suf_siy~");
        //      std::cout << "word: " << word_newchar << " word class: " << word_class << std::endl;
	return word_class;
      }
    re.assign(".*iy~$");
    if (boost::regex_match(get_base(word_newchar), re))
      {
	word_class.append("-suf_iy~");
        //      std::cout << "word: " << word_newchar << " word class: " << word_class << std::endl;
	return word_class;
      }
    re.assign("^mu.*$");
    if (boost::regex_match(get_base(word_newchar), re))
      {
        word_class.append("-pref_mu");
	//      std::cout << "word: " << word_newchar << " word class: " << word_class << std::endl;
        return word_class;
      }
    re.assign("^>a.*$");

    if (boost::regex_match(get_base(word_newchar), re))
      {
        word_class.append("-pref_>a");
	//      std::cout << "word: " << word_newchar << " word class: " << word_class << std::endl;
        return word_class;
      }
    re.assign("^ta.*$");
    if (boost::regex_match(get_base(word_newchar), re))
      {
        word_class.append("-pref_ta");
	//      std::cout << "word: " << word_newchar << " word class: " << word_class << std::endl;
        return word_class;
      }
    re.assign("^\\{i.*$");
    if (boost::regex_match(get_base(word_newchar), re))
      {
        word_class.append("-pref_{i");
	//      std::cout << "word: " << word_newchar << " word class: " << word_class << std::endl;
        return word_class;
      }
    re.assign("^ma.*$");
    if (boost::regex_match(get_base(word_newchar), re))
      {
        word_class.append("-pref_ma");
	//      std::cout << "word: " << word_newchar << " word class: " << word_class << std::endl;
        return word_class;
      }
    re.assign("^Eo.*$");
    if (boost::regex_match(get_base(word_newchar), re))
      {
        word_class.append("-pref_Eo");
	//      std::cout << "word: " << word_newchar << " word class: " << word_class << std::endl;
        return word_class;
      }
    re.assign("^Al.*$");
    if (boost::regex_match(get_base(word_newchar), re))
      {
        word_class.append("-pref_Al");
	//      std::cout << "word: " << word_newchar << " word class: " << word_class << std::endl;
        return word_class;
      }
    re.assign("^so.*$");
    if (boost::regex_match(get_base(word_newchar), re))
      {
        word_class.append("-pref_so");
	//      std::cout << "word: " << word_newchar << " word class: " << word_class << std::endl;
        return word_class;
      }
    re.assign(".*An$");
    if (boost::regex_match(get_base(word_newchar), re))
      {
	word_class.append("-suf_An");
        //      std::cout << "word: " << word_newchar << " word class: " << word_class << std::endl;
	return word_class;
      }
    re.assign(".*yA$");
    if (boost::regex_match(get_base(word_newchar), re))
      {
	word_class.append("-suf_yA~");
        //      std::cout << "word: " << word_newchar << " word class: " << word_class << std::endl;
	return word_class;
      }
    re.assign(".*na$");
    if (boost::regex_match(get_base(word_newchar), re))
      {
	word_class.append("-suf_na~");
        //      std::cout << "word: " << word_newchar << " word class: " << word_class << std::endl;
	return word_class;
      }
    re.assign("^<i.*$");
    if (boost::regex_match(get_base(word_newchar), re))
      {
        word_class.append("-pref_<i");
	//      std::cout << "word: " << word_newchar << " word class: " << word_class << std::endl;
        return word_class;
      }
    re.assign(".*Ar$");
    if (boost::regex_match(get_base(word_newchar), re))
      {
	word_class.append("-suf_Ar");
        //      std::cout << "word: " << word_newchar << " word class: " << word_class << std::endl;
	return word_class;
      }
    re.assign("^no.*$");
    if (boost::regex_match(get_base(word_newchar), re))
      {
        word_class.append("-pref_no");
	//      std::cout << "word: " << word_newchar << " word class: " << word_class << std::endl;
        return word_class;
      }
    re.assign(".*uw$");
    if (boost::regex_match(get_base(word_newchar), re))
      {
	word_class.append("-suf_uw");
        //      std::cout << "word: " << word_newchar << " word class: " << word_class << std::endl;
	return word_class;
      }

    re.assign(".*A<$");
    if (boost::regex_match(get_base(word_newchar), re))
      {
	word_class.append("-suf_A<");
        //      std::cout << "word: " << word_newchar << " word class: " << word_class << std::endl;
	return word_class;
      }
    //    std::cout << "word: " << word_newchar << " word class: " << word_class << std::endl;
  }
  return word_class;


}
