#include "ArabicUMapping.h"
#include <boost/regex.hpp>

namespace {
std::string get_base(const std::string& txt)
{

  std::string newtxt = "";
  boost::regex re;
  re.assign("-");
  newtxt = boost::regex_replace(txt,re,"");

  re.assign("^Al\\+");
  newtxt = boost::regex_replace(newtxt,re, "");//noun
  re.assign("\\+AF$");
  newtxt = boost::regex_replace(newtxt,re, ""); //noun
  re.assign("\\+[uiaoNKF]$");
  newtxt = boost::regex_replace(newtxt,re, ""); //both
  re.assign("\\+At$");
  newtxt = boost::regex_replace(newtxt,re, ""); //noun
  re.assign("\\+atayoni$");
  newtxt = boost::regex_replace(newtxt,re, ""); //noun
  re.assign("\\+atayo$");
  newtxt = boost::regex_replace(newtxt,re, ""); //noun
  re.assign("\\+ayoni$");
  newtxt = boost::regex_replace(newtxt,re, ""); //noun
  re.assign("\\+iyona$");
  newtxt = boost::regex_replace(newtxt,re, ""); //noun
  re.assign("\\+uw$");
  newtxt = boost::regex_replace(newtxt,re, ""); //noun
  re.assign("\\+ap$");
  newtxt = boost::regex_replace(newtxt,re, ""); //noun
  re.assign("\\+iyna$");
  newtxt = boost::regex_replace(newtxt,re, ""); //both
  re.assign("^sa\\+");
  newtxt = boost::regex_replace(newtxt,re, ""); //verb
  re.assign("^(ya|yu|ta|tu|>a|>u|na|nu|sa)\\+");
  newtxt = boost::regex_replace(newtxt,re, ""); //verb
  re.assign("\\+at$");
  newtxt = boost::regex_replace(newtxt,re, ""); //verb
  re.assign("\\+awona$");
  newtxt = boost::regex_replace(newtxt,re, ""); //verb
  re.assign("\\+uwA$");
  newtxt = boost::regex_replace(newtxt,re, ""); //verb
  re.assign("\\+awoA$");
  newtxt = boost::regex_replace(newtxt,re, ""); //verb
  re.assign("\\+ayo$");
  newtxt = boost::regex_replace(newtxt,re, ""); //both
  re.assign("\\+uwna$");
  newtxt = boost::regex_replace(newtxt,re, ""); //both
  re.assign("\\+Ani$");
  newtxt = boost::regex_replace(newtxt,re, ""); //both
  re.assign("\\+A$");
  newtxt = boost::regex_replace(newtxt,re, ""); //both
  re.assign("\\+iy$");
  newtxt = boost::regex_replace(newtxt,re, ""); //both
  re.assign("\\+");
  newtxt = boost::regex_replace(newtxt,re, "");
  re.assign("\\+LRBnullRRB");
  newtxt = boost::regex_replace(newtxt,re, "");
  return newtxt;
}

std::string replace(const std::string & word)
{
  std::string new_word = "";
  boost::regex re;

  re.assign("Q");
  new_word = boost::regex_replace(word,re,"\\`");

  re.assign("I");
  new_word = boost::regex_replace(new_word,re,"\\|");

  re.assign("G");
  new_word = boost::regex_replace(new_word,re,"\\>");

  re.assign("J");
  new_word = boost::regex_replace(new_word,re,"\\<");

  re.assign("O");
  new_word = boost::regex_replace(new_word,re,"\\&");

  re.assign("U");
  new_word = boost::regex_replace(new_word,re,"\\}");

  re.assign("V");
  new_word = boost::regex_replace(new_word,re,"\\{");

  re.assign("c");
  new_word = boost::regex_replace(new_word,re,"\\*");

  re.assign("L");
  new_word = boost::regex_replace(new_word,re,"\\$");

  re.assign("W");
  new_word = boost::regex_replace(new_word,re,"\\_");

  re.assign("C");
  new_word = boost::regex_replace(new_word,re,"\\~");

  re.assign("e");
  new_word = boost::regex_replace(new_word,re,"\\+");

  re.assign("B");
  new_word = boost::regex_replace(new_word,re,"\\'");
  return new_word;
 }
}



