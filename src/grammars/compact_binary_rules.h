// -*- mode: c++ -*-
#ifndef _COMPACT_BINARY_RULES_H_
#define _COMPACT_BINARY_RULES_H_

#include <vector>

namespace compact_binary_rules {
  
  // this structure stores rules sharing attribute rhs1
  // begin and end are precomputed for faster loops  
  template<typename info>
  struct vector_rhs1 {
    int rhs1;
    typedef std::vector<info, std::allocator<info> > data;
    data rules;
    typename data::const_iterator _begin;
    typename data::const_iterator _end;

    vector_rhs1() : rhs1(), rules(), _begin(), _end() {}
  };
  
  // this structure stores vector_rhs1 sharing attribute rhs0
  // begin and end are precomputed for faster loops
  template<typename info>
  struct vector_rhs0 {
    int rhs0;
    typedef std::vector< vector_rhs1<info>, std::allocator< vector_rhs1<info> > > data;
    data vrhs1;
    typename data::const_iterator _begin;
    typename data::const_iterator _end;

    vector_rhs0() : rhs0(), vrhs1(), _begin(), _end() {}
  };
  
  // this structure stores vector_rhs0
  // begin and end are precomputed for faster loops
  template<typename info>
  struct vector_brules {
    typedef std::vector< vector_rhs0<info>, std::allocator< vector_rhs0<info> > > data;
    data vrhs0;
    typename data::const_iterator _begin;
    typename data::const_iterator _end;
    
    vector_brules() : vrhs0(), _begin(), _end() {}

    template<class BinaryRule> 
    static vector_brules * convert(const std::vector<BinaryRule, std::allocator<BinaryRule> >&);
  };
  
}
#endif /* _COMPACT_BINARY_RULES_H_ */
