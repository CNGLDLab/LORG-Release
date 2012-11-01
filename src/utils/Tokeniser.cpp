#include "Tokeniser.h"

#include "boost/lexical_cast.hpp"


std::string Tokeniser::tokmode_to_string(TokMode mode)
{
  switch(mode) {
  case Raw :
    return "raw";
    break; // ?
  case Tok :
    return "tok";
    break; // ?
  case Tag :
    return "tag";
    break;
  case Lat :
    return "lat";
    break;
  default:
    throw std::out_of_range("You forgot a case in the tokmode conversion method !");
  }
}

TokMode Tokeniser::string_to_tokmode(const std::string& mode_string)
{
  if(mode_string == "raw")
    return Raw;
  if(mode_string == "tok")
    return Tok;
  if(mode_string == "tag")
    return Tag;
  if(mode_string == "lat")
    return Lat;

  throw std::out_of_range("Unknown input mode: " + mode_string);
}

Tokeniser::Tokeniser(TokeniserSpec* spec,
		     bool remove_punct,
		     TokMode mode,
		     const std::string& tokens_to_remove,
		     const std::string& delim,
                     char comment_char)
{
  m_spec.reset(spec);
  this->set_options(remove_punct, mode, tokens_to_remove, delim, comment_char);
}

Tokeniser::Tokeniser( TokeniserSpec* spec)
{
  m_spec.reset(spec);
}

Tokeniser::~Tokeniser()
{
}

void Tokeniser::set_options(bool remove_punct,
			    TokMode mode,
			    const std::string& tokens_to_remove,
			    const std::string& delim,
                            char comment_char)
{
  m_remove_punct = remove_punct;
  m_mode = mode;
  m_delim = delim;
  m_comment_char = comment_char;

  boost::unordered_set<std::string> m_unwanted_chars;
  std::string::size_type start = 0;
  std::string::size_type end = tokens_to_remove.find_first_of(' ',0);
  while(end != std::string::npos)
    {
      m_unwanted_chars.insert(tokens_to_remove.substr(start,end-start));
      start = end + 1;
      end = tokens_to_remove.find_first_of(' ',start);
    }
  m_unwanted_chars.insert(tokens_to_remove.substr(start));
}


void Tokeniser::tokenise_raw(const std::string& sentence, std::vector< Word >& result)
{
  if(m_spec->process(sentence))
    // std::cerr << "no full parse\n";
    result = m_spec->sentence();
}


struct read_helper_token
{
  std::vector<Word>& result;
  boost::tokenizer<boost::char_separator<char> >::const_iterator& token;
  const boost::tokenizer<boost::char_separator<char> >::const_iterator& end;

  read_helper_token(std::vector<Word>& r,
		    boost::tokenizer<boost::char_separator<char> >::const_iterator& t,
		    const boost::tokenizer<boost::char_separator<char> >::const_iterator& e)
    : result(r), token(t), end(e) {};

  void operator()(const std::string& form, unsigned pos)
  {
    result.push_back(Word(form,pos));
  }
};



struct read_helper_tokentag
{
  std::vector<Word>& result;
  boost::tokenizer<boost::char_separator<char> >::const_iterator& token;
  const boost::tokenizer<boost::char_separator<char> >::const_iterator& end;

  read_helper_tokentag(std::vector<Word>& r,
		    boost::tokenizer<boost::char_separator<char> >::const_iterator& t,
		    const boost::tokenizer<boost::char_separator<char> >::const_iterator& e)
    : result(r), token(t), end(e) {};

  void operator()(const std::string& form, unsigned pos)
  {
    std::vector<std::string> strtags;
    ++token;
    if(token == end || *token != "(" )
      throw MissingTagException();


    while(++token != end && *token != ")" ) {
      strtags.push_back(*token);
    }
    if(token == end )
      throw MissingTagException();

    result.push_back(Word(form,pos, pos+1, strtags));
  }
};


