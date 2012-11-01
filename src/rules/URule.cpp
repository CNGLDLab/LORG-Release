#include "URule.h"

#include "utils/SymbolTable.h"
#include "utils/LorgConstants.h"

#include <cassert>
#include <numeric>
//#include <iostream>

URule::URule(short l, short rhs0_, const std::vector<unary_proba_info>& probs) :
  AnnotatedRule(l), rhs0(rhs0_), probabilities()
{
  // the difficult thing here is to extend the vectors to the correct sze

  //  std::cout << probs.size() << std::endl;

  for(std::vector<unary_proba_info>::const_iterator it = probs.begin(); it != probs.end(); ++it) {

    //    std::cout << it->lhs_pos << " " << it->rhs0_pos << std::endl;
    if(it->lhs_pos >= (int) probabilities.size())
      probabilities.resize(it->lhs_pos+1);

    if(it->rhs0_pos >= (int) probabilities[it->lhs_pos].size()) {

      unsigned old_size = probabilities[it->lhs_pos].size();
      probabilities[it->lhs_pos].resize(it->rhs0_pos+1);

      for(unsigned i = old_size ; i < probabilities[it->lhs_pos].size(); ++i)
	probabilities[it->lhs_pos][i] = 0 ;

    }
    probabilities[it->lhs_pos][it->rhs0_pos] = it->probability;

  }
  //  std::cout << "urule created: " << *this << std::endl;

}

URule::URule(short l, short rhs0_, double prob) :
  AnnotatedRule(l), rhs0(rhs0_),  probabilities(std::vector< std::vector<double> >(1, std::vector<double>(1, prob))) {}

URule::URule(short l, short rhs0_, const std::vector< std::vector<double> >& probs)
  : AnnotatedRule(l), rhs0(rhs0_), probabilities(probs) {}

std::ostream& operator<<(std::ostream& out, const URule& rule)
{
  out.precision( 22 ) ;

  out << "int"
      << " " << SymbolTable::instance_nt().translate(rule.lhs)
      << " " << SymbolTable::instance_nt().translate(rule.rhs0);

  for(unsigned i = 0 ; i < rule.probabilities.size(); ++i)
    for(unsigned j = 0; j < rule.probabilities[i].size(); ++j)
      if(rule.probabilities[i][j] != 0)
	out << " (" << i << ',' << j << ',' << rule.probabilities[i][j] << ')';

  return out;
}



// URule * URule::convert_rule(const Rule& old)
// {
//   if (old.get_rhs().size() != 1)
//     return NULL;

//   return new URule(old.get_lhs(),old.get_rhs(0),1,1,std::vector<double>(1,old.get_probability()),old.is_lexical());
// }

// std::vector<Rule> URule::to_rules() const
// {
//   if (is_lexical())
//     return to_rules_lexical();
//   else
//     return to_rules_non_lexical();
// }

// std::vector<Rule> URule::to_rules_non_lexical() const
// {

//   std::vector<Rule> result;

//   std::string sb_lhs = SymbolTable::instance_nt()->translate(get_lhs());
//   std::string sb_rhs0= SymbolTable::instance_nt()->translate(get_rhs(0));

//   for(unsigned i = 0 ; i < probabilities.size(); ++i) {
//     std::ostringstream s_lhs;

//     s_lhs << sb_lhs;
//     if(sb_lhs != LorgConstants::tree_root_name )
//     	 s_lhs << "_" << i;
// //      s_lhs << "||" << i;


//     for(unsigned j = 0 ; j < probabilities[i].size(); ++j) {
//       std::ostringstream s_rhs0 ;
//    //   s_rhs0 << sb_rhs0 << "||" << j;
//       s_rhs0 << sb_rhs0 << "_" << j;
//       std::vector<int> rhs(1);
//       rhs[0] = SymbolTable::instance_nt()->insert(s_rhs0.str());

//       if(probabilities[i][j] != 0) {

