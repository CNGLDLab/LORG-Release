// -*- mode: c++ -*-
#ifndef TOKENISER_H
#define TOKENISER_H

#include "TokeniserSpec.h"

#include <boost/unordered_set.hpp>
#include <boost/tokenizer.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <memory>

#include <Bracketing.h>


class MissingTagException {};


enum TokMode {Raw, Tok, Tag, Lat};

class Tokeniser
{
public:
  Tokeniser( TokeniserSpec * spec,
	     bool remove_punct = false,
	     TokMode mode = Raw,
	     const std::string& tokens_to_remove = ". ; : \" ? ! `` , '' -- - ` ...",
	     const std::string& delim = " ",
             char comment_char = '#');

  //is this ever used ? (previous ctor has default parameters)
  Tokeniser( TokeniserSpec* spec);

  ~Tokeniser();

  void set_options(bool remove_punct = false,
		   TokMode mode = Raw,
		   const std::string& tokens_to_remove = ". ; : \" ? ! `` , '' -- - ` ...",
		   const std::string& delim = " ",
                   char comment_char = '#');

  void tokenise( const std::string& sentence, std::vector< Word >& result, std::vector< bracketing >& brackets);

  void tokenise_lat(std::string& raw_string_first_line, std::istream& input, std::vector<Word>& next_sentence, std::vector< bracketing >& brackets);

  bool tokenise( std::istream& input, std::string& raw_string, std::vector< Word>& next_sentence, std::vector< bracketing >& brackets, std::vector<std::string>& comments);

  static std::string tokmode_to_string(TokMode mode);
  static TokMode string_to_tokmode(const std::string& s);

private:
  void remove_punctuation( std::vector<Word>& sentence );
  void tokenise_raw(const std::string& sentence, std::vector<Word>& result);

  template <typename helper>
  void tokenise_toktag(const std::string& sentence, std::vector<Word>& result, std::vector<bracketing>& brackets);

  Word* tokenise_lattice_line(const std::string& line);


private:
  std::auto_ptr< TokeniserSpec > m_spec;
  bool m_remove_punct;
  TokMode m_mode;
  std::string m_delim;
  char m_comment_char;
  boost::unordered_set<std::string> m_unwanted_chars;
  std::ifstream* m_istream;

  Tokeniser(const Tokeniser&);
  Tokeniser& operator=(const Tokeniser&);

};

#endif // TOKENISER_H
