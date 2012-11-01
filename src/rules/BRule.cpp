#include "BRule.h"
#include "utils/SymbolTable.h"
#include "utils/LorgConstants.h"

#include <cassert>
#include <numeric>
//#include <iostream>



BRule::BRule(short l, short rhs0_, short rhs1_, const std::vector<binary_proba_info>& probs) :
  AnnotatedRule(l), rhs0(rhs0_), rhs1(rhs1_), probabilities()
{
  // the difficult thing is to extend the vectors if needed

  for(std::vector<binary_proba_info>::const_iterator it = probs.begin(); it != probs.end(); ++it) {

    if(it->lhs_pos >= (int) probabilities.size())
      probabilities.resize(it->lhs_pos+1);

    if(it->rhs0_pos >= (int) probabilities[it->lhs_pos].size())
      probabilities[it->lhs_pos].resize(it->rhs0_pos+1);

    if(it->rhs1_pos >= (int) probabilities[it->lhs_pos][it->rhs0_pos].size()) {

      unsigned old_size = probabilities[it->lhs_pos][it->rhs0_pos].size();
      probabilities[it->lhs_pos][it->rhs0_pos].resize(it->rhs1_pos+1);

      for(unsigned i = old_size ; i < probabilities[it->lhs_pos][it->rhs0_pos].size(); ++i)
	probabilities[it->lhs_pos][it->rhs0_pos][it->rhs1_pos] = 0 ;

      probabilities[it->lhs_pos][it->rhs0_pos][it->rhs1_pos] = it->probability;
    }
  }
}

BRule::BRule(short l, short rhs0_, short rhs1_, double prob) :
  AnnotatedRule(l), rhs0(rhs0_), rhs1(rhs1_),
  probabilities(vector_3d(1, std::vector< std::vector<double> >(1,std::vector<double>(1,prob))))
{}

BRule::BRule(short l, short rhs0_, short rhs1_, const std::vector<std::vector<std::vector<double> > >& probs)
  : AnnotatedRule(l), rhs0(rhs0_),rhs1(rhs1_), probabilities(probs) {}


std::ostream& operator<<(std::ostream& out, const BRule& rule)
{
  out.precision( 22 ) ;

  out << "int"
      << " " << SymbolTable::instance_nt().translate(rule.lhs)
      << " " << SymbolTable::instance_nt().translate(rule.rhs0)
      << " " << SymbolTable::instance_nt().translate(rule.rhs1);


  for(unsigned i = 0 ; i < rule.probabilities.size(); ++i)
    for(unsigned j = 0; j < rule.probabilities[i].size(); ++j)
      for(unsigned k = 0; k < rule.probabilities[i][j].size(); ++k)
	if(rule.probabilities[i][j][k] != 0)
	  out << " (" <<i << ',' << j << ',' << k << ','
	      << rule.probabilities[i][j][k] << ')';

  return out;
}

void BRule::update_inside_annotations(std::vector<double>& up,
				      const std::vector<double>& left,
				      const std::vector<double>& right) const
{
  for(size_t i = 0 ; i < probabilities.size();++i) {
    if(up[i] == LorgConstants::NullProba) continue;
    for(size_t j = 0 ; j < probabilities[i].size();++j) {
      if(left[j] == LorgConstants::NullProba || left[j] == 0.0) continue;
      double inner = 0.0;
      for(size_t k = 0 ; k < probabilities[i][j].size();++k) {
	if(right[k] == LorgConstants::NullProba || right[k] == 0.0) continue;
	inner += right[k] * probabilities[i][j][k];
      }
      //std::cout << *this << " " << up[i] << " " << i<< std::endl;
      up[i] += left[j] * inner;
    }
    assert(up[i] >= 0.0);
    assert(up[i] <= 1.0);
  }
}


// only for  forest construction : suppose only *one* annotation !
void BRule::update_inside_annotations(std::vector<double>& up,
                                      const double& left_right_precomputation) const
{
  if(up[0] == LorgConstants::NullProba) return;
  up[0] += probabilities[0][0][0] * left_right_precomputation;

    assert(up[0] >= 0.0);
    assert(up[0] <= 1.0);
}





