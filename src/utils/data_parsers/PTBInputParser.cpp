#include "PTBInputParser.h"
#include "utils/LorgConstants.h"

#include <fstream>

#include <iostream>
#include <iomanip>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

#include <boost/config/warning_disable.hpp>
#include <boost/foreach.hpp>

#include <boost/spirit/include/support_multi_pass.hpp>

#include <boost/spirit/include/classic_position_iterator.hpp>
namespace classic = boost::spirit::classic;

namespace fusion = boost::fusion;
namespace phoenix = boost::phoenix;
namespace qi = boost::spirit::qi;
//namespace ascii = boost::spirit::ascii;
//namespace unicode = boost::spirit::unicode;
//namespace mychar = boost::spirit::ascii;
namespace mychar = boost::spirit::standard;

template <typename Iterator>
struct ptbpstree_parser : qi::grammar<Iterator, std::vector<PtbPsTree>(), mychar::space_type>
{
  ptbpstree_parser() : ptbpstree_parser::base_type(ptbtrees)
  {
    using namespace qi::labels;

    ptbtrees %= +ptbtree;

    ptbtree %= '(' >> tree >> ')';

    // tree --> ( nonterm [ term | tree+ ] )
    // if the parser finds a nonterm or term node, it will add a daughter to the current node
    // if the parser finds a closing bracket, it will make the mother of the current node the current node
    // this will happen after every terminal node as well

    tree = '('
      >> id [_a = _1]
      >>
      (
       id [phoenix::push_back(qi::_b, phoenix::construct<PtbPsTree>(_1))]
       |
       +tree [phoenix::push_back(_b, _1)]
       )
      >> mychar::char_(')') [_val = phoenix::construct<PtbPsTree>(_a, _b)]
      ;


    id %= qi::lexeme[+(qi::char_ - ' ' - '\t' - '\n' - '\r' - '(' - ')')];



    ptbtrees.name("ptbtrees");
    ptbtree.name("ptbtree");
    tree.name("tree");
    id.name("id");

    // qi::on_error<qi::fail>(ptbtree, std::clog
    // 			   << phoenix::val("Error! Expecting ")
    // 			   <<  qi::_4                               // what failed?
    // 			   << phoenix::val(" here: \"")
    // 			   << phoenix::construct<std::string>(qi::_3, qi::_2)   // iterators to error-pos, end
    // 			   << phoenix::val("\"")
    // 			   << std::endl
    // 		   );
  }

  boost::spirit::qi::rule<Iterator, std::vector<PtbPsTree>(), mychar::space_type> ptbtrees;
  boost::spirit::qi::rule<Iterator, PtbPsTree(), mychar::space_type> ptbtree;
  boost::spirit::qi::rule<Iterator, PtbPsTree(), boost::spirit::locals< std::string, std::vector<PtbPsTree> >, mychar::space_type> tree;
  boost::spirit::qi::rule<Iterator, std::string(), mychar::space_type> id;
};


namespace {
  void add_top_node(PtbPsTree& oldtree)
  {
    PtbPsTree newtree;
    PtbPsTree::depth_first_iterator r_itr = newtree.add_root(LorgConstants::tree_root_name);
    newtree.add_first_daughter(r_itr,oldtree);
    oldtree = newtree;
  }

  struct add_top_node_funct
  {
    void operator()(PtbPsTree& tree) const {add_top_node(tree);}
  };

  PtbPsTree copy_add_top_node(const PtbPsTree& oldtree)
  {
    PtbPsTree newtree;
    PtbPsTree::depth_first_iterator r_itr = newtree.add_root(LorgConstants::tree_root_name);
    newtree.add_first_daughter(r_itr,oldtree);
    return newtree;
  }

  struct copy_add_top_node_funct
  {
    PtbPsTree operator()(const PtbPsTree& tree) const {return copy_add_top_node(tree);}
  };



}



std::vector<PtbPsTree> PTBInputParser::from_string( const std::string& str ) throw(ParseError)
{
  typedef std::string::const_iterator iterator_type;
  typedef ptbpstree_parser<iterator_type> parser;


  iterator_type iter = str.begin();
  iterator_type end  = str.end();

  parser p;
  std::vector<PtbPsTree> trees;

  bool r = phrase_parse(iter, end, p, mychar::space, trees);

  if(!r) {
    throw(ParseError());
    //    std::clog << "not read: " << std::string(iter,end) << std::endl;
  }

  add_top_node_funct atn;
  std::for_each(trees.begin(),trees.end(),atn);

  return trees;
}

void PTBInputParser::from_file(const char* filename,
			       std::vector<PtbPsTree>& trees) throw(ParseError)
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


  iterator_type iter = str.begin();
  iterator_type end  = str.end();
  //  typedef ptbpstree_parser<iterator_type> parser;

  // // iterate over stream input
  // in.unsetf(std::ios::skipws); // No white space skipping!
  // typedef std::istreambuf_iterator<char> base_iterator_type;

  // base_iterator_type in_begin(in);

  // // convert input iterator to forward iterator, usable by spirit parser
  // typedef boost::spirit::multi_pass<base_iterator_type> forward_iterator_type;
  // forward_iterator_type fwd_begin =
  //     boost::spirit::make_default_multi_pass(in_begin);
  // forward_iterator_type fwd_end;
  //typedef ptbpstree_parser<forward_iterator_type> parser;





  //read the trees in the file
   std::vector<PtbPsTree> local_trees;
   //   std::vector<double> local_doubles;

   // wrap forward iterator with position iterator, to record the position
   typedef classic::position_iterator2<iterator_type> pos_iterator_type;
   pos_iterator_type position_begin(iter, end, filename);
   pos_iterator_type position_end;


   typedef ptbpstree_parser<pos_iterator_type> parser;
   parser p;

   bool res;
   try {
     res =
       qi::phrase_parse(position_begin,
                        position_end,
                        p,
                        mychar::space,
                        local_trees);
   }
   catch(const qi::expectation_failure<pos_iterator_type>& e) {

     const classic::file_position_base<std::string>& pos = e.first.get_position();
     std::stringstream msg;
     msg <<"parse error at file " << pos.file << " line " << pos.line << " column " << pos.column << std::endl
         << "'" << e.first.get_currentline() << "'" << std::endl
         << std::setw(pos.column) << " " << "^- here";

     //     std::cout << msg.str() << std::endl;;


     throw std::runtime_error(msg.str());
   }


   std::cout << std::endl << local_trees.size() << " trees have been read"<< std::endl;

   if(position_begin != position_end) throw(ParseError(filename));


  //add a top node to all these trees
  std::for_each(local_trees.begin(),local_trees.end(), add_top_node_funct());

  //copy the trees in the vector to be returned
  trees.insert(trees.end(),local_trees.begin(),local_trees.end());

  in.close();

}
