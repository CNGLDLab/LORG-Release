#include "TrainingNode.h"

#include "utils/SymbolTable.h"

/////// resize_annotations
void TrainingNode::resize_annotations(unsigned size)
{
  annotations.resize(size);
}

void LexicalTrainingNode::resize_annotations(unsigned size)
{
  TrainingNode::resize_annotations(size);
}

void BinaryTrainingNode::resize_annotations(unsigned size)
{
  TrainingNode::resize_annotations(size);
  left->resize_annotations(size);
  right->resize_annotations(size);
}

void UnaryTrainingNode::resize_annotations(unsigned size)
{
  TrainingNode::resize_annotations(size);
  left->resize_annotations(size);
}
  
void TrainingNode::resize_annotations(const AnnotatedLabelsInfo& lookup)
{
  annotations.resize(lookup.get_number_of_annotations(lhs));
}

void LexicalTrainingNode::resize_annotations(const AnnotatedLabelsInfo& lookup)
{
  TrainingNode::resize_annotations(lookup);
}

void BinaryTrainingNode::resize_annotations(const AnnotatedLabelsInfo& lookup)
{
  TrainingNode::resize_annotations(lookup);
  left->resize_annotations(lookup);
  right->resize_annotations(lookup);
}

void UnaryTrainingNode::resize_annotations(const AnnotatedLabelsInfo& lookup)
{
  TrainingNode::resize_annotations(lookup);
  left->resize_annotations(lookup);
}


/////////////// reset_inside_probabilities


void TrainingNode::reset_inside_probabilities(double value)
{
  annotations.reset_inside_probabilities(value);
}

void LexicalTrainingNode::reset_inside_probabilities(double value)
{
  annotations.reset_inside_probabilities(value);
}

void BinaryTrainingNode::reset_inside_probabilities(double value)
{
  TrainingNode::reset_inside_probabilities(value);
  left->reset_inside_probabilities(value);
  right->reset_inside_probabilities(value);
}

void UnaryTrainingNode::reset_inside_probabilities(double value)
{
  TrainingNode::reset_inside_probabilities(value);
  left->reset_inside_probabilities(value);
}



/////////////// reset_outside_probabilities


void TrainingNode::reset_outside_probabilities(double value)
{
  annotations.reset_outside_probabilities(value);
}

void LexicalTrainingNode::reset_outside_probabilities(double value)
{
  annotations.reset_outside_probabilities(value);
}

void BinaryTrainingNode::reset_outside_probabilities(double value)
{
  TrainingNode::reset_outside_probabilities(value);
  left->reset_outside_probabilities(value);
  right->reset_outside_probabilities(value);
}

void UnaryTrainingNode::reset_outside_probabilities(double value)
{
  TrainingNode::reset_outside_probabilities(value);
  left->reset_outside_probabilities(value);
}



///////// reset_probabilities

void TrainingNode::reset_probabilities(double value)
{
  annotations.reset_probabilities(value);
}

void LexicalTrainingNode::reset_probabilities(double value)
{
  annotations.reset_probabilities(value);
 }

void BinaryTrainingNode::reset_probabilities(double value)
{
  TrainingNode::reset_probabilities(value);
  left->reset_probabilities(value);
  right->reset_probabilities(value);
}

void UnaryTrainingNode::reset_probabilities(double value)
{
  TrainingNode::reset_probabilities(value);
  left->reset_probabilities(value);
}

/////////////// update_delta_scores

void TrainingNode::update_delta_scores(DeltaMap &  delta_scores_map,
				       int split_number, 
				       const ProportionsMap & proportions, 
				       const scaled_array& root_inside) const 
{
  calculate_delta_score(delta_scores_map,split_number,proportions, root_inside);
}


void BinaryTrainingNode::update_delta_scores(DeltaMap &  delta_scores_map,
					     int split_number,
					     const ProportionsMap & proportions,
					     const scaled_array& root_inside) const 
{
  TrainingNode::update_delta_scores(delta_scores_map, split_number, proportions, root_inside);
  left->update_delta_scores(delta_scores_map, split_number, proportions, root_inside);
  right->update_delta_scores(delta_scores_map, split_number, proportions, root_inside);
}

void UnaryTrainingNode::update_delta_scores(DeltaMap &  delta_scores_map,
					    int split_number,
					    const ProportionsMap & proportions,
					    const scaled_array& root_inside) const 
{
  TrainingNode::update_delta_scores(delta_scores_map, split_number, proportions, root_inside);
  left->update_delta_scores(delta_scores_map, split_number, proportions, root_inside);
}



//////////////// calculate_delta_score

// using anonymous namespace is supposed
// to enable better optimization
namespace {

