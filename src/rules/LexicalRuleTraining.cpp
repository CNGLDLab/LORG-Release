#include "LexicalRuleTraining.h"

#include "utils/RandomGenerator.h"

#include <numeric>



LexicalRuleTraining::LexicalRuleTraining() : LexicalRule(), counts() {}

LexicalRuleTraining::LexicalRuleTraining(short l, int rhs0, double prob) :
  LexicalRule(l, rhs0, std::vector<double>(1, prob)), counts() {}

LexicalRuleTraining::LexicalRuleTraining(short l, int rhs0, double prob, unsigned size) :
  LexicalRule(l, rhs0, std::vector<double>(size, prob)), counts() {}


//inline
void LexicalRuleTraining::update_inside_annotations(std::vector<double>& up) const
{
  // for(unsigned i = 0 ; i < probabilities.size();++i) {
  //   //    if(probabilities[i])
  //     up[i] = probabilities[i];
  //   }

  //std::copy(probabilities.begin(),probabilities.end(),up.begin());
  up = probabilities; 

}


//inline
void LexicalRuleTraining::update_outside_annotations(const std::vector<double>& up,
					     double& left) const
{
  // for(unsigned i = 0 ; i < probabilities.size();++i) {
  //   //if( up[i] == 0 ||      probabilities[i] == 0) continue;
  //   left += up[i] * probabilities[i];
  // }

  left += std::inner_product(probabilities.begin(), probabilities.end(), up.begin(), 0.0);
}


//assume that the unary rule is lexical
void LexicalRuleTraining::split(unsigned n, double /*randomness*/)
{
  unsigned new_dim_lhs = probabilities.size() * n;
  std::vector<double> new_probabilities(new_dim_lhs,0);

  for(unsigned i = 0 ; i < probabilities.size(); ++ i)  {
    if(probabilities[i] == 0) continue;
    unsigned range_i = i*n;
    
    for(unsigned annot_a = 0 ; annot_a <  n; ++annot_a) {
      new_probabilities[range_i + annot_a] = probabilities[i] ; // we just copy the old rule's probability
    }
  }
  
  probabilities = new_probabilities;
}


void LexicalRuleTraining::merge(const Merge_map& annotation_sets_to_merge, 
				int split_size, 
				const ProportionsMap& proportions, 
				const AnnotatedLabelsInfo& a,
				const std::vector<std::map<int,int> >& annot_reorder)
{
  
  //  std::cout << *this << std::endl;
  
  
  //TODO get these centrally from grammar
  int lhs_dim =  probabilities.size();  
  int  new_lhs_dim = a.get_number_of_annotations(get_lhs());
  
  //lhs:  calculate the probabities when lhs is merged
  int lhs = get_lhs();
  if (annotation_sets_to_merge.find(lhs) != annotation_sets_to_merge.end() ){
    
    
    //for each annotation_set to merge with this label id
    for (unsigned int i = 0; i< annotation_sets_to_merge.find(lhs)->second.size();++i){
      
      
      int start =  annotation_sets_to_merge.find(lhs)->second[i];
      
      //for each annotation in set
      probabilities[start] *= proportions[lhs].at(start);

      for (int lhs_a=start+1; lhs_a < (start + split_size); ++lhs_a){
	//add to what is already there
	probabilities[start] += proportions[lhs].at(lhs_a) * probabilities[lhs_a];
	probabilities[lhs_a] = 0.0;//i.e. null
      }
    }	 
  }
  
  
  // Now create the new probabilities data structure 
  std::vector<double> new_probabilities(new_lhs_dim, 0);

  const std::map<int,int>& lhs_reorder_map = annot_reorder[lhs];
    
    //for each old annotation of LHS
    for(int i=0; i<lhs_dim;++i){
      
      if(lhs_reorder_map.count(i)) {
	new_probabilities[lhs_reorder_map.find(i)->second] = probabilities[i];
      }
      
    }
  
  probabilities  = new_probabilities;

  
  //		std::cout << *this << std::endl;
  
  
}
