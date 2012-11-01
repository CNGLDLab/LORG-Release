#ifndef ENGLISHSPEC_H
#define ENGLISHSPEC_H

#include "TokeniserSpec.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>

#include <boost/config/warning_disable.hpp>


namespace fusion = boost::fusion;
namespace phoenix = boost::phoenix;
namespace qi = boost::spirit::qi;
//namespace ascii = boost::spirit::ascii;
//namespace unicode = boost::spirit::unicode;
//namespace mychar = boost::spirit::ascii;
namespace mychar = boost::spirit::standard;


struct abbrevs_ : qi::symbols<char, std::string>
{
  abbrevs_()
  {
    add
      ("Jan." , "Jan.")("Feb." , "Feb." )("Mar." , "Mar.")("Apr." , "Apr.")
      ("Jun." , "Jun.")("Jul." , "Jul.")("Aug." , "Aug.")("Sep." , "Sep.")
      ("Oct." , "Oct.")("Nov." , "Nov.")("Dec." , "Dec.")
      ( "14-Sept." ,  "14-Sept.")
      ( "Calif." , "Calif.")
      ("Miss." , "Miss.")
      ("C'mon" , "C'mon")
      ("U.S.A." , "U.S.A.")
      ("U.S.S.R." , "U.S.S.R.")
      ("U.S.-U.S.S.R." , "U.S.-U.S.S.R.")
      ("F.A.O." , "F.A.O.")
      ("Pa." , "Pa.")
      ("Ore." , "Ore.")
      ("Del." , "Del.")
      ("La." , "La.")
      ("No." , "No.")
      ("Mont." , "Mont.")
      
      //  	|"A.C."|"A.D."|"Adm."|"A.E."|"A.F."|"A.G."|"A.H."|"a.k.a."|"A.L."|"Ala."|"a.m."|"A.P."|"Ariz."|"Ark."|"A.S."|"Aug."|"B.F."
//  	|"B.J."|"Bros."|"B.V."|"Cie."|"C.J."|"Co."|"CO."|"Col."|"Colo."|"Conn."|"Corp."|"CORP."|"Cos."|"C.R."|"C.W."|"D.C."|"Dr."
//  	|"D.T."|"E.E."|"E.F."|"E.R."|"etc."|"E.W."|"EXBT."|"F.E."|"felon."|"Fla."|"ft."|"F.W."|"Ga."|"G.D."|"Gen."
//  	|"G.m.b."|"Gov."|"H.F."|"H.G."|"H.H."|"H.J."|"H.L."|"Ill."|"Inc."|"Ind."|"InfoCorp."|"Ing."|"Japan-U.S."|"J.C."|"J.D."|"J.E."
//  	|"J.L."|"J.M."|"Jos."|"J.P."|"Jr."|"J.R."|"Ky."|"L.A."|"L.C."|"L.J."|"L.L."|"L.P."|"Lt."|"Ltd."|"M.A."|"Mass."|"M.B.A."|"Md."|"M.D."
//  	|"Messrs."|"Metruh."|"Mich."|"Minn."|"Mo."|"Mr."|"M.R."|"Mrs."|"Ms."|"N.C."|"Nev."|"N.H."|"N.J."|"N.M."|"non-U.S."
//  	|"N.V."|"N.Y."|"Ph."|"p.m."|"post-Oct."|"Prof."|"Pty."|"R.D."|"Rep."|"Reps."|"Rev."|"R.H."|"R.I."|"R.P."|"R.R."
//  	|"R.W."|"S.A."|"S.C."|"Sen."|"Sens."|"Sept."|"S.G."|"Sino-U.S."|"S.p."|"S.P."|"Sr."|"S.S."|"St."|"Tenn."|"U.K."|"U.N."|"U.S."
//  	|"v."|"Va."|"vs."|"W."|"Wash."|"W.D."|"W.I."|"Wis."|"W.J."|"W.R."|"W.T."|"W.Va."|"Y.J."
//  	|"e.g."|"i.e."|"M.S.S."|"M.S."|"S.F."|"Ph.D."|"E.T."|"I.Q."|"A.I."|"Sci."
//  	|"A."|"B."|"C."|"D."|"E."|"F."|"G."|"H."|"I."|"J."|"K."|"L."|"M."|"N."|"O."|"P."|"Q."|"R."|"S."|"T."|"U."|"V."|"W."|"X."|"Y."|"Z."
      ;
  }
  
} abbrevs;



