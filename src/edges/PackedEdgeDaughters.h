// -*- mode: c++ -*-
#ifndef PACKEDEDGEDAUGHTERS_H
#define PACKEDEDGEDAUGHTERS_H

#include "rules/BRuleC2f.h"
#include "rules/URuleC2f.h"
#include "rules/LexicalRuleC2f.h"

class Word;
//class PCKYAllCell;

/**
   \class RuleHolder
   \brief a class with an attribute rule
 */
template<class MyRule>
class RuleHolder
{
public:
  typedef MyRule ruletype;
protected:
  const MyRule * rule;
public:
  typedef MyRule rule_type;
  RuleHolder(const MyRule * r = 0) : rule(r) {};
  ~RuleHolder() {};
  inline const MyRule * get_rule() const {return rule;}
  inline void set_rule(const MyRule * r) {rule = r;}
};


/**
   \class PackedEdgeDaughters
   \brief represents a branching of possible daughters
*/

class PackedEdgeDaughters
{

public:
  PackedEdgeDaughters()  {};

  virtual bool is_binary() const =0;
  virtual bool is_lexical() const =0;
  ~PackedEdgeDaughters() {};
};


/**
   \class BinaryPackedEdgeDaughters
   \brief represents a binary branching of possible daughters + a binary rule
*/
template<class Cell>
class BinaryPackedEdgeDaughters : public PackedEdgeDaughters, public RuleHolder<BRuleC2f>
{
  Cell * left;
  Cell * right;
public:
  BinaryPackedEdgeDaughters(Cell *le, Cell *ri, const BRuleC2f * ru) :
    PackedEdgeDaughters(), RuleHolder<BRuleC2f>(ru), left(le),right(ri)
  {};

  ~BinaryPackedEdgeDaughters() {};

  inline bool is_binary() const {return true;}
  inline bool is_lexical() const {return false;}

  inline Cell * left_daughter() const  {return left;}
  inline Cell * right_daughter() const {return right;}

  inline bool operator==(const BinaryPackedEdgeDaughters& other)
  {
    return rule == other.rule && left == other.left && right ==other.right;
  }

};


/**
   \class UnaryPackedEdgeDaughters
   \brief represents a unary branching (!) + a unary rule
*/
template<class Cell>
class UnaryPackedEdgeDaughters : public PackedEdgeDaughters, public RuleHolder<URuleC2f>
{
  Cell * left;

public:
  UnaryPackedEdgeDaughters(Cell *le, const URuleC2f * ru) :
    PackedEdgeDaughters(), RuleHolder<URuleC2f>(ru), left(le)
  {};

  ~UnaryPackedEdgeDaughters() {};

  inline bool is_binary() const {return false;}
  inline bool is_lexical() const {return false;}
  inline Cell * left_daughter() const  {return left;}
};


/**
   \class LexicalPackedEdgeDaughters
   \brief represents a zero-ary branching (!) + a lexical rule
*/
class LexicalPackedEdgeDaughters : public PackedEdgeDaughters, public RuleHolder<LexicalRuleC2f>
{
  const Word* word;

  double relaxation;

public:
  LexicalPackedEdgeDaughters(const LexicalRuleC2f * ru, const Word* w) :
    PackedEdgeDaughters(), RuleHolder<LexicalRuleC2f>(ru), word(w),
    relaxation(0)
  {};

  ~LexicalPackedEdgeDaughters() {};

  inline bool is_binary() const {return false;}
  inline bool is_lexical() const {return true;}
  inline const Word* get_word() const {return word;}
};


#endif //PACKEDEDGEDAUGHTERS
