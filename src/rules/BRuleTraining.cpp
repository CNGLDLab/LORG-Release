#include "BRuleTraining.h"

#include "utils/SymbolTable.h"
#include <numeric>
#include "utils/RandomGenerator.h"

//uncomment to run benchmark, or better compile with -DBENCHMARK
//#define BENCHMARK

// optimized when we know that there is no NullProba
void BRuleTraining::update_inside_annotations(std::vector<double>& up,
				      const std::vector<double>& left,
				      const std::vector<double>& right) const
{
  for(unsigned short i = 0 ; i < probabilities.size();++i) {
    for(unsigned short j = 0 ; j < probabilities[i].size();++j) {
      if(left[j] == 0.0) continue;
      up[i] += left[j] * std::inner_product(probabilities[i][j].begin(), probabilities[i][j].end(), right.begin(), 0.0);
    }
  }
}

// optimized when we know that there is no NullProba
void BRuleTraining::update_outside_annotations(const std::vector<double>& up_out,
				       const std::vector<double>& left_in,
				       const std::vector<double>& right_in,
				       std::vector<double>& left_out,
				       std::vector<double>& right_out) const
{
  for(unsigned short i = 0; i < probabilities.size(); ++i) {
    if(up_out[i] == 0.0) continue;
    const std::vector<std::vector<double> >& dim_i = probabilities[i];
    for(unsigned short j = 0; j < dim_i.size(); ++j) {      
      const std::vector<double>& dim_j = dim_i[j];
      double temp4left = 0.0;
      double factor4right = up_out[i] * left_in[j];
      for(unsigned short k = 0; k < dim_j.size(); ++k) {
	const double& t = dim_j[k];
	temp4left += right_in[k] * t;
	right_out[k] += factor4right * t; 
      }
      left_out[j] += up_out[i] * temp4left;
    }
  }
}



// this is for one split
void BRuleTraining::split(unsigned n, unsigned randomness)
{
  // this returns a number in [0;1]
  // we need a number in [-0.5;0.5]
  RandomGenerator * random_generator = RandomGenerator::instance();

  static int top_idx = SymbolTable::instance_nt().get(LorgConstants::tree_root_name);
  bool parent_is_top = lhs == top_idx;

  unsigned max_annot_a = (parent_is_top) ? 1 : n;

  unsigned new_dim_lhs = (parent_is_top) ? 1 : probabilities.size() * n;


  //creating the new vector with the correct sizes
  vector_3d new_probabilities(new_dim_lhs);
  
  for(unsigned i = 0 ; i < probabilities.size(); ++ i) {
    for(unsigned annot_a = 0 ; annot_a < max_annot_a ; ++annot_a) {
      new_probabilities[i*n+annot_a].resize(probabilities[i].size() * n);

      for(unsigned j = 0 ; j < probabilities[i].size(); ++j) {
	for(unsigned annot_b = 0 ; annot_b < n ; ++annot_b) {
	  new_probabilities[i*n+annot_a][j*n+annot_b].resize(probabilities[i][j].size()*n,0.0);
	}
      }
    }
  }

  // filling new_probabilities

  double random_percent = randomness / 100.0;


  for(unsigned i = 0 ; i < probabilities.size(); ++ i) {
    unsigned range_i = i * n;
    
    for(unsigned j = 0 ; j < probabilities[i].size(); ++j) {
      unsigned range_j = j * n;
    
      for(unsigned k = 0 ; k < probabilities[i][j].size(); ++k) { 
	unsigned range_k = k * n;
	
	if(probabilities[i][j][k] == 0.0) continue;
	
	double new_base_value = probabilities[i][j][k]/(n*n);


	for(unsigned annot_a = 0 ; annot_a < max_annot_a ; ++annot_a) {
	  unsigned new_i = range_i + annot_a;


	  std::vector<double> left_random;
	  for(unsigned r = 0 ; r < n/2;++r) 
#ifdef BENCHMARK
	    left_random.push_back(0.20264675882366157 - 0.5);
#else
	  left_random.push_back(random_generator->next() - 0.5);
#endif
	  for(unsigned r = 0 ; r < n/2;++r) left_random.push_back(-left_random[r]);
	  if(n % 2  != 0)
	    left_random.push_back(0.0);
	    
	  //	  std::random_shuffle(left_random.begin(),left_random.end());

    	  for(unsigned annot_b = 0 ; annot_b < n ; ++annot_b) {
	    unsigned new_j = range_j + annot_b;

	    std::vector<double> right_random;
	    for(unsigned r = 0 ; r < n/2;++r) 
#ifdef BENCHMARK
	      right_random.push_back(0.46009189551900487 - 0.5);
#else
	    right_random.push_back(random_generator->next() - 0.5);
#endif
	    for(unsigned r = 0 ; r < n/2;++r) 
	      right_random.push_back(- right_random[r]);
	    
	    if(n % 2 != 0)
	      right_random.push_back(0.0);
	    
	    //	    std::random_shuffle(right_random.begin(),right_random.end());
	    
	    for(unsigned annot_c = 0 ; annot_c < n ; ++annot_c) {
	      
	      //see em_algo
	      	      
	      new_probabilities[new_i][new_j][range_k+annot_c]= new_base_value * 
		(1.0 + random_percent * 
		 (left_random[annot_b] + right_random[annot_c]));

	      assert(new_probabilities[new_i][new_j][range_k+annot_c] <= 1.0);
	      assert(new_probabilities[new_i][new_j][range_k+annot_c] >= 0.0);
	      assert(!std::isnan(new_probabilities[new_i][new_j][range_k+annot_c]));


	    }
	  }
	  
	}
      }
    }
  }
  probabilities = new_probabilities;
}
  
