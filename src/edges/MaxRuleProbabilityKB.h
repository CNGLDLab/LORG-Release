// -*- mode: c++ -*-
#ifndef _MAXRULEPROBABILITYKB_H_
#define _MAXRULEPROBABILITYKB_H_

#include "PackedEdgeProbability.h"
#include "PackedEdge.h"
#include "maxrule_functions.h"

class MaxRuleProbabilityKB
{
public:
  typedef std::vector<packed_edge_probability> heap_type;
  typedef PackedEdge<MaxRuleProbabilityKB> Edge;
  typedef PCKYAllCell<Edge> Cell;
  private:

  heap_type candidates;
  heap_type derivations;


  static double log_normalisation_factor;
  static unsigned size;
public:
  MaxRuleProbabilityKB() :  candidates(), derivations() {candidates.reserve(50);};
  ~MaxRuleProbabilityKB() {};

  static void set_size(unsigned k)
  {size = k;}

  static void set_log_normalisation_factor(double lnf) {log_normalisation_factor = lnf;};

  const packed_edge_probability& get(unsigned idx) const
  {return derivations[idx];}
  packed_edge_probability& get(unsigned idx)
  {return derivations[idx];}


  void update(const AnnotationInfo& a, const LexicalPackedEdgeDaughters& dtr);

  void update(const AnnotationInfo& a, const UnaryPackedEdgeDaughters<Cell>& dtr);

  void update(const AnnotationInfo& a, const BinaryPackedEdgeDaughters<Cell>& dtr);


  void finalize();
  void find_succ(Edge*,packed_edge_probability& pep, bool licence_unaries);
  void extend_derivation(Edge*, unsigned, bool) ;

  unsigned n_deriv() const {return derivations.size();};

  bool has_solution(unsigned i) const {return i <derivations.size();}

private:
  struct test_helper
  {
    const packed_edge_probability& pep;
    test_helper(const packed_edge_probability& p) : pep(p) {};

    bool operator()(const packed_edge_probability& p)
    {return (p.probability == pep.probability) //|| (p.dtrs == pep.dtrs)
        ;}
  };
};




double MaxRuleProbabilityKB::log_normalisation_factor = 0;
unsigned MaxRuleProbabilityKB::size = 0;


void MaxRuleProbabilityKB::update(const AnnotationInfo& a, const LexicalPackedEdgeDaughters& dtr)
{
  const LexicalRuleC2f* rule = dtr.get_rule();
  assert(rule != NULL);

  packed_edge_probability pep;
  pep.probability = maxrule_function::update_maxrule_probability(a, rule, log_normalisation_factor);

  //  std::cout << pep.probability << std::endl;
  assert(pep.probability <=0);

  pep.dtrs = &dtr;

  candidates.push_back(pep);

  if(derivations.empty())
     derivations.push_back(pep);
  else if(pep.probability > derivations[0].probability)
      derivations[0] = pep;
}

void MaxRuleProbabilityKB::update(const AnnotationInfo& a, const UnaryPackedEdgeDaughters<Cell>& dtr)
{
  packed_edge_probability pep;
  pep.dtrs = &dtr;
  //  std::cout << "before ump" << std::endl;
  pep.probability= maxrule_function::update_maxrule_probability<Edge>(a, dtr, log_normalisation_factor);

  //  std::cout << pep.probability << std::endl;
  assert(pep.probability <=0);

  candidates.push_back(pep);

  if(derivations.empty())
       derivations.push_back(pep);
  else if(pep.probability > derivations[0].probability)
       derivations[0] = pep;

}

void MaxRuleProbabilityKB::update(const AnnotationInfo& a, const BinaryPackedEdgeDaughters<PCKYAllCell<PackedEdge<MaxRuleProbabilityKB> > >& dtr)
{
  packed_edge_probability pep;
  pep.dtrs = &dtr;

  pep.probability= maxrule_function::update_maxrule_probability<Edge>(a, dtr, log_normalisation_factor);

  //  std::cout << candidates.size() << std::endl;
  //  std::cout << pep.probability << std::endl;
  //  std::cout << pep.dtrs << std::endl;


  //  std::cout << pep.probability << std::endl;
  assert(pep.probability <=0);

  candidates.push_back(pep);


  if(derivations.empty())
    derivations.push_back(pep);
  else if(pep.probability > derivations[0].probability)
    derivations[0] = pep;
}

