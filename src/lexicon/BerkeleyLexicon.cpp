#include "BerkeleyLexicon.h"

#include "utils/RandomGenerator.h"


BerkeleyLexicon::~BerkeleyLexicon() {}

BerkeleyLexicon::BerkeleyLexicon(const WordSignature& unknown_word_mapping_, double unknown_treebank_proportion_, int smoothing_threshold_):
  Lexicon(),
  unknown_word_map(unknown_word_mapping_),
  unknown_treebank_proportion(unknown_treebank_proportion_),
  smoothing_threshold(smoothing_threshold_),

  //default settings ... TODO add to LORG options
  smooth_parameter_0(0.5),
  smooth_parameter_1(0.1)
{}

//collects counts but also
//creates new rule, with lhs,rhs information, prob 0.0
void BerkeleyLexicon::collect_counts_from_treebank(std::vector<PtbPsTree>& treebanktrees)
{
  //  static RandomGenerator random_generator = RandomGenerator(0,1);

  SymbolTable& sym_tab_nt   = SymbolTable::instance_nt();
  SymbolTable& sym_tab_word = SymbolTable::instance_word();


  //add all the words to sym_tab_word
  for (unsigned tree_i = 0; tree_i< treebanktrees.size(); ++tree_i) {
    for(PtbPsTree::leaf_iterator j(treebanktrees[tree_i].lbegin()); j != treebanktrees[tree_i].lend(); ++j) {
      sym_tab_word.insert(*j);
    }
  }

  unsigned tree_i;

  unsigned treebank_size = treebanktrees.size();
  unsigned seen_word_limit = treebank_size * (1.0 -  unknown_treebank_proportion);
  //std::cout << "limit is "  << seen_word_limit << std::endl;

  //collect seen counts
  for (tree_i = 0; tree_i <= seen_word_limit; ++tree_i) {

    unsigned word_position = 0;
    for(PtbPsTree::leaf_iterator j(treebanktrees[tree_i].lbegin()); j != treebanktrees[tree_i].lend(); ++j, ++word_position) {
      // std::cout << "Tree: " << *i << std::endl;

      //get the tag node in i
      PtbPsTree::depth_first_iterator i = j;
      i.up();

      int lhs = sym_tab_nt.insert(*i);

      // increment count for lhs
      std::map<int,std::vector<double> >::iterator it_lhs = all_annotated_tag_counts.find(lhs);
      if (it_lhs == all_annotated_tag_counts.end()) {
	all_annotated_tag_counts.insert(std::make_pair(lhs,std::vector<double>(1, 1.0)));
      }
      else {
	++(it_lhs->second)[0];
      }

      //increment count for word
      int word = (sym_tab_word.get(*j));
      word_counts[word] += 1 /*+  random_generator.next()/100.0*/ ;

      //increment count for token
      ++all_tokens_total;

      //add word to words known from the training corpus
      known_words.insert(word);

      //increment of the TAG-WORD pair
      std::map<int, std::vector<double> >& counts_tag_fixed = annotated_tag_word_counts[lhs];

      //TAG-WORD first seen
      if(counts_tag_fixed[word].empty()) {
	counts_tag_fixed[word] =  std::vector<double>(1,0.0);
	lexical_rules.push_back(LexicalRuleTraining(lhs,word,0.0));
      }
      ++counts_tag_fixed[word][0];

      //add TAG-SIG
      std::string word_class = get_word_class(*j,word_position);
      int word_class_id = (sym_tab_word.insert(word_class));

      known_signatures.insert(word_class_id);
      //TAG-SIG first seen
      if(counts_tag_fixed[word_class_id].empty()) {
	counts_tag_fixed[word_class_id] =  std::vector<double>(1,0.0);
	//	additional_rules.push_back(LexicalRuleTraining(lhs,word_class_id,0.0));
      }
    }
  }

  //collect "unseen" counts
  //std::cout << "starting second phase at tree " << tree_i << std::endl;
  for (; tree_i < treebank_size; ++tree_i){
    PtbPsTree& current_tree = treebanktrees[tree_i];
    unsigned word_position = 0;
    for(PtbPsTree::leaf_iterator j(current_tree.lbegin()); j != current_tree.lend(); ++j, ++word_position) {

      PtbPsTree::depth_first_iterator i = j;
      i.up();

      int lhs = sym_tab_nt.insert(*i);
      int word_id = sym_tab_word.insert(*j);


      //if unknown
      if (!word_counts.count(word_id) || word_counts[word_id] == 0.0) {

	if (!unseen_annotated_tag_counts.count(lhs)){
	  unseen_annotated_tag_counts[lhs]=std::vector<double>(1, 0.0);
	}

	++unseen_annotated_tag_counts[lhs][0];
	++unseen_tokens_total;

	std::string word_class = get_word_class(*j,word_position);
	int word_class_id = (sym_tab_word.insert(word_class));
	known_signatures.insert(word_class_id);
	++word_counts[word_class_id];

	//create lexical rule LHS -> word_class if not already created
	std::map<int, std::vector<double> >& counts_tag_fixed = annotated_tag_word_counts[lhs];
	// TAG-SIG first seen
	if(counts_tag_fixed[word_class_id].empty()) {
	  counts_tag_fixed[word_class_id] =  std::vector<double>(1,0.0);
	  //	  additional_rules.push_back(LexicalRuleTraining(lhs,word_class_id,0.0));
	}
	++counts_tag_fixed[word_class_id][0];


      }

      known_words.insert(word_id);

      if (!all_annotated_tag_counts.count(lhs)) {
	all_annotated_tag_counts[lhs]=std::vector<double>(1, 0.0);
      }
      ++all_annotated_tag_counts[lhs][0];

      ++all_tokens_total;

      ++word_counts[word_id];

      std::map<int, std::vector<double> >& counts_tag_fixed = annotated_tag_word_counts[lhs];

      // TAG-word first seen
      if(counts_tag_fixed[word_id].empty()) {
	counts_tag_fixed[word_id] =  std::vector<double>(1,0.0);
	lexical_rules.push_back(LexicalRuleTraining(lhs,word_id,0.0));
      }
      ++counts_tag_fixed[word_id][0];

    }
  }

  //  std::cout << unseen_tokens_total << " / " << all_tokens_total << std::endl;





   // for(std::vector<LexicalRuleTraining>::const_iterator i(lexical_rules.begin()); i != lexical_rules.end();++i)
   //   std::cout << *i << std::endl;
  //std::cout << "known_words size " << known_words.size() << std::endl;
  //std::cout << "word couts size " << word_counts.size() << std::endl;


  //  std::cout << "collected everything" << std::endl;
}

