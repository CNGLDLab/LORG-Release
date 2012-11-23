// -*- mode: c++ -*-
#ifndef LORGOPTIONS_H_
#define LORGOPTIONS_H_

#include <boost/program_options.hpp>

namespace po = boost::program_options;
class ConfigTable;

class LorgOptions
{
public:
  friend class ConfigTable;
public:
  LorgOptions() : options(), positionals(), variables()
{

  // Options common to all LORG Applications
    po::options_description general_opts("general options");
    general_opts.add_options()
      ("help,h","prints this help")
      ("verbose,v","enables verbose output")
      ("config-file,c",po::value<std::string>(),"specifies a configuration file")
      ("output,o",po::value< std::string >(),"set the output file. If not set, write on stdout")
      ;
    options.add(general_opts);
  }

  void add_treebank_processing_options()
  {
    // Options for the trainer -- treebank processing
    po::options_description tb_opts("grammar options");
    tb_opts.add_options()

      ("nodes-to-remove",po::value<std::string>()->multitoken()->default_value("-NONE-"), "specifies a set of nodes that are to be removed from the trees")
      ("remove-functions",po::value<bool>()->default_value(true),"specifies whether grammatical functions are to be removed")
      ("replace-numbers",po::value<bool>()->default_value(false),"specifies whether numbers are replaced with the token NUMBER" )
      ("number-regex",po::value<std::string>()->default_value("^\\d+([,\\.]\\d+)?(,\\d+)?"),"specifies the regex to recognize numbers")
      ("vm",po::value<unsigned int>()->default_value(1),"triggers vertical annotation")
      ("pos-vm",po::value<bool>()->default_value(false),"triggers vertical annotation for preterminal")
      ("remove-same-unary",po::value<bool>()->default_value(true),"removes unary chains of the form X -> X")
      ("binarisation", po::value<std::string>()->default_value("right"),
       "set the binarization algorithm. Can be \"none\",\"left\" or \"right\"")
      ("hm", po::value<int>()->default_value(0),
       "set the horizontal markovisation for binarization. negative value is infinite.")
      ("sentence-max-length",po::value<unsigned int>()->default_value(0),"maximum length for sentences to be added to the treebank (0 means no maximum)")
      ("modified-treebank",po::value<std::string>()->default_value(""), "a filename to store the modified version of the treebank (signatures, functions,...)")
      ;
    options.add(tb_opts);
  }


  void add_unknown_word_mapping_options()
  {
    // This option is common to the trainer and the parsers
    po::options_description uwm_opts("unknown word mapping options");
    uwm_opts.add_options()
      ("unknown-word-mapping,w",po::value<std::string>()->default_value("generic"),
       "unknown word mapping type: generic, BerkeleyEnglish, BaselineFrench, EnglishIG, FrenchIG, Arabic, ArabicIG, ItalianIG")
      ;
    options.add(uwm_opts);
  }

  void add_lexicon_options()
  {
    // Options for the trainer
    po::options_description lex_opts("lexicon options");
    lex_opts.add_options()
      ("unknown-word-cutoff,u",po::value<unsigned>()->default_value(5),
       "any word occuring less than or equal to this value will be mapped to an unknown word symbol (for BasicLexicon only).")
      ("lexicon-type,l",po::value<std::string>()->default_value("basic"),"specify lexicon type \"basic\" , \"berkeley_sophisticated\"")
      ;
    options.add(lex_opts);
  }

  void add_grammar_options()
  {
    // Options for the EM trainer
    po::options_description grammar_opts("grammar options");
    grammar_opts.add_options()
      ("treebank", po::value<std::vector< std::string> >(), "set the treebank to read. If not set, read from stdin")
      ("filter-level",po::value<int>()->default_value(1),
       "set the minimal number of occurrences in the treebank for a rule to be added in the grammar. If no set, add all rules")
      ("split-size",po::value<unsigned>()->default_value(2),"number of splits for one iteration of split-merge EM")
      ("split-merge-cycles",po::value<unsigned>()->default_value(6),"number of split merge cycles")
      ("split-em",po::value<unsigned>()->default_value(50),"temporary setting - specifies exactly the number of EM iterations after split")
      ("merge-em",po::value<unsigned>()->default_value(20),"temporary setting - specifies exactly the number of EM iterations after merge")
      ("merge-percentage",po::value<unsigned>()->default_value(50),"The percentage of splits to merge on each split/merge iteration ")
      ("turn-off-merge",po::value<bool>()->default_value(false),"specifies whether to turn off merge cycles (default: false)")
      ("smooth-grammar",po::value<double>()->default_value(0.01),"smoothing factor for internal rules (between 0 and 1). 0 means no smoothing")
      ("smooth-lexicon",po::value<double>()->default_value(0.1),"smoothing factor for lexical rules(between 0 and 1). 0 means no smoothing")
      ("split-random",po::value<unsigned>()->default_value(1),"deviation used in splits")
      ("random-seed", po::value<unsigned>()->default_value(29),"random seed")
      ("prob-threshold",po::value<double>()->default_value(1e-30),"Rule pruning: get rid of rules that have a probability lower than this threshold.  If the default value of 0.0 is set, no rule pruning will occur.")
      ("base-grammar-only",po::value<bool>()->default_value(false),"no split/merge cycles - just output base grammar")
      ("lexical-smoothing", "performs lexical smoothing (sophisticated lexicon)")
      ("final-lex-em" ,po::value<bool>()->default_value(false), "do one extra em iteration on lexicon after merge")
      ("smooth-method", po::value<std::string>()->default_value("linear"), "smoothing method: linear or weighted")
      ("nbthreads", po::value<unsigned>()->default_value(1), "number of threads")
      ;
    options.add(grammar_opts);
  }

