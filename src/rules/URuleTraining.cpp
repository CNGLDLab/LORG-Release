#include "URuleTraining.h"

#include "utils/SymbolTable.h"

#include <numeric>
#include "utils/RandomGenerator.h"


//uncomment to run benchmark, or better compile with -DBENCHMARK
//#define BENCHMARK


void URuleTraining::update_inside_annotations(std::vector<double>& up,
					      const std::vector<double>& left) const
//
 {
  for(unsigned short i = 0 ; i < probabilities.size();++i) {
    //if(probabilities[i].empty()) continue;

    const std::vector<double>& rule_probs_i = probabilities[i];

    //   std::cout << *this << std::endl;
    
    for(unsigned short j = 0 ; j < rule_probs_i.size();++j) {
      //if(left[j] == 0) continue;
      //if(rule_probs_i[j] == 0) continue;
      up[i] += left[j] * rule_probs_i[j];
    }
    assert(up[i] >= 0.0);
    assert(up[i] <= 1.0);
  }
}


void URuleTraining::update_outside_annotations(const std::vector<double>& up,
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
    const std::vector<double>& dim_i = probabilities[i];
    for(unsigned short j = 0 ; j < dim_i.size();++j) {
      left[j] += up[i] * dim_i[j];
    }
  }
}


// this is for one split
void URuleTraining::split(unsigned n, unsigned randomness)
{
  // this returns a number in [0;1]
  // we need a number in [-0.5;0.5]
  RandomGenerator * random_generator = RandomGenerator::instance();


  double random_percent = double(randomness) / 100.0;


  static int top_idx = SymbolTable::instance_nt().get(LorgConstants::tree_root_name);
  bool parent_is_top = lhs == top_idx;

  unsigned max_annot_a = (parent_is_top) ? 1 : n;
  unsigned new_dim_lhs = (parent_is_top) ? 1 : probabilities.size() * n;
  

  //creating the new vector of probabilities
  std::vector< std::vector<double> > new_probabilities(new_dim_lhs);
  for(unsigned i = 0; i < probabilities.size(); ++i) {
    for(unsigned annot_a = 0 ; annot_a < max_annot_a ; ++annot_a) {
      new_probabilities[i*n+annot_a].resize(probabilities[i].size() * n, 0.0);
    }
  }
  
  //filling new_probabilities
  for(unsigned i = 0 ; i < probabilities.size(); ++ i) {
    unsigned range_i = i*n;
    
    for(unsigned j = 0 ; j < probabilities[i].size(); ++j)  {
      if(probabilities[i][j] == 0.0) continue;
      
      unsigned range_j = j*n;

      double new_base_value = probabilities[i][j]/n;


      for(unsigned annot_a = 0 ; annot_a < ((parent_is_top) ? 1 : n); ++annot_a) {

	unsigned new_i = range_i + annot_a;
    	
	std::vector<double> left_random;
#ifdef BENCHMARK
	for(unsigned r = 0 ; r < n/2;++r) left_random.push_back(0.029787513752517536 - 0.5);
#else
	for(unsigned r = 0 ; r < n/2;++r) left_random.push_back(random_generator->next() * 2 - 1);
#endif
	for(unsigned r = 0 ; r < n/2;++r) left_random.push_back(- left_random[r]);
	if(n % 2 != 0)
	  left_random.push_back(0.0);
	
	//	std::random_shuffle(left_random.begin(),left_random.end());

	for(unsigned annot_b = 0 ; annot_b < n ; ++annot_b) {

	  new_probabilities[new_i][range_j+annot_b] = new_base_value * (1.0 + (random_percent * left_random[annot_b]));
	  assert(new_probabilities[new_i][range_j+annot_b] <= 1.0);
	  assert(new_probabilities[new_i][range_j+annot_b] >= 0.0);
	  assert(!std::isnan(new_probabilities[new_i][range_j+annot_b]));
	}
      }
    }
  }
  
  probabilities = new_probabilities;
}
  

