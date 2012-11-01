// -*- mode: c++ -*-
#ifndef _GRAMMARANNOTATED_H_
#define _GRAMMARANNOTATED_H_

#include "Grammar.h"


#include <boost/unordered_map.hpp>
#include "AnnotatedLabelsInfo.h"
#include "utils/SymbolTable.h"

#include <numeric>


#include "rules/LexicalRuleC2f.h"
#include "rules/URuleC2f.h"
#include "rules/BRuleC2f.h"


#include "utils/Tree.h"

#define uomap boost::unordered_map


typedef std::pair< int, unsigned> asymb;
typedef uomap<asymb, uomap< asymb, asymb> > PathMatrix;

template <typename Bin, typename Un, typename Lex>
class GrammarAnnotated : public Grammar<Bin,Un,Lex>, public AnnotatedContents
{
public:
  GrammarAnnotated();
  virtual ~GrammarAnnotated() {};

  void init();
  void set_logmode();
  void remove_unlikely_annotations_all_rules(double threshold);

  const PathMatrix&
  get_unary_decoding_paths() const;

  GrammarAnnotated(const std::string& filename);

  GrammarAnnotated * create_projection(const std::vector<std::vector<double> >& expected_counts,
				       const std::vector<std::vector<std::vector<unsigned> > >& annotation_mapping) const;

  void
  compute_transition_probabilities(uomap<int, uomap<unsigned, uomap<int, uomap<unsigned, double > > > >&) const;

  std::vector<double> compute_priors() const;

  const std::vector< Tree<unsigned> >&  get_history_trees() const;


private:
    /**
     \brief create new unary rules encoding unary chains
     \param decoding_paths the matrix of best paths for decoding after parsing
     \param max_path_length the maximum length of chains to encode
     \param unaries the unary_rules to enrich with unary chains (modified)
   */
  void compute_unary_chains(PathMatrix& decoding_paths,
			    unsigned max_path_length,
			    std::vector<Un>& unaries);

  GrammarAnnotated (const std::vector<std::vector<double> >& conditional_probabilities,
		    const std::vector<std::vector<std::vector<unsigned> > >& mapping,
		    const std::vector<Bin>& old_binary_rules,
		    const std::vector<Un>& old_unary_rules,
		    const std::vector<Lex>& old_lexical_rules);


  // for unary chains encoding/decoding
  PathMatrix viterbi_decoding_paths;

  // history of annotations (trees of numbers)
  std::vector< Tree<unsigned> > history_trees;

};

template <typename Bin, typename Un, typename Lex>
  GrammarAnnotated<Bin,Un,Lex>::GrammarAnnotated()
  : Grammar<Bin,Un,Lex>(), AnnotatedContents(),
  viterbi_decoding_paths()
{}

namespace {
  template<typename Lex>
  std::vector<Lex> compute_new_unknowns(const std::vector<Lex>& lexical_rules)
  {
    typedef std::vector<Lex> LexVect;

    static int unknown_id = SymbolTable::instance_word().insert(LorgConstants::token_unknown);

    //map from tags to lexical rules
    uomap<int, Lex> tags2rules;

    std::set<int> tags_w_unk;

    for(typename LexVect::const_iterator i(lexical_rules.begin()); i != lexical_rules.end(); ++i) {
      if (i->get_rhs0() == unknown_id) {
	tags_w_unk.insert(i->get_lhs());
      }
    }


    for(typename LexVect::const_iterator i(lexical_rules.begin()); i != lexical_rules.end(); ++i) {

      const std::string& word = SymbolTable::instance_word().get_label_string(i->get_rhs0());


      //lexical to word signature
      if(!tags_w_unk.count(i->get_lhs()) && word.length() >= 3 && word.substr(0,3) == "UNK") {

      if(!tags2rules.count(i->get_lhs())) {
	Lex copy(*i);
	copy.set_rhs0(unknown_id);
	tags2rules.insert(std::make_pair(i->get_lhs(),copy));
      }
      else {

	Lex& old_rule = tags2rules.find(i->get_lhs())->second;
	std::vector<double>& old_probabilities = old_rule.get_probability();


	if(i->get_probability().size() > old_probabilities.size())
	  old_probabilities.resize(i->get_probability().size());

	for(unsigned a = 0; a < i->get_probability().size(); ++a) {
	  old_probabilities[a] += i->get_probability()[a];
        }



      }
      }
    }

    std::vector<Lex> result;
    for(typename uomap<int,Lex>::const_iterator i(tags2rules.begin()) ; i != tags2rules.end(); ++i) {
      result.push_back(i->second);
    }

    return result;
  }

