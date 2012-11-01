#include "BasicLexicon.h"

#include <boost/thread.hpp>

typedef std::map<std::pair<int,int> , int> lexical_counts_map;

BasicLexicon::~BasicLexicon() {}

BasicLexicon::BasicLexicon(const WordSignature& unknown_word_mapping_, int unknown_cutoff):
  unknown_word_cutoff(unknown_cutoff),  unknown_word_map(unknown_word_mapping_),
  annotated_node_counts()
{}

//lexical smoothing boolean ignored - we never do lexical smoothing...
void BasicLexicon::read_lexicon_from_Treebank(std::vector<PtbPsTree>& treebanktrees)
{

  std::map<int, unsigned> RHS_counts;
  std::map<int, unsigned> LHS_counts;
  lexical_counts_map lexical_counts;

  collect_base_lexical_counts(treebanktrees, LHS_counts, RHS_counts, lexical_counts);

  unsigned nb_unknown = 0;

  std::set<std::string> known_words;


  //map words with count <= cutoff to unknown token
  for(std::map<int, unsigned>::const_iterator i(RHS_counts.begin()); i != RHS_counts.end(); ++i) {
    if(i->second <= unknown_word_cutoff) {
      // std::cout << "yeah less than cutoff " << i->second << std::endl;
      ++nb_unknown;
    }
    else {
      (void) known_words.insert(SymbolTable::instance_word().translate(i->first));
    }
  }

  //replace low frequency words in treebank with unknown word symbol(s)
  std::clog << "replacing low frequency words ("<< nb_unknown  << " types) with unknown token..." << std::endl;
  replace_words_with_unknown(known_words, treebanktrees);

  lexical_counts.clear();
  RHS_counts.clear();
  LHS_counts.clear();

  //need to collect counts again like this because needed word position for word class mapping
  collect_base_lexical_counts(treebanktrees,LHS_counts,RHS_counts, lexical_counts);

  //normalise counts
  for(lexical_counts_map::const_iterator itr(lexical_counts.begin()); itr != lexical_counts.end(); ++itr){
    const lexical_pair& l = itr->first;
    double prob = double(itr->second) / double(LHS_counts[l.label]);
    lexical_rules.push_back(LexicalRuleTraining(l.label, l.word, prob));
  }

  std::clog << "created " << lexical_rules.size()  << " lexical rules" << std::endl;

  //	output_counts();

}


void BasicLexicon::update_annotated_rule_counts(LexicalRuleTraining& rule, const AnnotationInfo& IO_scores,
						const scaled_array& root_insides) const
{

  double new_scale = scaled_array::calculate_scalingfactor(IO_scores.outside_probabilities.scale -
							   root_insides.scale) / root_insides.array[0];

  std::vector<double>& lr = rule.get_counts();


  //for each annotation of the LHS
  for(unsigned i = 0 ; i < rule.get_probability().size();++i) {

    lr[i] += IO_scores.outside_probabilities.array[i] *  rule.get_probability()[i] * new_scale;

  }
}


//first calculates lexicon node counts then adds them to the grammar node counts.
//might be quicker to do in one step, merge the code for update_node_counts with this function
void BasicLexicon::add_lexicon_annotated_node_counts(std::vector< std::vector<double> >& GrammarNodeCountMap)
{
  //typedef boost::unordered_map< std::pair<int, int >, double> AnnotatedNodeCountMap;

  //first get the latest node counts
  update_node_counts();

  //add these to the grammar node counts datastructure (to be used for merge)
  for(AnnotatedNodeCountMap::const_iterator itr(annotated_node_counts.begin()); itr!= annotated_node_counts.end(); ++itr){
    //for now - assume that the POS tag set is distinct from the (internal) non-terminal set.
    //if this is not the case, changes will have to made in the Berkeley style lexicon.
    // for the basic lexicon the node counts for the preterms would have to be added to the other node
    // counts before maximisation of the grammar occurs.
    // (i.e. a function like this should be called but if the key is already in the GrammarNodeCountMap then the count should be incremented.)

    //assert(GrammarNodeCountMap.find(itr->first) == GrammarNodeCountMap.end());

    //assert(GrammarNodeCountMap[itr->first.first].size() == 0);
    //boost::unordered_map<unsigned, double> annotation_to_prob;
    //annotation_to_prob[itr->first.second] = itr->second;
    //GrammarNodeCountMap[itr->first.first] =annotation_to_prob;

    std::vector<double>& current_label_counts = GrammarNodeCountMap[itr->first];
    //    std::cout << itr->first.first << " "<<  itr->first.second << " " << itr->second << std::endl;

    // std::cout << "symbol: " << SymbolTable::instance_nt()->translate(itr->first.first)
    // 	      << " [" << itr->first.second << "] = " << itr->second << std::endl;

    current_label_counts = itr->second;

  }

}