void BerkeleyLexicon::create_additional_rules()
{
  ////////////  create all possible rules of the form TAG -> signature

  std::set<int> tags;
  //collect tags
  for(std::map<int, std::vector<double> >::const_iterator i(all_annotated_tag_counts.begin()); i != all_annotated_tag_counts.end(); ++i)
    tags.insert(i->first);

  //create new rules
  std::set<LexicalRuleTraining> new_sig_rules;
  for(std::set<int>::const_iterator t(tags.begin()); t != tags.end(); ++t)
    for(std::set<int>::const_iterator s(known_signatures.begin()); s != known_signatures.end(); ++s) {
      LexicalRuleTraining r(*t,*s,0.0, all_annotated_tag_counts[*t].size());
      //	std::cout << "adding " << r << std::endl;
      new_sig_rules.insert(r);

      std::map<int, std::vector<double> >& counts_tag_fixed = annotated_tag_word_counts[*t];
      std::map<int, std::vector<double> >::iterator it_word = counts_tag_fixed.find(*s);
      if(it_word == counts_tag_fixed.end()) {
	counts_tag_fixed.insert(std::make_pair(*s,std::vector<double>(all_annotated_tag_counts[*t].size(), 0.0)));
      }
    }

  //add new rules
  additional_rules.insert(additional_rules.end(),new_sig_rules.begin(),new_sig_rules.end());


  ////////////  create all possible rules of the form TAG -> RARE_WORD

  //collect rare words and rules
  std::set<int> rares;
  std::set<LexicalRuleTraining> rare_rules;
  for(std::vector<LexicalRuleTraining>::const_iterator i(lexical_rules.begin()); i != lexical_rules.end(); ++i) {
    if(known_words.count(i->get_word())) {
      if(word_counts[i->get_word()] <= smoothing_threshold) {
  	rares.insert(i->get_word());
  	rare_rules.insert(*i);
  	//      std::cout << "rare rule: " << *i << std::endl;
      }
    }
  }


  //create new rules
  std::set<LexicalRuleTraining> new_rare_rules;
  for(std::set<int>::const_iterator t(tags.begin()); t != tags.end(); ++t)
    for(std::set<int>::const_iterator w(rares.begin()); w != rares.end(); ++w) {
      LexicalRuleTraining r(*t,*w,0.0, all_annotated_tag_counts[*t].size());
      if(!rare_rules.count(r)) {
  	//	std::cout << "adding " << r << std::endl;
  	new_rare_rules.insert(r);

  	std::map<int, std::vector<double> >& counts_tag_fixed = annotated_tag_word_counts[*t];
  	std::map<int, std::vector<double> >::iterator it_word = counts_tag_fixed.find(*w);
  	if(it_word == counts_tag_fixed.end()) {
  	  counts_tag_fixed.insert(std::make_pair(*w,std::vector<double>(all_annotated_tag_counts[*t].size(), 0.0)));
  	}
      }
    }

  //add new rules
  //  lexical_rules.insert(lexical_rules.end(),new_rare_rules.begin(),new_rare_rules.end());
  additional_rules.insert(additional_rules.end(),new_rare_rules.begin(), new_rare_rules.end());

}