//we deal with probabilites not log probabilities...
//Merge_map data structure:
//TODO merge_class so that a merge object can be passed in instead of all these data structures which are essentially different views of the same data
void BRuleTraining::merge(const Merge_map& annotation_sets_to_merge, 
			  int split_size,
			  const ProportionsMap& proportions,
			  const AnnotatedLabelsInfo& a,
			  const std::vector<std::map<int,int> >& annot_reorder)
{
  //TODO get these centrally from grammar
  unsigned lhs_dim =  probabilities.size();
  unsigned rhs0_dim =  probabilities[0].size();
  unsigned rhs1_dim =  probabilities[0][0].size();
  
  
  unsigned  new_lhs_dim = a.get_number_of_annotations(get_lhs());
  unsigned new_rhs0_dim = a.get_number_of_annotations(get_rhs0());
  unsigned new_rhs1_dim = a.get_number_of_annotations(get_rhs1());
  
  //First calculate what the probabilities are for the merged rules  
  
  Merge_map::const_iterator annotation_sets_iter;
  
  
  //rhs0: calculate the probabities when rhs0 is merged
  int rhs0 = get_rhs0();
  if ((annotation_sets_iter = annotation_sets_to_merge.find(rhs0)) != annotation_sets_to_merge.end() ){
    
    //new_rhs0_dim = annotation_sets_to_merge[rhs0].size();
    //for each annotation_set to merge with this label id
    for (unsigned int i = 0; i< annotation_sets_iter->second.size();++i){
      // std::cout << "merging set " << annotation_sets_to_merge[rhs0][i] << std::endl;
      
      int start =  annotation_sets_iter->second[i];
      
      //for each annotation in set 
      //(don't need to look at the first one - we add to the first one in the innermost loop)
      for (int rhs0_a = start+1; rhs0_a < (start + split_size); ++rhs0_a){
	// std::cout << "for each annotion in set rhs0: " <<  rhs0_a << std::endl;
	
	//for each annotion of lhs
	// std::cout << "Total number of annotations for LHS: " <<  num_annotations_lhs << std::endl;
	for(unsigned lhs_a = 0; lhs_a < lhs_dim; ++lhs_a){
	  // std::cout << "for each annotion in set lhs: " <<  lhs_a << std::endl;
	  
	  //for each annotation of rhs1
	  //std::cout << "Total number of annotations for RHS1: " << num_annotations_rhs1 << std::endl;
	  for(unsigned rhs1_a = 0; rhs1_a < rhs1_dim; ++rhs1_a){
	    // std::cout << "for each annotion in set rhs1: " <<  rhs1_a << std::endl;
	    
	    probabilities[lhs_a][start][rhs1_a] += probabilities[lhs_a][rhs0_a][rhs1_a];
	    probabilities[lhs_a][rhs0_a][rhs1_a] =0;//i.e. null
	  } 
	}
      }	 
    }
  }
  
  //rhs1: calculate the probabities when rhs1 is merged
  int rhs1 = get_rhs1();
  if ((annotation_sets_iter = annotation_sets_to_merge.find(rhs1)) != annotation_sets_to_merge.end() ){
    
    //new_rhs1_dim = annotation_sets_to_merge[rhs1].size();
    //for each annotation_set to merge with this label id
    for (unsigned i = 0; i< annotation_sets_iter->second.size(); ++i){
      // std::cout << "merging set " << annotation_sets_to_merge[rhs0][i] << std::endl;
      
      int start =  annotation_sets_iter->second[i];
      
      //for each annotation in set 
      //(don't need to look at the first one - we add to the first one in the innermost loop)
      for (int rhs1_a = start + 1; rhs1_a < (start + split_size); ++rhs1_a){
	// std::cout << "for each annotion in set rhs0: " <<  rhs0_a << std::endl;
	
	//for each annotion of lhs
	// std::cout << "Total number of annotations for LHS: " <<  num_annotations_lhs << std::endl;
	for(unsigned lhs_a = 0; lhs_a< lhs_dim; ++lhs_a){
	  // std::cout << "for each annotion in set lhs: " <<  lhs_a << std::endl;
	  
	  //for each annotation of rhs1
	  //std::cout << "Total number of annotations for RHS1: " << num_annotations_rhs1 << std::endl;
	  for(unsigned rhs0_a = 0; rhs0_a < rhs0_dim; ++rhs0_a){
	    // std::cout << "for each annotion in set rhs1: " <<  rhs1_a << std::endl;
											 
	    probabilities[lhs_a][rhs0_a][start] += probabilities[lhs_a][rhs0_a][rhs1_a];
	    probabilities[lhs_a][rhs0_a][rhs1_a] =0;//i.e. null
	  } 
	}
      }	 
    }
  }
  
  //lhs:  calculate the probabilities when lhs is merged
  int lhs = get_lhs();
  if ((annotation_sets_iter = annotation_sets_to_merge.find(lhs)) != annotation_sets_to_merge.end() ){


    const std::vector<double>& current_props = proportions[lhs];

    //for each annotation_set to merge with this label id
    for (unsigned int i = 0; i< annotation_sets_iter->second.size();++i){
      
      unsigned start =  annotation_sets_iter->second[i];
      
      
      //if lhs is the first in the annotation set, overwrite (rather and add to) what is in the probabilities array
      for(unsigned rhs0_a = 0; rhs0_a < rhs0_dim; ++rhs0_a){
	for(unsigned rhs1_a = 0; rhs1_a < rhs1_dim; ++rhs1_a){
	  probabilities[start][rhs0_a][rhs1_a] *= current_props.at(start);
	} 
      }
      
      //for each annotation in set (no need to look at the first one here!)
      for (unsigned lhs_a = start+1; lhs_a < (start + split_size); ++lhs_a){
	
	//for each annotion of rhs0
	for(unsigned rhs0_a = 0; rhs0_a < rhs0_dim; ++rhs0_a){
	  // std::cout << "for each annotion in set lhs: " <<  lhs_a << std::endl;
	  
	  //for each annotation of rhs1
	  //std::cout << "Total number of annotations for RHS1: " << rhs1_dim << std::endl;
	  
	  for(unsigned rhs1_a = 0; rhs1_a < rhs1_dim; ++rhs1_a){
	    // std::cout << "for each annotion in set rhs1: " <<  rhs1_a << std::endl;
	    
	    //add to what is already there
	    probabilities[start][rhs0_a][rhs1_a] += current_props.at(lhs_a)*probabilities[lhs_a][rhs0_a][rhs1_a];
	    probabilities[lhs_a][rhs0_a][rhs1_a] =0;//i.e. null
	  } 
	}
      }	 
    }
  }
  
  // Now create the new probabilities data structure 
  vector_3d new_probabilities(new_lhs_dim, std::vector< std::vector<double> >(new_rhs0_dim, std::vector<double>(new_rhs1_dim)));
  //  std::cout << "creating new data structures...for rule "<< (Production) *this << std::endl;
  
  
  const std::map<int,int>& lhs_reorder_map = annot_reorder[lhs];
  
  //for each old annotation of LHS
  for(unsigned i = 0; i < lhs_dim; ++i) {
    //	std::cout << "annotation for lhs is " << i << " and new annotation is " << new_i << std::endl;
    
    if(lhs_reorder_map.count(i)) {
      
      const std::map<int,int>& rhs0_reorder_map = annot_reorder[rhs0]; 
      
      for(unsigned j = 0; j < rhs0_dim; ++j){
	// std::cout<< "\tannotation for rhs0 is " << j 
	
	if(rhs0_reorder_map.count(j)) {
	  
	  const std::map<int,int>& rhs1_reorder_map = annot_reorder[rhs1]; 
	  
	  for (unsigned k=0;k< rhs1_dim; ++k){
	    // std::cout <<"\t\tannotation for rhs1 is " << k 
	    
	    // std::cout <<"\t\t\tinserting into  new ..."<<new_i<<" "<<new_j<<" "<< new_k 
	    //           <<"\n\t\t\t from old...."  << i << " " << j << " " << k << " " << std::endl;
	    
	    if(rhs1_reorder_map.count(k)) {
	      new_probabilities[lhs_reorder_map.find(i)->second][rhs0_reorder_map.find(j)->second][rhs1_reorder_map.find(k)->second] = probabilities[i][j][k];
	    }
	  }
	}
      }
    }
  }
  probabilities = new_probabilities;

  //		std::cout << *this << std::endl;
}


