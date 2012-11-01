// -*- mode: c++ -*-
#ifndef WORD_H_
#define WORD_H_

#include <vector>
#include <iostream>
#include "utils/SymbolTable.h"
#include "lexicon/WordSignature.h"

class MetaProduction;

/**
  \class Word
  \brief represents a word in a sentence
  \author Wolfgang Seeker
*/
class Word
{
  friend class Tagger;
  friend class Tokeniser;

public:
  friend std::ostream& operator<<(std::ostream& out, const Word& word);

public:

  /**
     \brief Constructor
     \param str the given word form
     \param pos the assigned pos tags
  */
  //  Word(const std::string& str, int string_position, const std::vector<std::string>& pos  = std::vector<std::string>());
  Word(const std::string& str, int start_idx, int end_idx = -1, const std::vector<std::string>& pos  = std::vector<std::string>());

  bool is_tagged() const;

  short get_given_tag(unsigned i) const;

  int get_id() const;

  const std::string& get_form() const {return form;};

  const std::vector<const MetaProduction*>& get_rules() const;

  int get_start() const;
  int get_end() const;


public:
  static void init_lexicon_type(WordSignature* ws){wordsignature = ws;};

protected:
  static WordSignature* wordsignature; ///< use this ws to get the unknown word class/signature

  std::string form;                         ///< the actual word form

  int start;
  int end;

  int id;                                          ///< the assigned id
  int sigid;
  std::vector<short> tags;
  std::vector<const MetaProduction*> rules;   ///< the possible lexical rules with this word as rhs

private:
  Word();
};



inline
bool Word::is_tagged() const
{
  return !tags.empty();
}

inline
short Word::get_given_tag(unsigned i) const
{
  assert(i < tags.size());
  return tags[i];
}

inline
const std::vector<const MetaProduction*>& Word::get_rules() const
{
  return rules;
}

inline
int Word::get_id() const
{
  return id;
}

inline
int Word::get_start() const
{
  return start;
}

inline
int Word::get_end() const
{
  return end;
}

#endif // WORD_H_