//getting denominator counts
void BasicLexicon::update_node_counts(){
  //  typedef boost::unordered_map< std::pair<int, int >, double> AnnotatedNodeCountMap;
  annotated_node_counts.clear();

  //for each lexical rule
  for (std::vector<LexicalRuleTraining>::const_iterator iter(lexical_rules.begin()); iter != lexical_rules.end(); ++iter) {

    const std::vector<double>& rule_counts = iter->get_counts();

    std::vector<double>& current_annots = annotated_node_counts[iter->get_lhs()];
    current_annots.resize(rule_counts.size(),0.0);

    for(unsigned i = 0; i < rule_counts.size(); ++i) {
      current_annots[i] += rule_counts[i];
    }
  }

}

void BasicLexicon::maximisation(){
  // std::cout <<"maximizing ...lex" <<std::endl;
  update_node_counts();
  for (std::vector<LexicalRuleTraining>::iterator itr(lexical_rules.begin()); itr != lexical_rules.end(); ++itr){

    unsigned num_annotations = itr->get_num_annotations();
    //std::cout << "NUm Annot: " << num_annotations << std::endl;
    const std::vector<double>& current_node_counts = annotated_node_counts[itr->get_lhs()];

    for (unsigned i = 0; i < num_annotations; ++i) {
      //std::cout << "for annotation " << i << std::endl;


      double new_value = itr->get_count(i);
      if(new_value != 0)
	new_value /= current_node_counts[i];

      itr->set_probability(i,new_value);
      //  assert(new_value == itr->get_probability(i));

      assert(!std::isnan(new_value));
      assert(new_value <= 1);
      assert(new_value >= 0);

    }

  }

}


struct basic_lexicon_update_thread
{
  const BasicLexicon & lexicon;
  std::vector< std::pair<LexicalRuleTraining*, std::vector<lrule_occurrence> > >& lex_occurrences;
  unsigned begin;
  unsigned end;

  basic_lexicon_update_thread(const BasicLexicon& lexicon_,
      std::vector< std::pair<LexicalRuleTraining*, std::vector<lrule_occurrence> > >& lex_occurrences_,
      unsigned begin_,
      unsigned end_)
      : lexicon(lexicon_), lex_occurrences(lex_occurrences_), begin(begin_), end(end_) {};

  void operator()()
  {
    for(unsigned i = begin; i < end; ++i) {


      LexicalRuleTraining * rule = lex_occurrences[i].first;


      for(std::vector<lrule_occurrence>::iterator iter(lex_occurrences[i].second.begin());
          iter != lex_occurrences[i].second.end(); ++iter) {

        const TrainingNode * root = iter->root;
        const TrainingNode * node = iter->up;

        const scaled_array& tree_prob = root->get_annotations().inside_probabilities;

        lexicon.update_annotated_rule_counts(*rule, node->get_annotations(), tree_prob);


      }
    }
  }

};