void BRule::update_outside_annotations(const std::vector<double>& up_out,
				       const std::vector<double>& left_in,
				       const std::vector<double>& right_in,
				       std::vector<double>& left_out,
				       std::vector<double>& right_out) const
{
  for(unsigned short i = 0; i < probabilities.size(); ++i) {
    if(up_out[i] == LorgConstants::NullProba || up_out[i] == 0.0) continue;
    const std::vector<std::vector<double> >& dim_i = probabilities[i];
    for(unsigned short j = 0; j < dim_i.size(); ++j) {
      const std::vector<double>& dim_j = dim_i[j];
      double temp4left = 0.0;
      double factor4right = 0.0;
      if(left_in[j] != LorgConstants::NullProba) factor4right = up_out[i] * left_in[j];
      for(unsigned short k = 0; k < dim_j.size(); ++k) {
	const double& t = dim_j[k];
	// if(right_in[k] != LorgConstants::NullProba) temp4left += right_in[k] * t;
	// if(right_out[k] != LorgConstants::NullProba) right_out[k] += factor4right * t;

	// I and O are always Null at the same time
	if(right_in[k] != LorgConstants::NullProba) {
	  temp4left += right_in[k] * t;
	  right_out[k] += factor4right * t;
	}


      }
      if(left_out[j] != LorgConstants::NullProba) left_out[j] += up_out[i] * temp4left;
    }
  }
}

void BRule::remove_unlikely_annotations(const double& threshold)
{
  bool changed = false;

  for(unsigned i = 0; i < probabilities.size(); ++i)
    for(unsigned j = 0; j < probabilities[i].size(); ++j)
      for(unsigned k = 0; k < probabilities[i][j].size(); ++k)
	if(probabilities[i][j][k] < threshold) {
	  changed = true;
	  probabilities[i][j][k] = 0;
	}

  if(changed) compact();

}


void BRule::compact()
{
  //get rid of lines of zeros
  for(unsigned i = 0; i < probabilities.size(); ++i)
    for(unsigned j = 0; j < probabilities[i].size(); ++j) {
      bool allzeros = true;
      for(unsigned k = 0; k < probabilities[i][j].size(); ++k) {
  	if(probabilities[i][j][k] != 0.0) {
  	  allzeros = false;
  	  break;
  	}
      }
      if(allzeros) {
  	//probabilities[i][j] = std::vector<double>();
	//std::cout << "brule::compact inner level" << std::endl;
	std::vector<double>().swap(probabilities[i][j]);
      }
    }

  //get rid of lines of empty vectors
  for(unsigned i = 0; i < probabilities.size(); ++i) {
    bool allempty = true;
    for(unsigned j = 0; j < probabilities[i].size(); ++j) {
      if(!probabilities[i][j].empty()) {
  	allempty = false;
  	break;
      }
    }
    if(allempty) {
      //probabilities[i] = std::vector< std::vector<double> >();
      std::vector< std::vector<double> >().swap(probabilities[i]);
      //      std::cout << "brule::compact outer level" << std::endl;
    }
  }
}


void BRule::uncompact(unsigned rhs0_size, unsigned rhs1_size)
{

  //  std::cout << "before uncompact\n" << *this << std::endl;

  for(unsigned i = 0; i < probabilities.size(); ++i) {
    if(probabilities[i].size() != rhs0_size)
      probabilities[i].resize(rhs0_size);
    for(unsigned j = 0; j < probabilities[i].size(); ++j) {
      if(probabilities[i][j].size() != rhs1_size)
  	probabilities[i][j].resize(rhs1_size,0.0);
    }
  }

  //  std::cout << "after uncompact\n" << *this << std::endl;

}



bool BRule::operator==(const BRule& other) const
{
  return
    lhs  == other.lhs  &&
    rhs0 == other.rhs0 &&
    rhs1 == other.rhs1 &&
    probabilities == other.probabilities;
}