  struct remove_unlikely_helper_struct
  {
    const double& threshold;
    remove_unlikely_helper_struct(const double& threshold_)
      : threshold(threshold_) {};

    template<class T>
    void operator() (T& rule) const {rule.remove_unlikely_annotations(threshold);}
  };

  ///  a structure that compares unary rules (according to their symbolic part)
// A1 -> B1 (p1,...,pn) == A2 -> B2 (q1,...,qn) <=> A1 == A2 && B1 == B2
// used when looking for unary chains
template <typename T>
struct compare_urule
{
  const T& candidate;
  compare_urule(const T& r) : candidate(r) {}
  bool operator()(const T& test) const
  {return candidate.get_lhs() == test.get_lhs() && candidate.get_rhs0() == test.get_rhs0();}
};

}

template<typename Bin, typename Un, typename Lex>
void GrammarAnnotated<Bin,Un,Lex>::init()
{
  Grammar<Bin,Un,Lex>::init();

  // std::vector<Lex> nl = compute_new_unknowns(Grammar<Bin,Un,Lex>::lexical_rules);
  // Grammar<Bin,Un,Lex>::lexical_rules.insert(Grammar<Bin,Un,Lex>::lexical_rules.end(),nl.begin(),nl.end());

  //  std::clog << "uncompacting" << std::endl;

  for(typename std::vector<Un>::iterator urule_it(Grammar<Bin,Un,Lex>::unary_rules.begin());
      urule_it != Grammar<Bin,Un,Lex>::unary_rules.end(); ++urule_it) {
    urule_it->get_probability().resize(label_annotations.get_number_of_annotations(urule_it->get_lhs()));
    urule_it->uncompact(label_annotations.get_number_of_annotations(urule_it->get_rhs0()));
  }

  //  std::clog << "computing unary chains" << std::endl;

  compute_unary_chains(viterbi_decoding_paths,1, Grammar<Bin,Un,Lex>::unary_rules);

  // std::for_each(Grammar<Bin,Un,Lex>::unary_rules.begin(), Grammar<Bin,Un,Lex>::unary_rules.end(),
  //  		std::mem_fun_ref(&URule::compact));


  //  std::clog << "leaving init" << std::endl;

}

