#include "Feature.h"

template<unsigned n>
struct rule_feature : public function_feature
{
  virtual std::string operator()(PtbPsTree::const_depth_first_iterator& cdfi, const PtbPsTree&) const
  {
    //    std::cout << "entering" << std::endl;
    typedef PtbPsTree::const_depth_first_iterator const_iterator;
    std::stringstream ss(std::stringstream::in |std::stringstream::out);
    const_iterator ancestor_iter = cdfi;
    if(ancestor_iter->leaf())
      return "";

    bool fail = false;

    for (unsigned i = 0; i <= n; ++i)
      {
        ss << *ancestor_iter << "^";
        //        std::cout << ss.str() << std::endl;
        if(ancestor_iter->has_mother())
          ancestor_iter.up();
        else
          fail = true;
      }

    if(fail) {
      //      std::cout << "HERE1" << std::endl;
      return "";
    }

    ancestor_iter = cdfi;
    ancestor_iter.down_first();
    if(ancestor_iter->leaf()) {
      //      std::cout << "HERE2" << std::endl;
      return "";
    }

    ss << *ancestor_iter << "@";
    while(ancestor_iter->has_right_sister()) {
      ancestor_iter.right();
      ss << *ancestor_iter << "@";
    }

    //    std::cout << "rule is: " << ss.str() << std::endl;
    return ss.str();
  }

};

RuleFeature::RuleFeature() : Feature("Rule0", new rule_feature<0>()) {};
RuleFeature::~RuleFeature() {};

RuleParentFeature::RuleParentFeature() : Feature("Rule1", new rule_feature<1>()) {};
RuleParentFeature::~RuleParentFeature() {};

RuleGrandParentFeature::RuleGrandParentFeature() : Feature("Rule2", new rule_feature<2>()) {};
RuleGrandParentFeature::~RuleGrandParentFeature() {};


template<unsigned n>
struct ngram_feature : public function_feature
{
  virtual std::string operator()(PtbPsTree::const_depth_first_iterator& cdfi, const PtbPsTree&) const
  {
    //    std::cout << "entering" << std::endl;
    typedef PtbPsTree::const_depth_first_iterator const_iterator;
    std::stringstream ss(std::stringstream::in |std::stringstream::out);
    const_iterator copy = cdfi;


    //don't process terminals and preterminals
    if(copy->leaf())
      return "";
    else {
      const_iterator copycopy = copy;
      copycopy.down_first();
      if(copycopy->leaf())
        return "";
    }

    if(!copy->has_left_sister())
      ss << "BEGIN" << "_";

    ss << *copy << "_";

    for (unsigned i = 1; i < n; ++i)
      {
        if(copy->has_right_sister()) {
          copy.right();
          ss << *copy << "_";
        }
        else {
          ss << "STOP_";
        }
      }

    return ss.str();
  }
};

BiGramNodeFeature::BiGramNodeFeature() : Feature("BN", new ngram_feature<2>()) {};
BiGramNodeFeature::~BiGramNodeFeature() {};


struct heavy_function : function_feature
{
  virtual std::string operator()(PtbPsTree::const_depth_first_iterator& cdfi, const PtbPsTree& tree) const
  {
    typedef PtbPsTree::const_depth_first_iterator const_iterator;
    typedef PtbPsTree::const_leaf_iterator const_leaf_iterator;

    std::stringstream ss(std::stringstream::in |std::stringstream::out);
    const_iterator copy = cdfi;


    //don't process terminals and preterminals
    if(copy->leaf()) {
      return "";
    }
    else {
      const_iterator copycopy = copy;
      copycopy.down_first();
      if(copycopy->leaf()) {
        return "";
      }
    }

    unsigned n = tree.subtree(cdfi).number_of_leaves();
    // bin 1,2,3,4,5, [6,10[, >=10
    if(n >= 10) n = 10;
    else if(n>5) n = 6;

    //go to the last daughter
    do {
      copy.down_last();
    }
    while(!copy->leaf());

    const_leaf_iterator li(&tree, copy.get_node());
    ++li;

    std::string next("NOPUNCT");
    if (li == tree.lend())
      next = "END";
    else {
      const_iterator pos(&tree, li.get_node());
      pos.up();
      if((*pos).find_first_of("!?.,;:") || (*pos) == "PONCT")
        next = "PONCT";
    }

    ss << *cdfi << "_" << n << "_" << next ;



    return ss.str();
  }
};