template <typename helper>
void Tokeniser::tokenise_toktag(const std::string& sentence,
				std::vector< Word >& result, std::vector< bracketing >& brackets)
{
  static boost::char_separator<char> delimiter(m_delim.c_str());

  boost::tokenizer<boost::char_separator<char> > tokens(sentence,delimiter);

  unsigned pos = 0;
  unsigned open_pos = pos;
  bool open_pos_hard = false;

  const boost::tokenizer<boost::char_separator<char> >::const_iterator& end = tokens.end();

  for(boost::tokenizer<boost::char_separator<char> >::const_iterator token(tokens.begin());
      token != end; ++token) {

    std::string form(*token);
    //std::cout << "position " << string_position << " word " << *i << std::endl;
    if(form == "[[") {
      open_pos = pos;
      open_pos_hard = true;
    }
    else {
      if(form == "[") {
	open_pos = pos;
	open_pos_hard = false;
      }
      else {
	if(form == "]]" || form == "]") {
	  if( pos > 1 + open_pos) {
	    bool close_pos_hard = form == "]]";
	    brackets.push_back(bracketing(open_pos,open_pos_hard,pos-1,close_pos_hard));
	  }
	}
	else {
	  helper h(result,token,end);
	  h(form,pos++);
	}
      }
    }
  }
}



void Tokeniser::tokenise(const std::string& sentence, std::vector< Word >& result, std::vector< bracketing >& brackets)
{
  switch(m_mode) {
  case Tok :
    tokenise_toktag<read_helper_token>(sentence,result,brackets);
    break;
  case Tag :
    tokenise_toktag<read_helper_tokentag>(sentence,result,brackets);
    break;
  case Raw :
    tokenise_raw(sentence, result);
    break;
  default :
    break;
  }

  if(m_remove_punct)
    remove_punctuation(result);
}

bool Tokeniser::tokenise( std::istream& input,
			  std::string& raw_string,
			  std::vector< Word >& next_sentence,
			  std::vector< bracketing >& brackets,
                          std::vector<std::string>& comments)
{
  bool real = false;
  while(std::getline(input,raw_string)) {
    if(raw_string[0] == m_comment_char)
      comments.push_back(raw_string);
    else {
      real = true;
      break;
    }
  }
  if(real) {
    if (m_mode == Lat) // polyline node
      this->tokenise_lat(raw_string, input, next_sentence,brackets);
    else // monoline modes
      this->tokenise(raw_string, next_sentence,brackets);
    return true;
  }
  return false;
}

void Tokeniser::remove_punctuation( std::vector< Word >& sentence )
{
  std::vector< Word >::iterator del = sentence.begin();
  for(unsigned int i = 0; i < sentence.size(); ++i, ++del)
    {
      if(m_unwanted_chars.count(sentence[i].form))
	{
	  del = sentence.erase(del);
	  --i;
	}
    }
}


Word* Tokeniser::tokenise_lattice_line(const std::string& line)
{
  static boost::char_separator<char> delimiter(m_delim.c_str()); //space ?

  boost::tokenizer<boost::char_separator<char> > tokens(line, delimiter);

  // format: start_index end_index word [ list of pos ]
  // remark: [ foo ] means foo is optional
  const boost::tokenizer<boost::char_separator<char> >::const_iterator& end = tokens.end();
  boost::tokenizer<boost::char_separator<char> >::const_iterator token(tokens.begin());

  std::string start_index_as_string(*token);
  ++token;
  std::string end_index_as_string(*token);
  ++token;
  std::string form(*token);
  ++token;

  std::vector<std::string> pos_vect;
  for(; token != end; ++token) {
    pos_vect.push_back(*token);
  }

  int start, endi;
  try {
    start = boost::lexical_cast<int>(start_index_as_string);

  }
  catch(boost::bad_lexical_cast &) {
    std::cerr << start_index_as_string << " could not be converted to int";
    return NULL;
  }

  try {
  endi = boost::lexical_cast<int>(end_index_as_string);

  }
  catch(boost::bad_lexical_cast &) {
    std::cerr << end_index_as_string << " could not be converted to int";
      return NULL;
  }

  return new Word(form, start, endi, pos_vect);

}



void Tokeniser::tokenise_lat(std::string& raw_string_first_line,
                             std::istream& input,
                             std::vector<Word>& next_sentence,
                             std::vector< bracketing >& /* ignored brackets */)
{
  std::vector<std::string> lines;
  lines.push_back(raw_string_first_line);

  std::string line;

  //get all the lines
  while(std::getline(input, line)) {
    if(line == "") break;
    lines.push_back(line);
  }

  for (std::vector<std::string>::const_iterator i(lines.begin()); i != lines.end(); ++i)
    {
      Word* lw = tokenise_lattice_line(*i);
      if (lw) {
        next_sentence.push_back(*lw);
        delete lw;
      }
      else // todo: proper error handling
        std::cerr << "pb with line " << *i << std::endl;
    }

  raw_string_first_line = "non writable mode";

}