std::string ArabicUMapping::get_unknown_mapping(const std::string& word, unsigned /*position*/) const
{
  std::string word_class = "UNK";
  std::string word_lower_case=to_lower_case(word);

  std::string word_newchar = replace(word);

  bool hasDigit = word_newchar.find_first_of("0123456789") != std::string::npos;

  //Why don't we use initial position information ?

  if (hasDigit){
    word_class.append("-NUM");
  }

  unsigned word_length = word_newchar.size();

  if (word_length >= 5 && !hasDigit) {

    boost::regex re;

    //Nouns
    re.assign("^-?Al\\+.*"); // the definite article al always precedes a noun
    if (boost::regex_match(word_newchar, re))
      {
	word_class.append("-noun_Al");
	return word_class;
      }

    re.assign(".*p.*"); // the taa marbouta only occurs in nouns
    if (boost::regex_match(word_newchar, re))
      {
	word_class.append("-noun_ap");
	return word_class;
      }

    re.assign(".*\\+(N|F|K|AF)$"); // tanween at the end of a word always indicates a noun
    if (boost::regex_match(word_newchar, re))
      {
	word_class.append("-noun_tashkil");
	return word_class;
      }

    re.assign(".*\\+i$"); // kasrah always indicates a noun
    if (boost::regex_match(word_newchar, re))
      {
	word_class.append("-noun_tashkil");
	return word_class;
      }

    re.assign(".*[^+]{3,7}y~\\+.*$"); // yaa al-nasab indicates a noun constained by word length
    if (boost::regex_match(word_newchar, re))
      {
	word_class.append("-noun_yy");
	return word_class;
      }

    re.assign(".*[^+-]{4,7}A'\\+.*$"); // alif mamdoudah followed by hamzah on line indicates a noun constained by word length
    if (boost::regex_match(word_newchar, re))
      {
	word_class.append("-noun_aa");
	return word_class;
      }

    re.assign(".*\\+At.*$"); // feminine plural form indicates nouns
    if (boost::regex_match(word_newchar, re))
      {
	word_class.append("-noun_At");
	return word_class;
      }

    re.assign("\\{isoti.{3}A.+"); // the template {isotifEAl
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_masdar_i");return word_class;
    }

    re.assign("\\{ino.i.A.+"); // the template {inofiEAl
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_masdar_i");return word_class;
    }

    re.assign("\\{i.oti.A.+"); // the template {ifotiEAl
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_masdar_i");return word_class;
    }

    re.assign("\\{it~i.A.+"); // the template {ifotiEAl with shaddah
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_masdar_i");return word_class;
    }

    re.assign("ti.o.A.+"); // the template tifoEAl
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_masdar_t");return word_class;
    }

    re.assign("ta.o.iy.+"); // the template tafoEiyl
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_masdar_t");return word_class;
    }

    re.assign("mi.o.A.+"); // the template mifoEAl
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_instrument");return word_class;
    }

    re.assign("musota.o.i.");  // the template musotafoEil
    if (boost::regex_match(get_base(word_newchar), re)){
      //already covered with mu
      word_class.append("-noun_participle_active");return word_class;
    }

    re.assign("^mu.{5,10}$"); // word starting with mu indicates nouns
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_participle");return word_class;
    }

    re.assign("ma.o.[ia]."); // the template mafoEil and mafoEal
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_general");return word_class;
    }

    re.assign("ma.o.[a|i].An"); // the template mafoE[a|i]lAn
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_general");return word_class;
    }


    //plural forms
    re.assign("ma.A.iy.+"); // the template mafAEiyl
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_plural_m");return word_class;
    }
    //std::cout << "test " << ++i << std::endl;
    re.assign("ma.A.i."); // the template mafaEil
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_plural_m");return word_class;
    }

    re.assign("ma.o.uw."); // the template mafoEuwl
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_participle_passive");return word_class;
    }

    re.assign(".A.i.");  //the template fAEil, we need it avoid the template in the verb imperfective tense
    boost::regex re2;
    re2.assign("^-?(sa|ya|yu|ta|tu|na|nu|>a|>u)\\+.*");
    if (boost::regex_match(get_base(word_newchar), re) && !boost::regex_match(word_newchar, re2)) {
      word_class.append("-noun_participle_active");return word_class;
    }

    re.assign(">a.o.A."); // the template >foEAl
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_plural_a");return word_class;
    }

    re.assign(".awA.iy."); // the template fawAEiyl
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_plural_f");return word_class;
    }

    re.assign(".awA.i."); // the template fawAEil
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_plural_f");return word_class;
    }

    re.assign(">a.A.iy."); // the template >afAEiyl
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_plural_a");return word_class;
    }

    re.assign(".[aiu].o.An"); // the template faEolAn, fiEolAn, fuEolAn
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_plural_f");return word_class;
    }

    re.assign(".a.uw."); // the template faEuwl
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_general");return word_class;
    }

    re.assign(".a.iy."); // the template faEiyl
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_general");return word_class;
    }

    re.assign(">a.o.A."); // the template >afoEAl
    if (boost::regex_match(get_base(word_newchar), re)){
      // It does not return any occurences in this location because they are covered
      // by previous cases
      // if it is moved upfront it covers about 1100 nouns
      word_class.append("-noun_plural_a");return word_class;
    }

    re.assign(".u.uw."); // the template fuEuwl
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_plural_f");return word_class;
    }

    re.assign(".i.A."); // the template fiEAl
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_plural_f");return word_class;
    }

    re.assign(".u.~A."); // the template fuE~Al
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_plural_f");return word_class;
    }

    re.assign("\\|.A."); // the template |EAl such as IarA' ImAl
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_plural_a");return word_class;
    }

    re.assign(".a.A\\}i."); // the template faEA}il
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_plural_f");return word_class;
    }

    re.assign(".ayA.i."); // the template fayAEil
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_plural_f");return word_class;
    }

    re.assign(">a.A.i.");  // the template >afAEil
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_plural_a");return word_class;
    }

    re.assign("ya.A.iy.");  // the template yafAEiyl
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_plural_a");return word_class;
    }

    re.assign(".a.A.iy."); // the template faEAliyl
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_plural_f");return word_class;
    }

    re.assign("ta.A.iy.");  // the template tafAEiyl
    if (boost::regex_match(get_base(word_newchar), re)){
      // It does not return any occurences in this location because they are covered
      // by previous cases
      // if it is moved upfront it covers about 100 nouns
      word_class.append("-noun_plural_t");return word_class;
    }

    re.assign(".a.A.i.");  // the template faEAlil
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_plural_f");return word_class;
    }

    re.assign("mi.o.a."); // the template mifoEal
    //noun of instrument
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_instrument");return word_class;
    }

    re.assign("mi.o.A."); // the template mifoEAl
    //noun of instrument
    if (boost::regex_match(get_base(word_newchar), re)){
      // It does not return any occurences in this location because they are covered
      // by previous cases
      // if it is moved upfront it covers about 70 nouns
      word_class.append("-noun_instrument");return word_class;
    }

    re.assign(".A.uw."); // the template fAEuwl
    //noun of instrument
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_instrument");return word_class;
    }


    re.assign(".u.uw."); // the template fuEuwl
    //masdar
    if (boost::regex_match(get_base(word_newchar), re)){
      // It does not return any occurences in this location because they are covered
      // by previous cases
      // if it is moved upfront it covers about 1500 nouns
      word_class.append("-noun_masdar_f");return word_class;
    }

    re.assign("ta.a.~u."); // the template tafaE~ul
    //masdar
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_masdar_t");return word_class;
    }

    re.assign("ta.o.iy."); // the template tafoEiyl
    //masdar
    if (boost::regex_match(get_base(word_newchar), re)){
      // It does not return any occurences in this location because they are covered
      // by previous cases
      // if it is moved upfront it covers about 2300 nouns
      word_class.append("-noun_masdar_t");return word_class;
    }

    re.assign(".a.a.An"); // the template faEalAn
    //masdar
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_masdar_f");return word_class;
    }

    re.assign(".u.A."); // the template fuEAl
    //masdar
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_masdar_f");return word_class;
    }

    re.assign(".a.iy."); // the template faEiyl
    //masdar
    if (boost::regex_match(get_base(word_newchar), re)){
      // It does not return any occurences in this location because they are covered
      // by previous cases
      // if it is moved upfront it covers about 1000 nouns
      word_class.append("-noun_masdar_f");return word_class;
    }

    re.assign("<i.o.A."); // the template <ifoEAl
    //masdar
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_masdar_i");return word_class;
    }

    re.assign("ta.A.u."); // the template tafAEul
    //masdar
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-noun_masdar_t");return word_class;
    }

    boost::regex re3;
    re.assign(".a.o.");
    re2.assign(".*\\+(at|uwA|awoA)-?$");
    re3.assign(".*layos.*");
    //masdar
    // the template faEol
    // exclude verbs ending in feminine marker, and plural subjunctive
    if (boost::regex_match(get_base(word_newchar), re) && ! boost::regex_match(word_newchar,re2) && ! boost::regex_match(word_newchar,re3)) {
      // it adds about 1500 nouns
      word_class.append("-noun_masdar_f");return word_class;
    }


    //Verbs
    re.assign("^-?(ya|yu|ta|tu|>a|>u|na|nu|sa)\\+.*"); // imperfective prefixes
    if (boost::regex_match(word_newchar, re))
      {
	word_class.append("-verb_present");
	return word_class;
      }

    re.assign(".*\\+(at|awona|uwA|awoA)$");  // verb suffixes
    if (boost::regex_match(word_newchar, re))
      {
	word_class.append("-verb_suffixes");
	return word_class;
      }

    re.assign("\\{isota.o.a."); // the template {isotafoEal
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-verb_is");return word_class;;
    }

    re.assign("\\{isota.a.~"); // the template {isotafal~
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-verb_is");return word_class;;
    }

    re.assign("\\{ino.a.a."); // the template {inofaEal
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-verb_in");return word_class;;
    }

    re.assign("\\{ino.a.~"); // the template {inofal~
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-verb_in");return word_class;;
    }

    re.assign("\\{i.ota.a."); // the template {ifotaEal
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-verb_if");return word_class;;
    }

    re.assign("\\{i.ota.~"); // the template {ifotal~
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-verb_if");return word_class;;
    }

    re.assign("ta.A.a."); // the template tafAEal
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-verb_ta");return word_class;;
    }

    re.assign("ta.a.~a."); // the template tafaE~al
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-verb_ta");return word_class;;
    }

    re.assign(".A.a."); // the template fAEal
    re2.assign(".*(EAlam|gAyap|xAtam|qAlab).*");
    // this templates gets four irregular nouns: EAlam, gAyap, xAtam and qAlab
    if (boost::regex_match(get_base(word_newchar), re) && ! boost::regex_match(word_newchar, re2)) {
      word_class.append("-verb_fa");return word_class;;
    }

    re.assign(".a.~a.");
    re2.assign(".*>aw~al.*"); // the template faE~al
    // this templates gets one irregular noun: >aw~al
    if (boost::regex_match(get_base(word_newchar), re) && ! boost::regex_match(word_newchar, re2)) {
      word_class.append("-verb_fa");return word_class;;
    }

    re.assign("\\{i.o.awo.a."); // the template {ifoEawoEal
    //rare
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-verb_if");return word_class;;
    }

    re.assign("ta.a.o.a."); // the template tafaEolal
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-verb_ta");return word_class;;
    }

    re.assign("\\{i.o.a.a.~"); // the template {ifoEalal~
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-verb_if");return word_class;;
    }

    re.assign(".u.i."); // the template fuEil
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-verb_passive");return word_class;;
    }

    re.assign(".uw.i."); // the template fuwEil
    re2.assign("mu.*");
    if (boost::regex_match(get_base(word_newchar), re) && !boost::regex_match(get_base(word_newchar), re2)) {
      word_class.append("-verb_passive");return word_class;;
    }

    re.assign("\\{it~a.a."); // the template {it~aEal
    if (boost::regex_match(get_base(word_newchar), re)){
      word_class.append("-verb_if");return word_class;;
    }
  }

  return word_class;

}