template<typename Bin, typename Un, typename Lex>
void GrammarAnnotated<Bin,Un,Lex>::compute_unary_chains(PathMatrix& decoding_paths,
							unsigned max_path_length,
							std::vector<Un>& unaries)
{
  bool update;

  unsigned path_length = 0;

  do {
    ++path_length;
    update = false;

    // vector of (Urule,lhs,matrix of annotations)
    // the rule is the newly created rule obtained by combining 2 unary rules
    // the lhs is the intermediate symbol (ie B in A->B + B->C)
    // the matrix: M_ij is the best annotation for B to go from A_i to C_j
    // if M_ij is -1 it means that the path ends here
    typedef std::pair<Un,std::pair<int,std::vector< std::vector<int> > > > rule2label_paths;
    std::vector<rule2label_paths> new_rules_with_info;

    for(typename std::vector<Un>::const_iterator it1(unaries.begin()); it1 != unaries.end(); ++it1) {
      const Un& rule1 = *it1;

      //      std::clog << "Rule1: " << &rule1 << "\t" << rule1 << std::endl;

      for(typename std::vector<Un>::const_iterator it2(unaries.begin()); it2 != unaries.end(); ++it2) {
	const Un& rule2 = *it2;

        //std::clog << "Rule2: " <<&rule2 << "\t" << rule2 << std::endl;

	//composing A->B with B->C to create A->C
	if(rule2.get_lhs() != rule1.get_rhs0())
	  continue;

	//we don't want to create X->X
	if(rule2.get_rhs0() == rule1.get_lhs())
	  continue;

        if(SymbolTable::instance_nt().get_label_string(rule1.get_rhs0())[0] == 't')
          continue;

        //        std::clog << "path_length: " << path_length << "\tpRule1: " << &rule1 << "\t" << rule1 << "\tRule2: " << &rule2 << "\t" << rule2 << std::endl;



	//composing probabilities
	std::vector< std::vector<double> > probabilities(label_annotations.get_number_of_annotations(rule1.get_lhs()),
							 std::vector<double>(label_annotations.get_number_of_annotations(rule2.get_rhs0()),0.0));
	//annotation path
	std::vector< std::vector<int> > apath(label_annotations.get_number_of_annotations(rule1.get_lhs()),
					      std::vector<int>(label_annotations.get_number_of_annotations(rule2.get_rhs0()),-1));


	for(unsigned i = 0; i < probabilities.size(); ++i) {
	  for(unsigned j = 0; j < probabilities[i].size(); ++j) {
	    for(unsigned k = 0; k < rule1.get_probability()[i].size(); ++k) {

	      double candidate = rule1.get_probability(i,k) * rule2.get_probability(k,j);
	      // if(candidate > probabilities[i][j]) {
	      //   probabilities[i][j] = candidate;
	      //   apath[i][j] = k;
	      // }
	      probabilities[i][j] += candidate;
	      apath[i][j]=0;
	    }
	  }
	}

	Un composition(URule(rule1.get_lhs(), rule2.get_rhs0(),probabilities));

	// std::clog << "From:\n\t" << rule1 << "\n and:\n\t" << rule2
	// 	  << "\nI created:\n\t" << composition << std::endl;


	new_rules_with_info.push_back(std::make_pair(composition,std::make_pair(rule1.get_rhs0(),apath)));

      }
    }


    //    std::clog << "updating probs" << std::endl;

    //updating probabilities
    for(typename std::vector<rule2label_paths>::const_iterator it(new_rules_with_info.begin()); it != new_rules_with_info.end(); ++it){

      const Un& rule_candidate = it->first;
      int intermediate_symb = it->second.first;
      const std::vector< std::vector<int> >& apath = it->second.second;

      // doess A->C already exists ?
      typename std::vector<Un>::iterator old_rule_iter = std::find_if(unaries.begin(),unaries.end(),compare_urule<Un>(rule_candidate));

      //no
      // we add the new rule and the new paths
      if(old_rule_iter == unaries.end()) {
        //        std::clog << "one new rule: " << rule_candidate << std::endl;
	unaries.push_back(rule_candidate);

	for(unsigned i = 0; i < apath.size(); ++i)
	  for(unsigned j = 0; j < apath[i].size(); ++j)
	    decoding_paths[std::make_pair(rule_candidate.get_lhs(),i)][std::make_pair(rule_candidate.get_rhs0(),j)] = std::make_pair(intermediate_symb,apath[i][j]);

	update = true;
      }
      //yes
      // we update probabilities (and paths)
      //if the new ones are better
      else {
	// for(unsigned i = 0; i < label_annotations.get_number_of_annotations(rule_candidate.get_lhs()); ++i)
	//   for(unsigned j = 0; j < label_annotations.get_number_of_annotations(rule_candidate.get_rhs0()); ++j) {
	//     if(old_rule_iter->get_probability(i,j) < rule_candidate.get_probability(i,j)) {
	//       old_rule_iter->set_probability(i,j,rule_candidate.get_probability(i,j));
	//       decoding_paths[std::make_pair(rule_candidate.get_lhs(),i)][std::make_pair(rule_candidate.get_rhs0(),j)] = std::make_pair(intermediate_symb,apath[i][j]);
	//       update = true;
	//     }
	//   }


        //        std::clog << "updating: " << rule_candidate << std::endl;

	double sum_old = 0.0;
	double sum_candidate = 0.0;
	for(unsigned i = 0; i < label_annotations.get_number_of_annotations(rule_candidate.get_lhs()); ++i)
	  for(unsigned j = 0; j < label_annotations.get_number_of_annotations(rule_candidate.get_rhs0()); ++j) {
	    sum_old += old_rule_iter->get_probability(i,j);
	    sum_candidate += rule_candidate.get_probability(i,j);
	  }

	if(sum_candidate > sum_old) {
	  //	  std::clog << "old:\n" << *old_rule_iter << std::endl;
	  old_rule_iter->get_probability() = rule_candidate.get_probability();
	  //	  std::clog << "new:\n" << *old_rule_iter << std::endl;
	  for(unsigned i = 0; i < label_annotations.get_number_of_annotations(rule_candidate.get_lhs()); ++i)
	    for(unsigned j = 0; j < label_annotations.get_number_of_annotations(rule_candidate.get_rhs0()); ++j)
	      decoding_paths[std::make_pair(rule_candidate.get_lhs(),i)][std::make_pair(rule_candidate.get_rhs0(),j)] = std::make_pair(intermediate_symb,apath[i][j]);
	  update = true;
	}
      }
    }


  }
  while(update && path_length <= max_path_length);
}

