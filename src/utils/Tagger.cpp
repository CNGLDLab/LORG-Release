#include "Tagger.h"
#include "rules/MetaProduction.h"

#include <algorithm>


Tagger::Tagger(const std::vector< std::vector<const MetaProduction*> >* word_2_rule,
	       bool replace_number,const std::string& num_replace_regex) :
  word_2_rule_(word_2_rule), replace_number_(replace_number), num_replace_regex_(num_replace_regex)
{}


void Tagger::set_word_rules(const std::vector< std::vector<const MetaProduction*> >* word_2_rule)
{
  word_2_rule_ = word_2_rule;
}


// replace any number by a uniform sign
void Tagger::replace_number(Word& word) const
{
  static int number_id =  SymbolTable::instance_word().insert(LorgConstants::token_number);

  if(word.form[0] > 47 && word.form[0] < 58) { // to perform replacement faster
    boost::cmatch matched;
    if(boost::regex_match(word.form.c_str(), matched, num_replace_regex_))
      word.id = number_id;
  }
}

void Tagger::tag(Word& word) const
{
  typedef std::vector<const MetaProduction*>::const_iterator const_iterator;
  // only do this once ...
  static int unknown_id = SymbolTable::instance_word().insert(LorgConstants::token_unknown);
  static const std::vector<const MetaProduction*>& unknown_tags =  (*word_2_rule_)[unknown_id];

  if(word.is_tagged()) {

    // for all the predicted tags for this word
    for (unsigned i = 0; i < word.tags.size(); ++i) {

      int given_tag = word.get_given_tag(i);
      bool found = false;
      if(word.id != -1)
        // we try to find the first rule with the given tag in lhs position
        for(const_iterator r_iter = (*word_2_rule_)[word.id].begin(); r_iter != (*word_2_rule_)[word.id].end(); ++r_iter) {
          if((*r_iter)->get_lhs() == given_tag) {
            word.rules.push_back(*r_iter);
            found = true;
            break;
          }
        }

      if(!found && word.id != -1 && word.sigid != word.id && word.sigid != -1) {

        for(const_iterator r_iter = (*word_2_rule_)[word.sigid].begin(); r_iter != (*word_2_rule_)[word.sigid].end(); ++r_iter) {
          if((*r_iter)->get_lhs() == given_tag) {
            word.rules.push_back(*r_iter);
            found = true;
            break;
          }
        }


      }

      //if we couldn't find TAG -> word (or signature)
      //try to look for TAG -> UNKNOWN
      if(!found) {

        for(const_iterator r_iter = (*word_2_rule_)[unknown_id].begin(); r_iter != (*word_2_rule_)[unknown_id].end(); ++r_iter) {
          if((*r_iter)->get_lhs() == given_tag) {
            word.rules.push_back(*r_iter);
            found = true;
            break;
          }
        }

      }
      //maybe throw an exception here ...
      if(!found) {
        std::cerr << "Could not find assigned tag "
                  <<  SymbolTable::instance_nt().translate(word.tags[i])
                  << " for word " <<  word.form << std::endl;
        //HACK
        if(word.id != -1)
          word.rules= (*word_2_rule_)[word.id];
        else
          word.rules= unknown_tags;
      }
    }
  }

  else {
    //read tags from grammar
    //find all lexical rules with this word as its rhs
    //    std::cout << word.id << " : "  << word.form << std::endl;
    //    std::cout << word_2_rule_.size() << std::endl;
    if(word.id != -1)
      word.rules= (*word_2_rule_)[word.id];
    else
      word.rules= unknown_tags;

    //    std::cout << "nb tags :" << word.rules.size() << std::endl;
  }

  if(word.rules.empty()) {
    std::cerr << word.form << std::endl;
    throw std::logic_error("Can't tag word. Have you set unknown cutoff to zero ?");
  }
}


struct tag_helper
{
  const Tagger& tagger;
  tag_helper(const Tagger& t) : tagger(t){}
  void operator()(Word& w) {tagger.tag(w);}
};

struct replace_number_helper
{
  const Tagger& tagger;
  replace_number_helper(const Tagger& t) : tagger(t){}
  void operator()(Word& w) {tagger.replace_number(w);}
};

void Tagger::tag( std::vector< Word >& sentence ) const
{
  if(replace_number_)
    std::for_each(sentence.begin(), sentence.end(), replace_number_helper (*this));

  std::for_each(sentence.begin(), sentence.end(), tag_helper (*this));
}
