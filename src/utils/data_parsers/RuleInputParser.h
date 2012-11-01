#ifndef RULEINPUTPARSER_H
#define RULEINPUTPARSER_H

#include "rules/Rule.h"

#include "ParseError.h"

namespace RuleInputParser {
  void read_rulefile(const std::string& filename, 
		     std::vector<Rule>& lexicals,
		     std::vector<Rule>& unaries,
		     std::vector<Rule>& n_aries
		     ) throw(ParseError);
  
  void read_rulestring(const std::string& str, Rule& rule) throw(ParseError);

}


#endif // RULEINPUTPARSER_H