struct gt_pep
{
  bool operator()(const packed_edge_probability& p1, const packed_edge_probability& p2) const
  {
    return p1 > p2;
  }
};



void MaxRuleProbabilityKB:: finalize()
{
  //  std::cout << "size candidates: " << candidates.size() << std::endl;

  if(!candidates.empty()) {
    if(candidates.size() > size) {

      // std::cout << "BEFORE NTH " << size << std::endl;
      // for (unsigned i = 0; i < candidates.size(); ++i)
      //   {
      //     std::cout << candidates[i].probability << " ";

      //     if(candidates[i].dtrs->is_lexical())
      //       std::cout << *(static_cast<const LexicalPackedEdgeDaughters*>(candidates[i].dtrs)->get_rule());
      //     if(candidates[i].dtrs->is_binary())
      //       std::cout << *(static_cast<const BinaryPackedEdgeDaughters*>(candidates[i].dtrs)->get_rule());
      //     if(!candidates[i].dtrs->is_binary() && !candidates[i].dtrs->is_lexical())
      //       std::cout << *(static_cast<const UnaryPackedEdgeDaughters*>(candidates[i].dtrs)->get_rule());
      //     std::cout << std::endl;
      //   }

      std::nth_element(candidates.begin(),candidates.begin()+size,candidates.end(), gt_pep());
      candidates.resize(size);

      //       std::cout << "AFTER NTH " << size << std::endl;
      // for (int i = 0; i < candidates.size(); ++i)
      //   {
      //     std::cout << candidates[i].probability << " ";

      //     if(candidates[i].dtrs->is_lexical())
      //       std::cout << *(static_cast<const LexicalPackedEdgeDaughters*>(candidates[i].dtrs)->get_rule());
      //     if(candidates[i].dtrs->is_binary())
      //       std::cout << *(static_cast<const BinaryPackedEdgeDaughters*>(candidates[i].dtrs)->get_rule());
      //     if(!candidates[i].dtrs->is_binary() && !candidates[i].dtrs->is_lexical())
      //       std::cout << *(static_cast<const UnaryPackedEdgeDaughters*>(candidates[i].dtrs)->get_rule());
      //     std::cout << std::endl;
      //   }



    }
    std::make_heap(candidates.begin(),candidates.end());

    std::pop_heap(candidates.begin(),candidates.end());
    candidates.pop_back();
  }
}

void MaxRuleProbabilityKB::extend_derivation(Edge* edge, unsigned i, bool licence_unaries)
{
  if(derivations.size() == i) {
    return;
  }

  if(derivations.size() > 0) {

    packed_edge_probability& last = derivations[derivations.size() -1];

    //    std::cout << "last.probability " << last.probability << std::endl;

    assert(last.probability <= 0);

    find_succ(edge,last,licence_unaries);
    //    std::cout << "after find_succ" << std::endl;
  }

  if (!candidates.empty()) {

    //get next element from the candidatesidates and append it to derivations
    pop_heap(candidates.begin(),candidates.end());
    derivations.push_back(candidates.back());
    candidates.pop_back();

    //    std::cout << "in edge " << edge << " there are " << derivations.size() << " derivations." << std::endl;

  }



#ifndef NDEBUG
   for(unsigned j = 0; j < derivations.size(); ++j) {
  //   std::cout << "derivations " << j << ": " << derivations[j].probability << " " ;
  //   if(!(derivations[j].dtrs)) { std::cout << "NULL"; }
  //   else {
  //     if(derivations[j].dtrs->is_lexical())
  //       std::cout << *(static_cast<const LexicalPackedEdgeDaughters*>(derivations[j].dtrs)->get_rule());
  //     if(derivations[j].dtrs->is_binary())
  //       std::cout << *(static_cast<const BinaryPackedEdgeDaughters*>(derivations[j].dtrs)->get_rule());
  //     if(!derivations[j].dtrs->is_binary() && !derivations[j].dtrs->is_lexical())
  //       std::cout << *(static_cast<const UnaryPackedEdgeDaughters*>(derivations[j].dtrs)->get_rule());
  //   }
  //   std::cout << std::endl;

    assert(derivations[j].probability <= 0);
  }
#endif

}