void BasicLexicon::update_annotated_counts_from_trees(const std::vector<BinaryTrainingTree> & /*trees ignored*/,
						      bool /*ignored*/,
                                                      std::vector< std::pair<LexicalRuleTraining*, std::vector<lrule_occurrence> > >& lex_occurrences,
                                                      unsigned nbthreads
                                                      )
{
  boost::thread_group mygroup;

  for(unsigned i = 0; i < nbthreads; ++i) {
    mygroup.create_thread(basic_lexicon_update_thread(*this, lex_occurrences, i * lex_occurrences.size() / nbthreads, (i+1) * lex_occurrences.size() / nbthreads));
  }

    mygroup.join_all();


  // for(std::vector<BinaryTrainingTree>::const_iterator tree_iter(trees.begin()); tree_iter != trees.end(); ++tree_iter)
  //   traverse_leaf_nodes(*tree_iter);
}




//TODO do the tree traversal elsewhere
void BasicLexicon::traverse_leaf_nodes(const BinaryTrainingTree& tree)
{

  std::vector<LexicalTrainingNode*> lexnodes  = tree.get_lexical_nodes();
  const scaled_array& tree_prob = tree.get_root()->get_annotations().inside_probabilities;

  for(std::vector<LexicalTrainingNode*>::iterator it(lexnodes.begin()); it != lexnodes.end(); ++it) {

    update_annotated_rule_counts(*((*it)->get_rule()), (*it)->get_annotations(), tree_prob);
  }
}

void BasicLexicon::collect_base_lexical_counts(const std::vector<PtbPsTree>& treebanktrees,
					       std::map<int,unsigned>& LHS_counts, std::map<int,unsigned>& RHS_counts,
					       lexical_counts_map& lexical_counts)
{
  SymbolTable& sym_tab_nt = SymbolTable::instance_nt();
  SymbolTable& sym_tab_word = SymbolTable::instance_word();

  unsigned treebank_size = treebanktrees.size();

  for (unsigned tree_i=0; tree_i< treebank_size; ++tree_i){

    for(PtbPsTree::const_leaf_iterator j = treebanktrees[tree_i].lbegin();j !=treebanktrees[tree_i].lend(); ++j)
      {
	PtbPsTree::const_depth_first_iterator i = j;
	i.up();

	int lhs = sym_tab_nt.insert(*i);
	++LHS_counts[lhs];

	int rhs = sym_tab_word.insert(*j);
	++RHS_counts[rhs];

	++lexical_counts[lexical_pair(lhs, rhs)];
      }
  }
}

std::string BasicLexicon::get_word_class(const std::string& word, int position) const
{
  return unknown_word_map.get_unknown_mapping(word,position);
}

void BasicLexicon::replace_words_with_unknown(const std::set<std::string>& known_words, std::vector<PtbPsTree>& treebanktrees)
{
  unsigned treebank_size = treebanktrees.size();

  for (unsigned tree_i = 0; tree_i< treebank_size; ++tree_i){
    unsigned word_position=0;
    for(PtbPsTree::leaf_iterator i = treebanktrees[tree_i].lbegin(); i != treebanktrees[tree_i].lend(); ++i, ++word_position) {

      //if the word is in the list of unknown words - replace in the tree with unknown string
      if(!known_words.count(*i))
	{
	  //	  std::cout << *i << std::endl;
	  *i = get_word_class(*i,word_position);
	  SymbolTable::instance_word().insert(*i);
	  //	  std::cout << *i << std::endl;
	}
    }
  }
}

//input not used for this lexicon type- for historic reasons information taken from probabilities vectors in the rule
void BasicLexicon::resize_structures(AnnotatedLabelsInfo& /*annotated_labels_info*/){
  // std::vector<LexicalRule> lexical_rules;
  for( std::vector<LexicalRuleTraining>::iterator itr=lexical_rules.begin(); itr!= lexical_rules.end(); ++itr){
    itr->reset_counts();
  }

}

void BasicLexicon::reset_counts(){
  for( std::vector<LexicalRuleTraining>::iterator itr=lexical_rules.begin(); itr!= lexical_rules.end(); ++itr){
    itr->reset_counts_soft();
  }
}

