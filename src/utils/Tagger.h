// -*- mode: c++ -*-
#ifndef TAGGER_H
#define TAGGER_H

#include "../Word.h"
#include "SymbolTable.h"

#include <vector>
#include <boost/regex.hpp>

class Tagger
{
public:
  Tagger(const std::vector< std::vector<const MetaProduction*> >* word_2_rule = NULL,
	 bool replace_number = false,
	 const std::string& num_replace_regex = "^\\d+([,\\.]\\d+)?(,\\d+)?");

  void tag( std::vector< Word >& sentence ) const;
  void tag( Word& w ) const;
  void replace_number(Word& word) const;


  void set_word_rules(const std::vector< std::vector<const MetaProduction*> >* word_2_rule);

private:
  const std::vector< std::vector<const MetaProduction*> >* word_2_rule_;
  bool replace_number_;
  boost::regex num_replace_regex_;
};

#endif // TAGGER_H