//note: might run in to problems if preterminals symbols also occur as internal non-terminals...this assumes that that is not the case
void BerkeleyLexicon::read_lexicon_from_Treebank(std::vector<PtbPsTree>& treebanktrees)
{
  //	std::cout << "size of treebank is: " << tb.get_size() << std::endl;

  all_tokens_total=0;//=seen_word_total
  unseen_tokens_total=0; //=unseen tag total

  collect_counts_from_treebank(treebanktrees);

  //std::cout << "initialising prob vectors .. " << std::endl;
  initialise_probability_vectors();


  //initalisation = true
  calculate_probabilities();

    std::clog << "created " << lexical_rules.size()  << " lexical rules" << std::endl;
    std::clog << "created " << additional_rules.size()  << " additional rules" << std::endl;
  //output_counts();
}

void BerkeleyLexicon::initialise_probability_vectors()
{
  // for (std::map<int, std::vector<double> >::const_iterator itr(unseen_annotated_tag_counts.begin());
  //      itr != unseen_annotated_tag_counts.end(); ++itr ){
  //   p_annotated_tag_given_unknown[itr->first] = std::vector<double>(1,0.0);
  // }

  // for (std::vector<LexicalRuleTraining>::iterator lex_rule_itr(lexical_rules.begin()); lex_rule_itr != lexical_rules.end(); ++lex_rule_itr) {
  //   p_annotated_tag_given_unknown[lex_rule_itr->get_lhs()] = std::vector<double>(1,0.0);
  // }
for (std::map<int, std::vector<double> >::const_iterator itr = all_annotated_tag_counts.begin(); itr!= all_annotated_tag_counts.end(); ++itr ){
  p_annotated_tag_given_unknown[itr->first] = std::vector<double>(1,0.0);
 }

}


//assumes vectors are of the correct size.
void BerkeleyLexicon::calculate_probabilities()
{
  //Calculate tagging probabilities
  //P(word|tag) = P(tag|word) P(word) / P(tag)

  //for each lexical rule
  for (std::vector<LexicalRuleTraining>::iterator lex_rule_itr(lexical_rules.begin()); lex_rule_itr != lexical_rules.end(); ++lex_rule_itr) {
    //either word or word_class
    int word_id = lex_rule_itr->get_word();
    int tag_id = lex_rule_itr->get_lhs();
    unsigned number_of_annotations = lex_rule_itr->get_num_annotations();

    //mutable access to probabilities
    std::vector<double>& lex_prob = lex_rule_itr->get_probability();
    const std::vector<double>& c_tw = annotated_tag_word_counts[tag_id][word_id];

    assert(c_tw.size() >=1);
    ///////////////////////
    //known words model
    ///////////////////////
    //calculate all P(word|tag) but first find P(tag|word)
    //P(tag|word) = count(word,tag)/count(tag) iff count(word) > threshold
    //            = (count(word,tag) + smooth * P(tag|unknown) ) / count(word) + smooth
    //	iff (word_count.at(word_id) > smoothing_threshold){

    const std::vector<double>& c_t = all_annotated_tag_counts[tag_id];

      //for each of the pos tags annotations
      for(unsigned i=0; i< number_of_annotations; ++i){
	//P(word|tag) = count(word,tag)/count(word)
	// no ... this is P(tag|word)
	// but P(word|tag) = P(tag|word)*P(word)/P(tag)
	// which gives:
	lex_prob[i] = c_tw[i] / c_t[i];

	// if(lex_prob[i] == 0) {
	//   //	      std:cout << "ZERO" << std::endl;
	//   // just a test
	//   lex_prob[i] = 1e-87;
	// }


      }
  }
}