void MaxRuleProbabilityKB::find_succ(PackedEdge<MaxRuleProbabilityKB>* edge, packed_edge_probability& pep, bool licence_unaries)
{
  typedef PackedEdge<MaxRuleProbabilityKB> P;

  if(pep.dtrs->is_lexical())  { return;}
  // binary -> extend left and right daughters
  if(pep.dtrs->is_binary()) {
    const P::BinaryDaughters* d = static_cast<const P::BinaryDaughters*>(pep.dtrs);

    //extend to the left
    unsigned left_pos = d->get_rule()->get_rhs0();
    P* left  = (*(d->left_daughter()))[left_pos];
    unsigned nextleft = pep.left_index + 1;
    left->extend_derivation(nextleft+1,true);

    // we haven't reached the expected number of solutions
    if(nextleft < left->get_best().n_deriv()) {

      packed_edge_probability p(pep);
      p.left_index = nextleft;
      p.probability = maxrule_function::update_maxrule_probability<Edge>(edge->get_annotations(), *d, log_normalisation_factor, p.left_index, p.right_index);

      assert(p.probability <= 0);

      //      std::cout << p.probability << std::endl;

      // TODO : Find a proper way to remove duplicates !
      if (std::find_if(candidates.begin(), candidates.end(), test_helper(p)) == candidates.end()) {
        candidates.push_back(p);
        push_heap(candidates.begin(), candidates.end());
      }

    }

    //extend to the right
    unsigned right_pos = d->get_rule()->get_rhs1();
    P* right = (*(d->right_daughter()))[right_pos];
    unsigned nextright = pep.right_index + 1;

    right->extend_derivation(nextright+1,true);

    if(nextright < right->get_best().n_deriv()) {
      //        std::cout << "bin extending on the right" << std::endl;


      packed_edge_probability p(pep);
      p.right_index = nextright;
      p.probability = maxrule_function::update_maxrule_probability<Edge>(edge->get_annotations(), *d, log_normalisation_factor, p.left_index, p.right_index);

      assert(p.probability <= 0);

      //      std::cout << p.probability << std::endl;

      if(std::find_if(candidates.begin(), candidates.end(), test_helper(p)) == candidates.end()){
        candidates.push_back(p);
        push_heap(candidates.begin(), candidates.end());
      }
    }
  }

  //unary
  else {
    if(!licence_unaries) return;

    //      std::cout << "unary case" << std::endl;

    const P::UnaryDaughters* d = static_cast<const P::UnaryDaughters*>(pep.dtrs);

    //        std::cout << * d->get_rule() << std::endl;


    //extend to the left
    unsigned left_pos = d->get_rule()->get_rhs0();
    P* left  = (*(d->left_daughter()))[left_pos];
    unsigned nextleft = pep.left_index + 1;

    left->extend_derivation(nextleft+1, false);

    if(nextleft < left->get_best().n_deriv() ) {
      //        std::cout << "un extending" << std::endl;
      packed_edge_probability p(pep);
      p.left_index = nextleft;
      p.probability = maxrule_function::update_maxrule_probability<Edge>(edge->get_annotations(), *d, log_normalisation_factor, p.left_index);

      assert(p.probability <= 0);

      //      std::cout << p.probability << std::endl;


      if(std::find_if(candidates.begin(), candidates.end(), test_helper(p)) == candidates.end()){
        candidates.push_back(p);
        push_heap(candidates.begin(), candidates.end());
      }
    }
  }
}



#endif /* _MAXRULEPROBABILITYKB_H_ */