template<typename Bin, typename Un, typename Lex>
void GrammarAnnotated<Bin,Un,Lex>::set_logmode()
{
  std::for_each(Grammar<Bin,Un,Lex>::binary_rules.begin(),
		Grammar<Bin,Un,Lex>::binary_rules.end(),
		std::mem_fun_ref(&Bin::set_logmode));
  std::for_each(Grammar<Bin,Un,Lex>::unary_rules.begin(),
		Grammar<Bin,Un,Lex>::unary_rules.end(),
		std::mem_fun_ref(&Un::set_logmode));
  std::for_each(Grammar<Bin,Un,Lex>::lexical_rules.begin(),
		Grammar<Bin,Un,Lex>::lexical_rules.end(),
		std::mem_fun_ref(&Lex::set_logmode));
}

template<typename Bin, typename Un, typename Lex>
void GrammarAnnotated<Bin,Un,Lex>::remove_unlikely_annotations_all_rules(double threshold)
{
  remove_unlikely_helper_struct remover(threshold);
  std::for_each(Grammar<Bin,Un,Lex>::binary_rules.begin(),
		Grammar<Bin,Un,Lex>::binary_rules.end(),
		remover);


  //  std::clog << "bin before: " << Grammar<Bin,Un,Lex>::binary_rules.size() << std::endl;
  Grammar<Bin,Un,Lex>::binary_rules.erase(std::remove_if(Grammar<Bin,Un,Lex>::binary_rules.begin(),
							 Grammar<Bin,Un,Lex>::binary_rules.end(),
							 std::mem_fun_ref(&Bin::is_empty)),
					  Grammar<Bin,Un,Lex>::binary_rules.end());
  //  std::clog << "bin after: " << Grammar<Bin,Un,Lex>::binary_rules.size() << std::endl;


  std::for_each(Grammar<Bin,Un,Lex>::unary_rules.begin(),
		Grammar<Bin,Un,Lex>::unary_rules.end(),
		remover);

  //  std::clog << "un before: " << Grammar<Bin,Un,Lex>::unary_rules.size() << std::endl;
  Grammar<Bin,Un,Lex>::unary_rules.erase(std::remove_if(Grammar<Bin,Un,Lex>::unary_rules.begin(),
							Grammar<Bin,Un,Lex>::unary_rules.end(),
							std::mem_fun_ref(&Un::is_empty)),
					 Grammar<Bin,Un,Lex>::unary_rules.end());
  //  std::clog << "un after: " << Grammar<Bin,Un,Lex>::unary_rules.size() << std::endl;


  std::for_each(Grammar<Bin,Un,Lex>::lexical_rules.begin(),
		Grammar<Bin,Un,Lex>::lexical_rules.end(),
		remover);

  //  std::clog << "lex before: " << Grammar<Bin,Un,Lex>::lexical_rules.size() << std::endl;
  Grammar<Bin,Un,Lex>::lexical_rules.erase(std::remove_if(Grammar<Bin,Un,Lex>::lexical_rules.begin(),
							  Grammar<Bin,Un,Lex>::lexical_rules.end(),
							  std::mem_fun_ref(&Lex::is_empty)),
					   Grammar<Bin,Un,Lex>::lexical_rules.end());
  //  std::clog << "lex after: " << Grammar<Bin,Un,Lex>::lexical_rules.size() << std::endl;

}


template<typename Bin, typename Un, typename Lex>
const PathMatrix& GrammarAnnotated<Bin,Un,Lex>::get_unary_decoding_paths() const
{
  return viterbi_decoding_paths;
}

template<typename Bin, typename Un, typename Lex>
const std::vector< Tree<unsigned> >& GrammarAnnotated<Bin,Un,Lex>::get_history_trees() const
{
  return history_trees;
}

namespace{


  // replace current mapping codomain with its image
  void extend_mapping(std::vector< std::vector<unsigned> >& acc, const std::vector<std::vector<unsigned> >& nexts)
  {
    if(acc.empty())
      acc = nexts;
    else {
      for(unsigned annot = 0; annot < acc.size(); ++annot) {
	std::vector<unsigned> new_results;
	for(std::vector<unsigned>::const_iterator i(acc[annot].begin()); i != acc[annot].end(); ++i)
	  new_results.insert(new_results.end(), nexts[*i].begin(), nexts[*i].end());

	acc[annot] = new_results;
      }
    }
  }