//assumes vectors are of the correct size.
void BerkeleyLexicon::lexical_smoothing()
{
  //calculate all the P(tag|unknown) probs
  //iterate through POS tags
  for(std::map<int, std::vector<double> >::const_iterator itr(unseen_annotated_tag_counts.begin()); itr != unseen_annotated_tag_counts.end(); ++itr){
    int POS_id = itr->first;
    //iterate through annotations
    std::vector<double>& p_annotated_tag_given_unknown_POS_id = p_annotated_tag_given_unknown[POS_id];
    for (unsigned int i=0; i< itr->second.size(); ++i){
      double pos_count =  itr->second[i];
      p_annotated_tag_given_unknown_POS_id[i]= pos_count / unseen_tokens_total;

      // std::cout << SymbolTable::instance_nt()->get_label_string(POS_id) << " : "
      //  		<< i << " : " << pos_count << " : "
      // 		<< unseen_tokens_total << " : " << p_annotated_tag_given_unknown_POS_id[i] << std::endl;

    }
  }

  // hackish ....
  std::vector<std::vector<LexicalRuleTraining>::iterator> begins(2),ends(2);
  begins[0] = lexical_rules.begin();
  begins[1] = additional_rules.begin();
  ends[0] = lexical_rules.end();
  ends[1] = additional_rules.end();


  for(unsigned v = 0 ;  v < 2 ; ++v) {


    //for each lexical rule
    for (std::vector<LexicalRuleTraining>::iterator lex_rule_itr(begins[v]); lex_rule_itr != ends[v]; ++lex_rule_itr) {
      //either word or word_class
      int word_id = lex_rule_itr->get_word();
      int tag_id = lex_rule_itr->get_lhs();
      unsigned number_of_annotations = lex_rule_itr->get_num_annotations();

      //mutable access to probabilities
      std::vector<double>& lex_prob = lex_rule_itr->get_probability();
      const std::vector<double>& word_tag_counts = annotated_tag_word_counts[tag_id][word_id];

      assert(word_tag_counts.size() >=1);
      ///////////////////////
      //known words model
      ///////////////////////
      //caluculate all P(word|tag) but first find P(tag|word)
      //P(tag|word) = count(word,tag)/count(tag) iff count(word) > threshold
      //            = (count(word,tag) + smooth * P(tag|unknown) ) / count(word) + smooth
      //	iff (word_count.at(word_id) > smoothing_threshold){

      double c_w = word_counts[word_id];
      const std::vector<double>& c_t = all_annotated_tag_counts[tag_id];


      if (known_words.count(word_id)) {

	//	std::cout << SymbolTable::instance_word()->get_label_string(word_id) << std::endl;


	if(c_w <= smoothing_threshold) {

	  //	  std::cout << SymbolTable::instance_word()->get_label_string(word_id) << std::endl;


	  // std::cout << "c_w: " << c_w << std::endl;
	  // std::cout << "before rule is: " << *lex_rule_itr << std::endl;

	  //smooth
	  //P(word|tag) = count(word,tag)/count(tag) iff count(word) > threshold
	  //            = (count(word,tag) + smooth * P(tag|unknown) ) / count(word) + smooth

	  //for each of the pos tags annotations
	  std::map<int, std::vector<double> >::iterator patgu_itr = p_annotated_tag_given_unknown.find(tag_id);

	  if(patgu_itr == p_annotated_tag_given_unknown.end()) {
	    patgu_itr = (p_annotated_tag_given_unknown.insert(std::make_pair(tag_id, std::vector<double>(number_of_annotations,0.0)))).first;
	  }
	  const std::vector<double>& ptagid_given_unknown = patgu_itr->second;

	  double constant_part = c_w / (c_w + smooth_parameter_1);

	  for(unsigned i = 0; i < number_of_annotations; ++i){
	    //P(word|tag) = count(word,tag)/count(tag) iff count(word) > threshold
	    //            = (count(word,tag) + smooth * P(tag|unknown) ) / count(word) + smooth
	    //std::cout << "P_T_U[" << i << "]: " << ptagid_given_unknown[i] << std::endl;

	    lex_prob[i] = (word_tag_counts[i] + smooth_parameter_1 * ptagid_given_unknown[i]) * constant_part / c_t[i] ;

	    // if(lex_prob[i] == 0) {
	    //   //	      std:cout << "ZERO" << std::endl;
	    //   // just a test
	    //   lex_prob[i] = 1e-87;
	    // }


	  }

	  //      std::cout << "after rule is: " << *lex_rule_itr << std::endl;

	}
      }
      /////////////////////////
      //unknown words model
      //////////////////////////
      //P(word_class|tag) = P(tag|word_class) * P(word_class) / P(tag)
      else {
	//	std::cout << SymbolTable::instance_word()->get_label_string(word_id) << std::endl;
	//      std::cout << *lex_rule_itr << std::endl;

	const std::vector<double>& c_tw = annotated_tag_word_counts[tag_id][word_id];
	double constant_part = 1.0 / (c_w + smooth_parameter_0);
	assert(constant_part != 0.0);
	const std::vector<double>& ptagid_given_unknown = p_annotated_tag_given_unknown[tag_id];
	//const std::vector<double>& c_tu = unseen_annotated_tag_counts[tag_id];

	//      std::cout << *lex_rule_itr << std::endl;

	for(unsigned i = 0; i < number_of_annotations; ++i){
	  assert(i < lex_prob.size());


	  double c_t_u = ptagid_given_unknown[i];
	  //	std::cout << c_t_u << std::endl;

	  lex_prob[i] = (c_tw[i] + smooth_parameter_0 * c_t_u) * constant_part;
	  lex_prob[i] /= c_t[i] ;

	  // if(lex_prob[i] == 0) {
	  //   //	      std:cout << "ZERO" << std::endl;
	  //   // just a test
	  //   lex_prob[i] = 1e-87;
	  //   }

	}
	//      std::cout << *lex_rule_itr << std::endl;

      }

    }
  }
}