// 	result.push_back(Rule(SymbolTable::instance_nt()->insert(s_lhs.str()),
// 			      rhs,
// 			      false,
// 			      -1,
// 			      std::log(probabilities[i][j]),
// 			      true));
//       }
//     }
//   }

//   return result;
// }

// std::vector<Rule> URule::to_rules_lexical() const
// {

//   std::vector<Rule> result;

//   std::string sb_lhs = SymbolTable::instance_nt()->translate(get_lhs());
//   std::vector<int> rhs(1,get_rhs(0));
//  // std::cout << "toruleslex: " <<  sb_lhs  << " "<< SymbolTable::instance_word()->translate(get_rhs(0)) << " "<< get_probability(0,0) << std::endl;
// //  std::cout << "\t\t" <<  std::log(probabilities[0][0]) << std::endl;
//   for(unsigned i = 0 ; i < probabilities.size(); ++i) {
//     std::ostringstream s_lhs;
//   //  s_lhs << sb_lhs << "||" << i;

//      s_lhs << sb_lhs << "_" << i;

//      if(probabilities[i].size() > 0 && probabilities[i][0] != 0) {


//        result.push_back(Rule(SymbolTable::instance_nt()->insert(s_lhs.str()),
// 			     rhs,
// 			     true,
// 			     -1,
// 			     std::log(probabilities[i][0]),
// 			     true
// 			     ));
//      }
//   }

//   return result;
// }

//#include <iostream>



void URule::update_inside_annotations(std::vector<double>& up,
				      const std::vector<double>& left) const
{
  for(size_t i = 0 ; i < probabilities.size();++i) {
    if(up[i] == LorgConstants::NullProba) continue;
    //if(probabilities[i].empty()) continue;

    const std::vector<double>& rule_probs_i = probabilities[i];

    //   std::cout << *this << std::endl;

    for(size_t j = 0 ; j < rule_probs_i.size();++j) {
      if(left[j] == LorgConstants::NullProba) continue;
      //if(left[j] == 0) continue;
      //if(rule_probs_i[j] == 0) continue;
      up[i] += left[j] * rule_probs_i[j];
    }

     // if(up[i] < 0.0 || up[i] > 1.0)
     //   std::cout << *this << " " << up[i] <<std::endl;

    assert(up[i] >= 0.0);
    assert(up[i] <= 1.0);
  }
}
// {
//   std::cout << *this << std::endl;
//   std::cout << "size up: " << up.size() << std::endl;
//   std::cout << "size left: " << left.size() << std::endl;
//   for(unsigned short i = 0 ; i < probabilities.size();++i) {
//     std::cout << i << std::endl;
//     const std::vector<double>& dim_i = probabilities[i];
//     up[i] += std::inner_product(dim_i.begin(), dim_i.end(), left.begin(), 0.0);
//   }
// }




//inline
void URule::update_outside_annotations(const std::vector<double>& up,
				       std::vector<double>& left) const
// {
//   for(unsigned short i = 0 ; i < probabilities.size();++i) {
//     //if(up[i] == 0) continue;
//     //if(probabilities[i].empty()) continue;

//     const std::vector<double>& rule_probs_i = probabilities[i];
//     for(unsigned short j = 0 ; j < rule_probs_i.size();++j) {
//       //if(rule_probs_i[j] == 0) continue;
//       left[j] += up[i] * rule_probs_i[j];
//     }
//   }
// }
{
  for(unsigned short i = 0 ; i < probabilities.size();++i) {
    if(up[i] == LorgConstants::NullProba) continue;
    const std::vector<double>& dim_i = probabilities[i];
    for(unsigned short j = 0 ; j < dim_i.size();++j) {
      if(left[j] == LorgConstants::NullProba) continue;
      left[j] += up[i] * dim_i[j];
    }
  }
}

void URule::compact()
{
  //get rid of lines of zeros
  for(unsigned i = 0; i < probabilities.size(); ++i) {
    bool allzeros = true;
    for(unsigned j = 0; j < probabilities[i].size(); ++j) {
      if(probabilities[i][j] != 0) {
  	allzeros = false;
  	break;
      }
    }
    if(allzeros)
      //probabilities[i] = std::vector<double>();
      std::vector<double>().swap(probabilities[i]);
  }
}


