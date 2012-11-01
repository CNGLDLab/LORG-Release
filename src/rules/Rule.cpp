#include "Rule.h"
#include "boost/lexical_cast.hpp"

#include <cmath>


Rule::Rule(const int& llhs,const std::vector<int>& rrhs,const bool& llexical,const int& ccount,const double& pproba,bool mode) 
  : Production(llhs,rrhs,llexical), count(ccount)  , probability(pproba), log_mode(mode)
{
}


Rule::Rule(const Production& pprod,const int& ccount,const double& pproba, bool mode) 
  : Production(pprod), 
    count(ccount), probability(pproba), log_mode(mode)
{
}


Rule::~Rule()
{
}

void Rule::set_log_mode(bool m)
{
  if(m != log_mode) {

    if(m)
      probability = std::log(probability);
    else
      probability = std::exp(probability);

    log_mode = m;
  }
}



// format is: {lex|int} LHS RHS_1 ... RHS_n _ count proba
std::ostream& operator<<(std::ostream& out, const Rule& rule)
{
    out.precision( 22 ) ; 
  //  out.setf( std::ios::fmtflags(), std::ios::floatfield ) ; 
    
    return 
      out << static_cast<Production>(rule) << " _ "
	  << rule.count << " "
	  << (rule.log_mode ? rule.probability : std::log(rule.probability))
     // << std::exp(rule.probability);
      ;
    
}