  // compute annotation mapping between two levels
  std::vector<std::vector<std::vector<unsigned> > > compute_mapping(unsigned from, unsigned to,
								    const std::vector< std::vector<std::vector< std::vector<unsigned> > > >& annot_descendants)
  {
    std::vector<std::vector<std::vector<unsigned> > > result(SymbolTable::instance_nt().get_symbol_count());
    unsigned begin = from;
    unsigned end = to;

    while(begin < end) {

      const std::vector<std::vector< std::vector<unsigned> > >& mapping_begin = annot_descendants[begin];

      for(unsigned nt = 0; nt < mapping_begin.size(); ++nt)

	extend_mapping(result[nt], mapping_begin[nt]);

      ++begin;
    }

    return result;
  }

std::vector<uomap<unsigned,unsigned> > invert_mapping(std::vector<std::vector<std::vector<unsigned> > > mapping)
{
  std::vector<uomap<unsigned,unsigned> > res(mapping.size());

  for (unsigned i = 0; i < res.size(); ++i)
    {
      for (unsigned j = 0; j < mapping[i].size(); ++j)
	{
	  for (std::vector<unsigned>::const_iterator it(mapping[i][j].begin()); it != mapping[i][j].end(); ++it)
	    {
	      res[i][*it] = j;
	    }
	}
    }

  return res;
}


 void calculate_expected_counts(uomap<int, uomap<unsigned, uomap<int, uomap<unsigned, double > > > >& trans,
                                const AnnotatedLabelsInfo& ali,
                                std::vector<std::vector<double> >& result)
{
  static int root_id = SymbolTable::instance_nt().get_label_id(LorgConstants::tree_root_name);
  unsigned n_nts = SymbolTable::instance_nt().get_symbol_count();

  result.resize(n_nts);
  result[root_id].resize(1);

  std::vector<std::vector<double> > temp(result);


  result[root_id][0] = 1;
  temp[root_id][0] = 1;

  unsigned n_iter = 30;
  double epsilon = 1;



  while(n_iter-- && epsilon > 1.0e-10) {

    //        std::clog << n_iter << " : " << epsilon << std::endl;

    for (size_t i = 0; i < result.size(); ++i)
      {
        // if( (i % 100) == 0) {
        //   std::clog << "i : " << i << "\r";
        //   std::flush(std::clog);
        // }

        std::vector<double>& result_i = result[i];
        //if(n_iter == 50)
          result_i.resize(ali.get_number_of_annotations(i));

        uomap< unsigned, uomap<int,uomap<unsigned,double> > >& map = trans[i];

        for (unsigned annot_i = 0; annot_i < result_i.size(); ++annot_i)
        {
          //std::clog << "annot_i : " << annot_i << std::endl;

          const double& old_value = result_i[annot_i];
          uomap<int,uomap<unsigned,double> >& lhs_map = map[annot_i];

          for (unsigned j = 0; j < result.size(); ++j)
	  {

            //            std::clog << "j : " << j << std::endl;

            if((int) j == root_id) continue;

	    std::vector<double>& temp_j = temp[j];
            //if(n_iter == 50)
              temp_j.resize(ali.get_number_of_annotations(j));

            uomap<int,uomap<unsigned,double> >::iterator found_key;
            if( (found_key = lhs_map.find(j)) != lhs_map.end() ) {

              //                std::clog << "here" << std::endl;
              for (unsigned annot_j = 0; annot_j < temp_j.size(); ++annot_j)
              {
                // std::clog << "annot_j : " << annot_j << std::endl;
                //                try {
                temp_j[annot_j] += old_value * found_key->second[annot_j];
                  //                }
                  //                catch(std::out_of_range& e) {}
                //                    std::clog << "there" << std::endl;
              }
            }
          }
        }
      }

    //    std::clog << "updated temp" << std::endl;

    epsilon = 0;

    for (unsigned i = 0; i < result.size(); ++i)
      {
        //        std::cout << SymbolTable::instance_nt().translate(i) << std::endl;

	if((int) i == root_id) {
          //          std::cout << result[0][0] << std::endl;
          continue;
        }

        std::vector<double>& temp_i = temp[i];
        std::vector<double>& result_i = result[i];

	for (unsigned annot_i = 0; annot_i < result_i.size(); ++annot_i)
	  {
            epsilon += std::abs(temp_i[annot_i] - result_i[annot_i]);
	    result_i[annot_i] = temp_i[annot_i];
            // std::cout << annot_i
            //           << " : "
            //           << result_i[annot_i]
            //           << std::endl;
	    temp_i[annot_i] = 0;
	  }
      }
  }
}