std::string BerkeleyLexicon::get_word_class(const std::string& word, int position) const
{
  return unknown_word_map.get_unknown_mapping(word,position);
}

void BerkeleyLexicon::reset_counts()
{

  //  std::cout << unseen_tokens_total << " / " << all_tokens_total << std::endl;


  all_tokens_total = 0;
  unseen_tokens_total = 0;

  //set all word counts to zero
  for(std::map<int,double>::iterator itr= word_counts.begin(); itr!= word_counts.end(); ++itr){
    itr->second=0.0;
  }

  //set all annotated tag counts to zero
  for(std::map<int, std::vector<double> >::iterator itr= all_annotated_tag_counts.begin(); itr!= all_annotated_tag_counts.end(); ++itr){
    std::fill(itr->second.begin(),itr->second.end(),0.0);
  }

  //set all unseen annotated tag counts to zero
  for(std::map<int, std::vector<double> >::iterator itr= unseen_annotated_tag_counts.begin(); itr!= unseen_annotated_tag_counts.end(); ++itr){
    std::fill(itr->second.begin(),itr->second.end(),0.0);
  }

  for(id1_id2_annotated_counts_map::iterator itr= annotated_tag_word_counts.begin(); itr!= annotated_tag_word_counts.end(); itr++){
    for(std::map<int, std::vector<double> >::iterator itr2(itr->second.begin()); itr2 != itr->second.end(); ++itr2)
      std::fill(itr2->second.begin(),itr2->second.end(),0.0);
  }
}

void BerkeleyLexicon::resize_structures(AnnotatedLabelsInfo& annotated_labels_info)
{
  //  std::cout << unseen_tokens_total << " / " << all_tokens_total << std::endl;


  //reset these counts
  all_tokens_total =0.0;
  unseen_tokens_total=0.0;
  for (std::map<int, double >::iterator itr(word_counts.begin()); itr!= word_counts.end(); ++itr){
      itr->second = 0.0;
  }

  //resize count datastructures...intialise counts  to zero.
  for (std::map<int, std::vector<double> >::iterator itr= all_annotated_tag_counts.begin(); itr!= all_annotated_tag_counts.end(); ++itr){
    int new_num_annotations = annotated_labels_info.get_number_of_annotations(itr->first);
    itr->second = std::vector<double>(new_num_annotations, 0.0);
  }

  for (std::map<int, std::vector<double> >::iterator itr= unseen_annotated_tag_counts.begin(); itr!= unseen_annotated_tag_counts.end(); ++itr){
    int new_num_annotations = annotated_labels_info.get_number_of_annotations(itr->first);
    itr->second = std::vector<double>(new_num_annotations, 0.0);
  }
  //typedef std::map<std::pair<int,int> , std::vector<double> > id1_id2_annotated_counts_map;
  for (id1_id2_annotated_counts_map::iterator itr= annotated_tag_word_counts.begin(); itr!= annotated_tag_word_counts.end(); ++itr){
    int new_num_annotations = annotated_labels_info.get_number_of_annotations(itr->first);
    for(std::map<int,std::vector<double> >::iterator j(itr->second.begin()); j != itr->second.end(); ++j )
      j->second = std::vector<double>(new_num_annotations, 0.0);
  }

  // //split probability datastructures...intialise probs  to zero.
  // for (std::map<int, std::vector<double> >::iterator itr=  p_annotated_tag.begin(); itr!=  p_annotated_tag.end(); ++itr){
  //   int new_num_annotations = annotated_labels_info.get_number_of_annotations(itr->first);
  //   itr->second = std::vector<double>(new_num_annotations, 0.0);
  // }

  for (std::map<int, std::vector<double> >::iterator itr=   p_annotated_tag_given_unknown.begin(); itr!=   p_annotated_tag_given_unknown.end(); ++itr){
    int new_num_annotations = annotated_labels_info.get_number_of_annotations(itr->first);
    itr->second = std::vector<double>(new_num_annotations, 0.0);
  }

}