  void add_grammar_positionals()
  {
    positionals.add("treebank", -1);
  }


  void add_parser_options()
  {
    // options common to the simple and two-stage parsers
    po::options_description parser_opts("parser options");
    parser_opts.add_options()
      ("test-data", po::value<std::string>(), "sets the test data that is to parse")
      ("input-mode,i",po::value<std::string>()->default_value("tok"),"sets the input mode: \"raw\", \"tok\" or \"tag\"")
      ("remove-punctuation","specifies whether punctuation is removed from the input sentence")
      ("replace-numbers,n","replace numbers with |NUMBERS| during tokenization")
      ("number-regex",po::value<std::string>()->default_value("^\\d+([,\\.]\\d+)?(,\\d+)?"),"specifies the regex to recognize numbers")
      ("comment-char,x", po::value<char>()->default_value('#'), "lines starting with this character will not be parsed and treated as comments")
      ("max-length", po::value<unsigned>()->default_value(1000), "sets the maximum length for input sentences")
      ("always-output-forms", po::value<bool>()->default_value(true), "always output the original form of words. If false output signatures of unknowns instead")
      ("output-annotations", po::value<bool>()->default_value(false), "output trees with nodes decorated by annotations")
      ("extract-features,e", po::value<bool>()->default_value(false), "output features from trees (experimental)")
      ("output-format,O", po::value<std::string>()->default_value("unix"), "output format : unix [default] or json")
    ;
    options.add(parser_opts);
  }


  void add_simple_parser_options()
  {
    // option for the simpler parser
    po::options_description parser_opts("simple parser options");
    parser_opts.add_options()
      ("grammar,g", po::value<std::string>(), "sets the grammar for the simple parser")
      ;
    options.add(parser_opts);
  }


  void add_2sparser_options()
  {
    // options for the two stage parser
    po::options_description parser_opts("two-stage parser options");
    parser_opts.add_options()
      ("grammar,g", po::value<std::string>(), "sets the grammar for the parser")
      ("alternate-grammar,F", po::value< std::vector<std::string> >(), "sets the additional grammars for the (maxn parsing) parser")
      ("parser-type,p",po::value<std::string>()->default_value("max"),"sets the type of parsing - vit for Viterbi , max for MaxRule, maxn for MaxRule with several grammars, kmax for k-best maxrule")
      ("beam-threshold,b",po::value<double>()->default_value(0.0001), "sets the ratio for beaming")
      ("accurate", "set thresholds for accurate parsing")
      ("min-length-beam",po::value<unsigned>()->default_value(1), "minimum length of sentences to use beam")
      ("kbest,k",po::value<unsigned>()->default_value(1), "numbers of parses to return for each sentence (only used in kmax)")
      ("stubbornness,s", po::value<int>()->default_value(-1), "number of retries (with lower beam-threshold) if packed forest construction fails. Last try is performed without threshold. Set to negative value to skip this.")
      ("nbthreads", po::value<unsigned>()->default_value(2), "Number of threads for processing the chart")
      ;
    options.add(parser_opts);
  }

  void add_parser_positionals()
  {
    positionals.add("test-data", 1);
  }

private:
  void initialise( int argc, char** argv )
  {
    po::store(po::command_line_parser(argc, argv).options(options).positional(positionals).run(), variables);
    po::notify(variables);
  }

  po::options_description options;
  po::positional_options_description positionals;
  po::variables_map variables;
};

#endif // LORGOPTIONS_H_