 std::vector<std::vector<double> >
calculate_conditional_probs(const std::vector<std::vector<double> >& expected_counts,
			    const std::vector<std::vector<std::vector<unsigned> > >& mapping)
{
  std::vector<std::vector<double> > result(expected_counts);

  for (unsigned i = 0; i < mapping.size(); ++i)
    {
      const std::vector<std::vector<unsigned> >& mapping_i = mapping[i];
      const std::vector<double>& exp_i = expected_counts[i];
      std::vector<double>& result_i = result[i];


      for (unsigned annot_i = 0; annot_i < mapping_i.size(); ++annot_i)
	{
	  double sum = 0;
          const std::vector<unsigned>& m = mapping_i[annot_i];
          for (std::vector<unsigned>::const_iterator it(m.begin()); it != m.end(); ++it)
	    {
	      sum+= exp_i[*it];
	    }
	  for (std::vector<unsigned>::const_iterator it(m.begin()); it != m.end(); ++it)
	    {
	      result_i[*it] /= sum;
	    }
	}
    }

  return result;
}

struct project_rule
{
  const std::vector<std::vector<double> >& conditional_probabilities;
  const std::vector<std::vector<std::vector<unsigned> > >& mapping;
  std::vector<uomap<unsigned,unsigned> > inverted;


  project_rule(const std::vector<std::vector<double> >& conditional_probabilities_,
	       const std::vector<std::vector<std::vector<unsigned> > >& mapping_) :
    conditional_probabilities(conditional_probabilities_),
    mapping(mapping_),
    inverted(invert_mapping(mapping_))
  {}


  //TODO: iterate on new_probs instead of old_probs
  // would be more efficient (especially for binary rules)


  LexicalRuleC2f operator()(const LexicalRuleC2f& old_rule) const
  {
    int lhs = old_rule.get_lhs();
    LexicalRuleC2f new_rule(LexicalRule(lhs, old_rule.get_word(),
                                        std::vector<double>(mapping[lhs].size())));

    std::vector<double>& probs = new_rule.get_probability();
    const std::vector<double>& old_probs = old_rule.get_probability();

    const uomap<unsigned,unsigned>& inverted_lhs = inverted[lhs];
    const std::vector<double>& conditional_probabilities_lhs = conditional_probabilities[lhs];

    for (unsigned i = 0; i < old_probs.size(); ++i)
      {
	unsigned new_lhs_annot = inverted_lhs.find(i)->second;
	probs[new_lhs_annot] += old_probs[i] * conditional_probabilities_lhs[i];
      }

    return new_rule;
  }

  URuleC2f operator()(const URuleC2f& old_rule) const
  {
    //    std::clog << old_rule << std::endl;

    int lhs = old_rule.get_lhs();
    int rhs = old_rule.get_rhs0();
    URuleC2f new_rule(URule(lhs, rhs,
                            std::vector< std::vector<double> >(mapping[lhs].size(),
                                                               std::vector<double>(mapping[rhs].size()))));

    const std::vector<std::vector<double> >& old_probs = old_rule.get_probability();
    std::vector<std::vector<double> >& new_probs = new_rule.get_probability();


    const uomap<unsigned,unsigned>& inverted_lhs = inverted[lhs];
    const uomap<unsigned,unsigned>& inverted_rhs = inverted[rhs];
    const std::vector<double>& conditional_probabilities_lhs = conditional_probabilities[lhs];



    for (unsigned i = 0; i < old_probs.size(); ++i)
      {

        const std::vector<double>& old_probs_i = old_probs[i];
        const double& conditional_probabilities_lhs_i = conditional_probabilities_lhs[i];

        std::vector<double>& new_probs_new_lhs_annot = new_probs[inverted_lhs.find(i)->second];

	for (unsigned j = 0; j < old_probs_i.size(); ++j)
	  {
	    unsigned new_rhs_annot = inverted_rhs.find(j)->second;
	    new_probs_new_lhs_annot[new_rhs_annot] += old_probs_i[j] * conditional_probabilities_lhs_i;
	  }
      }


    //    std::clog << new_rule << std::endl;

    return new_rule;
  }


