#include "compact_binary_rules.h"

#include <algorithm>
#include <stdexcept>

#include <boost/spirit/home/phoenix/bind/bind_member_function.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_container.hpp>

using namespace compact_binary_rules;

using namespace boost::phoenix;


template<class BinaryRule, typename info>
info transform(const BinaryRule&)
{
  throw std::runtime_error("compact_binary_rules::transform should be specialised");
}


template<class BinaryRule, typename info>
void build_vector_rhs1(const std::vector<const BinaryRule*, std::allocator<const BinaryRule*> >& pre,
		       vector_rhs1<info>& result )
{
  for(typename std::vector<const BinaryRule*, std::allocator<const BinaryRule*> >::const_iterator viter = pre.begin();
      viter != pre.end(); ++viter) {
    result.rules.push_back(transform<BinaryRule,info>(*(*viter)));
  }
  
  result._begin = result.rules.begin();
  result._end = result.rules.end();
  result.rhs1 = pre.front()->get_rhs1();
}


  template<class BinaryRule, typename info>
    void build_vector_rhs0(const std::vector<std::vector<const BinaryRule*, std::allocator<const BinaryRule*> > ,
			   std::allocator<std::vector<const BinaryRule*, std::allocator<const BinaryRule*> > > >& pre,
			 vector_rhs0<info>& result)
{
  result.vrhs1.resize(pre.size());
  
  for(unsigned i = 0; i < pre.size(); ++i)
    build_vector_rhs1<BinaryRule,info>(pre[i],result.vrhs1[i]);
  
  result._begin = result.vrhs1.begin();
  result._end = result.vrhs1.end();
  result.rhs0 = pre.front().front()->get_rhs0();
}


  // I had to specify allocators for some reason I dont know
  // the first argument is just a vector of vector of vector of BinaryRule pointers
  template<class BinaryRule, typename info>
  void build_vector_brules(const std::vector<std::vector<std::vector<const BinaryRule*, std::allocator< const BinaryRule*> > ,
			   std::allocator< std::vector<const BinaryRule*, std::allocator<const BinaryRule*> > > >,
			   std::allocator< std::vector<std::vector<const BinaryRule*, std::allocator<const BinaryRule*> > ,
			   std::allocator< std::vector<const BinaryRule*, std::allocator<const BinaryRule*> > > > > >& pre,
			     vector_brules<info>& result)
{
  result.vrhs0.resize(pre.size());
  
  for(unsigned i = 0; i < pre.size(); ++i)
    build_vector_rhs0<BinaryRule,info>(pre[i],result.vrhs0[i]);
  
  result._begin = result.vrhs0.begin();
  result._end = result.vrhs0.end();
}

template<typename info>
template<class BinaryRule>
vector_brules<info> *
vector_brules<info>::convert(const std::vector<BinaryRule,  std::allocator<BinaryRule> >& binary_rules)
{
  typedef std::vector<BinaryRule, std::allocator<BinaryRule> > vbr;
  typedef std::vector<const BinaryRule*, std::allocator<const BinaryRule*> > vbrp;
  typedef std::vector<vbrp,std::allocator<vbrp> > vvbrp;
  typedef std::vector<vvbrp,std::allocator<vvbrp> > vvvbrp;
  
  //partition brules according to their rhs0  
  vvbrp brulesranked;
  
  for(typename vbr::const_iterator brules_itr = binary_rules.begin();
      brules_itr != binary_rules.end(); ++brules_itr) {

    int rhs0 = brules_itr->get_rhs0();
    
    typename vvbrp::iterator br_itr = std::find_if(brulesranked.begin(),brulesranked.end(),
						   bind(&BinaryRule::get_rhs0,front(arg_names::arg1)) == rhs0
						   );
    
    if(br_itr != brulesranked.end())
      br_itr->push_back(&(*brules_itr));
    else 
      brulesranked.push_back(vbrp(1,&(*brules_itr)));
  }
  
  vvvbrp prebrulesrankedranked;
  prebrulesrankedranked.resize(brulesranked.size());
  
  //in each partition, repartition brules according to their rhs1
  for(unsigned i = 0; i < brulesranked.size();++i) {
    vbrp same_rhs0_vect = brulesranked[i];
    for(typename vbrp::const_iterator same_rhs0_vect_itr = same_rhs0_vect.begin();
	same_rhs0_vect_itr != same_rhs0_vect.end();++same_rhs0_vect_itr) {
      
      int rhs1 = (*same_rhs0_vect_itr)->get_rhs1();
      
      typename vvbrp::iterator itr = std::find_if(prebrulesrankedranked[i].begin(),
						 prebrulesrankedranked[i].end(),
						 bind(&BinaryRule::get_rhs1,front(arg_names::arg1)) == rhs1
						 );
      
      if(itr != prebrulesrankedranked[i].end())
	itr->push_back(*same_rhs0_vect_itr);
      else
	prebrulesrankedranked[i].push_back(vbrp(1,*same_rhs0_vect_itr));
    }
  }

  
  vector_brules<info> * res = new vector_brules<info>();
  
  build_vector_brules<BinaryRule,info>(prebrulesrankedranked,*res);

  return res;
}



