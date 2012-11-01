#include "AnnotHistoriesParser.h"


#include <fstream>


#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

#include <boost/config/warning_disable.hpp>

namespace fusion = boost::fusion;
namespace phoenix = boost::phoenix;
namespace qi = boost::spirit::qi;
//namespace ascii = boost::spirit::ascii;
//namespace unicode = boost::spirit::unicode;
//namespace mychar = boost::spirit::ascii;
namespace mychar = boost::spirit::standard;



template <typename Iterator>
struct ptbpstree_parser : boost::spirit::qi::grammar<Iterator, std::vector< Tree<unsigned> >(), mychar::space_type>
{
  ptbpstree_parser() : ptbpstree_parser::base_type(ptbtrees)
  {    
    using namespace qi::labels;
    ptbtrees %= +ptbtree;
      
    ptbtree = '(' >> tree [_val = _1] >> ')';
    
    // tree --> ( nonterm [ term | tree+ ] )
    // if the parser finds a nonterm or term node, it will add a daughter to the current node
    // if the parser finds a closing bracket, it will make the mother of the current node the current node
    // this will happen after every terminal node as well
    
    tree
      =
      (id  [_val = phoenix::construct< Tree<unsigned> >(_1)] )
      | 
      ( qi::char_('(') >>  id [_a = _1] >> +tree [phoenix::push_back(_b,_1)] >> qi::char_(')') [ _val = phoenix::construct< Tree<unsigned> >(_a,_b)])
      ;
    

    id %= qi::uint_;
    
    ptbtrees.name("ptbtrees");
    ptbtree.name("ptbtree");
    tree.name("tree");
    id.name("id");
    
    qi::on_error<qi::fail>(ptbtree, std::clog 
			   << phoenix::val("Error! Expecting ")
			   <<  _4                               // what failed?
			   << phoenix::val(" here: \"")
			   << phoenix::construct<std::string>(_3, _2)   // iterators to error-pos, end
			   << phoenix::val("\"")
			   << std::endl
			   );
  }
  
  qi::rule<Iterator, std::vector< Tree<unsigned> >(), mychar::space_type> ptbtrees;
  qi::rule<Iterator,  Tree<unsigned> (), mychar::space_type> ptbtree;
  qi::rule<Iterator,  Tree<unsigned> (), boost::spirit::locals< unsigned, std::vector< Tree<unsigned> > >, mychar::space_type> tree;
  qi::rule<Iterator, unsigned(), mychar::space_type> id;
};


std::vector< Tree<unsigned> > AnnotHistoriesParser::from_string( const std::string& str ) throw(ParseError)
{
  typedef std::string::const_iterator iterator_type;
  typedef ptbpstree_parser<iterator_type> parser;

  
  iterator_type iter = str.begin();
  iterator_type end  = str.end();
  
  parser p;
  std::vector< Tree<unsigned> > trees;
  
  bool r = phrase_parse(iter, end, p, mychar::space, trees);
  
  if(!r) {
    throw(ParseError());
    //    std::clog << "not read: " << std::string(iter,end) << std::endl;
  }
  
  return trees;
}

void AnnotHistoriesParser::from_file(const char* filename, 
			       std::vector< Tree<unsigned> >& trees) throw(ParseError)
{

  std::ifstream in(filename);
  if(!in)
    std::cerr << "stream problems, uh oh ...\n";

  std::string str;
  
  in.unsetf(std::ios::skipws); // No white space skipping!
  std::copy(std::istream_iterator<char>(in),
	    std::istream_iterator<char>(),
	    std::back_inserter(str));
  
  typedef std::string::const_iterator iterator_type;
  typedef ptbpstree_parser<iterator_type> parser;

  iterator_type iter = str.begin();
  iterator_type end  = str.end();

  parser p;
  
  //read the trees in the file
  bool res = phrase_parse(iter, end, p,mychar::space,  trees);
  if(!res) throw(ParseError(filename));


  // for(std::vector<Tree<unsigned> >::const_iterator i(trees.begin()); i != trees.end(); ++i)
  //   std::cout << *i << std::endl;

  in.close();

}
