#include "BinaryTrainingTree.h"

#include "utils/SymbolTable.h"


BinaryTrainingTree::BinaryTrainingTree() : root(NULL) {}

//need to fix that somehow
BinaryTrainingTree::~BinaryTrainingTree()
{}

inline unsigned nb_subtrees(const PtbPsTree& tree, const PtbPsTree::const_depth_first_iterator& position)
{
  unsigned res = 0;
  PtbPsTree::const_depth_first_iterator copy = position;
  copy.down_first();
  while(copy != tree.dfend()) {++res; copy.right();}
  return res;
}

TrainingNode * BinaryTrainingTree::create_training_node(const PtbPsTree& tree,
							PtbPsTree::const_depth_first_iterator current,
							MAP<std::pair<int,std::pair<int,int> >,BRuleTraining*> * brulemap,
							MAP<std::pair<int,int>,URuleTraining*> * urulemap,
							MAP<std::pair<int,int>,LexicalRuleTraining*> * lrulemap)
{
  switch(nb_subtrees(tree,current)) {

  case 2 : {
    BinaryTrainingNode * new_node = new BinaryTrainingNode();
    new_node->lhs = SymbolTable::instance_nt().insert(*current);

    PtbPsTree::const_depth_first_iterator first_daughter_iter = current;
    first_daughter_iter.down_first();
    new_node->left = create_training_node(tree,first_daughter_iter,brulemap,urulemap,lrulemap);

    PtbPsTree::const_depth_first_iterator last_daughter_iter = current;
    last_daughter_iter.down_last();
    new_node->right = create_training_node(tree,last_daughter_iter,brulemap,urulemap,lrulemap);

    new_node->rule = NULL;
    if (brulemap)  {
      //std::cout << "binary" << std::endl;

      MAP<std::pair<int,std::pair<int,int> >,BRuleTraining*>::iterator it;

      if((it = brulemap->find(std::make_pair(new_node->lhs,
					     std::make_pair(new_node->left->get_lhs(),new_node->right->get_lhs())))) != brulemap->end()) {
    	  	new_node->rule = it->second;
      }

      return new_node;
    }
    break;
  }

  case 1 : {
    PtbPsTree::const_depth_first_iterator first_daughter_iter = current;
    first_daughter_iter.down_first();

    if(first_daughter_iter->leaf()) {  //create lexical node
      LexicalTrainingNode * new_node = new LexicalTrainingNode();
      new_node->lhs = SymbolTable::instance_nt().insert(*current);
      //      int word_id = SymbolTable::instance_word()->insert(*first_daughter_iter);


      new_node->rule = NULL;
      int word_id = SymbolTable::instance_word().get_label_id(*first_daughter_iter);
      MAP<std::pair<int,int>,LexicalRuleTraining*>::iterator it;
      if((it = lrulemap->find(std::make_pair(new_node->lhs,word_id))) != lrulemap->end()) {
	new_node->rule = it->second;
      }
      //assert(new_node->rule !=0);
      // doesn't work with validation trees ?
      if(new_node->rule == NULL)
	std::cout << "No rule for " << *current << " -> " <<
	  *first_daughter_iter << std::endl;

      return new_node;
    }

    else {

      UnaryTrainingNode * new_node = new UnaryTrainingNode();
      new_node->lhs = SymbolTable::instance_nt().insert(*current);

      new_node->left = create_training_node(tree,first_daughter_iter,brulemap,urulemap,lrulemap);

      new_node->rule = NULL;
      MAP<std::pair<int,int>,URuleTraining*>::iterator it;
      if((it = urulemap->find(std::make_pair(new_node->lhs,new_node->left->get_lhs()))) != urulemap->end()) {
    	  new_node->rule = it->second;
      }

      return new_node;
    }
    break;
  }
  default: std::cout << "pb in tree" << std::endl;
  }
  return NULL;
}

BinaryTrainingTree::BinaryTrainingTree(const PtbPsTree& tree,
				       MAP<std::pair<int,std::pair<int,int> >,BRuleTraining*> * brulemap,
				       MAP<std::pair<int,int>,URuleTraining*> * urulemap,
				       MAP<std::pair<int,int>,LexicalRuleTraining*> * lrulemap)
  :
  root(create_training_node(tree, tree.dfbegin(),brulemap,urulemap,lrulemap))
{
  root->resize_annotations(1);
}

void BinaryTrainingTree::print_leaf_node_probs()
{
  SymbolTable& sym_tab_nt = SymbolTable::instance_nt();
  SymbolTable& sym_tab_word = SymbolTable::instance_word();
  std::vector<LexicalTrainingNode*> lexnodes  = this->get_lexical_nodes();
  //std::cout << "PRINTING LEAF NODES..." << std::endl;
  for(std::vector<LexicalTrainingNode*>::iterator it =lexnodes.begin();
      it != lexnodes.end(); ++it) {

    LexicalRuleTraining* rule = (*it)->get_rule();
    const AnnotationInfo& IO_scores = (*it)->get_annotations();
    int pos_id = rule->get_lhs();
    int word_id =rule->get_rhs0();
    std::cout << "(" << sym_tab_nt.get_label_string(pos_id) << " " << sym_tab_word.get_label_string(word_id) << " ";
    for(unsigned i = 0; i < rule->get_num_annotations(); i++)
      {
	std::cout << "[" << i << " " << IO_scores.inside_probabilities.get_scaled_value(i) << "] " ;
      }
    // std::cout << "] ";
  }
  std::cout <<std::endl;

}