std::vector<double> BerkeleyLexicon::compute_weight(const LexicalRuleTraining& rule, const AnnotationInfo& IO_scores,
						    const scaled_array& tree_probability) const
{
  unsigned num_annotations =rule.get_num_annotations();
  double factor = scaled_array::calculate_scalingfactor(IO_scores.outside_probabilities.scale - tree_probability.scale) / tree_probability.array[0];

  std::vector<double> weights(num_annotations);

  for(unsigned i = 0 ; i < num_annotations; ++i) {
    const double& outside = IO_scores.outside_probabilities.array[i];
    weights[i] = outside * rule.get_probability(i) * factor;
  }

  return weights;
}




//dodgy (IMHO) way to update unknown counts, a la Berkeley.  rule prob is the known rule prob (should be the unknown one?), tree prob
//and Inside Outside scores are taken from a tree with all known words.
void BerkeleyLexicon::update_annotated_counts_from_node_4unknown(int lhs, int word, int word_signature, const std::vector<double>& weight)
{
  std::vector<double>& unseen_tagcounts = unseen_annotated_tag_counts[lhs];
  std::vector<double>& seen_counts = annotated_tag_word_counts[lhs][word_signature];
  // should be updated somewhere else !!!
  seen_counts.resize(annotated_tag_word_counts[lhs][word].size());
  unseen_tagcounts.resize(seen_counts.size());

  for (unsigned i = 0; i < weight.size(); ++i) {
    unseen_tagcounts[i] += weight[i];
    unseen_tokens_total += weight[i];
    word_counts[word_signature] += weight[i];
    seen_counts[i] += weight[i];
  }
}

void BerkeleyLexicon::update_annotated_counts_from_node(int lhs, int word, const std::vector<double>& weight)
{
  std::vector<double>& counts = annotated_tag_word_counts[lhs][word];
  std::vector<double>& tag_counts = all_annotated_tag_counts[lhs];

  for (unsigned i = 0; i < weight.size(); ++i) {
    counts[i]+= weight[i];
    tag_counts[i] += weight[i];
    word_counts[word] += weight[i];
    all_tokens_total +=weight[i];
  }
}

void BerkeleyLexicon::traverse_leaf_nodes(const BinaryTrainingTree& tree, bool secondhalf, bool last_iteration)
{
  std::vector<LexicalTrainingNode*> lexnodes(tree.get_lexical_nodes());
  const scaled_array& tree_prob = tree.get_root()->get_annotations().inside_probabilities;

  unsigned word_position = 0;
  for(std::vector<LexicalTrainingNode*>::const_iterator it(lexnodes.begin()); it != lexnodes.end(); ++it, ++word_position) {

    const LexicalRuleTraining& rule =*((*it)->get_rule());
    std::vector<double> weights(compute_weight(rule, (*it)->get_annotations(), tree_prob));


    // TODO: investigate why last_iteration causes a bug here
    // smoothed grammars are missing additional rule probabilities while split and merged are not
    // this should speed up training and shouldn't impact results
    if( //last_iteration &&
	secondhalf &&  word_counts[rule.get_word()] == 0) {

       // std::cout << SymbolTable::instance_word()->translate(rule.get_word())
       // 		 << " "
       // 		 << get_word_class(SymbolTable::instance_word()->translate(rule.get_word()),word_position)
       // 		 << std::endl;

      int word_signature = SymbolTable::instance_word().get((get_word_class(SymbolTable::instance_word().translate(rule.get_word()),word_position)));
      update_annotated_counts_from_node_4unknown(rule.get_lhs(), rule.get_word(), word_signature, weights);
    }

    update_annotated_counts_from_node(rule.get_lhs(), rule.get_word(),weights);

  }
}