HeavyFeature::HeavyFeature() : Feature("H", new heavy_function()) {};
HeavyFeature::~HeavyFeature() {};



template<unsigned nfirst, unsigned nlast>
struct neighbours_function : function_feature
{
  virtual std::string operator()(PtbPsTree::const_depth_first_iterator& cdfi,
                                 const PtbPsTree& tree) const
  {
    typedef PtbPsTree::const_depth_first_iterator const_iterator;
    typedef PtbPsTree::const_leaf_iterator const_leaf_iterator;

    std::stringstream ss(std::stringstream::in |std::stringstream::out);
    const_iterator copy = cdfi;
    const_iterator copycopy = copy;

    ss << *cdfi << "^";

    //don't process terminals and preterminals
    if(copy->leaf()) {
      return "";
    }
    else {
      copycopy.down_first();
      if(copycopy->leaf()) {
        return "";
      }
    }

    unsigned n = tree.subtree(cdfi).number_of_leaves();
    // bin 1,2,3,4,5, [6,10[, >=10
    if(n >= 10) n = 10;
    else if(n>5) n = 6;

    ss << n << "^";

    //go to the first daughter
    do {
      copy.down_first();
    }
    while(!copy->leaf());



    //go to the last daughter
    copycopy = cdfi;
    do {
      copycopy.down_last();
    }
    while(!copycopy->leaf());

    const_leaf_iterator lfirst(&tree, copy.get_node());
    const_leaf_iterator llast(&tree,  copycopy.get_node());



    for (unsigned i = 0; i < nfirst; ++i)
      {
        const_iterator d = lfirst;
        d.up();
        ss << *d << "^";
        ++lfirst;
        if(lfirst == tree.lend())
          break;
      }

    ss << "<>";

    for (unsigned i = 0; i < nlast; ++i)
      {
        const_iterator d = llast;
        d.up();
        ss << *d << "^";
        --llast;
        if(llast == tree.lend())
          break;
      }

    return ss.str();
  }
};

NeighboursFeature::NeighboursFeature() : Feature("N10", new neighbours_function<1,0>()) {};
NeighboursFeature::~NeighboursFeature() {};

NeighboursExtFeature::NeighboursExtFeature() : Feature("N21", new neighbours_function<2,1>()) {};
NeighboursExtFeature::~NeighboursExtFeature() {};


template<unsigned nancestors, bool word>
struct word_function : function_feature
{
  virtual std::string operator()(PtbPsTree::const_depth_first_iterator& cdfi,
                                 const PtbPsTree&) const
  {
    typedef PtbPsTree::const_depth_first_iterator const_iterator;
    typedef PtbPsTree::const_leaf_iterator const_leaf_iterator;


    if(cdfi->leaf()) {
      const_iterator i(cdfi);
      std::stringstream ss(std::stringstream::in |std::stringstream::out);

      if(word)
        ss << *i;
      else
        ss << "_";

      for (unsigned j = 0; j < nancestors; ++j)
        {
          if(i->has_mother()) {
            i.up();
            ss << "^" << *i;
          }
          else {
            ss << "^" << "TOP";
            break;
          }
        }
      return ss.str();
    }
    else return "";
  }
};

WordFeature2::WordFeature2() : Feature("W2", new word_function<2,true>()) {};
WordFeature2::~WordFeature2() {};

WordFeature3::WordFeature3() : Feature("W3", new word_function<3,true>()) {};
WordFeature3::~WordFeature3() {};

WordFeatureGen2::WordFeatureGen2() : Feature("WG2", new word_function<2,false>()) {};
WordFeatureGen2::~WordFeatureGen2() {};

WordFeatureGen3::WordFeatureGen3() : Feature("WG3", new word_function<3,false>()) {};
WordFeatureGen3::~WordFeatureGen3() {};
