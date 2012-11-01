// -*- mode: c++ -*-
#ifndef _PARSERCKY_H_
#define _PARSERCKY_H_

#include "grammars/compact_binary_rules.h"

namespace {
  //////////useful functions ///////////////////
  template<class T>
  inline
  bool exists(const std::vector<T>& v, T elem)
  {
    return find(v.begin(),v.end(),elem) != v.end();
  }

  template<class T>
  inline
  bool add_ifabsent(std::vector<T>& v, T elem)
  {
    if(!exists(v,elem)) {
      v.push_back(elem);
      return true;
    }
    return false;
  }
  ///////////////////////////////////////////////
}


template<class MyGrammar>
class ParserCKY
{
public:
  typedef typename MyGrammar::MyBinaryRule Bin;
  typedef typename MyGrammar::MyUnaryRule Un;
  typedef typename MyGrammar::MyLexicalRule Lex;

  typedef compact_binary_rules::vector_brules<const Bin *> vector_brules;
  typedef compact_binary_rules::vector_rhs0<const Bin *> vector_rhs0;
  typedef compact_binary_rules::vector_rhs1<const Bin *> vector_rhs1;


  ParserCKY( MyGrammar* grammar);
  virtual ~ParserCKY();


  /**
     \brief returns the "word_id->lexical rules" map
  */
  const std::vector< std::vector<const MetaProduction *> >&
  get_words_to_rules() const {return words_to_rules;}

  /**
     \brief returns the total number of non-terminals in the grammar
  */
  unsigned get_nonterm_count() const {return n_nonterminals;}


protected:
  bool
  rules_for_unary_exist(int rhs_id) const {return !unary_rhs_2_rules[rhs_id].empty();}

  void remove_lex_rule(Lex* l);

protected:
  Grammar<Bin,Un,Lex> * grammar; ///< the grammar

  vector_brules * brules; //// the structure used to access binary rules
  std::vector< std::vector<const Un*> >  unary_rhs_2_rules; // fast access unary rules from rhs


  std::vector<short> unary_rhs_from_binary; /// lhs of binary rules which are also rhs of unary rules
  std::vector<short> unary_rhs_from_pos;  /// pos tags which are also rhs of unary rules

  ///< retrieval of all rules associated with a word
  ///<(i.e. where RHS = word )
  ///< index of vector is word_id
  std::vector< std::vector<const MetaProduction*> > words_to_rules;

  unsigned n_nonterminals;

private:
  /**
     \brief add a lexical rule to the grammar and
     add the lhs of the rule to the vector of pos tags
     \param lrule the lexical rule to add
  */
  void add_lex_rule(const Lex& lrule);

    /**
     \brief add a unary rule to the grammar and update helping vectors
     \param urule the unary rule to add
     \param bin_lhs_vect a vector containing the lhs'es of binary rules
     \param pos_vect a vector containing pos tags
  */
  void add_unary_rule(const Un & urule, const std::vector<short>& bin_lhs_vect, const std::vector<short>& pos_vect);
};

template <typename MyGrammar>
ParserCKY<MyGrammar>::ParserCKY(MyGrammar* g) :
  grammar(g),
  brules(NULL), unary_rhs_2_rules(),
  unary_rhs_from_binary(), unary_rhs_from_pos(),
  n_nonterminals(0)
{
  grammar->init();

  std::vector<short> bin_lhs; //lhs'es in binary rules
  std::vector<short> lex_lhs; //lhs'es in lexical rules
  std::vector<short> nts; // all the non-terminals in rules

  //collect lex_lhs nts
  for(typename std::vector<Lex>::const_iterator it(grammar->lexical_rules.begin());
      it != grammar->lexical_rules.end(); ++it) {
    add_lex_rule(*it);
    add_ifabsent(lex_lhs,it->get_lhs());
    add_ifabsent(nts,it->get_lhs());
  }

  //collect bin_lhs nts
  for(typename std::vector<Bin>::const_iterator it(grammar->binary_rules.begin());
      it != grammar->binary_rules.end(); ++it) {
    add_ifabsent(bin_lhs, it->get_lhs());
    add_ifabsent(nts,it->get_lhs());
  }

  //collect nts and unary rules
  for(typename std::vector<Un>::const_iterator it(grammar->unary_rules.begin());
      it != grammar->unary_rules.end();++it) {
    add_unary_rule(*it,bin_lhs,lex_lhs);
    add_ifabsent(nts,it->get_lhs());
  }

  //set attribute nt_count
  n_nonterminals = nts.size();

  // compact binary rules
  brules = compact_binary_rules::vector_brules<const Bin*>::convert(grammar->binary_rules);
}

template <typename MyGrammar>
ParserCKY<MyGrammar>::~ParserCKY()
{
  delete brules;
  brules = NULL;
  delete grammar;
  grammar = NULL;
}

template<typename MyGrammar>
void ParserCKY<MyGrammar>::add_lex_rule(const Lex& r)
{
  int word_id = r.get_rhs0();

  assert(word_id >= 0);

  if(word_id >= (int) words_to_rules.size()) { // new word
    words_to_rules.resize(word_id+1);
  }

  words_to_rules[word_id].push_back(&r);
  //std::cerr << "adding a lexical rule for " << word_id << std::endl;
}

template<class MyGrammar>
void ParserCKY<MyGrammar>::remove_lex_rule(Lex* r)
{
  assert(r != NULL);
  int word_id = r->get_rhs0();

  assert(word_id >= 0);
  words_to_rules[word_id].erase(std::remove(words_to_rules[word_id].begin(),
					    words_to_rules[word_id].end(),r),
				words_to_rules[word_id].end());
}


template<class MyGrammar>
void ParserCKY<MyGrammar>::add_unary_rule(const Un& r,
					  const std::vector<short>& blhs,
					  const std::vector<short>& llhs)
{
  short rhs_id = short(r.get_rhs0());

  if(rhs_id >= (int) unary_rhs_2_rules.size())
    unary_rhs_2_rules.resize(rhs_id+1);

  unary_rhs_2_rules[rhs_id].push_back(&r);

  if(exists(blhs,rhs_id) &&(!exists(unary_rhs_from_binary,rhs_id)))
    unary_rhs_from_binary.push_back(rhs_id);

  if(exists(llhs,rhs_id) && (!exists(unary_rhs_from_pos,rhs_id)))
    unary_rhs_from_pos.push_back(rhs_id);
}


#endif /* _PARSERCKY_H_ */