void URuleTraining::merge(const Merge_map& annotation_sets_to_merge, 
			  int split_size,
			  const ProportionsMap& proportions,
			  const AnnotatedLabelsInfo& a,
			  const std::vector<std::map<int,int> >& annot_reorder)
{
  //TODO get these centrally from grammar
  unsigned lhs_dim =  probabilities.size();
  unsigned rhs_dim =  probabilities[0].size();
  
  unsigned new_lhs_dim = a.get_number_of_annotations(get_lhs());
  unsigned new_rhs_dim = a.get_number_of_annotations(get_rhs0());
  
  // std::cout << "new lhs dim is " << new_lhs_dim 
  // 	     << " from sets to merge we have " <<  annotation_sets_to_merge[get_lhs()].size() 
  // 	     << std::endl;
  
  
  //First calculate what the probabilities are for the merged rules  
  
  //the iterator that stores annotations for lhs and rhs;
  Merge_map::const_iterator annotation_sets_iter;
  
  //rhs0: calculate the probabities when rhs0 is merged
  int rhs = get_rhs0();
  
  if ((annotation_sets_iter = annotation_sets_to_merge.find(rhs)) != annotation_sets_to_merge.end() ){
    // std::cout << "it's here alrighty RHS...." << rhs << '\n'
    // 	       << "new rhs dim is " << new_rhs_dim 
    // 	       << " from sets to merge we have " <<  annotation_sets_to_merge[get_rhs(0)].size() 
    // 	       << std::endl;
    
    
    //for each annotation_set to merge with this label id
    for (unsigned i = 0; i< annotation_sets_iter->second.size();++i){
      // std::cout << "merging set " << annotation_sets_to_merge[rhs0][i] << std::endl;
      
      int start =  annotation_sets_iter->second[i];
      
      //for each annotation in set 
      //(don't need to look at the first one - we add to the first one in the innermost loop)
      for (int rhs_a=start+1; rhs_a< (start + split_size); ++rhs_a){
	// std::cout << "for each annotion in set rhs0: " <<  rhs0_a << std::endl;
	
	//for each annotion of lhs
	// std::cout << "Total number of annotations for LHS: " << num_annotations_lhs << std::endl;
	for(unsigned lhs_a = 0; lhs_a < lhs_dim; ++lhs_a){
	  
	  probabilities[lhs_a][start] += probabilities[lhs_a][rhs_a];
	  probabilities[lhs_a][rhs_a] = 0.0;//i.e. null  
	}
      }	 
    }
  }
  
  //lhs:  calculate the probabities when lhs is merged
  int lhs = get_lhs();
  
  if ((annotation_sets_iter = annotation_sets_to_merge.find(lhs)) != annotation_sets_to_merge.end() ){
    //std::cout << "it's here alrighty LHS...." << lhs << std::endl;
    //new_lhs_dim = annotation_sets_to_merge[lhs].size();
    
    //for each annotation_set to merge with this label id
    for (unsigned int i = 0; i< annotation_sets_iter->second.size();i++){
      
      int start =  annotation_sets_iter->second[i];
      
      for(unsigned rhs_a = 0; rhs_a< rhs_dim; ++rhs_a)
	probabilities[start][rhs_a] *= proportions[lhs].at(start);

      for (int lhs_a = start + 1 ; lhs_a < (start + split_size); ++lhs_a){
	//for each annotion of rhs0
	for(unsigned rhs_a = 0; rhs_a< rhs_dim; ++rhs_a) {
	
	  //add to what is already there
	  probabilities[start][rhs_a] += proportions[lhs].at(lhs_a) * probabilities[lhs_a][rhs_a];
	  probabilities[lhs_a][rhs_a] = 0.0;//i.e. null
	}
      }
    }	 
  }
  
  // Now create the new probabilities data structure 
  std::vector< std::vector<double> > new_probabilities(new_lhs_dim, std::vector<double>(new_rhs_dim));

  const std::map<int,int>& lhs_reorder_map = annot_reorder[lhs];
    
    //for each old annotation of LHS
    for(unsigned i = 0; i < lhs_dim; ++i){
      
      if(lhs_reorder_map.count(i)) {

	const std::map<int,int>& rhs_reorder_map = annot_reorder[rhs]; 
      
	  for(unsigned j = 0; j < rhs_dim; ++j){

	    if(rhs_reorder_map.count(j))
	      new_probabilities[lhs_reorder_map.find(i)->second][rhs_reorder_map.find(j)->second] = probabilities[i][j];
	
	  }
      }
    }
  
  probabilities = new_probabilities;
  
  //		std::cout << *this << std::endl;
  
  
}


void URuleTraining::reset_counts()
{
  counts = std::vector< std::vector<double> >(probabilities.size());
  for(unsigned i = 0; i < probabilities.size(); ++i)
	counts[i] = std::vector<double>(probabilities[i].size(),0);
      
}


void URuleTraining::reset_counts_soft()
{
  for(unsigned i = 0 ; i < counts.size(); ++i)
    std::fill(counts[i].begin(), counts[i].end(), 0);
}


void URuleTraining::update_probability(const AnnotatedNodeCountMap& nc_map)
{
  const std::vector<double>& node_counts = nc_map[get_lhs()];

  for (unsigned i = 0; i < counts.size(); ++i) {
    
    const double node_count = node_counts.at(i);
    for (unsigned j = 0; j < counts[i].size(); ++j) {	
      
      counts[i][j] /= node_count;
      
      assert(!std::isnan(counts[i][j]));
      assert(counts[i][j] <= 1);
      assert(counts[i][j] >= 0);
    }
  }
  counts.swap(probabilities);
}



//inline
void URuleTraining::update_rule_frequencies(const scaled_array& left_in,
					    const scaled_array& up_out,
					    const scaled_array& root_insides)
{
  double new_scale = scaled_array::calculate_scalingfactor(left_in.scale + up_out.scale 
							       - root_insides.scale) / root_insides.array[0];

  for(unsigned i = 0 ; i < probabilities.size();++i) {
    if(up_out.array[i] ==0) continue;
    const std::vector<double>& rule_probs_i = probabilities[i];
    //if(rule_probs_i.empty()) continue;
      
    double up_tree = up_out.array[i] * new_scale;

    for(unsigned j = 0 ; j < rule_probs_i.size();++j) {
      //if(left_in.array[j] == 0) continue;
      //if(rule_probs_i[j] == 0)  continue;
      counts[i][j] += up_tree * left_in.array[j] * rule_probs_i[j];
    }
  }
}