  BRuleC2f operator()(const BRuleC2f& old_rule) const
  {
    int lhs = old_rule.get_lhs();
    int rhs0 = old_rule.get_rhs0();
    int rhs1 = old_rule.get_rhs1();
    BRuleC2f new_rule(BRule(lhs, rhs0, rhs1,
                            std::vector< std::vector< std::vector<double> > >(mapping[lhs].size(),
                                                                              std::vector< std::vector<double> >(mapping[rhs0].size(),
                                                                                                                 std::vector<double>(mapping[rhs1].size())))));


    const std::vector< std::vector<std::vector<double> > >& old_probs = old_rule.get_probability();
    std::vector< std::vector<std::vector<double> > >& new_probs = new_rule.get_probability();

    const uomap<unsigned,unsigned>& lhs_map = inverted[lhs];
    const uomap<unsigned, unsigned>& rhs0_map = inverted[rhs0];
    const uomap<unsigned, unsigned>& rhs1_map = inverted[rhs1];

    for (unsigned i = 0; i < old_probs.size(); ++i)
      {
	std::vector< std::vector<double> >& new_probs_lhs = new_probs[lhs_map.find(i)->second];
	double conditional = conditional_probabilities[lhs][i];
        const std::vector< std::vector<double> >& old_probs_i = old_probs[i];

	for (unsigned j = 0; j < old_probs_i.size(); ++j)
	  {
	    std::vector<double>& new_probs_lhs_rhs0 = new_probs_lhs[rhs0_map.find(j)->second];
            const std::vector<double>& old_probs_i_j = old_probs_i[j];
	    for (unsigned k = 0; k < old_probs_i_j.size(); ++k)
	      {
		new_probs_lhs_rhs0[rhs1_map.find(k)->second] += old_probs_i_j[k] * conditional;
	      }
	  }
      }

    return new_rule;
  }
};

}








template<typename Bin, typename Un, typename Lex>
GrammarAnnotated<Bin,Un,Lex> *
GrammarAnnotated<Bin,Un,Lex>::create_projection(const std::vector<std::vector<double> >& expected_counts,
						const std::vector<std::vector<std::vector<unsigned> > >& annotation_mapping) const
{

  std::vector<std::vector<double> > conditional_probabilities = calculate_conditional_probs(expected_counts, annotation_mapping);

  GrammarAnnotated * result =  new GrammarAnnotated(conditional_probabilities, annotation_mapping,
						    Grammar<Bin,Un,Lex>::binary_rules,
						    Grammar<Bin,Un,Lex>::unary_rules,
						    Grammar<Bin,Un,Lex>::lexical_rules);


  return result;
}


// compute probabilities p(\alpha X_i \beta | Y_j )
//in table  table[Y,j][X,i] or [Y,j][word_id,0]
template<typename Bin, typename Un, typename Lex>
void GrammarAnnotated<Bin,Un,Lex>::compute_transition_probabilities(uomap<int, uomap<unsigned, uomap<int, uomap<unsigned, double > > > >& result) const
{
  for (typename std::vector<Bin>::const_iterator b(Grammar<Bin,Un,Lex>::binary_rules.begin());
       b != Grammar<Bin,Un,Lex>::binary_rules.end(); ++b)
    {
      const std::vector<std::vector<std::vector<double> > >& probs = b->get_probability();

      uomap<unsigned, uomap<int, uomap<unsigned, double > > >& lhs_map = result[b->get_lhs()];


      for (unsigned i = 0; i < probs.size(); ++i)
	{

          uomap<unsigned, double >& rhs0_map = lhs_map[i][b->get_rhs0()];
          uomap<unsigned, double >& rhs1_map = lhs_map[i][b->get_rhs1()];
          const std::vector<std::vector<double> >& probs_i = probs[i];

	  for (unsigned j = 0; j < probs_i.size(); ++j)
	    {
	      double& rhs0_entry = rhs0_map[j];
              const std::vector<double>& probs_i_j = probs_i[j];

	      for (unsigned k = 0; k < probs_i_j.size(); ++k)
		{
		  rhs0_entry += probs_i_j[k];
		  rhs1_map[k] += probs_i_j[k];
		}
	    }
	}
    }


  for (typename std::vector<Un>::const_iterator u(Grammar<Bin,Un,Lex>::unary_rules.begin());
       u != Grammar<Bin,Un,Lex>::unary_rules.end(); ++u)
    {
      const std::vector<std::vector<double> >& probs = u->get_probability();
      uomap<unsigned, uomap<int, uomap<unsigned, double > > >& lhs_map = result[u->get_lhs()];

      for (unsigned i = 0; i < probs.size(); ++i)
	{
          const std::vector<double>& probs_i = probs[i];
	  uomap<unsigned,double>& rhs0_map = lhs_map[i][u->get_rhs0()];
	  for (unsigned j = 0; j < probs_i.size(); ++j)
	    {
	      rhs0_map[j] += probs_i[j];
	    }
	}
    }

  // for (typename std::vector<Lex>::const_iterator l(Grammar<Bin,Un,Lex>::lexical_rules.begin());
  //      l != Grammar<Bin,Un,Lex>::lexical_rules.end(); ++l)
  //   {
  //     const std::vector<double>& probs = l->get_probability();
  //     uomap<unsigned, uomap<int, uomap<unsigned, double > > >& lhs_map = result[l->get_lhs()];


  //     for (unsigned i = 0; i < probs.size(); ++i)
  //       {
  //         lhs_map[i][l->get_word()][0] += probs[i];
  //       }
  //   }
}