void BerkeleyLexicon::update_annotated_counts_from_trees(const std::vector<BinaryTrainingTree> & trees,
							 bool last_iteration,
                                                         std::vector< std::pair<LexicalRuleTraining*, std::vector<lrule_occurrence> > >& lex_occurrences,
                                                         unsigned nbthreads)
{

  unsigned treebank_size = trees.size();
  unsigned seen_word_limit = treebank_size * (1.0 -  unknown_treebank_proportion);

  // int seen_word_limit = treebank_size - (treebank_size * unknown_treebank_proportion);
  //std::cout << "limit is "  << seen_word_limit << std::endl;

  for (unsigned tree_i=0; tree_i< treebank_size; ++tree_i) {

    traverse_leaf_nodes(trees[tree_i], tree_i > seen_word_limit, last_iteration);
  }
  //std::cout << "outputing counts for all trees: "  << std::endl;
  //output_counts();
}


void BerkeleyLexicon::add_lexicon_annotated_node_counts(std::vector< std::vector<double> >& grammar_AnnotatedNodeCountMap)
{
  //append the POS tag counts store in all_annotated_tag_counts
  //to the general grammar annotate_node_count_map (for use with the merge step)
  for (std::map<int, std::vector<double> >::const_iterator itr(all_annotated_tag_counts.begin()); itr!= all_annotated_tag_counts.end(); ++itr){
    grammar_AnnotatedNodeCountMap[itr->first] = itr->second;
  }
}


void BerkeleyLexicon::maximisation()
{
  //std::cout << "maximisation..." << std::endl;
  calculate_probabilities();
  //print_rule_probabilities();
}

void BerkeleyLexicon::output_counts()
{
  SymbolTable& sym_tab_nt = SymbolTable::instance_nt();
  SymbolTable& sym_tab_word = SymbolTable::instance_word();

  std::cout << "all tokens total " << all_tokens_total << std::endl;
  std::cout << " unseen words " << 	unseen_tokens_total << std::endl;
  std::cout << "Tag counter" << std::endl;
  for(std::map<int, std::vector<double> >::const_iterator itr = all_annotated_tag_counts.begin(); itr!= all_annotated_tag_counts.end(); itr++) {
    std::cout << sym_tab_nt.translate(itr->first) << std::endl;
    //assert(itr->second.size() == 1);
    for(unsigned i = 0 ; i< itr->second.size(); i++){
      std::cout <<  "\t " << i << ": " << itr->second[i] << std::endl;
    }

  }
  std::cout << "***************************************************" << std::endl;
  std::cout << "unseenTag counter  (total=" << unseen_tokens_total << std::endl;
  for(std::map<int, std::vector<double> >::const_iterator itr = unseen_annotated_tag_counts.begin(); itr!= unseen_annotated_tag_counts.end(); itr++) {

    std::cout << sym_tab_nt.translate(itr->first) << std::endl;
    for(unsigned i = 0 ; i< itr->second.size(); i++){
      std::cout <<  "\t " << i << ": " << itr->second[i] << std::endl;
    }
  }
  //	std::map<int,int> word_counts;
  std::cout << "***************************************************" << std::endl;
  std::cout << "Word counter  " << std::endl;
  for(std::map<int,double>::const_iterator itr = word_counts.begin(); itr!= word_counts.end(); itr++) {
    std::cout << sym_tab_word.translate(itr->first) << " " << itr->second << std::endl;
  }
  //	typedef std::map<std::pair<int,int> , int> id1_id2_counts_map;
  //id1_id2_counts_map tag_word_counts;

  std::cout << "***************************************************" << std::endl;
  std::cout << "Tag-Word counter  " << std::endl;



  for(id1_id2_annotated_counts_map::const_iterator itr = annotated_tag_word_counts.begin(); itr!= annotated_tag_word_counts.end(); itr++) {
        for(std::map<int, std::vector<double> >::const_iterator j(itr->second.begin()); j != itr->second.end(); ++itr) {
    std::cout << sym_tab_nt.translate(itr->first) << " " << sym_tab_word.translate(j->first) << std::endl;
    for(unsigned i = 0 ; i< j->second.size(); i++){
      std::cout <<  "\t " << i << ": " << j->second[i] << std::endl;
    }
	}
  }

  //		std::map<int,double> p_tag_given_unknown;
  std::cout << "***************************************************" << std::endl;
  std::cout << "P(tag|unknown) total size is " << p_annotated_tag_given_unknown.size() << std::endl;
  for(std::map<int, std::vector<double> >::const_iterator itr =  p_annotated_tag_given_unknown.begin(); itr!=  p_annotated_tag_given_unknown.end(); itr++) {
    std::cout << sym_tab_nt.translate(itr->first) << " [id="  << itr->first << "]" <<std::endl;
    for(unsigned i = 0 ; i< itr->second.size(); i++){
      std::cout <<  "\t " << i << ": " << itr->second[i] << std::endl;
    }
  }


  //		std::map<int,int> unseen_tag_counts;
}