void BasicLexicon::output_counts(const std::map< int, int>& LHS_counts, const std::map< int, int>& RHS_counts,
				 const lexical_counts_map& /*lexical_counts*/,
				 const lexical_counts_map& lex_and_unknown_counts)

{

  //	std::map< int, int> RHS_counts;
  //	std::map< int, int> LHS_counts;

  //	typedef std::map<std::pair<int,int> , int> id1_id2_counts_map;
  //	id1_id2_counts_map tag_word_counts;

  SymbolTable& sym_tab_nt = SymbolTable::instance_nt();
  SymbolTable& sym_tab_word = SymbolTable::instance_word();

  std::cout << "Tag counter" << std::endl;
  for(std::map<int,int>::const_iterator itr = LHS_counts.begin(); itr!= LHS_counts.end(); itr++)
    {
      std::cout << sym_tab_nt.translate(itr->first) << " " << itr->second << std::endl;
    }
  std::cout << "***************************************************" << std::endl;
  std::cout << "word counts " << std::endl;
  for(std::map<int,int>::const_iterator itr = RHS_counts.begin(); itr!= RHS_counts.end(); itr++)
    {
      std::cout << sym_tab_word.translate(itr->first) << " " << itr->second << std::endl;
    }
  //	typedef std::map<std::pair<int,int> , int> lexical_counts_map;
  //	lexical_counts_map lexical_counts;
  //	lexical_counts_map lex_and_unknown_counts;
  std::cout << "***************************************************" << std::endl;
  std::cout << "word-tag counter " << std::endl;
  for(lexical_counts_map::const_iterator itr = lex_and_unknown_counts.begin(); itr!= lex_and_unknown_counts.end(); itr++)
    {
      //l_itr->first.first,l_itr->first.second
      std::cout << sym_tab_nt.translate(itr->first.label) << " " << sym_tab_word.translate(itr->first.word) << " " << itr->second << std::endl;
    }
  //	typedef boost::unordered_map< std::pair<int, int >, URule> production_2_urule_map;
  // //URule(lhs,rhs0,probability,lex)
  //	  production_2_urule_map lexical_rules;

  //std::cout << "***************************************************" << std::endl;
  //std::cout << "probs " << std::endl;
  //for(std::vector<URule>::const_iterator itr = lexical_rules.begin(); itr!= lexical_rules.end(); itr++)
  //{
  //l_itr->first.first,l_itr->first.second
  //		std::cout << sym_tab_nt->translate(itr->first.first) << " " << sym_tab_word->translate(itr->first.second) << " " << itr->second.get_probability(0,0) << std::endl;
  //}

}

struct copy_helper
{
  template<typename T>
  const T& operator()(const T& rule) const {return rule;}
};

Lexicon * BasicLexicon::copy() const
{
  BasicLexicon * new_lex = new BasicLexicon(unknown_word_map,unknown_word_cutoff);

  new_lex->lexical_rules.resize(lexical_rules.size());
  copy_helper copy;
  std::transform(lexical_rules.begin(),lexical_rules.end(),new_lex->lexical_rules.begin(),copy);

  new_lex->annotated_node_counts = annotated_node_counts;

  return new_lex;
}

void BasicLexicon::copy(Lexicon*& dest) const
{
  if(dest == NULL) {
    BasicLexicon * new_lex = new BasicLexicon(unknown_word_map,unknown_word_cutoff);
    dest = new_lex;
  }

  BasicLexicon * new_lex = static_cast<BasicLexicon*>(dest);

  new_lex->lexical_rules.resize(lexical_rules.size());
  copy_helper copy;
  std::transform(lexical_rules.begin(),lexical_rules.end(),new_lex->lexical_rules.begin(),copy);

  new_lex->annotated_node_counts = annotated_node_counts;
}

//dummies
void BasicLexicon::lexical_smoothing() {}
void BasicLexicon::create_additional_rules() {}