// // I think this method does'nt work with compacted rules ...
// void BRule::test_annotations(const std::map<int,unsigned> & lookup) const{
//   assert(probabilities.size() == lookup.at(get_lhs()));
//   assert(probabilities[0].size() == lookup.at(get_rhs(0)));
//   assert(probabilities[0][0].size() == lookup.at(get_rhs(1)));
// }


void BRuleTraining::reset_counts()
{
  counts = std::vector<std::vector<std::vector<double> > >(probabilities.size());

  for(unsigned i = 0; i < probabilities.size(); ++i) {
    counts[i] = std::vector< std::vector<double> >(probabilities[i].size());
    for(unsigned j = 0; j < probabilities[i].size(); ++j)
      counts[i][j] = std::vector<double>(probabilities[i][j].size(),0.0);
    }
}


void BRuleTraining::reset_counts_soft()
{
  for(unsigned i = 0 ; i < counts.size(); ++i)
    for(unsigned j = 0 ; j < counts[i].size(); ++j)
      std::fill(counts[i][j].begin(), counts[i][j].end(), 0.0);
}


void BRuleTraining::update_probability(const AnnotatedNodeCountMap& nc_map)
{
  const std::vector<double>& node_counts = nc_map[get_lhs()];

  for (unsigned i = 0; i < counts.size(); ++i) {
    
    const double& node_count = node_counts.at(i);
    
    for (unsigned j = 0; j < counts[i].size(); ++j) {
      for (unsigned k = 0; k < counts[i][j].size(); ++k) {
	
	counts[i][j][k] /= node_count ;

	assert(!std::isnan(counts[i][j][k]));
	assert(counts[i][j][k] <= 1);
	assert(counts[i][j][k] >= 0);
      }
    }
  }

  counts.swap(probabilities);
}


void BRuleTraining::update_rule_frequencies(const scaled_array& left_in,
					    const scaled_array& right_in,
					    const scaled_array& up_out,
					    const scaled_array& root_insides)
{  
  double new_scale = scaled_array::calculate_scalingfactor(left_in.scale + right_in.scale + up_out.scale - 
							       root_insides.scale) / root_insides.array[0];
  for(unsigned short i = 0; i < probabilities.size(); ++i) {
    if(up_out.array[i] == 0.0) continue;
    double up_tree = up_out.array[i] * new_scale;
    //   const std::vector<std::vector<double> >& dim_i = probabilities[i];
    for(unsigned short j = 0; j < probabilities[i].size(); ++j) {
      double up_tree_left = up_tree * left_in.array[j];
      //const std::vector<double>& dim_j = dim_i[j];
      for(unsigned short k = 0 ; k < probabilities[i][j].size() ; ++k) {
	counts[i][j][k] += up_tree_left * right_in.array[k] * probabilities[i][j][k];
      }
    }
  }
}


