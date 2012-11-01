#include "RuleInputParser.h"
#include "utils/SymbolTable.h"

#include <fstream>
//#include "boost/lexical_cast.hpp"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/home/phoenix/bind/bind_member_function.hpp>
#include <boost/spirit/home/phoenix/bind/bind_function.hpp>


namespace fusion = boost::fusion;
namespace phoenix = boost::phoenix;
namespace qi = boost::spirit::qi;
//namespace ascii = boost::spirit::ascii;
//namespace unicode = boost::spirit::unicode;
//namespace mychar = boost::spirit::ascii;
namespace mychar = boost::spirit::standard;



template <typename Iterator>
struct rule_parser : boost::spirit::qi::grammar<Iterator, Rule(), mychar::space_type>
{
  rule_parser() : rule_parser::base_type(rule)
  {    
    using namespace qi::labels;


    symbol %= qi::raw[qi::lexeme[((qi::char_ - '_' - ' ') >> *(qi::char_ - ' '))]];

    nt_symbol = symbol [qi::labels::_val = phoenix::bind(&SymbolTable::insert, SymbolTable::instance_nt(), qi::labels::_1)];

    t_symbol  = symbol [qi::labels::_val = phoenix::bind(&SymbolTable::insert, SymbolTable::instance_word(), qi::labels::_1)];

    internal = 
      qi::lit("int") [_c = false]
      >
      nt_symbol  [_a = qi::labels::_1]
      >>
        +nt_symbol [phoenix::push_back(_b, qi::labels::_1)]
      >>
      qi::lit("_")
      >>
      qi::int_ [_d = qi::labels::_1]
      >>
      qi::long_double [_e = qi::labels::_1]
      >>
      qi::eps[ _val = phoenix::construct<Rule>(_a,_b,_c,_d,_e,true)];
    


    lexical = 
      qi::lit("lex") [_c = true]
      >
      nt_symbol   [_a = qi::labels::_1]
      >>
      +t_symbol [ phoenix::push_back(_b, qi::labels::_1)]
      >>
      qi::lit("_")
      >>
      qi::int_ [_d = qi::labels::_1]
      >>
      qi::long_double [_e = qi::labels::_1]
      >>
      qi::eps[ _val = phoenix::construct<Rule>(_a,_b,_c,_d,_e,true)];
    
    
    rule %= lexical | internal;

    rule.name("rule");
    lexical.name("lexical");
    internal.name("internal");
    nt_symbol.name("nt_symbol");
    symbol.name("symbol");
    t_symbol.name("t_symbol");
    //    my_double_.name("my_double");


    qi::on_error<qi::fail>
      (
       rule
       , std::clog
       << phoenix::val("Error! Expecting ")
       <<  qi::labels::_4                               // what failed?
       << phoenix::val(" here: \"")
       << phoenix::construct<std::string>( qi::labels::_3,  qi::labels::_2)   // iterators to error-pos, end
       << phoenix::val("\"")
       << std::endl
       );
  }

  boost::spirit::qi::rule<Iterator, Rule(), mychar::space_type> rule;
  boost::spirit::qi::rule<Iterator, Rule(), boost::spirit::locals<int, std::vector<int>, bool, int, double>, mychar::space_type> lexical;
  boost::spirit::qi::rule<Iterator, Rule(), boost::spirit::locals<int, std::vector<int>, bool, int, double>, mychar::space_type> internal;
  boost::spirit::qi::rule<Iterator, int(), mychar::space_type> nt_symbol;
  boost::spirit::qi::rule<Iterator, int(), mychar::space_type> t_symbol;
  boost::spirit::qi::rule<Iterator, std::string(), mychar::space_type> symbol;
};


void RuleInputParser::read_rulestring(const std::string& str, Rule& prod) throw(ParseError)
{
  std::string::const_iterator iter = str.begin();
  std::string::const_iterator end = str.end();
  
  typedef std::string::const_iterator iterator_type;
  typedef rule_parser<iterator_type> parser;
  
  parser p;
  
  bool r = phrase_parse(iter, end, p, mychar::space, prod);
  
  if(!r) {
    //std::clog << "not read: " << std::string(iter,end) << std::endl;
    throw(ParseError());
  }
  
}


void RuleInputParser::read_rulefile(const std::string& filename, 
				    std::vector<Rule>& lexicals,
				    std::vector<Rule>& unaries,
				    std::vector<Rule>& n_aries
				    ) throw(ParseError)
{
  
  std::ifstream in_file(filename.c_str(),std::ios::in);
  
  if (!in_file)
  {
    std::cerr << "can't open file: " + filename  << std::endl;
    exit(1);
  }

  std::string str;

  in_file.unsetf(std::ios::skipws); // No white space skipping!
  std::copy(
	    std::istream_iterator<char>(in_file),
	    std::istream_iterator<char>(),
	    std::back_inserter(str));
  

  std::string::const_iterator iter = str.begin();
  std::string::const_iterator end = str.end();
  
  typedef std::string::const_iterator iterator_type;
  typedef rule_parser<iterator_type> parser;
  
  parser p;
  
  bool res;
  do {
    Rule r;
    res = phrase_parse(iter, end, p, mychar::space, r);

    if(!res) {
      // std::cout << "not read: " << std::string(iter,end) << std::endl;
      throw(ParseError(filename));
    }

    if(r.is_lexical()) 
      lexicals.push_back(r);
    else
      if(r.is_unary()) 
	unaries.push_back(r);
      else 
	n_aries.push_back(r);
  } while(res && iter != end);
  
  in_file.close();
}
