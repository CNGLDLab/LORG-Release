// -*- mode: c++ -*-
#ifndef _ANNOTHISTORIESPARSER_H_
#define _ANNOTHISTORIESPARSER_H_

#include "utils/Tree.h"
#include "ParseError.h"

#include <string>


namespace AnnotHistoriesParser
{
  std::vector< Tree<unsigned> > from_string( const std::string& str ) throw(ParseError);
  void from_file(const char * filename, std::vector< Tree<unsigned> >& trees) throw(ParseError);
}


#endif /* _ANNOTHISTORIESPARSER_H_ */
