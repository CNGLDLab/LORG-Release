// -*- mode: c++ -*-
#ifndef PTBINPUTPARSER_H
#define PTBINPUTPARSER_H

#include "utils/PtbPsTree.h"

#include <string>
#include "ParseError.h"

namespace PTBInputParser
{
  std::vector<PtbPsTree> from_string( const std::string& str ) throw(ParseError);
  void from_file(const char * filename, std::vector<PtbPsTree>& trees) throw(ParseError);
}

#endif // PTBINPUTPARSER_H