  inline bool is_top_node (int lhs){
    static int top = SymbolTable::instance_nt().get(LorgConstants::tree_root_name);
    return lhs == top;
  }

}

void TrainingNode::calculate_delta_score(DeltaMap &  delta_scores_map, int split_size, 
					 const ProportionsMap & proportions,
					 const scaled_array& root_inside) const
{
  if (is_top_node(lhs)) { return; }
 
  unsigned number_of_annotations = annotations.get_size();
  std::vector<double> in_out(number_of_annotations);
  unsigned number_of_splits = number_of_annotations / (unsigned) split_size;
  //std::cout << "number of annotations: " << number_of_annotations << " num splits " << split_size << std::endl;
    
  assert((number_of_annotations % split_size) == 0) ;
    
  for(unsigned latent_a=0; latent_a < number_of_annotations; ++latent_a) {
    in_out[latent_a] =  annotations.inside_probabilities.array[latent_a] *
      annotations.outside_probabilities.array[latent_a] ;
    }
        
    unsigned start = 0;
    //for each split set of the node
    for (unsigned i = 0; i< number_of_splits; ++i){
      
      double merged_inside = 0.0;
      double merged_outside = 0.0;

      const std::vector<double>& current_proportions = proportions[get_lhs()];
	    
      for (unsigned j=start; j< (start+ split_size); ++j){
	
	//say we are at node x
	//IN(x,split i) += proportion(label of node x,annotation j) * IN(x, annotation j) 
	merged_inside += current_proportions[j] * annotations.inside_probabilities.array[j];	
	      
	//OUT(x,split i) + = OUT(x, annotation j) 
	merged_outside += annotations.outside_probabilities.array[j];
      }
      //std::cout << "merged inside  " << merged_inside << " merged outside " << merged_outside << std::endl;
	    
      //likelihood after merge of split i
      double likelihood_merge = merged_inside * merged_outside ;
	    
      for (unsigned latent_a=0;latent_a < number_of_annotations; ++latent_a){
	      
	//if not an annotation that's being merged...
	if (latent_a < start || latent_a >= (start + split_size)){
	  likelihood_merge += in_out[latent_a];
	}
      }
	
      //delta score update
      std::pair<int,int> delta_key = std::make_pair(get_lhs(),start);
      
      assert(likelihood_merge > 0.0);
      
      //initialise
      if (delta_scores_map.find(delta_key) == delta_scores_map.end()){
	
	
	delta_scores_map[delta_key] = 0.0;
      }
	    
      delta_scores_map[delta_key] += std::log(root_inside.array[0]) -  std::log(likelihood_merge) + 
	scaled_array::calculate_logscalingfactor(root_inside.scale - 
						 (annotations.inside_probabilities.scale + 
						  annotations.outside_probabilities.scale));
	
      
      start += split_size;
    }

}

/////////////////////// compute_inside_probability

void LexicalTrainingNode::compute_inside_probability()
{
  if(rule) // training tree should always go there
    rule->update_inside_annotations(annotations.inside_probabilities.array);
  else // validation tree
    annotations.inside_probabilities.array = std::vector<double>(annotations.inside_probabilities.array.size(),1.0);
}

void BinaryTrainingNode::compute_inside_probability()
{
  left->compute_inside_probability();
  right->compute_inside_probability();

  annotations.reset_inside_probabilities();
  
  if(rule) {
    rule->update_inside_annotations(annotations.inside_probabilities.array,
				    left->get_annotations().inside_probabilities.array, 
				    right->get_annotations().inside_probabilities.array);

    annotations.inside_probabilities.scale_array(left->get_annotations().inside_probabilities.scale + 
						 right->get_annotations().inside_probabilities.scale );
  }
  else // validation tree ?
    annotations.inside_probabilities.array = std::vector<double>(annotations.inside_probabilities.array.size(),1.0);
}


void UnaryTrainingNode::compute_inside_probability()
{
  left->compute_inside_probability();

  annotations.reset_inside_probabilities();
  
  if(rule) {
    rule->update_inside_annotations(annotations.inside_probabilities.array, 
				    left->get_annotations().inside_probabilities.array);
    annotations.inside_probabilities.scale_array(left->get_annotations().inside_probabilities.scale);
  }
  else // validation ?
    annotations.inside_probabilities.array = std::vector<double>(annotations.inside_probabilities.array.size(),1.0);

}

//////////// compute_outside_probability


void LexicalTrainingNode::compute_outside_probability()
{
  //  rule->update_outside_annotations(annotations.outside_probabilities, word_annotation_outside);
}

