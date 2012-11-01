// -*- mode: c++ -*-
#ifndef SIMPLELORGPARSEAPP_H
#define SIMPLELORGPARSEAPP_H

#include "LorgParseApp.h"

#include "grammars/Grammar.h"
#include "rules/Rule.h"
#include "parsers/ParserCKYBest.h"

// //should not be included here: defines TokMode
// #include "utils/TokeniserFactory.h"

class SimpleLorgParseApp : public LorgParseApp
{
public:
  SimpleLorgParseApp();
  ~SimpleLorgParseApp();
  int run();

private:
  bool read_config(ConfigTable& configuration);
  LorgOptions get_options() const;

  Grammar<Rule,Rule,Rule>* grammar;
  ParserCKYBest* parser;

  //not defined -> forbidden
  SimpleLorgParseApp(const SimpleLorgParseApp&);
  SimpleLorgParseApp& operator=(const SimpleLorgParseApp);
};


#endif // SIMPLELORGPARSEAPP_H