void URule::uncompact(unsigned rhs_size)
{
  for(unsigned i = 0; i < probabilities.size(); ++i) {
    probabilities[i].resize(rhs_size,0);
  }
}

void URule::remove_unlikely_annotations(const double& threshold)
{
  bool changed = false;

  for(unsigned i = 0; i < probabilities.size(); ++i)
    for(unsigned j = 0; j < probabilities[i].size(); ++j)
      if(probabilities[i][j] < threshold) {
	probabilities[i][j] = 0;
	changed = true;
      }

  if(changed) compact();
}


bool URule::operator==(const URule& other) const
{
  return
    lhs  == other.lhs  &&
    rhs0 == other.rhs0 &&
    probabilities == other.probabilities;
}


bool URule::operator<(const URule& other) const
{
  return
    lhs < other.lhs ||
    (lhs  == other.lhs  &&
     ( rhs0 < other.rhs0 ||
     (rhs0 == other.rhs0 &&
      probabilities < other.probabilities)));
}

void URule::set_probability(const double& value)
{
  for(unsigned i = 0 ; i < probabilities.size(); ++i)
    for(unsigned j = 0; j < probabilities[i].size(); ++j)
	probabilities[i][j] = value;
}


// p_x = p(A_x -> B_y ), y fixed !!!
// p'_x = (1 - \alpha) p_x + \alpha (average_x' (p_x'))
// assume the rule is not compacted
void URule::linear_smooth(const double& alpha)
{
  if(probabilities.size() == 1) return;

  double alphabar = 1 - alpha;
  double other_factor = alpha / (probabilities.size() - 1);

  std::vector< std::vector<double> > new_probabilities(probabilities.size(),
						       std::vector<double>(probabilities[0].size(),0.0));

  for(unsigned i = 0; i < probabilities.size(); ++i)
    for(unsigned o = 0; o < probabilities.size(); ++o)
      for(unsigned j = 0; j < probabilities[i].size(); ++j) {
	new_probabilities[i][j] += ( i == o ? alphabar : other_factor) * probabilities[o][j];
      }

  probabilities = new_probabilities;
}


// assume the rule is not compacted
void URule::weighted_smooth(const double& alpha, const std::vector<std::vector<double> > & weights)
{
  if(probabilities.size() == 1) return;

  const std::vector<double> & myweights = weights[lhs];

  std::vector< std::vector<double> > new_probabilities(probabilities.size(),
						       std::vector<double>(probabilities[0].size(),0.0));

  for(unsigned i = 0; i < probabilities.size(); ++i) {

    double denominator = std::accumulate(myweights.begin(),myweights.end(), - myweights[i]);
    double alphabar = (1 - alpha) / myweights[i];
    double other_factor = alpha / denominator;

    for(unsigned o = 0; o < probabilities.size(); ++o)
      for(unsigned j = 0; j < probabilities[i].size(); ++j) {
	new_probabilities[i][j] += ( i == o ? alphabar : other_factor) * probabilities[o][j] * myweights[o];
      }
  }

  probabilities = new_probabilities;
}

// assume the rule is not compacted
void URule::generation_smooth(const std::vector<std::vector<std::vector<double> > > & weights)
{
  if(probabilities.size() == 1) return;

  const std::vector<std::vector<double> > & myweights = weights[lhs];

  std::vector< std::vector<double> > new_probabilities(probabilities.size(),
						       std::vector<double>(probabilities[0].size(),0.0));

  for(unsigned i = 0; i < probabilities.size(); ++i) {
    for(unsigned o = 0; o < probabilities.size(); ++o)
      for(unsigned j = 0; j < probabilities[i].size(); ++j) {
	new_probabilities[i][j] +=  probabilities[o][j] * myweights[o][i];
      }
  }

  probabilities = new_probabilities;
}
