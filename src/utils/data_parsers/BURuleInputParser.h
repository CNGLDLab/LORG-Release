// -*- mode: c++ -*-
#ifndef BURULEINPUTPARSER_H
#define BURULEINPUTPARSER_H

#include "rules/BRule.h"
#include "rules/URule.h"
#include "rules/LexicalRule.h"

#include "ParseError.h"

#include "utils/Tree.h"

namespace BURuleInputParser {
void read_rulefile(const std::string& filename,
		     std::vector<LexicalRule>& lexicals,
		     std::vector<URule>& unaries,
                     std::vector<BRule>& binaries,
                     std::map<short, unsigned short>&  num_annotations_map,
                     std::vector< Tree<unsigned> >& history_trees
		     ) throw(ParseError);

  void read_rulestring(const std::string& str, AnnotatedRule** rule_ptr) throw(ParseError); // should be casted ?

}


#endif // BURULEINPUTPARSER_H
