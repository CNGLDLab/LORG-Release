#include "LexiconFactory.h"


LexiconFactory::lex_type LexiconFactory::string_2_lex_type(const std::string& s)
{

  if(s == "berkeley_sophisticated")
    return Bsophisticated;

  //  if(s == "basic")
  return Basic;
}

Lexicon * LexiconFactory::create_lexicon(lex_type type, WordSignature& ws, unsigned cutoff, bool verbose)
{
  if (type == Bsophisticated){
    if(verbose) std::clog << "Setting lexicon to Berkeley lexicon type (sophisticated) " << std::endl ;
    return new BerkeleyLexicon(ws,0.5,200);
  }
  else { // if(type == Basic) {
    if(verbose) std::clog << "Setting lexicon type to Basic " << std::endl ;
    return new BasicLexicon(ws,cutoff);
  }

}


Lexicon * LexiconFactory::create_lexicon(ConfigTable& conf)
{
  bool verbose = conf.exists("verbose");

  unsigned unknown_word_cutoff = 0;
  if (conf.exists("unknown-word-cutoff"))
    unknown_word_cutoff = conf.get_value<unsigned>("unknown-word-cutoff");

  if (verbose)
    std::clog << "Unknown word cutoff set to " << unknown_word_cutoff << "\n";


  lex_type type = string_2_lex_type(conf.get_value<std::string>("lexicon-type"));

  WordSignature& ws = *WordSignatureFactory::create_wordsignature(conf);

  return create_lexicon(type,ws,unknown_word_cutoff, verbose);
}