bool BRule::operator<(const BRule& other) const
{
  return
    lhs < other.lhs ||
    (
     lhs  == other.lhs  &&
     (rhs0 < other.rhs0 ||
      (
       rhs0 == other.rhs0 &&
       (rhs1 < other.rhs1 ||
	( rhs1 == other.rhs1 &&
	  probabilities < other.probabilities)))));
}

void BRule::set_probability(const double& value)
{
  for(unsigned i = 0 ; i < probabilities.size(); ++i)
    for(unsigned j = 0; j < probabilities[i].size(); ++j)
      for(unsigned k = 0; k < probabilities[i][j].size(); ++k)
	probabilities[i][j][k] = value;
}


// p_x = p(A_x -> B_y C_z), y and z fixed !!!
// p'_x = (1 - \alpha) p_x + \alpha (average_x' (p_x'))
// assume the rule is not compacted
void BRule::linear_smooth(const double& alpha)
{
  if(probabilities.size() == 1) return;

  double alphabar = 1 - alpha;
  double other_factor = alpha / (probabilities.size() - 1);

  std::vector< std::vector< std::vector<double> > >
    new_probabilities(probabilities.size(),
		      std::vector< std::vector<double> >(probabilities[0].size(),
							 std::vector<double>(probabilities[0][0].size(),0.0)));

  for(unsigned i = 0; i < probabilities.size(); ++i)
    for(unsigned o = 0; o < probabilities.size(); ++o)
      for(unsigned j = 0; j < probabilities[i].size(); ++j)
	for(unsigned k = 0; k < probabilities[i][j].size(); ++k) {
	  new_probabilities[i][j][k] += ( i == o ? alphabar : other_factor) * probabilities[o][j][k];
	}

  probabilities = new_probabilities;
}


// assume the rule is not compacted
void BRule::weighted_smooth(const double& alpha, const std::vector<std::vector<double> > & weights)
{
  if(probabilities.size() == 1) return;

  std::vector< std::vector< std::vector<double> > >
    new_probabilities(probabilities.size(),
		      std::vector< std::vector<double> >(probabilities[0].size(),
							 std::vector<double>(probabilities[0][0].size(),0.0)));

  const std::vector<double> & myweights = weights[lhs];

  for(unsigned i = 0; i < probabilities.size(); ++i) {

    double denominator = std::accumulate(myweights.begin(),myweights.end(), - myweights[i]);
    double alphabar = (1 - alpha) / myweights[i];
    double other_factor = alpha / denominator;


    for(unsigned o = 0; o < probabilities.size(); ++o)
      for(unsigned j = 0; j < probabilities[i].size(); ++j)
	for(unsigned k = 0; k < probabilities[i][j].size(); ++k) {
	  new_probabilities[i][j][k] += ( i == o ? alphabar : other_factor ) * probabilities[o][j][k] * myweights[o];
	}
  }
  probabilities = new_probabilities;
}


// assume the rule is not compacted
void BRule::generation_smooth(const std::vector<std::vector<std::vector<double> > > & weights)
{
  if(probabilities.size() == 1) return;

  //  std::cout << *this << std::endl;


  std::vector< std::vector< std::vector<double> > >
    new_probabilities(probabilities.size(),
		      std::vector< std::vector<double> >(probabilities[0].size(),
							 std::vector<double>(probabilities[0][0].size(),0.0)));

  const std::vector<std::vector<double> > & myweights = weights[lhs];

  for(unsigned i = 0; i < probabilities.size(); ++i) {
    for(unsigned o = 0; o < probabilities.size(); ++o)  {
      //  std::cout << myweights[i][o] << std::endl;
      for(unsigned j = 0; j < probabilities[i].size(); ++j) {
	//	std::cout << "before k" << std::endl;
	for(unsigned k = 0; k < probabilities[i][j].size(); ++k) {
	  //	  std::cout << "size myweights: " << myweights.size();
	  //	  std::cout << "i: " << i << " k: " << k << std::endl;
	  new_probabilities[i][j][k] += probabilities[o][j][k] * myweights[o][i];
	}
      }
    }
  }
  probabilities = new_probabilities;


  //  std::cout << *this << std::endl;

}