void BerkeleyLexicon::print_leaf_node_probs(BinaryTrainingTree* tree)
{
	SymbolTable& sym_tab_nt = SymbolTable::instance_nt();
	SymbolTable& sym_tab_word = SymbolTable::instance_word();
  std::vector<LexicalTrainingNode*> lexnodes  = tree->get_lexical_nodes();
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


void BerkeleyLexicon::print_rule_probabilities()
{

  SymbolTable& sym_tab_nt = SymbolTable::instance_nt();
  SymbolTable& sym_tab_word = SymbolTable::instance_word();

  std::cout << "PRINTING RULE PROBABILITIES...." << std::endl;

  // lexical_rules;
  for( std::vector<LexicalRuleTraining>::iterator itr=lexical_rules.begin(); itr!= lexical_rules.end(); itr++) {
    int pos_id = itr->get_lhs();
    int word_id =itr->get_rhs0();
    for(unsigned i = 0; i < itr->get_num_annotations(); i++) {
      std::cout << sym_tab_nt.get_label_string(pos_id) << "[" << pos_id << "]_" << i << " "
		<< sym_tab_word.get_label_string(word_id) << "[" << word_id << "]_"
		<< itr->get_probability(i) << std::endl;
    }
  }
}

struct copy_helper
{
  template<typename T>
  const T& operator()(const T& rule) const {return rule;}
};

Lexicon * BerkeleyLexicon::copy() const
{
  BerkeleyLexicon * other = new BerkeleyLexicon(unknown_word_map, unknown_treebank_proportion, smoothing_threshold);
  other->smooth_parameter_0 = smooth_parameter_0;
  other->smooth_parameter_1 = smooth_parameter_1;
  other->all_tokens_total = all_tokens_total;
  other->unseen_tokens_total = unseen_tokens_total;
  other->word_counts = word_counts;
  other->known_words = known_words;
  other->known_signatures = known_signatures;
  other->all_annotated_tag_counts = all_annotated_tag_counts;
  other->unseen_annotated_tag_counts = unseen_annotated_tag_counts;
  other->annotated_tag_word_counts = annotated_tag_word_counts;
  other->p_annotated_tag_given_unknown =  p_annotated_tag_given_unknown;

  other->lexical_rules.resize(lexical_rules.size());
  std::transform(lexical_rules.begin(),lexical_rules.end(), other->lexical_rules.begin(),copy_helper());

  // other->additional_rules.resize(additional_rules.size());
  // std::transform(additional_rules.begin(),additional_rules.end(), other->additional_rules.begin(),copy_helper());


  return other;
}

void BerkeleyLexicon::copy(Lexicon*& dest) const
{
  if(dest == NULL) {
    BerkeleyLexicon * new_other = new BerkeleyLexicon(unknown_word_map, unknown_treebank_proportion, smoothing_threshold);
    dest = new_other;
  }

  BerkeleyLexicon* other = static_cast<BerkeleyLexicon*>(dest);

  other->smooth_parameter_0 = smooth_parameter_0;
  other->smooth_parameter_1 = smooth_parameter_1;
  other->all_tokens_total = all_tokens_total;
  other->unseen_tokens_total = unseen_tokens_total;
  other->word_counts = word_counts;
  other->known_words = known_words;
  other->known_signatures = known_signatures;
  other->all_annotated_tag_counts = all_annotated_tag_counts;
  other->unseen_annotated_tag_counts = unseen_annotated_tag_counts;
  other->annotated_tag_word_counts = annotated_tag_word_counts;
  other->p_annotated_tag_given_unknown =  p_annotated_tag_given_unknown;

  other->lexical_rules.resize(lexical_rules.size());
  std::transform(lexical_rules.begin(),lexical_rules.end(), other->lexical_rules.begin(),copy_helper());
  // other->additional_rules.resize(additional_rules.size());
  // std::transform(additional_rules.begin(),additional_rules.end(), other->additional_rules.begin(),copy_helper());
}
