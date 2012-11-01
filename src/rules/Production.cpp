#include "Production.h"
#include "utils/SymbolTable.h"

#ifdef DEBUG
#include "boost/lexical_cast.hpp"
#else
#include <sstream>
#endif


Production::Production(const int llhs,const std::vector<int>& rrhs,const bool llexical) 
  : MetaProduction(llhs), rhs(rrhs), lexical(llexical) {}

Production::~Production()
{
}

// format is: {lex|int} LHS RHS_1 ... RHS_n
std::ostream& operator<<(std::ostream& os, const Production& prod)
{
  if (prod.lexical)
    os << "lex ";
  else
    os << "int ";
  
  os << SymbolTable::instance_nt().translate(prod.lhs);
  
  for(std::vector<int>::const_iterator it=prod.rhs.begin();
      it != prod.rhs.end(); ++it)
    {
      if (prod.lexical)
	os <<" " << SymbolTable::instance_word().translate(*it);
      else
	os <<" " << SymbolTable::instance_nt().translate(*it);
    };
  
  return os;
}

#ifdef DEBUG
std::string create_debug_name(std::vector<int> rhs,bool right)
{
  std::string new_name;

  if(right) {
    for(std::vector<int>::iterator i = rhs.begin(); i != rhs.end() - 1 ; ++i)
      new_name += "(" + SymbolTable::instance_nt()->translate(*i) + ")";
  }
  else{ //left
    for(std::vector<int>::iterator i = rhs.begin()+1; i != rhs.end() ; ++i)
      new_name += "(" + SymbolTable::instance_nt()->translate(*i) + ")";
  }
  
  new_name = "["+new_name+"]";

  return new_name;
}
#endif

std::string create_name(std::vector<int> rhs, bool right)
{
  // to name newly created non-terminals
  static int cpt = 0; // static useful ? (yes if we call this method twice...)
  static std::map<std::vector<int>,int> record;
  
  std::ostringstream new_name;
  std::map<std::vector<int>, int>::iterator ans;
  std::vector<int> keyvector;
  
  if(right)
    keyvector.insert(keyvector.end(),rhs.begin(),rhs.end()-1);
  else // left
    keyvector.insert(keyvector.end(),rhs.begin()+1,rhs.end());
  
  if ((ans=record.find(keyvector)) == record.end()) {
    record[keyvector] = ++cpt;
    new_name << "[" << cpt << "]";
  }
  else 
    new_name << "[" << ans->second  << "]";  
  
  return new_name.str();

}

// //not used anymore
// std::pair<Production,Production> Production::binarize_simple(bool right) const
// {
//   std::vector<int> rhs = get_rhs();
//   std::vector<int> rhsprime(2,0);
//   std::vector<int> rhssecond;
  
// #ifndef DEBUG
//   std::string new_name = create_name(rhs,right);
// #else
//   std::string new_name = create_debug_name(rhs,right);
// #endif    
//   int new_id = SymbolTable::instance_nt()->insert(new_name);
  

//   if(right) {
//     rhsprime[0] = new_id;
//     rhsprime[1] = rhs[rhs.size()-1];
//   }
//   else{ 
//     rhsprime[0] = rhs.front();
//     rhsprime[1] = new_id;
//   }
  
//   Production rprime(get_lhs(),rhsprime,false);
  
//   if(right)
//     rhssecond.insert(rhssecond.end(),rhs.begin(),rhs.end()-1);
//   else
//     rhssecond.insert(rhssecond.end(),rhs.begin()+1,rhs.end());
  
  
//   Production rsecond(new_id,rhssecond,false);
  
//   std::pair<Production,Production> result(rprime,rsecond);
//   return result;
// };


// std::pair<Production,Production> Production::binarize_right_simple() const
// {
//   return binarize_simple(true);
// }

// std::pair<Production,Production> Production::binarize_left_simple() const
// {
//   return binarize_simple(false);
// }


// std::list<Production> Production::binarize(std::pair<Production,Production> (Production::*funct)() const) const
// {
//   std::list<Production> accumulator;
//   Production p = *this;
  
//   while(p.get_rhs().size() > 2) {
//     std::pair<Production,Production> pp = (p.*funct)();
//     accumulator.push_back(pp.first);
//     p = pp.second;
//   }
//   accumulator.push_back(p);

//   return accumulator;
// }

// std::list<Production> Production::binarize_right() const
// {
//   return binarize(&Production::binarize_right_simple);
// }

// std::list<Production> Production::binarize_left() const
// {
//   return binarize(&Production::binarize_left_simple);
// }
