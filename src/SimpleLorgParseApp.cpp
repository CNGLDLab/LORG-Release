#include "SimpleLorgParseApp.h"

#include "utils/Tagger.h"
#include "ParseSolution.h"
#include "utils/LorgConstants.h"

#include "utils/data_parsers/RuleInputParser.h"

template<>
Grammar<Rule, Rule, Rule>::Grammar(const std::string& filename)
{
  RuleInputParser::read_rulefile(filename,
				 lexical_rules,
				 unary_rules,
				 binary_rules);
}


SimpleLorgParseApp::SimpleLorgParseApp()
  : LorgParseApp(), grammar(NULL)
{}


SimpleLorgParseApp::~SimpleLorgParseApp()
{
  delete parser;
  delete grammar;
}

int SimpleLorgParseApp::run()
{
  if(verbose) std::clog << "Start parsing process.\n";

  std::vector<Word> s;
  std::vector< bracketing > brackets;
  std::string test_sentence;
  int count = 0;

  int start_symbol = SymbolTable::instance_nt().get(LorgConstants::tree_root_name);

  clock_t parse_start = (verbose) ? clock() : 0;

  std::vector<std::string> comments;
      while(tokeniser->tokenise(*in,test_sentence,s,brackets, comments)) {
    clock_t sent_start = (verbose) ? clock() : 0;

    // should be "extra-verbose"
    // if(verbose) {
    //   std::clog << "Tokens: ";
    //   for(std::vector<Word>::const_iterator i = s.begin(); i != s.end(); i++)
    // 	std::clog << "<" << i->form << ">";
    //   std::clog << "\n";
    // }


    // the pointer to the solution
    PtbPsTree*  best_tree = NULL;

    // check length of input sentence
    if(s.size() <= max_length) {

      // tag
      tagger->tag(s);

      // create and initialise chart
      ParserCKYBest::Chart chart(s,parser->get_nonterm_count(),brackets);

      // parse
      parser->parse(chart);

      // get results
      best_tree = chart.get_best_tree(start_symbol, 0, always_output_forms, false);
    }

    *out << unix_parse_solution(test_sentence,
			   ++count,
			   s.size(),
			   best_tree, LorgConstants::NullProba, //FIXME get real prob
			   (verbose) ? (clock() - sent_start) / double(CLOCKS_PER_SEC) : 0,
			   verbose)
	 << '\n';

    delete best_tree;
    s.clear();
    brackets.clear();
  }

  if(verbose){
    std::clog << "overall time: " << (clock() - parse_start) / double(CLOCKS_PER_SEC) << "s\n";
  }

  return 0; //everything's fine
}



LorgOptions SimpleLorgParseApp::get_options() const
{
  LorgOptions options(LorgParseApp::get_options());
  options.add_simple_parser_options();
  return options;
}



bool SimpleLorgParseApp::read_config(ConfigTable& configuration)
{

  if(LorgParseApp::read_config(configuration) == false)
    return false;

  // get training grammar
  if(configuration.exists("grammar")) {
    const std::string& training_filename = configuration.get_value<std::string>("grammar");
    if(verbose)
      std::cerr << "Setting grammar to " << training_filename << ".\n";

    //create grammar of rules and associated probs from training file
    grammar = new Grammar<Rule,Rule,Rule>(training_filename);
    // should check if the grammar is correctly built

  }
  else {
    std::cerr << "Training grammar wasn't set. Exit program." << std::endl;
    return false;
  }


  parser = new ParserCKYBest(grammar);

  tagger->set_word_rules(&(parser->get_words_to_rules()));

  return true;
}
