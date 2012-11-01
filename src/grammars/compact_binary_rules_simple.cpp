
/// template specialisations

#include "compact_binary_rules.cpp"
#include "rules/Rule.h"

template<>
const Rule* transform(const Rule& rule) 
{
  return &rule;
}


template 
compact_binary_rules::vector_brules<const Rule*> * 
compact_binary_rules::vector_brules<const Rule*>::convert<Rule>(std::vector<Rule, std::allocator<Rule> > const&);