struct auxs_ : qi::symbols<char, std::string >
{
  auxs_()
  {
    add
      ("doesn't" , "does")
      ("don't" , "do")
      ("didn't" , "did")
      ("haven't" , "have" )
      ("hasn't", "has")
      ("hadn't" ,  "had")
      ("isn't", "is")
      ("aren't", "are")
      ("wasn't" , "was")
      ("weren't" , "were")
      ("won't" , "wo")
      ("wouldn't" , "would")
      ("mustn't" , "must") 
      ("needn't" , "need")
      ("couldn't" , "could")
      ("shouldn't" , "should")

      ("Doesn't" , "Does")
      ("Don't" , "Do")
      ("Haven't" , "Have" )
      ("Hasn't", "Has")
      ("Isn't", "Is")
      ("Aren't", "Are")
      ("Wasn't" , "Was")
      ("Weren't" , "Were")
      ("Wouldn't" , "Would")
      ("Won't" , "Wo")
      ("Mustn't" , "Must") 
      ("Needn't" , "Need")
      ("Couldn't" , "Could")
      ("Shouldn't" , "Should")
      ("Didn't" , "Did")
      ("Hadn't" ,  "Had")
      ;
  }
} neg_auxs;


struct special_chars_ : qi::symbols<char, std::string>
{
  special_chars_()
  {
    add
      ("--" , "--")
      ("..." ,"...")
      ("\"" , "''") //<--- TODO:distinction ''/ ``
      ("$", "$")
      ("%" , "%")
      ("&" , "&")
      //      ("/" , "/")
      ("(" , "-LRB-")
      (")" , "-RRB-")
      ("[" , "-LSB")
      ("]" , "-RSB-" )
      ("{" , "-LCB-")
      ("}" , "-RCB-")
      ("\\" , "\\") 
      ("#" , "#")
      ("~" , "~")
      ("*" , "\\*")
      ( "+" , "+")  
      ("-" , "-") 
      (">" , ">") 
      ("<", "<")
      ("=", "=")
      ("'" , "'" )
      ("`", "`")
      ("@", "@")
      ;
  }
}special_chars;


struct puncts_ : qi::symbols<char, std::string>
{
  puncts_() {
    add
      ("!", "!")
      ("?" , "?")
      (":", ":")
      (";", ";") 
      ("." , ".") 
      ("," , ",") 
      ;
  }
}puncts;


struct num_puncts_ : qi::symbols<char, std::string>
{
  num_puncts_() {
    add
      (":", ":")
      ("." , ".") 
      ("," , ",") 
      ;
  }
}num_puncts;

struct contractions_ : qi::symbols<char, std::string> 
{ contractions_() {
    add
      ("'d" , "'d") 
      ("'ll" , "'ll")
      ("'ve" , "'ve")
      ("'s" , "'s")
      ("'re" , "'re")
      ("'m" , "'m")
      ;
  }
}contractions; 


template <typename Iterator>
struct word_parser : boost::spirit::qi::grammar<Iterator, std::vector<std::string>()>
{
//   static void print(const std::string& c)
//   {
//     std::cout << c << std::endl;
//   };