void BinaryTrainingNode::compute_outside_probability()
{
  left->get_annotations().reset_outside_probabilities();
  right->get_annotations().reset_outside_probabilities();

  rule->update_outside_annotations(annotations.outside_probabilities.array,
				   left->get_annotations().inside_probabilities.array,
				   right->get_annotations().inside_probabilities.array,
				   left->get_annotations().outside_probabilities.array,
				   right->get_annotations().outside_probabilities.array);


  // rule->update_outside_annotations_left(annotations.outside_probabilities.array,
  // 					left->get_annotations().outside_probabilities.array,
  // 					right->get_annotations().inside_probabilities.array);

  left->get_annotations().outside_probabilities.scale_array(annotations.outside_probabilities.scale +
							    right->get_annotations().inside_probabilities.scale);
  
  // rule->update_outside_annotations_right(annotations.outside_probabilities.array,
  // 					 left->get_annotations().inside_probabilities.array,
  // 					 right->get_annotations().outside_probabilities.array);

  right->get_annotations().outside_probabilities.scale_array(annotations.outside_probabilities.scale +
							     left->get_annotations().inside_probabilities.scale);

  
  left->compute_outside_probability();
  right->compute_outside_probability();
}

void UnaryTrainingNode::compute_outside_probability()
{
  left->get_annotations().reset_outside_probabilities();

  rule->update_outside_annotations(annotations.outside_probabilities.array,
				   left->get_annotations().outside_probabilities.array);

  left->get_annotations().outside_probabilities.scale_array(annotations.outside_probabilities.scale);

  
  left->compute_outside_probability();
}

//////////////// update_rule_frequencies

void LexicalTrainingNode::update_rule_frequencies(const scaled_array& /*root_insides*/)
{// performed by the lexicon
}

void BinaryTrainingNode::update_rule_frequencies(const scaled_array& root_insides)
{
  
  rule->update_rule_frequencies(left->get_annotations().inside_probabilities,
				right->get_annotations().inside_probabilities, 
				get_annotations().outside_probabilities, 
				root_insides);
  //now visit the daughters
  left->update_rule_frequencies(root_insides);
  right->update_rule_frequencies(root_insides);
  
}



void UnaryTrainingNode::update_rule_frequencies(const scaled_array& root_insides)
{
  rule->update_rule_frequencies(left->get_annotations().inside_probabilities, 
				get_annotations().outside_probabilities,  
				root_insides);
  left->update_rule_frequencies(root_insides);
}


/////////////

bool LexicalTrainingNode::contains_empty_rules() const
{
  return rule == NULL;
}

bool BinaryTrainingNode::contains_empty_rules() const
{
  return rule == NULL || left->contains_empty_rules()
    || right->contains_empty_rules();
}


bool UnaryTrainingNode::contains_empty_rules() const
{
  return rule == NULL || left->contains_empty_rules();
}


/////////////////////////////

void LexicalTrainingNode::get_lexical_nodes(std::vector<LexicalTrainingNode*>& buffer) 
{
  buffer.push_back(this);
}


void BinaryTrainingNode::get_lexical_nodes(std::vector<LexicalTrainingNode*>& buffer) 
{
  left->get_lexical_nodes(buffer);
  right->get_lexical_nodes(buffer);
}

void UnaryTrainingNode::get_lexical_nodes(std::vector<LexicalTrainingNode*>& buffer) 
{left->get_lexical_nodes(buffer);}


///////////////////////////////

void LexicalTrainingNode::add_occurrences(boost::unordered_map<BRuleTraining*,std::vector<brule_occurrence> >&, 
					  boost::unordered_map<URuleTraining*,std::vector<urule_occurrence> >&,
                                          boost::unordered_map<LexicalRuleTraining*,std::vector<lrule_occurrence> >& lmap,
					  const TrainingNode* root) const
{
  lmap[rule].push_back(lrule_occurrence(this,root));
}


void BinaryTrainingNode::add_occurrences(boost::unordered_map<BRuleTraining*, std::vector<brule_occurrence> >& bmap,
					 boost::unordered_map<URuleTraining*, std::vector<urule_occurrence> >& umap,
                                         boost::unordered_map<LexicalRuleTraining*, std::vector<lrule_occurrence> >& lmap,
					 const TrainingNode* root)  const
{
  bmap[rule].push_back(brule_occurrence(left,right,this,root));
  left->add_occurrences(bmap, umap, lmap, root);
  right->add_occurrences(bmap, umap, lmap, root);
}

void UnaryTrainingNode::add_occurrences(boost::unordered_map<BRuleTraining*, std::vector<brule_occurrence> >& bmap,
					boost::unordered_map<URuleTraining*, std::vector<urule_occurrence> >& umap,
                                        boost::unordered_map<LexicalRuleTraining*, std::vector<lrule_occurrence> >& lmap,
					const TrainingNode* root) const
{
  umap[rule].push_back(urule_occurrence(left,this,root));
  left->add_occurrences(bmap, umap, lmap, root);
}
