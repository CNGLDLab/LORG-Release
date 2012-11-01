
#include "compact_binary_rules.cpp"
///////////////////////////////////////
#include "rules/BRuleC2f.h"

template<>
const BRuleC2f * transform(const BRuleC2f& rule)
{
  return &rule;
}

template 
compact_binary_rules::vector_brules<const BRuleC2f*>* 
compact_binary_rules::vector_brules<const BRuleC2f*>::convert<BRuleC2f>(std::vector<BRuleC2f, std::allocator<BRuleC2f> > const&);
