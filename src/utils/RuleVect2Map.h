// -*- mode: c++ -*-
#ifndef _RULEVECT2MAP_H_
#define _RULEVECT2MAP_H_

#include "rules/BRule.h"
#include "rules/URule.h"
#include "rules/LexicalRule.h"

#include <boost/unordered_map.hpp>


struct common
{
  // Key create_key(const Value& v)
  // {
  //   throw std::runtime_error("use specialisation of create_key");
  // }

  std::pair<int,std::pair<int,int> > create_key(const BRule& rule)
  {
    return std::make_pair(rule.get_lhs(),std::make_pair(rule.get_rhs0(),rule.get_rhs1()));
  }

  std::pair<int,int> create_key(const URule& rule)
  {
    return std::make_pair(rule.get_lhs(),rule.get_rhs0());
  }

  std::pair<int,int> create_key(const LexicalRule& rule)
  {
    return std::make_pair(rule.get_lhs(),rule.get_rhs0());
  }
};

// helper structure: build maps from vectors of rules
// keys are (lhs,rhs0,rhs1) or (lhs,rhs0) depending on the type of rules

template<typename Key, typename Value>
struct rulevect2map : public common
{
  typedef MAP<Key,Value*> map_type;
  map_type& map;

  rulevect2map(map_type& mapt) : map(mapt) {}

  rulevect2map(map_type& mapt, std::vector<Value>& values) : map(mapt)
  {
    add_all(values);
  }

  void operator()(Value& rule)
  {
    map.insert(std::make_pair(create_key(rule),&rule));
  }

  void add_all(std::vector<Value>& values)
  {
    std::for_each(values.begin(), values.end(), *this);
  }

};


template<typename Key, typename Value>
struct rulevect2mapvect : public common
{
  typedef MAP< Key,std::vector<Value*> >map_type;
  map_type& map;

  rulevect2mapvect(map_type& mapt) : map(mapt) {}

  rulevect2mapvect(map_type& mapt, std::vector<Value>& values) : map(mapt)
  {
    add_all(values);
  }

  void operator()(Value& rule)
  {
    Key key = create_key(rule);
    typename map_type::iterator i;
    if((i = map.find(key)) == map.end()) {
      map.insert(std::make_pair(create_key(rule),std::vector<Value*>(1,&rule)));
    }
    else {
      i->second.push_back(&rule);
    }
  }

  void add_all(std::vector<Value>& values)
  {
    std::for_each(values.begin(), values.end(), *this);
  }

};




#endif /* _RULEVECT2MAP_H_ */