template<typename Bin, typename Un, typename Lex>
GrammarAnnotated<Bin,Un,Lex>::GrammarAnnotated
(const std::vector<std::vector<double> >& conditional_probabilities,
 const std::vector<std::vector<std::vector<unsigned> > >& mapping,
 const std::vector<Bin>& old_binary_rules,
 const std::vector<Un>& old_unary_rules,
 const std::vector<Lex>& old_lexical_rules) :

  Grammar<Bin,Un,Lex>(), AnnotatedContents(), viterbi_decoding_paths()

{
  //set label annotations
  // TODO get rid of it ?
  std::vector<unsigned short> la(mapping.size());
  for (unsigned short i = 0; i < mapping.size(); ++i)
    {
      la[i] = mapping[i].size();
    }

  label_annotations.set_num_annotations_map(la);


  project_rule pr(conditional_probabilities, mapping);

  //  std::clog << "before binaries" << std::endl;
  Grammar<Bin,Un,Lex>::binary_rules.reserve(old_binary_rules.size());
  std::transform(old_binary_rules.begin(),old_binary_rules.end(),
		 std::back_inserter(Grammar<Bin,Un,Lex>::binary_rules),
		 pr);
  //  std::clog << "after binaries" << std::endl;
  // std::copy(Grammar<Bin,Un,Lex>::binary_rules.begin(),
  // 	    Grammar<Bin,Un,Lex>::binary_rules.end(),
  //   	    std::ostream_iterator<Bin> (std::clog, "\n"));

  //  std::clog << "before unaries" << std::endl;
  Grammar<Bin,Un,Lex>::unary_rules.reserve(old_unary_rules.size());
  std::transform(old_unary_rules.begin(),old_unary_rules.end(),
		 std::back_inserter(Grammar<Bin,Un,Lex>::unary_rules),
		 pr);
  //  std::clog << "after unaries" << std::endl;
  // std::copy(Grammar<Bin,Un,Lex>::unary_rules.begin(),
  // 	    Grammar<Bin,Un,Lex>::unary_rules.end(),
  //   	    std::ostream_iterator<Un> (std::clog, "\n"));

  //  std::clog << "before lexicals" << std::endl;
  Grammar<Bin,Un,Lex>::lexical_rules.reserve(old_lexical_rules.size());
  std::transform(old_lexical_rules.begin(),old_lexical_rules.end(),
		 std::back_inserter(Grammar<Bin,Un,Lex>::lexical_rules),
		 pr);
  //  std::clog << "after lexicals" << std::endl;

  // std::copy(Grammar<Bin,Un,Lex>::lexical_rules.begin(),
  // 	    Grammar<Bin,Un,Lex>::lexical_rules.end(),
  //   	    std::ostream_iterator<Lex> (std::clog, "\n"));

}


// assume only one annotation per symbol !!!!!
template<typename Bin, typename Un, typename Lex>
std::vector<double>
GrammarAnnotated<Bin,Un,Lex>::compute_priors() const
{

  uomap<int, uomap<unsigned, uomap<int, uomap< unsigned, double > > > > transition_probabilities;
  compute_transition_probabilities(transition_probabilities);

  std::vector<std::vector<double> > expected_counts;
  calculate_expected_counts(transition_probabilities, get_annotations_info(), expected_counts);

  // assume only one annotation !!!!!
  double sum = 0;
  for (unsigned i = 0; i < expected_counts.size(); ++i)
    {
      sum += expected_counts[i][0];
    }

  std::vector<double> res(expected_counts.size());

  for (unsigned i = 0; i < res.size(); ++i)
    {
      res[i] = expected_counts[i][0] / sum;
    }

  return res;
}



#endif /* _GRAMMARANNOTATED_H_ */
