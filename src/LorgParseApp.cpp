#include "LorgParseApp.h"

#include "lexicon/WordSignatureFactory.h"

LorgOptions LorgParseApp::get_options() const
{
  LorgOptions options;
  options.add_parser_options();
  options.add_unknown_word_mapping_options();
  options.add_parser_positionals();
  return options;
}

bool LorgParseApp::read_config(ConfigTable& configuration)
{
  if(LorgApp::read_config(configuration) == false)
    return false;

 // get output stream or write to stdout
 if(configuration.exists("output")) {
   const std::string& output_filename = configuration.get_value<std::string>("output");
   std::ofstream * outstream = new std::ofstream(output_filename.c_str());
   if(*outstream) {
     out = outstream;
     if(verbose) std::clog << "Setting ouput file to " << output_filename << std::endl;
   }
   else {
     std::clog << "Could not open file " << output_filename << std::endl;
     return false;
   }
 }
 else {
   if(verbose) std::clog << "Writing output to stdout." << std::endl;
   out = &std::cout;
 }

  // get input stream or read from stdin
  if(configuration.exists("test-data")) {
    const std::string& input_filename = configuration.get_value<std::string>("test-data");
    std::ifstream * instream = new std::ifstream(input_filename.c_str());
    if(*instream) {
      in = instream;
      if(verbose) std::clog << "Setting test file to " << input_filename << std::endl;
    }
    else {
      std::clog << "Could not open file " << input_filename << std::endl;
      return false;
    }
  }
  else {
    if(verbose)	std::clog << "Reading input from stdin." << std::endl;
    in = &std::cin;
  }

  //get max_length
  max_length = configuration.get_value<unsigned>("max-length");

  // read parts of the configuration dedicated to word signature
  // and initialize word class
  Word::init_lexicon_type(WordSignatureFactory::create_wordsignature(configuration));

  always_output_forms = configuration.get_value<bool>("always-output-forms");





  // get rm_punctuation
  bool rm_punctuation = configuration.exists("remove-punctuation");
  if(rm_punctuation) {
    if(verbose) std::clog << "Removing punctuation from input.\n";
  }
  else {
    if(verbose) std::clog << "Not removing punctuation from input.\n";
  }

  // get input mode
  TokMode input_mode;
  try {
    input_mode = Tokeniser::string_to_tokmode(configuration.get_value<std::string>("input-mode"));
  }
  catch(std::out_of_range&) { // should be a proper exception here
    std::clog << "Unknown input mode: " <<  configuration.get_value<std::string>("input-mode") << std::endl;
    return false;
  }

  if(verbose)
    std::clog << "Input mode set to: " << Tokeniser::tokmode_to_string(input_mode);


  char comment_char = configuration.get_value<char>("comment-char");


  //creating tokeniser for english: TODO write tokeniser for other languages ?
  tokeniser = std::auto_ptr<Tokeniser>(TokeniserFactory::create_tokeniser(TokeniserFactory::English, rm_punctuation, input_mode, "", " ", comment_char));

  // get replace_numbers

  std::string number_regex = configuration.get_value<std::string>("number-regex");
  bool replace_numbers = configuration.exists("replace-numbers");
  if(verbose) {
    if(replace_numbers) {
      std::clog << "Replacing numbers from input with special token." << std::endl;
      std::clog << "Using number regex: " << number_regex << std::endl;
    }
    else {
      std::clog << "Not replacing numbers from input with special token." << std::endl;
    }
  }

  tagger = std::auto_ptr<Tagger>(new Tagger(NULL, replace_numbers, number_regex));

  return true;
}