  word_parser() : word_parser::base_type(sentence)
  {    
    using namespace qi::labels;

    sentence = 
      +(
 	(neg_auxs       [phoenix::push_back(_val, qi::labels::_1)]  >> qi::eps [phoenix::push_back(_val, "n't")])
	| qi::space     [phoenix::push_back(_val, " " )]
	| abbrevs       [phoenix::push_back(_val, qi::labels::_1 )]
	| compound      [phoenix::push_back(_val, qi::labels::_1 )]
	| contractions  [phoenix::push_back(_val, qi::labels::_1 )]
	| word          [phoenix::push_back(_val, qi::labels::_1 )]
	| number        [phoenix::push_back(_val, qi::labels::_1 )]
	| special_chars [phoenix::push_back(_val, qi::labels::_1 )]
	| puncts        [phoenix::push_back(_val, qi::labels::_1 )]
	| rest          [phoenix::push_back(_val, qi::labels::_1 )]
	);
    
    
    rest%=qi::lexeme[+ qi::char_];
    
    compound
      =  ( word | number ) [_val += qi::labels::_1]  
      >> *( 
	   (qi::char_("-") [_val += "-"] | qi::char_("/") [_val += "\\/"])
	   >> ( word | number ) [_val += qi::labels::_1]  
	    )
      ;

    word %= qi::lexeme[+ qi::alpha ];
    
    number = 
      -(qi::char_('-') [_val += "-"] | qi::char_('+') [_val += "+"]) 
      >> 
      ( +qi::digit [_val += qi::labels::_1] 
	>> *( 
	     num_puncts [_val += qi::labels::_1]
	     >> 
	     +qi::digit [_val += qi::labels::_1]  
	      ) 
	)
      ;
    
    
    qi::on_error<qi::fail>
      (
       sentence
       , std::clog
       << phoenix::val("Error! Expecting ")
       <<  qi::labels::_4                               // what failed?
       << phoenix::val(" here: \"")
       << phoenix::construct<std::string>( qi::labels::_3,  qi::labels::_2)   // iterators to error-pos, end
       << phoenix::val("\"")
       << std::endl
       );
  }
  
  boost::spirit::qi::rule<Iterator, std::string()> rest;
  boost::spirit::qi::rule<Iterator, std::string()> number;
  boost::spirit::qi::rule<Iterator, std::string()> word;
   boost::spirit::qi::rule<Iterator, std::string()> compound;
  boost::spirit::qi::rule<Iterator, std::vector<std::string>()> sentence;
};



class EnglishSpec : public TokeniserSpec
{
public:
  EnglishSpec()
    : TokeniserSpec(),
      m_left_quote(true)
  {}
  
  ~EnglishSpec()
  {}
  
  virtual bool process( const std::string& sentence );
  virtual std::vector< Word > sentence() const;
  virtual void clear();

private:

private:
  bool m_left_quote;
  std::vector< Word > m_sentence;
};



inline
bool EnglishSpec::process( const std::string& sentence )
{
  clear();

  std::string::const_iterator iter = sentence.begin();
  std::string::const_iterator end = sentence.end();
  
  typedef std::string::const_iterator iterator_type;
  typedef word_parser<iterator_type> parser;
  
  parser p;
  std::vector<std::string> vect;

  bool r = parse(iter, end, p, vect);
  

//   for(std::vector<std::string>::const_iterator iter = vect.begin();
//       iter != vect.end(); ++iter)
//     std::cout << "<" << *iter << "> ";
//   std::cout << std::endl;

  //TODO note that the position should be the position of the word in the sentence - haven't tested if this is correct here.
  int position =0;
  for(std::vector<std::string>::const_iterator iter = vect.begin();iter != vect.end(); ++iter){
    if (*iter != " ") {
      if (*iter == "''") {
    	  if(m_left_quote) 
    		  m_sentence.push_back(Word("``",position));
    	  else
    		  m_sentence.push_back(Word("''",position));
    	  m_left_quote  = !m_left_quote;
      }
      else
    	  m_sentence.push_back(Word(*iter,position));
    }
    position++;
  }
      
  
  if(!r)
    std::clog << "not read: " << std::string(iter,end) << std::endl;
  
  return r;
}

inline
std::vector< Word > EnglishSpec::sentence() const
{
  return m_sentence;
}

inline
void EnglishSpec::clear()
{
  m_sentence.clear();
}

#endif // ENGLISHSPEC_H
